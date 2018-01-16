#include <string.h>

#include <unistd.h>

#include <ti/drivers/I2C.h>
#include <pthread.h>

/* Example/Board Header files */
#include "Board.h"

#include "bma222drv.h"
#include "tmp006drv.h"

#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_shadow_interface.h"

/*!
 * The goal of this sample application is to demonstrate the capabilities of shadow.
 * This device(say Connected Window) will open the window of a room based on temperature
 * It can report to the Shadow the following parameters:
 *  1. temperature of the room (double)
 *  2. status of the window (open or close)
 * It can act on commands from the cloud. In this case it will open or close the window based on the json object "windowOpen" data[open/close]
 *
 * The two variables from a device's perspective are double temperature and bool windowOpen
 * The device needs to act on only on windowOpen variable, so we will create a primitiveJson_t object with callback
 The Json Document in the cloud will be
 {
 "reported": {
 "temperature": 0,
 "windowOpen": false
 },
 "desired": {
 "windowOpen": false
 }
 }
 */

#define MAX_LENGTH_OF_UPDATE_JSON_BUFFER 200

I2C_Handle initI2s();
PWM_Handle pwmInit();
void pwmUpdate(PWM_Handle, uint16_t);
uint8_t temperatureReading(I2C_Handle);
uint8_t accelerometerReading(I2C_Handle);
int16_t adjustPWM(PWM_Handle pwmHandle, int16_t pwmDuty);

int8_t      xVal, yVal, zVal;
float       temperatureVal;
uint8_t     temperatureDesired;
float       fTemperatureDesired;

void ShadowUpdateStatusCallback(const char *pThingName, ShadowActions_t action,
        Shadow_Ack_Status_t status, const char *pReceivedJsonDocument,
        void *pContextData)
{
    if (status == SHADOW_ACK_TIMEOUT) {
        IOT_INFO("Update Timeout--");
    } else if (status == SHADOW_ACK_REJECTED) {
        IOT_INFO("Update RejectedXX");
    } else if (status == SHADOW_ACK_ACCEPTED) {
        IOT_INFO("Update Accepted !!");
    }
}

void windowActuate_Callback(const char *pJsonString, uint32_t JsonStringDataLen,
      jsonStruct_t *pContext)
{
    bool windowOpened = *(bool *)(pContext->pData);
    if (pContext != NULL) {
        IOT_INFO("Delta - Window state changed to %d", *(bool *)(pContext->pData));
    }
    if ( windowOpened )
        GPIO_write(Board_LED0, Board_LED_ON);
    else
        GPIO_write(Board_LED0, Board_LED_OFF);

}

void tempDesired_Callback(const char *pJsonString, uint32_t JsonStringDataLen,
      jsonStruct_t *pContext)
{
    if (pContext != NULL) {
        temperatureDesired = *(uint8_t *)(pContext->pData);
        fTemperatureDesired = temperatureDesired;
        IOT_INFO("Delta - Temperature Desired changed to %d", temperatureDesired);

    }
}

void runAWSClient(void)
{
    IoT_Error_t rc = FAILURE;

    AWS_IoT_Client mqttClient;
    PWM_Handle pwmHandle;
    int16_t pwmDuty;
    // new i2C stuff
    I2C_Handle  i2cHandle;

    char JsonDocumentBuffer[MAX_LENGTH_OF_UPDATE_JSON_BUFFER];
    size_t sizeOfJsonDocumentBuffer = sizeof(JsonDocumentBuffer) / sizeof(JsonDocumentBuffer[0]);

	temperatureVal = 0.0;
    fTemperatureDesired = 0.0;
    temperatureDesired = 0;
	pwmDuty = 0;

    bool windowOpen = false;
    GPIO_write(Board_LED0, Board_LED_OFF);
/*
    jsonStruct_t windowActuator;
    windowActuator.cb = windowActuate_Callback;
    windowActuator.pData = &windowOpen;
    windowActuator.pKey = "windowOpen";
    windowActuator.type = SHADOW_JSON_BOOL;
*/
    jsonStruct_t temperatureHandler;
    temperatureHandler.cb = NULL;
    temperatureHandler.pKey = "temperature";
    temperatureHandler.pData = &temperatureVal;
    temperatureHandler.type = SHADOW_JSON_FLOAT;

    jsonStruct_t tempDesiredHandler;
    tempDesiredHandler.cb = tempDesired_Callback;
    tempDesiredHandler.pKey = "tempDesired";
    tempDesiredHandler.pData = &temperatureDesired;
    tempDesiredHandler.type = SHADOW_JSON_UINT8;

    jsonStruct_t xValHandler;
    xValHandler.cb = NULL;
    xValHandler.pKey = "xval";
    xValHandler.pData = &xVal;
    xValHandler.type = SHADOW_JSON_INT8;

    jsonStruct_t yValHandler;
    yValHandler.cb = NULL;
    yValHandler.pKey = "yval";
    yValHandler.pData = &yVal;
    yValHandler.type = SHADOW_JSON_INT8;

    jsonStruct_t zValHandler;
    zValHandler.cb = NULL;
    zValHandler.pKey = "zval";
    zValHandler.pData = &zVal;
    zValHandler.type = SHADOW_JSON_INT8;

    jsonStruct_t pwmDutyHandler;
    pwmDutyHandler.cb = NULL;
    pwmDutyHandler.pKey = "pwmDuty";
    pwmDutyHandler.pData = &pwmDuty;
    pwmDutyHandler.type = SHADOW_JSON_UINT16;

    IOT_INFO("\nAWS IoT SDK Version(dev) %d.%d.%d-%s\n", VERSION_MAJOR,
            VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

    IOT_DEBUG("Using rootCA %s", AWS_IOT_ROOT_CA_FILENAME);
    IOT_DEBUG("Using clientCRT %s", AWS_IOT_CERTIFICATE_FILENAME);
    IOT_DEBUG("Using clientKey %s", AWS_IOT_PRIVATE_KEY_FILENAME);

    ShadowInitParameters_t sp = ShadowInitParametersDefault;
    sp.pHost = AWS_IOT_MQTT_HOST;
    sp.port = AWS_IOT_MQTT_PORT;
    sp.pClientCRT = AWS_IOT_CERTIFICATE_FILENAME;
    sp.pClientKey = AWS_IOT_PRIVATE_KEY_FILENAME;
    sp.pRootCA = AWS_IOT_ROOT_CA_FILENAME;
    sp.enableAutoReconnect = false;
    sp.disconnectHandler = NULL;

    IOT_INFO("Shadow Init");
    rc = aws_iot_shadow_init(&mqttClient, &sp);
    if(SUCCESS != rc) {
        IOT_ERROR("Shadow Connection Error");
        return;
    }

    ShadowConnectParameters_t scp = ShadowConnectParametersDefault;
    scp.pMyThingName = AWS_IOT_MY_THING_NAME;
    scp.pMqttClientId = AWS_IOT_MQTT_CLIENT_ID;
    scp.mqttClientIdLen = (uint16_t) strlen(AWS_IOT_MQTT_CLIENT_ID);

    IOT_INFO("Shadow Connect");
    rc = aws_iot_shadow_connect(&mqttClient, &scp);
    if(SUCCESS != rc) {
        IOT_ERROR("Shadow Connection Error");
        return;
    }

    /*
     * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
     *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
     *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
     */
    rc = aws_iot_shadow_set_autoreconnect_status(&mqttClient, true);
    if(SUCCESS != rc) {
        IOT_ERROR("Unable to set Auto Reconnect to true - %d", rc);
    }

    rc = aws_iot_shadow_register_delta(&mqttClient, &tempDesiredHandler);

    if(SUCCESS != rc) {
        IOT_ERROR("Shadow Register Delta Error");
    }

    i2cHandle = initI2s();
    pwmHandle = pwmInit();

    // loop and publish a change in temperature
    while(NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc) {
        rc = aws_iot_shadow_yield(&mqttClient, 200);
        if(NETWORK_ATTEMPTING_RECONNECT == rc) {
            usleep(1000);
            // If the client is attempting to reconnect we will skip the rest of the loop.
            continue;
        }
        IOT_INFO("On Device: window state %s", windowOpen ? "true" : "false");

        accelerometerReading(i2cHandle);
        temperatureReading(i2cHandle);

        rc = aws_iot_shadow_init_json_document(JsonDocumentBuffer, sizeOfJsonDocumentBuffer);
        if(SUCCESS == rc) {
            rc = aws_iot_shadow_add_reported(JsonDocumentBuffer,
                    sizeOfJsonDocumentBuffer, 6,
                    &temperatureHandler,
                    &tempDesiredHandler,
                    &xValHandler,
                    &yValHandler,
                    &zValHandler,
                    &pwmDutyHandler);

            if(SUCCESS == rc) {
                rc = aws_iot_finalize_json_document(JsonDocumentBuffer, sizeOfJsonDocumentBuffer);
                if(SUCCESS == rc) {
                    IOT_INFO("Update Shadow: %s", JsonDocumentBuffer);
                    rc = aws_iot_shadow_update(&mqttClient, AWS_IOT_MY_THING_NAME, JsonDocumentBuffer,
                                               ShadowUpdateStatusCallback, NULL, 4, true);
                }
            }
        }
        temperatureReading(i2cHandle);
        pwmDuty = adjustPWM(pwmHandle, pwmDuty);
        usleep(500000);
        temperatureReading(i2cHandle);
        pwmDuty = adjustPWM(pwmHandle, pwmDuty);
        usleep(500000);
    }

    if(SUCCESS != rc) {
        IOT_ERROR("An error occurred in the loop %d", rc);
    }

    /* Deinitialized I2C */
    I2C_close(i2cHandle);
    IOT_INFO("I2C closed!\n");

    IOT_INFO("Disconnecting");
    rc = aws_iot_shadow_disconnect(&mqttClient);

    if(SUCCESS != rc) {
        IOT_ERROR("Disconnect error %d", rc);
    }

}


int16_t adjustPWM(PWM_Handle pwmHandle, int16_t pwmDuty) {
    int16_t savePwmDuty = pwmDuty;
    if ( temperatureVal < (fTemperatureDesired - 0.5) ) {
        pwmDuty += 50;
    } else if ( temperatureVal > (fTemperatureDesired + 0.5) ) {
        pwmDuty -= 50;
    } else  if ( temperatureVal < (fTemperatureDesired - 0.25) ) {
        pwmDuty += 25;
    } else if ( temperatureVal > (fTemperatureDesired + 0.25) ) {
        pwmDuty -= 25;
    }
    if ( pwmDuty >= 3000 ) pwmDuty = 3000;
    if ( pwmDuty <= 0 ) pwmDuty = 0;
    if ( pwmDuty != savePwmDuty ) {
        pwmUpdate(pwmHandle, pwmDuty);
    }
    return pwmDuty;

}
I2C_Handle initI2s() {

    I2C_Params  i2cParams;
    I2C_Handle  i2cHandle;

    I2C_init();
    IOT_INFO("Starting the i2ctmp006 example\n");

    /* Create I2C for usage */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2cHandle = I2C_open(Board_I2C0, &i2cParams);
    if (i2cHandle == NULL)
    {
        IOT_ERROR("[Link local task] Error Initializing I2C\n\r");
    }
    else {
        IOT_INFO("I2C Initialized!\n");
    }

    /* Setup mutex operations for sensors reading */
//    pthread_mutex_init(&sensorLockObj , (pthread_mutexattr_t*)NULL);
    return i2cHandle;
}

//*****************************************************************************
//
//! Function to read temperature. This reads the IR Temperature sensor
//! as opposed to only the ambient die temp, so it's vastly more "unstable"
//! as just a sensor on the CC3220S-LAUNCHXL Launchpad, but it's also
//! a lot more fun that way.
//!
//! \param  none
//!
//! \return SUCCESS or FAILURE
//!
//*****************************************************************************
uint8_t temperatureReading(I2C_Handle i2cHandle)
{
    int32_t status;
    float fTempRead;

    /* Read temperature axis values */
    status = TMP006DrvGetTemp(i2cHandle, &fTempRead);
/*
    if (status != 0)
    {
        // try to read again
        status = TMP006DrvGetTemp(i2cHandle, &fTempRead);
        // leave previous values
        if (status != 0)
        {
            IOT_ERROR("[Link local task] Failed to read data from temperature sensor\n\r");
        }
    }
*/
    if (status == 0)
    {
//        fTempRead = (fTempRead > 100) ? 100 : fTempRead;
        temperatureVal = fTempRead;
    }

    return status;
}

//*****************************************************************************
//
//! Function to read accelarometer
//!
//! \param  none
//!
//! \return SUCCESS or FAILURE
//!
//*****************************************************************************
uint8_t accelerometerReading(I2C_Handle i2cHandle)
{
    int8_t     xValRead, yValRead, zValRead;
    int32_t status;

/*
    if (sensorLockObj != NULL)
    {
        pthread_mutex_lock(&sensorLockObj);
    }
*/
    /* Read accelarometer axis values */
    status = BMA222ReadNew(i2cHandle, &xValRead, &yValRead, &zValRead);
/*
    if (status != 0)
    {
        // try to read again
        status = BMA222ReadNew(i2cHandle, &xValRead, &yValRead, &zValRead);
        // leave previous values
        if (status != 0)
        {
            IOT_ERROR("[Link local task] Failed to read data from accelarometer\n\r");
        }
    }
*/
    if (status == 0)
    {
        xVal = xValRead;
        yVal = yValRead;
        zVal = zValRead;
    }
/*
    if (sensorLockObj != NULL)
    {
        pthread_mutex_unlock(&sensorLockObj);
    }
*/
    return status;
}

PWM_Handle pwmInit()
{
    /* Period and duty in microseconds */
    uint16_t   pwmPeriod = 3000;

    PWM_Handle pwmHandle;

    /* Sleep time in microseconds */
    pwmHandle = NULL;
    PWM_Params params;

    /* Call driver init functions. */
    PWM_init();

    PWM_Params_init(&params);
    params.dutyUnits = PWM_DUTY_US;
    params.dutyValue = 0;
    params.periodUnits = PWM_PERIOD_US;
    params.periodValue = pwmPeriod;

    pwmHandle = PWM_open(Board_PWM0, &params);

    if ( pwmHandle != NULL ) {
        PWM_start(pwmHandle);
        PWM_setDuty(pwmHandle, 0);
    }

    return pwmHandle;
}
/*
 *  ======== mainThread ========
 *  Task periodically increments the PWM duty for the on board LED.
 */
void pwmUpdate(PWM_Handle pwmHandle, uint16_t duty)
{
    if ( pwmHandle != NULL ) {
        PWM_setDuty(pwmHandle, duty);
    }
}
