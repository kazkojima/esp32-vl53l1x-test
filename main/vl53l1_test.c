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

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#include "vl53l1_api.h"

VL53L1_Dev_t dev;
VL53L1_DEV Dev = &dev;

#define NSLIDE 7
#define SSLIDE 2
#define NRANGE (NSLIDE * NSLIDE)
#define IDXSLIDE(i,j) (NSLIDE * (j) + (i))

extern int sockfd;

struct rnpkt {
    uint16_t range[NRANGE];
} pkt;

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
    VL53L1_UserRoi_t Roi;
    Roi.TopLeftX = 0;
    Roi.TopLeftY = 3;
    Roi.BotRightX = 3;
    Roi.BotRightY = 0;
    status = VL53L1_SetUserROI(Dev, &Roi);
    status = VL53L1_StartMeasurement(Dev);

    int ix = 0, iy = 0;

    if(status) {
        printf("VL53L1_StartMeasurement failed \n");
        while(1);
    }	

    float val;
    if (0/*isInterrupt*/) {
    } else {
        do // polling mode
            {
                status = VL53L1_WaitMeasurementDataReady(Dev);
                if(!status) {
                    status = VL53L1_GetRangingMeasurementData(Dev, &RangingData);
                    if(status==0) {
                        val = RangingData.RangeMilliMeter;
                        printf("(%d, %d) %3.1f\n", ix, iy, val/10.0);
                        pkt.range[IDXSLIDE(ix/2, iy/2)] = val;
                    }
                    // Set the next ROI
                    ix = (ix + SSLIDE) % (NSLIDE * SSLIDE);
                    if (ix == 0)
                        iy = (iy + SSLIDE) % (NSLIDE * SSLIDE);
                    if (ix == 0 && iy == 0) {
                        int n = send(sockfd, &pkt, sizeof(pkt), 0);
                        if (n < 0) {
                        }
                    }
                    Roi.TopLeftX = ix;
                    Roi.TopLeftY = ix + 3;
                    Roi.BotRightX = iy + 3;
                    Roi.BotRightY = iy;
                    status = VL53L1_SetUserROI(Dev, &Roi);
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

    while (sockfd < 0) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    AutonomousLowPowerRangingTest();

    while(1) {
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
