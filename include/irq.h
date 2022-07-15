/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define irq_lock() arch_irq_lock()

#define irq_unlock(key) arch_irq_unlock(key)

/**
 * @brief Enable an IRQ.
 *
 * This routine enables interrupts from source @a irq.
 *
 * @param irq IRQ line.
 */
#define irq_enable(irq) arch_irq_enable(irq)

/**
 * @brief Disable an IRQ.
 *
 * This routine disables interrupts from source @a irq.
 *
 * @param irq IRQ line.
 */
#define irq_disable(irq) arch_irq_disable(irq)

/**
 * @brief Get IRQ enable state.
 *
 * This routine indicates if interrupts from source @a irq are enabled.
 *
 * @param irq IRQ line.
 *
 * @return interrupt enable state, true or false
 */
#define irq_is_enabled(irq) arch_irq_is_enabled(irq)