/*******************************************************************************
** File: example_app.h
**
** Purpose:
**   This is the main header file for the EXAMPLE application.
**
*******************************************************************************/
#ifndef _EXAMPLE_APP_H_
#define _EXAMPLE_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "example_device.h"
#include "example_events.h"
#include "example_platform_cfg.h"
#include "example_perfids.h"
#include "example_msg.h"
#include "example_msgids.h"
#include "example_version.h"
#include "hwlib.h"

/* TODO: This is specific to the example application, remove if using template generator */
#include "mgr_msg.h"
#include "mgr_msgids.h"

/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define EXAMPLE_PIPE_DEPTH 32

/*
** Enabled and Disabled Definitions
*/
#define EXAMPLE_DEVICE_DISABLED 0
#define EXAMPLE_DEVICE_ENABLED  1

/*
** EXAMPLE global data structure
** The cFE convention is to put all global app data in a single struct.
** This struct is defined in the `example_app.h` file with one global instance
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    EXAMPLE_Hk_tlm_t HkTelemetryPkt; /* EXAMPLE Housekeeping Telemetry Packet */

    /*
    ** Operational data  - not reported in housekeeping
    */
    CFE_MSG_Message_t *MsgPtr;    /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t    CmdPipe;   /* Pipe Id for HK command pipe */
    uint32             RunStatus; /* App run status for controlling the application state */

    /*
     ** Device data
     ** TODO: Make specific to your application
     */
    EXAMPLE_Device_tlm_t DevicePkt; /* Device specific data packet */

    /*
    ** Device protocol
    ** TODO: Make specific to your application
    */
    uart_info_t ExampleUart; /* Hardware protocol definition */

} EXAMPLE_AppData_t;

/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern EXAMPLE_AppData_t EXAMPLE_AppData; /* EXAMPLE App Data */

/*
**
** Local function prototypes.
**
** Note: Except for the entry point (EXAMPLE_AppMain), these
**       functions are not called from any other source module.
*/
void  EXAMPLE_AppMain(void);
int32 EXAMPLE_AppInit(void);
void  EXAMPLE_ProcessCommandPacket(void);
void  EXAMPLE_ProcessGroundCommand(void);
void  EXAMPLE_ProcessTelemetryRequest(void);
void  EXAMPLE_ReportHousekeeping(void);
void  EXAMPLE_ReportDeviceTelemetry(void);
void  EXAMPLE_ResetCounters(void);
void  EXAMPLE_Enable(void);
void  EXAMPLE_Disable(void);
void  EXAMPLE_Configure(void);
int32 EXAMPLE_VerifyCmdLength(CFE_MSG_Message_t *msg, uint16 expected_length);

/* TODO: This is specific to the example application, remove if using template generator */
void EXAMPLE_ProcessMgrHk(void);

#endif /* _EXAMPLE_APP_H_ */
