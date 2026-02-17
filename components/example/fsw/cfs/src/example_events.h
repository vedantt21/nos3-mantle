/************************************************************************
** File:
**    example_events.h
**
** Purpose:
**  Define EXAMPLE application event IDs
**
*************************************************************************/

#ifndef _EXAMPLE_EVENTS_H_
#define _EXAMPLE_EVENTS_H_

/* Standard app event IDs */
#define EXAMPLE_RESERVED_EID        0
#define EXAMPLE_STARTUP_INF_EID     1
#define EXAMPLE_LEN_ERR_EID         2
#define EXAMPLE_PIPE_ERR_EID        3
#define EXAMPLE_SUB_CMD_ERR_EID     4
#define EXAMPLE_SUB_REQ_HK_ERR_EID  5
#define EXAMPLE_PROCESS_CMD_ERR_EID 6

/* Standard command event IDs */
#define EXAMPLE_CMD_ERR_EID         10
#define EXAMPLE_CMD_NOOP_INF_EID    11
#define EXAMPLE_CMD_RESET_INF_EID   12
#define EXAMPLE_CMD_ENABLE_INF_EID  13
#define EXAMPLE_ENABLE_INF_EID      14
#define EXAMPLE_ENABLE_ERR_EID      15
#define EXAMPLE_CMD_DISABLE_INF_EID 16
#define EXAMPLE_DISABLE_INF_EID     17
#define EXAMPLE_DISABLE_ERR_EID     18

/* Device specific command event IDs */
#define EXAMPLE_CMD_CONFIG_EN_ERR_EID  20
#define EXAMPLE_CMD_CONFIG_VAL_ERR_EID 21
#define EXAMPLE_CMD_CONFIG_INF_EID     22
#define EXAMPLE_CMD_CONFIG_DEV_ERR_EID 23

/* Standard telemetry event IDs */
#define EXAMPLE_DEVICE_TLM_ERR_EID 30
#define EXAMPLE_REQ_HK_ERR_EID     31

/* Device specific telemetry event IDs */
#define EXAMPLE_REQ_DATA_ERR_EID        32
#define EXAMPLE_REQ_DATA_STATUS_ERR_EID 33

/* Hardware protocol event IDs */
#define EXAMPLE_UART_INIT_ERR_EID  40
#define EXAMPLE_UART_CLOSE_ERR_EID 41

#endif /* _EXAMPLE_EVENTS_H_ */
