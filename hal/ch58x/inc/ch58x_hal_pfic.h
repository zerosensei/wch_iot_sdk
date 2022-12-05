/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_CH58X_INC_CH58X_HAL_PFIC_H
#define HAL_CH58X_INC_CH58X_HAL_PFIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch58x_common.h"

/* memory mapped structure for Program Fast Interrupt Controller (PFIC) */
typedef struct __attribute__((packed))
{
    volatile uint32_t ISR[8];
    volatile uint32_t IPR[8];
    volatile uint32_t ITHRESDR;
    volatile uint32_t FIBADDRR;
    volatile uint32_t CFGR;
    volatile uint32_t GISR;
    uint8_t           RESERVED0[0x10];
    volatile uint32_t FIOFADDRR[4];
    uint8_t           RESERVED1[0x90];
    volatile uint32_t IENR[8];
    uint8_t           RESERVED2[0x60];
    volatile uint32_t IRER[8];
    uint8_t           RESERVED3[0x60];
    volatile uint32_t IPSR[8];
    uint8_t           RESERVED4[0x60];
    volatile uint32_t IPRR[8];
    uint8_t           RESERVED5[0x60];
    volatile uint32_t IACTR[8];
    uint8_t           RESERVED6[0xE0];
    volatile uint8_t  IPRIOR[256];
    uint8_t           RESERVED7[0x810];
    volatile uint32_t SCTLR;
} PFIC_Type;

#define PFIC         ((PFIC_Type *)0xE000E000)

#define PFIC_KEY1    ((uint32_t)0xFA050000)
#define PFIC_KEY2    ((uint32_t)0xBCAF0000)
#define PFIC_KEY3    ((uint32_t)0xBEEF0000)

/*******************************************************************************
 * @fn      PFIC_EnableIRQ
 *
 * @brief   Enable Interrupt
 *
 * @param   IRQn    - Interrupt Numbers
 */
static inline void PFIC_EnableIRQ(IRQn_Type IRQn)
{
    PFIC->IENR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn)&0x1F));
}

/*******************************************************************************
 * @fn      PFIC_DisableIRQ
 *
 * @brief   Disable Interrupt
 *
 * @param   IRQn    - Interrupt Numbers
 */
static inline void PFIC_DisableIRQ(IRQn_Type IRQn)
{
    uint32_t tem;
    tem = PFIC->ITHRESDR;
    PFIC->ITHRESDR = 0x10;
    PFIC->IRER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn)&0x1F));
    PFIC->ITHRESDR = tem;
    __asm__ volatile("nop");
    __asm__ volatile("nop");

}

/*******************************************************************************
 * @fn      PFIC_GetStatusIRQ
 *
 * @brief   Get Interrupt Enable State
 *
 * @param   IRQn    - Interrupt Numbers
 *
 * @return  1: Interrupt Enable
 *          0: Interrupt Disable
 */
static inline uint32_t PFIC_GetStatusIRQ(IRQn_Type IRQn)
{
    return ((uint32_t)((PFIC->ISR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn)&0x1F))) ? 1 : 0));
}

/*******************************************************************************
 * @fn      PFIC_GetPendingIRQ
 *
 * @brief   Get Interrupt Pending State
 *
 * @param   IRQn    - Interrupt Numbers
 *
 * @return  1: Interrupt Pending Enable
 *          0: Interrupt Pending Disable
 */
static inline uint32_t PFIC_GetPendingIRQ(IRQn_Type IRQn)
{
    return ((uint32_t)((PFIC->IPR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn)&0x1F))) ? 1 : 0));
}

/*******************************************************************************
 * @fn      PFIC_SetPendingIRQ
 *
 * @brief   Set Interrupt Pending
 *
 * @param   IRQn    - Interrupt Numbers
 */
static inline void PFIC_SetPendingIRQ(IRQn_Type IRQn)
{
    PFIC->IPSR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn)&0x1F));
}

/*******************************************************************************
 * @fn      PFIC_ClearPendingIRQ
 *
 * @brief   Clear Interrupt Pending
 *
 * @param   IRQn    - Interrupt Numbers
 */
static inline void PFIC_ClearPendingIRQ(IRQn_Type IRQn)
{
    PFIC->IPRR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn)&0x1F));
}

/*******************************************************************************
 * @fn      PFIC_GetActive
 *
 * @brief   Get Interrupt Active State
 *
 * @param   IRQn    - Interrupt Numbers
 *
 * @return  1: Interrupt Active
 *          0: Interrupt No Active.
 */
static inline uint32_t PFIC_GetActive(IRQn_Type IRQn)
{
    return ((uint32_t)((PFIC->IACTR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn)&0x1F))) ? 1 : 0));
}

/*******************************************************************************
 * @fn      PFIC_SetPriority
 *
 * @brief   Set Interrupt Priority
 *
 * @param   IRQn        - Interrupt Numbers
 * @param   priority    - bit7:         pre-emption priority
 *                        bit6-bit4:    subpriority
 */
static inline void PFIC_SetPriority(IRQn_Type IRQn, uint8_t priority)
{
    PFIC->IPRIOR[(uint32_t)(IRQn)] = priority;
}

/*******************************************************************************
 * @fn      PFIC_FastINT0CFG
 *
 * @brief   Set fast Interrupt 0
 *
 * @param   IRQn    - Interrupt Numbers
 * @param   addr    - interrupt service addr
 */
static inline void PFIC_FastINT0CFG(IRQn_Type IRQn, uint32_t addr)
{
    PFIC->FIBADDRR = addr & 0xF0000000;
    PFIC->FIOFADDRR[0] = (IRQn << 24) | (addr & 0x000FFFFF);
}

/*******************************************************************************
 * @fn      PFIC_FastINT1CFG
 *
 * @brief   Set fast Interrupt 1
 *
 * @param   IRQn    - Interrupt Numbers
 * @param   addr    - interrupt service addr
 */
static inline void PFIC_FastINT1CFG(IRQn_Type IRQn, uint32_t addr)
{
    PFIC->FIBADDRR = addr & 0xF0000000;
    PFIC->FIOFADDRR[1] = (IRQn << 24) | (addr & 0x000FFFFF);
}

/*******************************************************************************
 * @fn      PFIC_FastINT2CFG
 *
 * @brief   Set fast Interrupt 2, occupied if use BLE.
 *
 * @param   IRQn    - Interrupt Numbers
 * @param   addr    - interrupt service addr
 */
static inline void PFIC_FastINT2CFG(IRQn_Type IRQn, uint32_t addr)
{
    PFIC->FIBADDRR = addr & 0xF0000000;
    PFIC->FIOFADDRR[2] = (IRQn << 24) | (addr & 0x000FFFFF);
}

/*******************************************************************************
 * @fn      PFIC_FastINT3CFG
 *
 * @brief   Set fast Interrupt 3, occupied if use BLE.
 *
 * @param   IRQn    - Interrupt Numbers
 * @param   addr    - interrupt service addr
 */
static inline void PFIC_FastINT3CFG(IRQn_Type IRQn, uint32_t addr)
{
    PFIC->FIBADDRR = addr & 0xF0000000;
    PFIC->FIOFADDRR[3] = (IRQn << 24) | (addr & 0x000FFFFF);
}


/*******************************************************************************
 * @fn      PFIC_SetFastIRQ
 *
 * @brief   Set Fast Interrupt
 *
 * @param   addr    - Fast interrupt service function base address
 * @param   IRQn    - Interrupt Numbers
 * @param   num     - Fast Interrupt Numbers
 */
static inline void PFIC_SetFastIRQ(uint32_t addr, IRQn_Type IRQn, uint8_t num)
{
    if(num > 3)
        return;
    PFIC->FIBADDRR = addr;
    PFIC->FIOFADDRR[num] = ((uint32_t)IRQn << 24) | (addr & 0xfffff);
}

/*******************************************************************************
 * @fn      PFIC_SystemReset
 *
 * @brief   Initiate a system reset request
 */
static inline void PFIC_SystemReset(void)
{
    PFIC->CFGR = PFIC_KEY3 | (1 << 7);
}

/*******************************************************************************
 * @fn      PFIC_HaltPushCfg
 *
 * @brief   Enable Hardware Stack
 *
 * @param   NewState    - DISABLE or ENABLE
 */
static inline void PFIC_HaltPushCfg(bool NewState)
{
    if(NewState != false)
    {
        PFIC->CFGR = PFIC_KEY1;
    }
    else
    {
        PFIC->CFGR = PFIC_KEY1 | (1 << 0);
    }
}

/*******************************************************************************
 * @fn      PFIC_INTNestCfg
 *
 * @brief   Enable Interrupt Nesting
 *
 * @param   NewState    - DISABLE or ENABLE
 */
static inline void PFIC_INTNestCfg(bool NewState)
{
    if(NewState != false)
    {
        PFIC->CFGR = PFIC_KEY1;
    }
    else
    {
        PFIC->CFGR = PFIC_KEY1 | (1 << 1);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_CH58X_INC_CH58X_HAL_PFIC_H */
