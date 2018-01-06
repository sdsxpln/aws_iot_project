/*
 * Copyright 2015-2017 Texas Instruments Incorporated. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#include <network_interface.h>
#include <aws_iot_error.h>

#include <ti/net/tls.h>
#include <ti/net/ssock.h>

extern uint32_t NetWiFi_isConnected(void);

static void iot_tls_set_connect_params(Network *pNetwork, char *pRootCALocation,
        char *pDeviceCertLocation, char *pDevicePrivateKeyLocation,
        char *pDestinationURL, uint16_t DestinationPort, uint32_t timeout_ms,
        bool ServerVerificationFlag) {

    pNetwork->tlsConnectParams.DestinationPort = DestinationPort;
    pNetwork->tlsConnectParams.pDestinationURL = pDestinationURL;
    pNetwork->tlsConnectParams.pDeviceCertLocation = pDeviceCertLocation;
    pNetwork->tlsConnectParams.pDevicePrivateKeyLocation =
            pDevicePrivateKeyLocation;
    pNetwork->tlsConnectParams.pRootCALocation = pRootCALocation;
    pNetwork->tlsConnectParams.timeout_ms = timeout_ms;
    pNetwork->tlsConnectParams.ServerVerificationFlag = ServerVerificationFlag;
}

IoT_Error_t iot_tls_init(Network *pNetwork, char *pRootCALocation,
        char *pDeviceCertLocation, char *pDevicePrivateKeyLocation,
        char *pDestinationURL, uint16_t DestinationPort, uint32_t timeout_ms,
        bool ServerVerificationFlag)
{
    if (pNetwork == NULL) {
        return (NULL_VALUE_ERROR);
    }

    iot_tls_set_connect_params(pNetwork, pRootCALocation, pDeviceCertLocation,
            pDevicePrivateKeyLocation, pDestinationURL, DestinationPort,
            timeout_ms, ServerVerificationFlag);

    pNetwork->connect = iot_tls_connect;
    pNetwork->read = iot_tls_read;
    pNetwork->write = iot_tls_write;
    pNetwork->disconnect = iot_tls_disconnect;
    pNetwork->isConnected = iot_tls_is_connected;
    pNetwork->destroy = iot_tls_destroy;

    pNetwork->tlsDataParams.ssock = NULL;
    pNetwork->tlsDataParams.tlsH = NULL;

    return (SUCCESS);
}

IoT_Error_t iot_tls_connect(Network *pNetwork, TLSConnectParams *TLSParams)
{
    IoT_Error_t ret = SUCCESS;
    int ip;
    int skt = 0;
    struct sockaddr_in address;
    TLSConnectParams *tlsParams;
    TLSDataParams *tlsDataParams;
    struct hostent *dnsEntry;
    struct in_addr **addr_list;

    if (pNetwork == NULL) {
        return (NULL_VALUE_ERROR);
    }

    if (TLSParams != NULL) {
        iot_tls_set_connect_params(pNetwork, TLSParams->pRootCALocation,
                TLSParams->pDeviceCertLocation, TLSParams->pDevicePrivateKeyLocation,
                TLSParams->pDestinationURL, TLSParams->DestinationPort,
                TLSParams->timeout_ms, TLSParams->ServerVerificationFlag);
    }

    /* Use TLS params in Network struct */
    tlsParams = &pNetwork->tlsConnectParams;
    tlsDataParams = &pNetwork->tlsDataParams;

    /*
     *  Create TLS context.
     */
    tlsDataParams->tlsH = TLS_create(TLS_METHOD_CLIENT_TLSV1_2);
    if (tlsDataParams->tlsH == NULL) {
        ret = NETWORK_SSL_INIT_ERROR;
        goto QUIT;
    }

    /* Specify the root CA certificate */
    TLS_setCertFile(tlsDataParams->tlsH, TLS_CERT_TYPE_CA, TLS_CERT_FORMAT_DER,
            pNetwork->tlsConnectParams.pRootCALocation);

    /* Specify the device certificate */
    TLS_setCertFile(tlsDataParams->tlsH, TLS_CERT_TYPE_CERT, TLS_CERT_FORMAT_DER,
            pNetwork->tlsConnectParams.pDeviceCertLocation);

    /* Specify the device private key */
    TLS_setCertFile(tlsDataParams->tlsH, TLS_CERT_TYPE_KEY, TLS_CERT_FORMAT_DER,
            pNetwork->tlsConnectParams.pDevicePrivateKeyLocation);

    dnsEntry = gethostbyname(tlsParams->pDestinationURL);
    if (dnsEntry == NULL) {
        ret = NETWORK_ERR_NET_UNKNOWN_HOST;
        goto QUIT;
    }
    
    /* Get the first IP address returned from DNS */
    addr_list = (struct in_addr **)dnsEntry->h_addr_list;
    ip = (*addr_list[0]).s_addr;

    skt = socket(AF_INET, SOCK_STREAM, SL_SEC_SOCKET);
    if (skt < 0) {
        ret = NETWORK_ERR_NET_SOCKET_FAILED;
        goto QUIT;
    }

    tlsDataParams->ssock = Ssock_create(skt);
    if (tlsDataParams->ssock == NULL) {
        ret = NETWORK_ERR_NET_SOCKET_FAILED;
        goto QUIT;
    }

    if (Ssock_startTLS(tlsDataParams->ssock, tlsDataParams->tlsH) != 0) {
        ret = NETWORK_SSL_CERT_ERROR;
        goto QUIT;
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(tlsParams->DestinationPort);
    address.sin_addr.s_addr = htonl(ip);

    if (connect(skt, (struct sockaddr  *)&address,
            sizeof(address)) < 0) {
        ret = NETWORK_ERR_NET_CONNECT_FAILED;
        goto QUIT;
    }


QUIT:

    if (ret != SUCCESS) {
        if (tlsDataParams->ssock) {
            Ssock_delete(&tlsDataParams->ssock);
        }

        if (tlsDataParams->tlsH) {
            TLS_delete((TLS_Handle *)&tlsDataParams->tlsH);
        }

        if (skt >= 0) {
            close(skt);
        }
    }

    return (ret);
}

IoT_Error_t iot_tls_is_connected(Network *pNetwork)
{
    return ((IoT_Error_t)NetWiFi_isConnected());
}

IoT_Error_t iot_tls_write(Network *pNetwork, unsigned char *pMsg, size_t len,
            Timer *timer, size_t *numbytes)
{
    Ssock_Handle ssock = NULL;
    int bytes = 0;

    if (pNetwork == NULL || pMsg == NULL ||
            pNetwork->tlsDataParams.ssock == NULL || numbytes == NULL) {
        return (NULL_VALUE_ERROR);
    }

    ssock = pNetwork->tlsDataParams.ssock;

    bytes = Ssock_send(ssock, pMsg, len, 0);
    if (bytes > 0) {
        *numbytes = (size_t)bytes;
        return (SUCCESS);
    }

    return (NETWORK_SSL_WRITE_ERROR);
}

IoT_Error_t iot_tls_read(Network *pNetwork, unsigned char *pMsg, size_t len,
        Timer *timer, size_t *numbytes)
{
    int bytes = 0;
    int skt;
    struct timeval tv;
    Ssock_Handle ssock = NULL;
    uint32_t timeout;

    if (pNetwork == NULL || pMsg == NULL ||
            pNetwork->tlsDataParams.ssock == NULL || timer == NULL ||
            numbytes == NULL) {
        return (NULL_VALUE_ERROR);
    }

    ssock = pNetwork->tlsDataParams.ssock;

    timeout = left_ms(timer);
    if (timeout == 0) {
        /* sock timeout of 0 == block forever; just read + return if expired */
        timeout = 1;
    }

    tv.tv_sec = 0;
    tv.tv_usec = timeout * 1000;

    skt = Ssock_getSocket(ssock);

    if (setsockopt(skt, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,
            sizeof(struct timeval)) == 0) {
        bytes = Ssock_recvall(ssock, pMsg, len, 0);
        if (bytes > 0) {
            *numbytes = (size_t)bytes;
            return (SUCCESS);
        }
        else if (bytes == Ssock_TIMEOUT) {
            /* nothing to read in the socket buffer */
            return (NETWORK_SSL_NOTHING_TO_READ);
        }
    }
    return (NETWORK_SSL_READ_ERROR);
}

IoT_Error_t iot_tls_disconnect(Network *pNetwork)
{
    /*
     * TLS and Ssock have no disconnect equivalent. AWS framework code always
     * calls disconnect()/destroy() back to back so OK to have this empty.
     */
    return (SUCCESS);
}

IoT_Error_t iot_tls_destroy(Network *pNetwork)
{
    int skt;
    Ssock_Handle ssock = NULL;

    if (pNetwork == NULL || pNetwork->tlsDataParams.ssock == NULL) {
        return (NULL_VALUE_ERROR);
    }

    TLS_delete((TLS_Handle *)&pNetwork->tlsDataParams.tlsH);
    ssock = pNetwork->tlsDataParams.ssock;
    skt = Ssock_getSocket(ssock);
    Ssock_delete(&ssock);
    close(skt);

    return (SUCCESS);
}
