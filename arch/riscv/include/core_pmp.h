/*
 * Copyright (c) 2020 BayLibre, SAS
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CORE_PMP_H_
#define CORE_PMP_H_

/* Configuration register flags (cfg_val)*/
#define PMP_R		0x01	/* Allow read */
#define PMP_W		0x02	/* Allow write */
#define PMP_X		0x04	/* Allow execute */
#define PMP_A		0x18	/* Address-matching mode field */
#define PMP_L		0x80	/* PMP entry is locked */
#define PMP_OFF		0x00	/* Null region */
#define PMP_TOR		0x08	/* Top of range */
#define PMP_NA4		0x10	/* Naturally aligned four-byte region */
#define PMP_NAPOT	0x18	/* Naturally aligned power-of-two region */

#define PMP_SHIFT_ADDR	2
#define PMP_TYPE_MASK		0x18
#define TO_PMP_ADDR(addr)	((addr) >> PMP_SHIFT_ADDR)
#define FROM_PMP_ADDR(addr)	((addr) << PMP_SHIFT_ADDR)
#define TO_NAPOT_RANGE(size)	(((size) - 1) >> 1)
#define TO_PMP_NAPOT(addr, size)	TO_PMP_ADDR(addr | \
					TO_NAPOT_RANGE(size))

#ifdef CONFIG_PMP_STACK_GUARD
#define PMP_GUARD_ALIGN_AND_SIZE CONFIG_PMP_STACK_GUARD_MIN_SIZE
#else
#define PMP_GUARD_ALIGN_AND_SIZE 0
#endif /* CONFIG_PMP_STACK_GUARD */

#ifndef CONFIG_PMP_SLOT
#define CONFIG_PMP_SLOT     8
#endif

#ifdef CONFIG_64BIT
#define	RISCV_PMP_CFG_NUM	(CONFIG_PMP_SLOT >> 3)
#else
#define	RISCV_PMP_CFG_NUM	(CONFIG_PMP_SLOT >> 2)
#endif

struct _stack_info {
    uintptr_t start;
	size_t size;
};

struct k_pmp {
    struct _stack_info stack_info;
    unsigned long u_pmpcfg[RISCV_PMP_CFG_NUM];
    unsigned long u_pmpaddr[CONFIG_PMP_SLOT];
};

/**
 * @brief Reset to 0 all PMP setup registers
 */
void z_riscv_pmp_clear_config(void);

/**
 * @brief Configure RISCV user access to the stack
 *
 * Determine and save allow access setup in pmp structure.
 *
 * @param pmp pmp info data pointer.
 */
void z_riscv_init_user_accesses(struct k_pmp *pmp);

/**
 * @brief Apply RISCV user access to the stack
 *
 * Write user access setup saved in this pmp structure.
 *
 * @param pmp pmp info data pointer.
 */
void z_riscv_configure_user_allowed_stack(struct k_pmp *pmp);

/**
 * @brief Add a new RISCV stack access
 *
 * Add a new memory permission area in the existing
 * pmp.
 *
 * @param pmp pmp info data pointer.
 * @param addr   Start address of the memory area.
 * @param size   Size of the memory area.
 * @param flags  Pemissions: PMP_R, PMP_W, PMP_X, PMP_L
 *
 * @retval 0 if successful
 * @retval -EINVAL if invalid parameters supplied
 * @retval -ENOSPC if no free PMP entry
 */
int z_riscv_pmp_add_dynamic(struct k_pmp *pmp,
			unsigned long addr,
			unsigned long size,
			unsigned char flags);

/**
 * @brief Initialize PMP setup value to 0
 *
 * @param pmp pmp info data pointer.
 */
void z_riscv_pmp_init(struct k_pmp *pmp);

/**
 * @brief Check memory region permissions
 *
 * Given a memory region, return whether the current memory management hardware
 * configuration would allow a user pmp to read/write that region. Used by
 * system calls to validate buffers coming in from userspace.
 *
 * Notes:
 * The function is guaranteed to never return validation success, if the entire
 * buffer area is not user accessible.
 *
 * The function is guaranteed to correctly validate the permissions of the
 * supplied buffer, if the user access permissions of the entire buffer are
 * enforced by a single, enabled memory management region.
 *
 * In some architectures the validation will always return failure
 * if the supplied memory buffer spans multiple enabled memory management
 * regions (even if all such regions permit user access).
 *
 * @warning 0 size buffer has undefined behavior.
 *
 * @param pmp pmp info data pointer
 * @param addr start address of the buffer
 * @param size the size of the buffer
 * @param write If nonzero, additionally check if the area is writable.
 *	  Otherwise, just check if the memory can be read.
 *
 * @return nonzero if the permissions don't match.
 */
int arch_buffer_validate(struct k_pmp *pmp, void *addr, size_t size, int write);

#endif /* CORE_PMP_H_ */
