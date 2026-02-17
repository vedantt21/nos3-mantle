/*******************************************************************************
** File: example_app.c
**
** Purpose:
**   This file contains the source code for the EXAMPLE application.
**
*******************************************************************************/

/*
** Include Files
*/
#include <arpa/inet.h>
#include "example_app.h"

/*
** Global Data
*/
EXAMPLE_AppData_t EXAMPLE_AppData;

/*
** Application entry point and main process loop
*/
void EXAMPLE_AppMain(void)
{
    int32 status = OS_SUCCESS;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(EXAMPLE_PERF_ID);

    /*
    ** Perform application initialization
    */
    status = EXAMPLE_AppInit();
    if (status != CFE_SUCCESS)
    {
        EXAMPLE_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Main loop
    */
    while (CFE_ES_RunLoop(&EXAMPLE_AppData.RunStatus) == true)
    {
        /*
        ** Performance log exit stamp
        */
        CFE_ES_PerfLogExit(EXAMPLE_PERF_ID);

        /*
        ** Pend on the arrival of the next Software Bus message
        ** Note that this is the standard, but timeouts are available
        */
        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&EXAMPLE_AppData.MsgPtr, EXAMPLE_AppData.CmdPipe,
                                      CFE_SB_PEND_FOREVER);

        /*
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution.
        */
        CFE_ES_PerfLogEntry(EXAMPLE_PERF_ID);

        /*
        ** If the CFE_SB_ReceiveBuffer was successful, then continue to process the command packet
        ** If not, then exit the application in error.
        ** Note that a SB read error should not always result in an app quitting.
        */
        if (status == CFE_SUCCESS)
        {
            EXAMPLE_ProcessCommandPacket();
        }
        else
        {
            CFE_EVS_SendEvent(EXAMPLE_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "EXAMPLE: SB Pipe Read Error = %d",
                              (int)status);
            EXAMPLE_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Disable component, which cleans up the interface, upon exit
    */
    EXAMPLE_Disable();

    /*
    ** Performance log exit stamp
    */
    CFE_ES_PerfLogExit(EXAMPLE_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(EXAMPLE_AppData.RunStatus);
}

/*
** Initialize application
*/
int32 EXAMPLE_AppInit(void)
{
    int32 status = OS_SUCCESS;

    EXAMPLE_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY); /* as default, no filters are used */
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("EXAMPLE: Error registering for event services: 0x%08X\n", (unsigned int)status);
        return status;
    }

    /*
    ** Create the Software Bus command pipe
    */
    status = CFE_SB_CreatePipe(&EXAMPLE_AppData.CmdPipe, EXAMPLE_PIPE_DEPTH, "EXAMPLE_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(EXAMPLE_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "Error Creating SB Pipe,RC=0x%08X",
                          (unsigned int)status);
        return status;
    }

    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(EXAMPLE_CMD_MID), EXAMPLE_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(EXAMPLE_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X", EXAMPLE_CMD_MID,
                          (unsigned int)status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(EXAMPLE_REQ_HK_MID), EXAMPLE_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(EXAMPLE_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X", EXAMPLE_REQ_HK_MID,
                          (unsigned int)status);
        return status;
    }

    /*
    ** Subscribe to MGR HK for Science Pass Information
    ** TODO: This is specific to the example application, remove if using template generator
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(MGR_HK_TLM_MID), EXAMPLE_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(EXAMPLE_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X", MGR_HK_TLM_MID,
                          (unsigned int)status);
        return status;
    }

    /*
    ** TODO: Subscribe to any other messages here
    */

    /*
    ** Initialize the published HK message - this HK message will contain the
    ** telemetry that has been defined in the EXAMPLE_HkTelemetryPkt for this app.
    */
    CFE_MSG_Init(CFE_MSG_PTR(EXAMPLE_AppData.HkTelemetryPkt.TlmHeader), CFE_SB_ValueToMsgId(EXAMPLE_HK_TLM_MID),
                 EXAMPLE_HK_TLM_LNGTH);

    /*
    ** Initialize the device packet message
    ** This packet is specific to your application
    */
    CFE_MSG_Init(CFE_MSG_PTR(EXAMPLE_AppData.DevicePkt.TlmHeader), CFE_SB_ValueToMsgId(EXAMPLE_DEVICE_TLM_MID),
                 EXAMPLE_DEVICE_TLM_LNGTH);

    /*
    ** TODO: Initialize any other messages that this app will publish
    */

    /*
    ** Always reset all counters during application initialization
    */
    EXAMPLE_ResetCounters();

    /*
    ** Initialize application data
    ** Note that counters are excluded as they were reset in the previous code block
    */
    EXAMPLE_AppData.HkTelemetryPkt.DeviceEnabled          = EXAMPLE_DEVICE_DISABLED;
    EXAMPLE_AppData.HkTelemetryPkt.DeviceHK.DeviceCounter = 0;
    EXAMPLE_AppData.HkTelemetryPkt.DeviceHK.DeviceConfig  = 0;
    EXAMPLE_AppData.HkTelemetryPkt.DeviceHK.DeviceStatus  = 0;

    /*
     ** Send an information event that the app has initialized.
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(EXAMPLE_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
                               "EXAMPLE App Initialized. Version %d.%d.%d.%d", EXAMPLE_MAJOR_VERSION,
                               EXAMPLE_MINOR_VERSION, EXAMPLE_REVISION, EXAMPLE_MISSION_REV);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("EXAMPLE: Error sending initialization event: 0x%08X\n", (unsigned int)status);
    }
    return status;
}

/*
** Process packets received on the EXAMPLE command pipe
*/
void EXAMPLE_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_GetMsgId(EXAMPLE_AppData.MsgPtr, &MsgId);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        /*
        ** Ground Commands with command codes fall under the EXAMPLE_CMD_MID (Message ID)
        */
        case EXAMPLE_CMD_MID:
            EXAMPLE_ProcessGroundCommand();
            break;

        /*
        ** Housekeeping requests with command codes fall under the EXAMPLE_REQ_HK_MID (Message ID)
        */
        case EXAMPLE_REQ_HK_MID:
            EXAMPLE_ProcessTelemetryRequest();
            break;

        /*
        ** Update science pass information
        ** TODO: This is specific to the example application, remove if using template generator
        */
        case MGR_HK_TLM_MID:
            EXAMPLE_ProcessMgrHk();
            break;

        /*
        ** TODO: Add additional message IDs as needed
        */

        /*
        ** All other invalid messages that this app doesn't recognize,
        ** increment the command error counter and log as an error event.
        */
        default:
            /* Increment the command error counter upon receipt of an invalid command packet */
            EXAMPLE_AppData.HkTelemetryPkt.CommandErrorCount++;

            /* Send event failure to the console*/
            CFE_EVS_SendEvent(EXAMPLE_PROCESS_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
                              "EXAMPLE: Invalid command packet, MID = 0x%x", CFE_SB_MsgIdToValue(MsgId));
            break;
    }
    return;
}

/*
** Process ground commands
** TODO: Add additional commands required by the specific component
*/
void EXAMPLE_ProcessGroundCommand(void)
{
    CFE_SB_MsgId_t    MsgId       = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case
    */
    CFE_MSG_GetMsgId(EXAMPLE_AppData.MsgPtr, &MsgId);

    /*
    ** Ground Commands have a command code (_CC) associated with them
    ** Pull this command code from the message and then process
    */
    CFE_MSG_GetFcnCode(EXAMPLE_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case EXAMPLE_NOOP_CC:
            /*
            ** Verify the command length immediately after CC identification
            */
            if (EXAMPLE_VerifyCmdLength(EXAMPLE_AppData.MsgPtr, sizeof(EXAMPLE_NoArgs_cmd_t)) == OS_SUCCESS)
            {
#ifdef EXAMPLE_CFG_DEBUG
                OS_printf("EXAMPLE: EXAMPLE_NOOP_CC received \n");
#endif

                /* Do any necessary checks, none for a NOOP */

                /* Increment command success or error counter, NOOP can only be successful */
                EXAMPLE_AppData.HkTelemetryPkt.CommandCount++;

                /* Do the action, none for a NOOP */

                /* Increment device success or error counter, none for NOOP as application only */

                /* Send event success or failure to the console, NOOP can only be successful */
                CFE_EVS_SendEvent(EXAMPLE_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "EXAMPLE: NOOP command received");
            }
            break;

        /*
        ** Reset Counters Command
        */
        case EXAMPLE_RESET_COUNTERS_CC:
            if (EXAMPLE_VerifyCmdLength(EXAMPLE_AppData.MsgPtr, sizeof(EXAMPLE_NoArgs_cmd_t)) == OS_SUCCESS)
            {
#ifdef EXAMPLE_CFG_DEBUG
                OS_printf("EXAMPLE: EXAMPLE_RESET_COUNTERS_CC received \n");
#endif
                EXAMPLE_ResetCounters();
            }
            break;

        /*
        ** Enable Command
        */
        case EXAMPLE_ENABLE_CC:
            if (EXAMPLE_VerifyCmdLength(EXAMPLE_AppData.MsgPtr, sizeof(EXAMPLE_NoArgs_cmd_t)) == OS_SUCCESS)
            {
#ifdef EXAMPLE_CFG_DEBUG
                OS_printf("EXAMPLE: EXAMPLE_ENABLE_CC received \n");
#endif
                EXAMPLE_Enable();
            }
            break;

        /*
        ** Disable Command
        */
        case EXAMPLE_DISABLE_CC:
            if (EXAMPLE_VerifyCmdLength(EXAMPLE_AppData.MsgPtr, sizeof(EXAMPLE_NoArgs_cmd_t)) == OS_SUCCESS)
            {
#ifdef EXAMPLE_CFG_DEBUG
                OS_printf("EXAMPLE: EXAMPLE_DISABLE_CC received \n");
#endif
                EXAMPLE_Disable();
            }
            break;

        /*
        ** Set Configuration Command
        ** Note that this is an example of a command that has additional arguments
        */
        case EXAMPLE_CONFIG_CC:
            if (EXAMPLE_VerifyCmdLength(EXAMPLE_AppData.MsgPtr, sizeof(EXAMPLE_Config_cmd_t)) == OS_SUCCESS)
            {
#ifdef EXAMPLE_CFG_DEBUG
                OS_printf("EXAMPLE: EXAMPLE_CONFIG_CC received \n");
#endif
                EXAMPLE_Configure();
            }
            break;

        /*
        ** TODO: Edit and add more command codes as appropriate for the application
        */

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the command error counter upon receipt of an invalid command */
            EXAMPLE_AppData.HkTelemetryPkt.CommandErrorCount++;

            /* Send invalid command code failure to the console */
            CFE_EVS_SendEvent(EXAMPLE_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
                              "EXAMPLE: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x",
                              CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}

/*
** Process Telemetry Request - Triggered in response to a telemetry request
*/
void EXAMPLE_ProcessTelemetryRequest(void)
{
    CFE_SB_MsgId_t    MsgId       = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /* MsgId is only needed if the command code is not recognized. See default case */
    CFE_MSG_GetMsgId(EXAMPLE_AppData.MsgPtr, &MsgId);

    /* Pull this command code from the message and then process */
    CFE_MSG_GetFcnCode(EXAMPLE_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        case EXAMPLE_REQ_HK_TLM:
            EXAMPLE_ReportHousekeeping();
            break;

        case EXAMPLE_REQ_DATA_TLM:
            EXAMPLE_ReportDeviceTelemetry();
            break;

        /*
        ** TODO: Edit, add, or remove telemetry request codes appropriate for the application
        */

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            EXAMPLE_AppData.HkTelemetryPkt.CommandErrorCount++;

            /* Send invalid command code failure to the console */
            CFE_EVS_SendEvent(EXAMPLE_DEVICE_TLM_ERR_EID, CFE_EVS_EventType_ERROR,
                              "EXAMPLE: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x",
                              CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}

/*
** Report Application Housekeeping
*/
void EXAMPLE_ReportHousekeeping(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (EXAMPLE_AppData.HkTelemetryPkt.DeviceEnabled == EXAMPLE_DEVICE_ENABLED)
    {
        status = EXAMPLE_RequestHK(&EXAMPLE_AppData.ExampleUart,
                                  (EXAMPLE_Device_HK_tlm_t *)&EXAMPLE_AppData.HkTelemetryPkt.DeviceHK);
        if (status == OS_SUCCESS)
        {
            EXAMPLE_AppData.HkTelemetryPkt.DeviceCount++;
        }
        else
        {
            EXAMPLE_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(EXAMPLE_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
                              "EXAMPLE: Request device HK reported error %d", status);
        }
    }
    /* Intentionally do not report errors if disabled */

    /* Time stamp and publish housekeeping telemetry */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *)&EXAMPLE_AppData.HkTelemetryPkt);
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *)&EXAMPLE_AppData.HkTelemetryPkt, true);
    return;
}

/*
** Collect and Report Device Telemetry
*/
void EXAMPLE_ReportDeviceTelemetry(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (EXAMPLE_AppData.HkTelemetryPkt.DeviceEnabled == EXAMPLE_DEVICE_ENABLED)
    {
        status = EXAMPLE_RequestData(&EXAMPLE_AppData.ExampleUart,
                                    (EXAMPLE_Device_Data_tlm_t *)&EXAMPLE_AppData.DevicePkt.Example);
        if (status == OS_SUCCESS)
        {
            /* Update packet count */
            EXAMPLE_AppData.HkTelemetryPkt.DeviceCount++;

            /* Time stamp and publish data telemetry */
            CFE_SB_TimeStampMsg((CFE_MSG_Message_t *)&EXAMPLE_AppData.DevicePkt);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *)&EXAMPLE_AppData.DevicePkt, true);
        }
        else
        {
            EXAMPLE_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(EXAMPLE_REQ_DATA_ERR_EID, CFE_EVS_EventType_ERROR,
                              "EXAMPLE: Request device data reported error %d", status);
        }

        /* Check device status and act on error */
        if (EXAMPLE_AppData.HkTelemetryPkt.DeviceHK.DeviceStatus != 0)
        {
            /* Any bit is an error, halting communication until device power cycled */
            EXAMPLE_Disable();

            /* Send device status error to the console */
            CFE_EVS_SendEvent(EXAMPLE_REQ_DATA_STATUS_ERR_EID, CFE_EVS_EventType_ERROR,
                              "EXAMPLE: Request device data reported status error %d",
                              EXAMPLE_AppData.HkTelemetryPkt.DeviceHK.DeviceStatus);
        }
    }
    /* Intentionally do not report errors if device disabled */
    return;
}

/*
** Ingest science MGR data and save it
** TODO: This is specific to the example application, remove if using template generator
*/
void EXAMPLE_ProcessMgrHk(void)
{
    MGR_Hk_tlm_t *pMsg = (MGR_Hk_tlm_t *)EXAMPLE_AppData.MsgPtr;

    EXAMPLE_AppData.DevicePkt.PassNumber   = pMsg->SciPassCount;
    EXAMPLE_AppData.DevicePkt.RegionStatus = pMsg->ScienceStatus;
    return;
}

/*
** Reset all global counter variables
*/
void EXAMPLE_ResetCounters(void)
{
    /* Do any necessary checks, none for reset counters */

    /* Increment command success or error counter, omitted as action is to reset */

    /* Do the action, clear all global counter variables */
    EXAMPLE_AppData.HkTelemetryPkt.CommandErrorCount = 0;
    EXAMPLE_AppData.HkTelemetryPkt.CommandCount      = 0;
    EXAMPLE_AppData.HkTelemetryPkt.DeviceErrorCount  = 0;
    EXAMPLE_AppData.HkTelemetryPkt.DeviceCount       = 0;

    /* Increment device success or error counter, none as application only */

    /* Send event success to the console */
    CFE_EVS_SendEvent(EXAMPLE_CMD_RESET_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "EXAMPLE: RESET counters command received");
    return;
}

/*
** Enable Component
** TODO: Edit for your specific component implementation
*/
void EXAMPLE_Enable(void)
{
    int32 status = OS_SUCCESS;

    /* Do any necessary checks, confirm that device is currently disabled */
    if (EXAMPLE_AppData.HkTelemetryPkt.DeviceEnabled == EXAMPLE_DEVICE_DISABLED)
    {
        /* Increment command success counter */
        EXAMPLE_AppData.HkTelemetryPkt.CommandCount++;

        /*
        ** Do the action, initialize hardware interface and set enabled
        ** TODO: Make specific to your application depending on protocol in use
        ** Note that other components provide examples for the different protocols
        */
        EXAMPLE_AppData.ExampleUart.deviceString  = EXAMPLE_CFG_STRING;
        EXAMPLE_AppData.ExampleUart.handle        = EXAMPLE_CFG_HANDLE;
        EXAMPLE_AppData.ExampleUart.isOpen        = PORT_CLOSED;
        EXAMPLE_AppData.ExampleUart.baud          = EXAMPLE_CFG_BAUDRATE_HZ;
        EXAMPLE_AppData.ExampleUart.access_option = uart_access_flag_RDWR;

        status = uart_init_port(&EXAMPLE_AppData.ExampleUart);
        if (status == OS_SUCCESS)
        {
            EXAMPLE_AppData.HkTelemetryPkt.DeviceEnabled = EXAMPLE_DEVICE_ENABLED;

            /* Increment device success counter */
            EXAMPLE_AppData.HkTelemetryPkt.DeviceCount++;

            /* Send device event success to the console */
            CFE_EVS_SendEvent(EXAMPLE_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "EXAMPLE: Device enabled successfully");
        }
        else
        {
            /* Increment device error counter */
            EXAMPLE_AppData.HkTelemetryPkt.DeviceErrorCount++;

            /* Send device event failure to the console */
            CFE_EVS_SendEvent(EXAMPLE_UART_INIT_ERR_EID, CFE_EVS_EventType_ERROR,
                              "EXAMPLE: Device UART port initialization error %d", status);
        }
    }
    else
    {
        /* Increment command error count */
        EXAMPLE_AppData.HkTelemetryPkt.CommandErrorCount++;

        /* Send command event failure to the console */
        CFE_EVS_SendEvent(EXAMPLE_ENABLE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "EXAMPLE: Device enable failed, already enabled");
    }
    return;
}

/*
** Disable Component
** TODO: Edit for your specific component implementation
*/
void EXAMPLE_Disable(void)
{
    int32 status = OS_SUCCESS;

    /* Do any necessary checks, confirm that device is currently enabled */
    if (EXAMPLE_AppData.HkTelemetryPkt.DeviceEnabled == EXAMPLE_DEVICE_ENABLED)
    {
        /* Increment command success counter */
        EXAMPLE_AppData.HkTelemetryPkt.CommandCount++;

        /*
        ** Do the action, close hardware interface and set disabled
        ** TODO: Make specific to your application depending on protocol in use
        ** Note that other components provide examples for the different protocols
        */
        status = uart_close_port(&EXAMPLE_AppData.ExampleUart);
        if (status == OS_SUCCESS)
        {
            EXAMPLE_AppData.HkTelemetryPkt.DeviceEnabled = EXAMPLE_DEVICE_DISABLED;

            /* Increment device success counter */
            EXAMPLE_AppData.HkTelemetryPkt.DeviceCount++;

            /* Send device event success to the console */
            CFE_EVS_SendEvent(EXAMPLE_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "EXAMPLE: Device disabled successfully");
        }
        else
        {
            /* Increment device error counter */
            EXAMPLE_AppData.HkTelemetryPkt.DeviceErrorCount++;

            /* Send device event failure to the console */
            CFE_EVS_SendEvent(EXAMPLE_UART_CLOSE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "EXAMPLE: Device UART port close error %d", status);
        }
    }
    else
    {
        /* Increment command error count */
        EXAMPLE_AppData.HkTelemetryPkt.CommandErrorCount++;

        /* Send command event failure to the console */
        CFE_EVS_SendEvent(EXAMPLE_DISABLE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "EXAMPLE: Device disable failed, already disabled");
    }
    return;
}

/*
** Configure Component
** TODO: Edit for your specific component implementation
*/
void EXAMPLE_Configure(void)
{
    int32                status        = OS_SUCCESS;
    int32                device_status = OS_SUCCESS;
    EXAMPLE_Config_cmd_t *config_cmd    = (EXAMPLE_Config_cmd_t *)EXAMPLE_AppData.MsgPtr;

    /* Do any necessary checks, confirm that device is currently enabled */
    if (EXAMPLE_AppData.HkTelemetryPkt.DeviceEnabled != EXAMPLE_DEVICE_ENABLED)
    {
        status = OS_ERROR;
        /* Increment command error count */
        EXAMPLE_AppData.HkTelemetryPkt.CommandErrorCount++;

        /* Send event logging failure of check to the console */
        CFE_EVS_SendEvent(EXAMPLE_CMD_CONFIG_EN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "EXAMPLE: Configuration command invalid when device disabled");
    }

    /* Do any necessary checks, confirm valid configuration value */
    if (config_cmd->DeviceCfg == 0xFFFFFFFF) // 4294967295
    {
        status = OS_ERROR;
        /* Increment command error count */
        EXAMPLE_AppData.HkTelemetryPkt.CommandErrorCount++;

        /* Send event logging failure of check to the console */
        CFE_EVS_SendEvent(EXAMPLE_CMD_CONFIG_VAL_ERR_EID, CFE_EVS_EventType_ERROR,
                          "EXAMPLE: Configuration command with value %u is invalid", config_cmd->DeviceCfg);
    }

    if (status == OS_SUCCESS)
    {
        /* Increment command success counter */
        EXAMPLE_AppData.HkTelemetryPkt.CommandCount++;

        /* Do the action, command device to with a new configuration */
        device_status = EXAMPLE_CommandDevice(&EXAMPLE_AppData.ExampleUart, EXAMPLE_DEVICE_CFG_CMD, config_cmd->DeviceCfg);
        if (device_status == OS_SUCCESS)
        {
            /* Increment device success counter */
            EXAMPLE_AppData.HkTelemetryPkt.DeviceCount++;

            /* Send device event success to the console */
            CFE_EVS_SendEvent(EXAMPLE_CMD_CONFIG_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "EXAMPLE: Configuration command received: %u", config_cmd->DeviceCfg);
        }
        else
        {
            /* Increment device error counter */
            EXAMPLE_AppData.HkTelemetryPkt.DeviceErrorCount++;

            /* Send device event failure to the console */
            CFE_EVS_SendEvent(EXAMPLE_CMD_CONFIG_DEV_ERR_EID, CFE_EVS_EventType_ERROR,
                              "EXAMPLE: Configuration command received: %u", config_cmd->DeviceCfg);
        }
    }
    return;
}

/*
** Verify command packet length matches expected
*/
int32 EXAMPLE_VerifyCmdLength(CFE_MSG_Message_t *msg, uint16 expected_length)
{
    int32             status        = OS_SUCCESS;
    CFE_SB_MsgId_t    msg_id        = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t cmd_code      = 0;
    size_t            actual_length = 0;

    CFE_MSG_GetSize(msg, &actual_length);
    if (expected_length != actual_length)
    {
        CFE_MSG_GetMsgId(msg, &msg_id);
        CFE_MSG_GetFcnCode(msg, &cmd_code);

        CFE_EVS_SendEvent(EXAMPLE_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid msg length: ID = 0x%X,  CC = %d, Len = %ld, Expected = %d",
                          CFE_SB_MsgIdToValue(msg_id), cmd_code, actual_length, expected_length);

        status = OS_ERROR;

        /* Increment the command error counter upon receipt of an invalid command length */
        EXAMPLE_AppData.HkTelemetryPkt.CommandErrorCount++;
    }
    return status;
}
