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

void initI2s(void);
uint8_t temperatureReading(void);
uint8_t accelerometerReading(void);

// new i2C stuff
int8_t      xVal, yVal, zVal;
float       temperatureVal;
I2C_Handle  i2cHandle;


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

void runAWSClient(void)
{
    IoT_Error_t rc = FAILURE;

    AWS_IoT_Client mqttClient;

    char JsonDocumentBuffer[MAX_LENGTH_OF_UPDATE_JSON_BUFFER];
    size_t sizeOfJsonDocumentBuffer = sizeof(JsonDocumentBuffer) / sizeof(JsonDocumentBuffer[0]);

	temperatureVal = 0.0;

    bool windowOpen = false;
    GPIO_write(Board_LED0, Board_LED_OFF);

    jsonStruct_t windowActuator;
    windowActuator.cb = windowActuate_Callback;
    windowActuator.pData = &windowOpen;
    windowActuator.pKey = "windowOpen";
    windowActuator.type = SHADOW_JSON_BOOL;

    jsonStruct_t temperatureHandler;
    temperatureHandler.cb = NULL;
    temperatureHandler.pKey = "temperature";
    temperatureHandler.pData = &temperatureVal;
    temperatureHandler.type = SHADOW_JSON_FLOAT;

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

    rc = aws_iot_shadow_register_delta(&mqttClient, &windowActuator);

    if(SUCCESS != rc) {
        IOT_ERROR("Shadow Register Delta Error");
    }

    initI2s();
//    pwmledInit(3000);
//    pwmled(1500);

    // loop and publish a change in temperature
    while(NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc) {
        rc = aws_iot_shadow_yield(&mqttClient, 200);
        if(NETWORK_ATTEMPTING_RECONNECT == rc) {
            usleep(1000);
            // If the client is attempting to reconnect we will skip the rest of the loop.
            continue;
        }
        IOT_INFO("On Device: window state %s", windowOpen ? "true" : "false");

        temperatureReading();
        accelerometerReading();

        rc = aws_iot_shadow_init_json_document(JsonDocumentBuffer, sizeOfJsonDocumentBuffer);
        if(SUCCESS == rc) {
            rc = aws_iot_shadow_add_reported(JsonDocumentBuffer,
                    sizeOfJsonDocumentBuffer, 5,
                    &temperatureHandler,
                    &windowActuator,
                    &xValHandler,
                    &yValHandler,
                    &zValHandler);

            if(SUCCESS == rc) {
                rc = aws_iot_finalize_json_document(JsonDocumentBuffer, sizeOfJsonDocumentBuffer);
                if(SUCCESS == rc) {
                    IOT_INFO("Update Shadow: %s", JsonDocumentBuffer);
                    rc = aws_iot_shadow_update(&mqttClient, AWS_IOT_MY_THING_NAME, JsonDocumentBuffer,
                                               ShadowUpdateStatusCallback, NULL, 4, true);
                }
            }
        }
        sleep(1);
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

void initI2s() {

    I2C_Params  i2cParams;

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
uint8_t temperatureReading(void)
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
uint8_t accelerometerReading(void)
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
