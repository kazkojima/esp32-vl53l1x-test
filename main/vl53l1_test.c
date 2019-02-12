/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "soc/gpio_struct.h"

#include "vl53l1_api.h"

VL53L1_Dev_t dev;
VL53L1_DEV Dev = &dev;

#define NRANGE 2
struct rnpkt {
    uint16_t range[NRANGE];
};

/* Autonomous ranging loop*/
static void
AutonomousLowPowerRangingTest(void)
{
    static VL53L1_RangingMeasurementData_t RangingData;
    printf("Autonomous Ranging Test\n");
    int status = VL53L1_WaitDeviceBooted(Dev);
    status = VL53L1_DataInit(Dev);
    status = VL53L1_StaticInit(Dev);
    status = VL53L1_SetDistanceMode(Dev, VL53L1_DISTANCEMODE_LONG);
    status = VL53L1_SetMeasurementTimingBudgetMicroSeconds(Dev, 50000);
    status = VL53L1_SetInterMeasurementPeriodMilliSeconds(Dev, 100);
    VL53L1_UserRoi_t Roi0, Roi1;
    Roi0.TopLeftX = 0;
    Roi0.TopLeftY = 15;
    Roi0.BotRightX = 7;
    Roi0.BotRightY = 0;
    Roi1.TopLeftX = 8;
    Roi1.TopLeftY = 15;
    Roi1.BotRightX = 15;
    Roi1.BotRightY = 0;
    status = VL53L1_SetUserROI(Dev, &Roi0);
    status = VL53L1_StartMeasurement(Dev);

    int roi = 0;

    if(status) {
        printf("VL53L1_StartMeasurement failed \n");
        while(1);
    }	

    float left = 0, right = 0;
    if (0/*isInterrupt*/) {
    } else {
        do // polling mode
            {
                status = VL53L1_WaitMeasurementDataReady(Dev);
                if(!status) {
                    status = VL53L1_GetRangingMeasurementData(Dev, &RangingData);
                    if(status==0) {
#if 0
                        printf("%d,%d,%.2f,%.2f\n", RangingData.RangeStatus,
                               RangingData.RangeMilliMeter,
                               (RangingData.SignalRateRtnMegaCps/65536.0),
                               RangingData.AmbientRateRtnMegaCps/65336.0);
#else
                        if (roi & 1) {
                            left = RangingData.RangeMilliMeter;
                            printf("L %3.1f R %3.1f\n", right/10.0, left/10.0);
                        } else
                            right = RangingData.RangeMilliMeter;
#endif
                    }
                    if (++roi & 1) {
                        status = VL53L1_SetUserROI(Dev, &Roi1);
                    } else {
                        status = VL53L1_SetUserROI(Dev, &Roi0);
                    }
                    status = VL53L1_ClearInterruptAndStartMeasurement(Dev);
                }
            }
        while (1);
    }
    //  return status;
}

static int i2c_handle = CONFIG_I2C_NUM;

void
rn_task(void *pvParameters)
{
    // xshut high
    gpio_set_direction(CONFIG_XSHUT_IO, GPIO_MODE_OUTPUT);
    gpio_set_level(CONFIG_XSHUT_IO, 1);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    Dev->I2cHandle = &i2c_handle;
    Dev->I2cDevAddr = 0x52;

    uint8_t byteData;
    uint16_t wordData;

    VL53L1_RdByte(Dev, 0x010F, &byteData);
    printf("VL53L1X Model_ID: %02X\n\r", byteData);
    VL53L1_RdByte(Dev, 0x0110, &byteData);
    printf("VL53L1X Module_Type: %02X\n\r", byteData);
    VL53L1_RdWord(Dev, 0x010F, &wordData);
    printf("VL53L1X: %02X\n\r", wordData);

    AutonomousLowPowerRangingTest();

    while(1) {
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
