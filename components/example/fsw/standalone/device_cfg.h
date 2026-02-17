#ifndef _EXAMPLE_CHECKOUT_DEVICE_CFG_H_
#define _EXAMPLE_CHECKOUT_DEVICE_CFG_H_

/*
** EXAMPLE Checkout Configuration
*/
#define EXAMPLE_CFG
/* Note: NOS3 uart requires matching handle and bus number */
#define EXAMPLE_CFG_STRING      "/dev/usart_16"
#define EXAMPLE_CFG_HANDLE      16
#define EXAMPLE_CFG_BAUDRATE_HZ 115200
#define EXAMPLE_CFG_MS_TIMEOUT  250
#define EXAMPLE_CFG_DEBUG

#endif /* _EXAMPLE_CHECKOUT_DEVICE_CFG_H_ */
