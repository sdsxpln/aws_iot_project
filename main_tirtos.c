/*
 *  ======== main_tirtos.c ========
 */
/*
 *  ======== main_tirtos.c ========
 */
#include <stdint.h>

/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include <ti/sysbios/BIOS.h>
#include <ti/display/Display.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/net/tls.h>

/* Example/Board Header files */
#include "Board.h"

#include "certs.h"
#include "aws_iot_config.h"

/*
 * The following macro is disabled by default. This is done to prevent the
 * certificate files from being written to flash every time the program
 * is run.  If an update to the cert files are needed, just update the
 * corresponding arrays, and rebuild with this macro defined. Note
 * you must remember to disable it otherwise the files will keep being
 * overwritten each time.
 */
#ifdef OVERWRITE_CERTS
static bool overwriteCerts = true;
#else
static bool overwriteCerts = false;
#endif

Display_Handle AWSIOT_display;

extern void NetWiFi_init(void);
extern void runAWSClient(void);

/*
 *  ======== flashCerts ========
 *  Utility function to flash the contents of a buffer (PEM format) into the
 *  filename/path specified by certName (DER format)
 */
void flashCerts(uint8_t *certName, uint8_t *buffer, uint32_t bufflen)
{
    int status = 0;
    int16_t slStatus = 0;
    SlFsFileInfo_t fsFileInfo;

    /* Check if the cert file already exists */
    slStatus = sl_FsGetInfo(certName, 0, &fsFileInfo);

    /* If the cert doesn't exist, write it (or overwrite if specified to) */
    if (slStatus == SL_ERROR_FS_FILE_NOT_EXISTS || overwriteCerts == true) {

        Display_printf(AWSIOT_display, 0, 0, "Flashing certificate file ...");

        /* Convert the cert to DER format and write to flash */
        status = TLS_writeDerFile(buffer, bufflen, TLS_CERT_FORMAT_PEM,
                (const char *)certName);

        if (status != 0) {
            Display_printf(AWSIOT_display, 0, 0,
                    "Error: Could not write file %s to flash (%d)\n",
                    certName, status);
            while(1);
        }
        Display_printf(AWSIOT_display, 0, 0, " successfully wrote file %s to flash\n",
                certName);
    }
}

/*
 *  ======== awsThreadFxn ========
 */
void *awsThreadFxn(void *arg0)
{
    /* Initialize SimpleLink */
    NetWiFi_init();

    /* Flash Certificate Files */
    flashCerts((uint8_t *)AWS_IOT_ROOT_CA_FILENAME, (uint8_t *)root_ca_pem,
            strlen(root_ca_pem));

    flashCerts((uint8_t *)AWS_IOT_CERTIFICATE_FILENAME,
            (uint8_t *)client_cert_pem, strlen(client_cert_pem));

    flashCerts((uint8_t *)AWS_IOT_PRIVATE_KEY_FILENAME,
            (uint8_t *)client_private_key_pem, strlen(client_private_key_pem));

    runAWSClient();

    return (NULL);
}

/*
 *  ======== main ========
 */
int main(void)
{
// start original
    pthread_attr_t pthreadAttrs;
    pthread_t slThread;
    pthread_t awsThread;
//    pthread_t timerThread;
    struct sched_param  priParam;
    int                 detachState;
//    int status;

    Board_initGeneral();
    GPIO_init();
    SPI_init();
    PWM_init();
    Timer_init();
    Display_init();

    GPIO_write(Board_LED0, Board_LED_OFF);
    GPIO_write(Board_LED1, Board_LED_OFF);
    GPIO_write(Board_LED2, Board_LED_OFF);


    /* Open the display for output */
    AWSIOT_display = Display_open(Display_Type_UART, NULL);
    if (AWSIOT_display == NULL) {
        /* Failed to open display driver */
        while (1);
    }

    // GPIO_write(Board_LED0, Board_LED_ON);

    /* Create the sl_Task thread */
    pthread_attr_init(&pthreadAttrs);

    priParam.sched_priority = 1;
    pthread_attr_setschedparam(&pthreadAttrs, &priParam);

    detachState = PTHREAD_CREATE_DETACHED;
    pthread_attr_setdetachstate(&pthreadAttrs, detachState);

    /* Create the Simplelink thread */
    pthread_attr_setstacksize(&pthreadAttrs, 2048);
    pthread_create(&slThread, &pthreadAttrs, sl_Task, NULL);

    /* Create the AWS thread */
    pthread_attr_setstacksize(&pthreadAttrs, 3328);
    pthread_create(&awsThread, &pthreadAttrs, awsThreadFxn, NULL);

    pthread_attr_destroy(&pthreadAttrs);

    /*  To enable low power mode, uncomment the following line.
     *  Please be aware that your JTAG connection will be
     *  dropped when entering low power mode. You must reset the
     *  board in order to re-establish your JTAG connection.
     */
    /* Power_enablePolicy(); */

    BIOS_start();
// end original
    return (0);
}
