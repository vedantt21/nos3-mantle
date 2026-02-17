/************************************************************************
** File:
**   $Id: example_platform_cfg.h  $
**
** Purpose:
**  Define example Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _EXAMPLE_PLATFORM_CFG_H_
#define _EXAMPLE_PLATFORM_CFG_H_

/*
** Default EXAMPLE Configuration
*/
#ifndef EXAMPLE_CFG
/* Notes:
**   NOS3 uart requires matching handle and bus number
*/
#define EXAMPLE_CFG_STRING      "usart_18"
#define EXAMPLE_CFG_HANDLE      18
#define EXAMPLE_CFG_BAUDRATE_HZ 115200 /* Only change if required by device */
#define EXAMPLE_CFG_MS_TIMEOUT  50 /* Max 255 --> 50 is a safe default */
/* Note: Debug flag disabled (commented out) by default */
//#define EXAMPLE_CFG_DEBUG
#endif

#endif /* _EXAMPLE_PLATFORM_CFG_H_ */
