/*******************************************************************************
** File:
**   example_msg.h
**
** Purpose:
**  Define EXAMPLE application commands and telemetry messages
**
*******************************************************************************/
#ifndef _EXAMPLE_MSG_H_
#define _EXAMPLE_MSG_H_

#include "cfe.h"
#include "example_device.h"

/*
** Ground Command Codes
** TODO: Add additional commands required by the specific component
*/
#define EXAMPLE_NOOP_CC           0
#define EXAMPLE_RESET_COUNTERS_CC 1
#define EXAMPLE_ENABLE_CC         2
#define EXAMPLE_DISABLE_CC        3
#define EXAMPLE_CONFIG_CC         4

/*
** Telemetry Request Command Codes
** TODO: Add additional commands required by the specific component
*/
#define EXAMPLE_REQ_HK_TLM   0
#define EXAMPLE_REQ_DATA_TLM 1

/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    CFE_MSG_CommandHeader_t CmdHeader;

} EXAMPLE_NoArgs_cmd_t;

/*
** EXAMPLE write configuration command
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint32                  DeviceCfg;

} EXAMPLE_Config_cmd_t;

/*
** EXAMPLE device telemetry definition
*/
typedef struct
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    EXAMPLE_Device_Data_tlm_t  Example;

    /* TODO: This is specific to the example application, remove if using template generator */
    uint16 PassNumber;
    uint8  RegionStatus;

} __attribute__((packed)) EXAMPLE_Device_tlm_t;
#define EXAMPLE_DEVICE_TLM_LNGTH sizeof(EXAMPLE_Device_tlm_t)

/*
** EXAMPLE housekeeping type definition
*/
typedef struct
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    uint8                     CommandErrorCount;
    uint8                     CommandCount;
    uint8                     DeviceErrorCount;
    uint8                     DeviceCount;

    /*
    ** TODO: Edit and add specific telemetry values to this struct
    */
    uint8                  DeviceEnabled;
    EXAMPLE_Device_HK_tlm_t DeviceHK;

} __attribute__((packed)) EXAMPLE_Hk_tlm_t;
#define EXAMPLE_HK_TLM_LNGTH sizeof(EXAMPLE_Hk_tlm_t)

#endif /* _EXAMPLE_MSG_H_ */
