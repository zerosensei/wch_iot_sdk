/********************************** (C) COPYRIGHT *******************************
 * File Name          : startup_ch57x.s
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2020/04/30
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

	.section	.init,"ax",@progbits
	.global	_start
	.align	1
_start:
	j	handle_reset

    .section    .vector,"ax",@progbits
    .align  1
_vector_base:
    .option norvc;
    .word   0
    .word   0
        j   NMI_Handler                 /* NMI Handler */
        j   HardFault_Handler           /* Hard Fault Handler */
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
        j   SysTick_Handler            /* SysTick Handler */
    .word   0
        j   SW_Handler                 /* SW Handler */
    .word   0
    /* External Interrupts */
        j   TMR0_IRQHandler            /* 0:  TMR0 */
        j   GPIOA_IRQHandler           /* GPIOA */
        j   GPIOB_IRQHandler           /* GPIOB */
        j   SPI0_IRQHandler            /* SPI0 */
        j   BB_IRQHandler              /* BLEB */
        j   LLE_IRQHandler             /* BLEL */
        j   USB_IRQHandler             /* USB */
        .word   0
        j   TMR1_IRQHandler            /* TMR1 */
        j   TMR2_IRQHandler            /* TMR2 */
        j   UART0_IRQHandler           /* UART0 */
        j   UART1_IRQHandler           /* UART1 */
        j   RTC_IRQHandler             /* RTC */
        j   ADC_IRQHandler             /* ADC */
        .word   0
        j   PWMX_IRQHandler            /* PWMX */
        j   TMR3_IRQHandler            /* TMR3 */
        j   UART2_IRQHandler           /* UART2 */
        j   UART3_IRQHandler           /* UART3 */
        j   WDOG_BAT_IRQHandler        /* WDOG_BAT */

    .option rvc;

    .section    .vector_handler, "ax", @progbits
    .weak   NMI_Handler
    .weak   HardFault_Handler
    .weak   SysTick_Handler
    .weak   SW_Handler
    .weak   TMR0_IRQHandler
    .weak   GPIOA_IRQHandler
    .weak   GPIOB_IRQHandler
    .weak   SPI0_IRQHandler
    .weak   BB_IRQHandler
    .weak   LLE_IRQHandler
    .weak   USB_IRQHandler
    .weak   TMR1_IRQHandler
    .weak   TMR2_IRQHandler
    .weak   UART0_IRQHandler
    .weak   UART1_IRQHandler
    .weak   RTC_IRQHandler
    .weak   ADC_IRQHandler
    .weak   PWMX_IRQHandler
    .weak   TMR3_IRQHandler
    .weak   UART2_IRQHandler
    .weak   UART3_IRQHandler
    .weak   WDOG_BAT_IRQHandler

NMI_Handler:  1:  j 1b
HardFault_Handler:  1:  j 1b
SysTick_Handler:  1:  j 1b
SW_Handler:  1:  j 1b
TMR0_IRQHandler:  1:  j 1b
GPIOA_IRQHandler:  1:  j 1b
GPIOB_IRQHandler:  1:  j 1b
SPI0_IRQHandler:  1:  j 1b
BB_IRQHandler:  1:  j 1b
LLE_IRQHandler:  1:  j 1b
USB_IRQHandler:  1:  j 1b
TMR1_IRQHandler:  1:  j 1b
TMR2_IRQHandler:  1:  j 1b
UART0_IRQHandler:  1:  j 1b
UART1_IRQHandler:  1:  j 1b
RTC_IRQHandler:  1:  j 1b
ADC_IRQHandler:  1:  j 1b
PWMX_IRQHandler:  1:  j 1b
TMR3_IRQHandler:  1:  j 1b
UART2_IRQHandler:  1:  j 1b
UART3_IRQHandler:  1:  j 1b
WDOG_BAT_IRQHandler:  1:  j 1b

	.section	.handle_reset,"ax",@progbits
	.weak	handle_reset
	.align	1
handle_reset:
.option push 
.option	norelax 
	la gp, __global_pointer$
.option	pop 
1:
	la sp, _eusrstack 
2:
    li t0, 0x1888
    csrs mstatus, t0
	la t0, _vector_base
    ori t0, t0, 1
	csrw mtvec, t0

    call sys_init 

    call main


