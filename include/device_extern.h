#ifndef INCLUDE_DEVICE_EXTERN_H
#define INCLUDE_DEVICE_EXTERN_H

#include <dev_cfg.h>

/* GPIO */
extern const struct device DEVICE_NAME_GET(gpioa);
extern const struct device DEVICE_NAME_GET(gpiob);

/* UART */
#ifdef CONFIG_WCH_UART_0
extern const struct device DEVICE_NAME_GET(uart0);
#endif

#ifdef CONFIG_WCH_UART_1
extern const struct device DEVICE_NAME_GET(uart1);
#endif

#ifdef CONFIG_WCH_UART_2
extern const struct device DEVICE_NAME_GET(uart2);
#endif

#ifdef CONFIG_WCH_UART_3
extern const struct device DEVICE_NAME_GET(uart3);
#endif


/* RTC */
extern const struct device DEVICE_NAME_GET(rtc);

/* BAT */
extern const struct device DEVICE_NAME_GET(bat);

/* USB */
extern const struct device DEVICE_NAME_GET(usb);


#endif /* INCLUDE_DEVICE_EXTERN_H */