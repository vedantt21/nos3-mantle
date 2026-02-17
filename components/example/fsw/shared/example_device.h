/*******************************************************************************
** File: example_device.h
**
** Purpose:
**   This is the header file for the EXAMPLE device.
**
*******************************************************************************/
#ifndef _EXAMPLE_DEVICE_H_
#define _EXAMPLE_DEVICE_H_

/*
** Required header files.
*/
#include "device_cfg.h"
#include "hwlib.h"

#ifndef EXAMPLE_CFG
#include "example_platform_cfg.h"
#endif

/*
** Type definitions
** TODO: Make specific to your application
*/
#define EXAMPLE_DEVICE_HDR   0xDEAD
#define EXAMPLE_DEVICE_HDR_0 0xDE
#define EXAMPLE_DEVICE_HDR_1 0xAD

#define EXAMPLE_DEVICE_NOOP_CMD     0x00
#define EXAMPLE_DEVICE_REQ_HK_CMD   0x01
#define EXAMPLE_DEVICE_REQ_DATA_CMD 0x02
#define EXAMPLE_DEVICE_CFG_CMD      0x03

#define EXAMPLE_DEVICE_TRAILER   0xBEEF
#define EXAMPLE_DEVICE_TRAILER_0 0xBE
#define EXAMPLE_DEVICE_TRAILER_1 0xEF

#define EXAMPLE_DEVICE_HDR_TRL_LEN 4
#define EXAMPLE_DEVICE_CMD_SIZE    9

/*
** EXAMPLE device housekeeping telemetry definition
*/
typedef struct
{
    uint32_t DeviceCounter;
    uint32_t DeviceConfig;
    uint32_t DeviceStatus;

} __attribute__((packed)) EXAMPLE_Device_HK_tlm_t;
#define EXAMPLE_DEVICE_HK_LNGTH sizeof(EXAMPLE_Device_HK_tlm_t)
#define EXAMPLE_DEVICE_HK_SIZE  EXAMPLE_DEVICE_HK_LNGTH + EXAMPLE_DEVICE_HDR_TRL_LEN

/*
** EXAMPLE device data telemetry definition
*/
typedef struct
{
    uint32_t DeviceCounter;
    uint16_t DeviceDataX;
    uint16_t DeviceDataY;
    uint16_t DeviceDataZ;

} __attribute__((packed)) EXAMPLE_Device_Data_tlm_t;
#define EXAMPLE_DEVICE_DATA_LNGTH sizeof(EXAMPLE_Device_Data_tlm_t)
#define EXAMPLE_DEVICE_DATA_SIZE  EXAMPLE_DEVICE_DATA_LNGTH + EXAMPLE_DEVICE_HDR_TRL_LEN

/*
** Prototypes
*/
int32_t EXAMPLE_ReadData(uart_info_t *device, uint8_t *read_data, uint8_t data_length);
int32_t EXAMPLE_CommandDevice(uart_info_t *device, uint8_t cmd, uint32_t payload);
int32_t EXAMPLE_RequestHK(uart_info_t *device, EXAMPLE_Device_HK_tlm_t *data);
int32_t EXAMPLE_RequestData(uart_info_t *device, EXAMPLE_Device_Data_tlm_t *data);

#endif /* _EXAMPLE_DEVICE_H_ */
