
#include <time.h>
#include <unistd.h>

#include <semaphore.h>

#include <ti/display/Display.h>
#include <ti/net/sntp/sntp.h>

#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/net/wifi/device.h>

#define NTP_PORT          123
#define NTP_SERVERS       1
#define NTP_SERVERS_SIZE  (NTP_SERVERS * sizeof(struct sockaddr_in))
#define NTP_TIMEOUT       30000

/* Must wait at least 15 sec to retry NTP server (RFC 4330) */
#define NTP_POLL_TIME     15

char NTP_HOSTNAME[] = "pool.ntp.org";

extern Display_Handle AWSIOT_display;

void startNTP(void);

/*
 *  ======== setTime ========
 */
void setTime(uint32_t t)
{
    SlDateTime_t dt;
    struct tm tm;
    struct timespec tspec;
    time_t ts;

    tspec.tv_sec = t;
    clock_settime(CLOCK_REALTIME, &tspec);

    time(&ts);
    tm = *localtime(&ts);

    /* Set system clock on network processor to validate certificate */
    dt.tm_day  = tm.tm_mday;
    /* tm.tm_mon is the month since January, so add 1 to get the actual month */
    dt.tm_mon  = tm.tm_mon + 1;
    /* tm.tm_year is the year since 1900, so add 1900 to get the actual year */
    dt.tm_year = tm.tm_year + 1900;
    dt.tm_hour = tm.tm_hour;
    dt.tm_min  = tm.tm_min;
    dt.tm_sec  = tm.tm_sec;
    sl_DeviceSet(SL_DEVICE_GENERAL, SL_DEVICE_GENERAL_DATE_TIME,
            sizeof(SlDateTime_t), (unsigned char *)(&dt));
}

/*
 *  ======== getTime ========
 */
uint32_t getTime(void)
{
    struct timespec tspec;

    clock_gettime(CLOCK_REALTIME, &tspec);

    return (tspec.tv_sec);
}

/*
 *  ======== startNTP ========
 */
void startNTP(void)
{
    int ret;
    int ip;
    time_t ts;
    struct sockaddr_in ntpAddr = {0};
    struct hostent *dnsEntry;
    struct in_addr **addr_list;

    dnsEntry = gethostbyname(NTP_HOSTNAME);
    if (dnsEntry == NULL) {
        Display_printf(AWSIOT_display, 0, 0,
                "startNTP: NTP host cannot be resolved!");
    }

    /* Get the first IP address returned from DNS */
    addr_list = (struct in_addr **)dnsEntry->h_addr_list;
    ip = (*addr_list[0]).s_addr;

    ntpAddr.sin_addr.s_addr = htonl(ip);
    ntpAddr.sin_port = htons(NTP_PORT);
    ntpAddr.sin_family = AF_INET;

    do {
        ret = SNTP_getTime((struct sockaddr *)&ntpAddr, getTime, setTime);
        if (ret != 0) {
            Display_printf(AWSIOT_display, 0, 0,
                "startNTP: couldn't get time (%d), will retry in %d secs ...",
                ret, NTP_POLL_TIME);
            sleep(NTP_POLL_TIME);
            Display_printf(AWSIOT_display, 0, 0, "startNTP: retrying ...");
        }
    } while (ret != 0);

    ts = time(NULL);
    Display_printf(AWSIOT_display, 0, 0,
            "Current time: %s\n", ctime(&ts));
}
