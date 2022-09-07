/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_CH57X_INC_CH57X_HAL_SYS_H
#define HAL_CH57X_INC_CH57X_HAL_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch57x_common.h"

typedef enum {
    STA_RAM_READABLE = 0U,          // FlashROM 代码和数据区 是否可读
    STA_RESET_EN = 2U,              // RST#外部手动复位输入功能是否开启
    STA_BOOT_EN,                    // 系统引导程序 BootLoader 是否开启
    STA_DEBUG_EN,                   // 系统仿真调试接口是否开启
    STA_IN_BOOT,                    // 当前系统是否处于Bootloader 区	
} sys_status_t;


/* Safe access */
 __attribute__((always_inline)) static inline void sys_safe_access_enable(void)
{
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
}

__attribute__((always_inline)) static inline void sys_safe_access_disable(void)
{
    R8_SAFE_ACCESS_SIG = 0;
}

__attribute__((always_inline)) static inline uint8_t sys_is_safe_access_enabled(void)
{
    return R8_SAFE_ACCESS_SIG & RB_SAFE_ACC_ACT;
}

__attribute__((always_inline)) static inline uint8_t sys_get_safe_access_tmr(void)
{
    return (R8_SAFE_ACCESS_SIG & RB_SAFE_ACC_TIMER) >> WCH_BIT(4);
}


__attribute__((always_inline)) static inline void __WFI(void){
    PFIC->SCTLR &= ~(1<<3);	
    __asm__ volatile ("wfi");
}


__attribute__((always_inline)) static inline void __WFE(void){
    PFIC->SCTLR |= (1<<3)|(1<<5);		
    __asm__ volatile ("wfi");
    PFIC->SCTLR |= (1<<3);
    __asm__ volatile ("wfi");
}

static inline uint8_t hal_sys_get_status(sys_status_t sta)
{
    return R8_GLOB_CFG_INFO & sta;
}

void hal_sys_reset(void);
void hal_sys_init(void);
void hal_sys_delay_us(uint16_t t);
void hal_sys_delay_ms(uint16_t t);

#ifdef __cplusplus
}
#endif

#endif /* HAL_CH57X_INC_CH57X_HAL_SYS_H */