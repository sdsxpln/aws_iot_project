/*
 * Copyright 2015-2016 Texas Instruments Incorporated. All Rights Reserved.
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

#ifndef PLATFORM_TIRTOS_NETWORK_PLATFORM_H_
#define PLATFORM_TIRTOS_NETWORK_PLATFORM_H_

#include <ti/net/tls.h>
#include <ti/net/ssock.h>

/**
 * @brief TLS Connection Parameters
 *
 * Defines a type containing TLS specific parameters to be passed down to the
 * TLS networking layer to create a TLS secured socket.
 */
typedef struct TLSDataParams {
    Ssock_Handle ssock;
    TLS_Handle tlsH;
} TLSDataParams;


#endif
