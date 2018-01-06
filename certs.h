/*
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== certflasher.c ========
 */
#ifndef CERTS_H_
#define CERTS_H_

/*
 * USER STEP: Copy the lines in the root CA certificate file between
 *
 *            -----BEGIN CERTIFICATE-----
 *            ...
 *            -----END CERTIFICATE-----
 *
 *            Paste the lines in string format for the following array:
 */

const char root_ca_pem[] = "\
MIIE0zCCA7ugAwIBAgIQGNrRniZ96LtKIVjNzGs7SjANBgkqhkiG9w0BAQUFADCB\
yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\
ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp\
U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW\
ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0\
aG9yaXR5IC0gRzUwHhcNMDYxMTA4MDAwMDAwWhcNMzYwNzE2MjM1OTU5WjCByjEL\
MAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZW\
ZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJpU2ln\
biwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJp\
U2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9y\
aXR5IC0gRzUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCvJAgIKXo1\
nmAMqudLO07cfLw8RRy7K+D+KQL5VwijZIUVJ/XxrcgxiV0i6CqqpkKzj/i5Vbex\
t0uz/o9+B1fs70PbZmIVYc9gDaTY3vjgw2IIPVQT60nKWVSFJuUrjxuf6/WhkcIz\
SdhDY2pSS9KP6HBRTdGJaXvHcPaz3BJ023tdS1bTlr8Vd6Gw9KIl8q8ckmcY5fQG\
BO+QueQA5N06tRn/Arr0PO7gi+s3i+z016zy9vA9r911kTMZHRxAy3QkGSGT2RT+\
rCpSx4/VBEnkjWNHiDxpg8v+R70rfk/Fla4OndTRQ8Bnc+MUCH7lP59zuDMKz10/\
NIeWiu5T6CUVAgMBAAGjgbIwga8wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8E\
BAMCAQYwbQYIKwYBBQUHAQwEYTBfoV2gWzBZMFcwVRYJaW1hZ2UvZ2lmMCEwHzAH\
BgUrDgMCGgQUj+XTGoasjY5rw8+AatRIGCx7GS4wJRYjaHR0cDovL2xvZ28udmVy\
aXNpZ24uY29tL3ZzbG9nby5naWYwHQYDVR0OBBYEFH/TZafC3ey78DAJ80M5+gKv\
MzEzMA0GCSqGSIb3DQEBBQUAA4IBAQCTJEowX2LP2BqYLz3q3JktvXf2pXkiOOzE\
p6B4Eq1iDkVwZMXnl2YtmAl+X6/WzChl8gGqCBpH3vn5fJJaCGkgDdk+bW48DW7Y\
5gaRQBi5+MHt39tBquCWIMnNZBU4gcmU7qKEKQsTb47bDN0lAtukixlE0kF6BWlK\
WE9gyn6CagsCqiUXObXbf+eEZSqVir2G3l6BFoMtEMze/aiCKm0oHw0LxOXnGiYZ\
4fQRbxC1lfznQgUy286dUV4otp6F01vvpX1FQHKOtw5rDgb7MzVIcbidJ4vEZV8N\
hnacRHr2lVz2XTIIM6RUthg/aFzyQkqFOFSDX9HoLPKsEdao7WNq";


/*
 * USER STEP: Copy the lines in the cert.pem certificate file between
 *
 *            -----BEGIN CERTIFICATE-----
 *            ...
 *            -----END CERTIFICATE-----
 *
 *            Paste the lines in string format for the following array:
 */
const char client_cert_pem[] = "\
MIIDWjCCAkKgAwIBAgIVAKJlIcC1dr3hJyvxe+GemkbQ5mRXMA0GCSqGSIb3DQEB\
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0xNzExMTcyMDA5\
MzFaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCTcV9dGV6fgPdiYCgQ\
HRVKdDxc1Sok3ll2HKCmAL7sPGsdk+WvYi40QLa3u1vOvwZAyfqhgsZfm5Dp1dsM\
8JP7aEqwcviVNAjzXP/arT4KzfuOpzrM5DPP2Oa3vqMU7N7IE0T15xIsTNa/E6wn\
T0uOqS8fG7Jh49Kpnzu1gPKT/EZKvYaYYA4n2fpKSDRW9kRNE5UT92OSCApMNbiG\
kzqouX4U42px3x7Y3Rin/ZRvfIyBzNesukgMhMUSIo/C4ZCRgKqijSP6erMts7b4\
axFE0AOcO2X+WhXtXU0g9wZJRJe+x8WVaEgVsLIHbf02j2kvppQEFW4dLppKglEW\
HYkpAgMBAAGjYDBeMB8GA1UdIwQYMBaAFMClEMuFq3HXwc2BXSv/lxgcg29CMB0G\
A1UdDgQWBBT8ba5TblftT3aiEwNjVWln60o7iDAMBgNVHRMBAf8EAjAAMA4GA1Ud\
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAEmz2Z7ZtLH4TAKusyCXxXzsG\
kEJXJQqvs7VcAyZIzZ8yALqEoxaewf6aR6u+q3wmAgHIjNU9HQ/T9KU/4ekSGnO/\
MiJRno8EqAhbvMWBcmSqH3WHucsrhkV6weFpqb8QRpA1JNmAltsv0zDvk19YF0cl\
ri2YPAAnf577c5pO7yWo6j3TE6NS75Gl3ybhB3J9bpQdnx+Fds3UBNMAeBFjvjih\
vCSpVVBz5p+kCnIAhN/Avphbg/TRW6QRzXBFu1tl5ts2F2OSGLNLULx0glALtfIy\
2M4VnYoRa4Q+19aXFqkMlmV9gRmeW1rl+SkNxjG3iixp0Qmsq18sPpuSHhzp9g==";

/*
 * USER STEP: Copy the lines in the privateKey.pem certificate file between
 *
 *            -----BEGIN CERTIFICATE-----
 *            ...
 *            -----END CERTIFICATE-----
 *
 *            Paste the lines in string format for the following array:
 */
const char client_private_key_pem[] = "\
MIIEogIBAAKCAQEAk3FfXRlen4D3YmAoEB0VSnQ8XNUqJN5ZdhygpgC+7DxrHZPl\
r2IuNEC2t7tbzr8GQMn6oYLGX5uQ6dXbDPCT+2hKsHL4lTQI81z/2q0+Cs37jqc6\
zOQzz9jmt76jFOzeyBNE9ecSLEzWvxOsJ09LjqkvHxuyYePSqZ87tYDyk/xGSr2G\
mGAOJ9n6Skg0VvZETROVE/djkggKTDW4hpM6qLl+FONqcd8e2N0Yp/2Ub3yMgczX\
rLpIDITFEiKPwuGQkYCqoo0j+nqzLbO2+GsRRNADnDtl/loV7V1NIPcGSUSXvsfF\
lWhIFbCyB239No9pL6aUBBVuHS6aSoJRFh2JKQIDAQABAoIBAHb6lL7W2xGRi4+a\
Epo2LdkgBRnQdiq8NqrmHQj4LxkJBFMg5sbpTmmiExAeo84pPg2IL7SniBbcvUrE\
5PgDJxdIKIVMsaYNEG/6CwjWpesoHygVcJCoUdsWp898ibM4FeqUCiD47yRKeTxF\
zrUF+u3LPpg2zHLL2lJAiXlE7ftV6pAfXPH+xR6IsWzuIxqnwk2R0USIc7UBz9eK\
CRY9yflm7HCdpNzvXnO8KTDLrnGJMVrV46osx0/Wj+844iNvR8kpLCW0anUi4kRP\
sY+AvpxgryuT49zmnSNTRD46NdEt5ZfWbkoO2VDRrNCEsq0I4aYwcBMOs6pR+Q1+\
CGIZ3+UCgYEA3sYkNu2I6usbEorZqijeABk8htNiS2xT9wGsbooB712qM7jjb80q\
8Udrj9ALCph2ZPlUp8P9ijgw+ciACPnGTuI1mZ2VBMhpZ+tiTEk8n0yOGS5hXDKv\
bDHTZCG4HIR6YIgc2egrSwzaIYUIoDYPUeUIh+1rd8aqAw8EYGwypB8CgYEAqW74\
lJ3OR1P45tvh9ggcTIWlrTMVQ9eMRWU89nk/9vA5gcSmgtaZvbkztGox7Rep3zt8\
nSxEgsoSDIWXsjW6xVW8XIPzztoYk8gHH1aFzfslXNjnQE/9UBQSsNoxYhoBv8uC\
VylzITcopSE4rsln3qP4pjCHYHmQ+LATI/g26bcCgYBqqQ+ZxOzzHRHfNN+2Ws6/\
2VVjcD31wq4I66I1Ypo+kuf0z6cFVoQi9YEIYCGJNyooekAP+wSfcvvv/BFhOsW5\
6xCithItMg9VlBuCTuZZlvvKDqzKyfG3qbt+i65s6Pj20vSNWF2yFzsxJVLV8kwv\
d6s3+XKk7FdmE4CS8frlHQKBgHny91PwaQIJvIJdDLwKhmPFZcPNmqajw8BG56P3\
DUTtDlcKdVaCo1FctYxLSvf9KjUUGdL+g6knS+pbVR3dcjqKRPLJvnfJnhrvw8CY\
g+IHu5KiQ3+UkxxJ9lH4GcckNNIY5g1aNhVYIBcKXEPhjrM+jb7/+tBDb0avZHdl\
NhDtAoGAf6RDoI/gTp/6BQmT1I8JKaqe46ClrhjOJcGBDJqdd+T/pBtBm3xuGAoH\
emvS2hlBwZcOoTIT3w7tJtOK9tRqL8OoDjinLNEJqYyRLmYMbbGQ31w5Unu6MpEQ\
Qobr+7o+ZRtilNakHs1QnUMRsdrUNXLo1p2ohXcgMcdKXwz8Gt4=";

#endif /* CERTS_H_ */
