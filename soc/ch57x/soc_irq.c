/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <irq.h>
#include "soc.h"

void arch_irq_enable(unsigned int irq)
{
    PFIC_EnableIRQ(irq);
}

void arch_irq_disable(unsigned int irq)
{
    PFIC_DisableIRQ(irq);
}

void arch_irq_priority_set(unsigned int irq, unsigned int prio)
{
    PFIC_SetPriority(irq, prio);
}

int arch_irq_is_enabled(unsigned int irq)
{
    return PFIC_GetStatusIRQ(irq);
}

