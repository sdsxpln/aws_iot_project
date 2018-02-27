#ifndef __PROCESSPARAMS_H_
#define __PROCESSPARAMS_H_

#ifdef __cplusplus
extern "C" {
#endif

// so ... need to wrap in semaphores?
// new i2C stuff
typedef struct
{
    I2C_Handle i2cHandle;
    PWM_Handle pwmHandle;
    int16_t pwmDuty;
    int8_t xVal, yVal, zVal;
    float temperatureVal;
    uint8_t temperatureDesired;
    float fTemperatureDesired;
} ProcessParams;


#ifdef __cplusplus
}
#endif

#endif
