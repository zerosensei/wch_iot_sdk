/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <core_pmp.h>
#include <kernel_arch_func.h>
#include <logging/log.h>

LOG_MODULE_REGISTER(arch, CONFIG_ARCH_LOG_LEVEL);

#ifdef CONFIG_64BIT
# define PR_ADDR "0x%016lx"
#else
# define PR_ADDR "0x%08lx"
#endif

#ifdef CONFIG_64BIT
# define PMPCFG_NUM(index)	(((index) / 8) * 2)
# define PMPCFG_SHIFT(index)	(((index) % 8) * 8)
#else
# define PMPCFG_NUM(index)	((index) / 4)
# define PMPCFG_SHIFT(index)	(((index) % 4) * 8)
#endif

#if defined(CONFIG_PMP_POWER_OF_TWO_ALIGNMENT)
# define PMP_MODE_DEFAULT		PMP_MODE_NAPOT
# define PMP_USED_ENTRY_DEFAULT		1 /* NAPOT region use 1 PMP entry */
#else
# define PMP_MODE_DEFAULT		PMP_MODE_TOR
# define PMP_USED_ENTRY_DEFAULT		2 /* TOR region use 2 PMP entry */
#endif

enum pmp_region_mode {
	PMP_MODE_NA4,
	/* If NAPOT mode region size is 4, apply NA4 region to PMP CSR. */
	PMP_MODE_NAPOT,
	PMP_MODE_TOR,
};

/* Region definition data structure */
struct riscv_pmp_region {
	uint32_t start;
	uint32_t size;
	uint8_t perm;
	enum pmp_region_mode mode;
};

extern char __rom_region_start[];
extern char __rom_region_end[];

/*
 * RISC-V PMP regions usage (from low number to high):
 *   dynamic region -> static region -> MPRV region
 *
 * Lower number PMP region has higher priority. For overlapping PMP
 * regions, static regions should have lower priority than dynamic
 * regions. MPRV region should have the lowest priority.
 */
static uint8_t static_regions_num;
static uint8_t mprv_regions_num;

/* Get the number of PMP regions */
static inline uint8_t get_num_regions(void)
{
	return CONFIG_PMP_SLOT;
}

/* Get the maximum PMP region index for dynamic PMP region */
static inline uint8_t max_dynamic_region(void)
{
	return get_num_regions() - static_regions_num -
		mprv_regions_num - 1;
}

static struct riscv_pmp_region static_regions[] = {
	{
		/*
		 * Program and RO data: RX permission for both
		 * user/supervisor mode.
		 */
		.start = (unsigned long) __rom_region_start,
		.perm = PMP_R | PMP_X | PMP_L,
		.mode = PMP_MODE_DEFAULT,
	},
};

#ifdef CONFIG_PMP_STACK_GUARD
/*
 * Basically, RISC-V PMP regions affecting M-mode permission should be
 * locked and can't be a dynamic PMP region. RISC-V PMP stack guard rely
 * on consistently using mstatus.MPRV bit in M-mode which can emulate
 * U-mode memory protection policy in M-mode when MPRV bit is enabled.
 *
 * For consistent MPRV in M-mode, all memory accesses are denied by
 * default (like U-mode default). We need to use a lowest priority PMP
 * region (MPRV region) to permit memory access of whole memory.
 *
 */
static const struct riscv_pmp_region mprv_region = {
	/*
	 * Enable all other memory access with lowest priority.
	 *
	 * Special case: start = size = 0 means whole memory.
	 */
	.start = 0,
	.size = 0,
	.perm = PMP_R | PMP_W | PMP_X,
	.mode = PMP_MODE_NAPOT,
};
#endif

enum {
	CSR_PMPCFG0,
	CSR_PMPCFG1,
	CSR_PMPCFG2,
	CSR_PMPCFG3,
	CSR_PMPADDR0,
	CSR_PMPADDR1,
	CSR_PMPADDR2,
	CSR_PMPADDR3,
	CSR_PMPADDR4,
	CSR_PMPADDR5,
	CSR_PMPADDR6,
	CSR_PMPADDR7,
	CSR_PMPADDR8,
	CSR_PMPADDR9,
	CSR_PMPADDR10,
	CSR_PMPADDR11,
	CSR_PMPADDR12,
	CSR_PMPADDR13,
	CSR_PMPADDR14,
	CSR_PMPADDR15
};

static unsigned long csr_read_enum(int pmp_csr_enum)
{
	unsigned long res = -1;

	switch (pmp_csr_enum) {
	case CSR_PMPCFG0:
		res = csr_read(0x3A0); break;
	case CSR_PMPCFG1:
		res = csr_read(0x3A1); break;
	case CSR_PMPCFG2:
		res = csr_read(0x3A2); break;
	case CSR_PMPCFG3:
		res = csr_read(0x3A3); break;
	case CSR_PMPADDR0:
		res = csr_read(0x3B0); break;
	case CSR_PMPADDR1:
		res = csr_read(0x3B1); break;
	case CSR_PMPADDR2:
		res = csr_read(0x3B2); break;
	case CSR_PMPADDR3:
		res = csr_read(0x3B3); break;
	case CSR_PMPADDR4:
		res = csr_read(0x3B4); break;
	case CSR_PMPADDR5:
		res = csr_read(0x3B5); break;
	case CSR_PMPADDR6:
		res = csr_read(0x3B6); break;
	case CSR_PMPADDR7:
		res = csr_read(0x3B7); break;
	case CSR_PMPADDR8:
		res = csr_read(0x3B8); break;
	case CSR_PMPADDR9:
		res = csr_read(0x3B9); break;
	case CSR_PMPADDR10:
		res = csr_read(0x3BA); break;
	case CSR_PMPADDR11:
		res = csr_read(0x3BB); break;
	case CSR_PMPADDR12:
		res = csr_read(0x3BC); break;
	case CSR_PMPADDR13:
		res = csr_read(0x3BD); break;
	case CSR_PMPADDR14:
		res = csr_read(0x3BE); break;
	case CSR_PMPADDR15:
		res = csr_read(0x3BF); break;
	default:
		break;
	}
	return res;
}

static void csr_write_enum(int pmp_csr_enum, unsigned long value)
{
	switch (pmp_csr_enum) {
	case CSR_PMPCFG0:
		csr_write(0x3A0, value); break;
	case CSR_PMPCFG1:
		csr_write(0x3A1, value); break;
	case CSR_PMPCFG2:
		csr_write(0x3A2, value); break;
	case CSR_PMPCFG3:
		csr_write(0x3A3, value); break;
	case CSR_PMPADDR0:
		csr_write(0x3B0, value); break;
	case CSR_PMPADDR1:
		csr_write(0x3B1, value); break;
	case CSR_PMPADDR2:
		csr_write(0x3B2, value); break;
	case CSR_PMPADDR3:
		csr_write(0x3B3, value); break;
	case CSR_PMPADDR4:
		csr_write(0x3B4, value); break;
	case CSR_PMPADDR5:
		csr_write(0x3B5, value); break;
	case CSR_PMPADDR6:
		csr_write(0x3B6, value); break;
	case CSR_PMPADDR7:
		csr_write(0x3B7, value); break;
	case CSR_PMPADDR8:
		csr_write(0x3B8, value); break;
	case CSR_PMPADDR9:
		csr_write(0x3B9, value); break;
	case CSR_PMPADDR10:
		csr_write(0x3BA, value); break;
	case CSR_PMPADDR11:
		csr_write(0x3BB, value); break;
	case CSR_PMPADDR12:
		csr_write(0x3BC, value); break;
	case CSR_PMPADDR13:
		csr_write(0x3BD, value); break;
	case CSR_PMPADDR14:
		csr_write(0x3BE, value); break;
	case CSR_PMPADDR15:
		csr_write(0x3BF, value); break;
	default:
		break;
	}
}

/*
 * @brief Set a Physical Memory Protection slot
 *
 * Configure a memory region to be secured by one of the 16 PMP entries.
 *
 * @param index		Number of the targeted PMP entry (0 to 15 only).
 * @param cfg_val	Configuration value (cf datasheet or defined flags)
 * @param addr_val	Address register value
 *
 * This function shall only be called from Secure state.
 *
 * @return -1 if bad argument, 0 otherwise.
 */
static int riscv_pmp_set(unsigned int index, uint8_t cfg_val, unsigned long addr_val)
{
	unsigned long reg_val;
	unsigned long shift, mask;
	int pmpcfg_csr;
	int pmpaddr_csr;

	if (index >= get_num_regions()) {
		return -1;
	}

	/* Calculate PMP config/addr register, shift and mask */
	pmpcfg_csr = CSR_PMPCFG0 + PMPCFG_NUM(index);
	pmpaddr_csr = CSR_PMPADDR0 + index;
	shift = PMPCFG_SHIFT(index);
	mask = 0xFFUL << shift;

	reg_val = csr_read_enum(pmpcfg_csr);
	reg_val = reg_val & ~mask;
	reg_val = reg_val | ((unsigned long)cfg_val << shift);

	csr_write_enum(pmpaddr_csr, addr_val);
	csr_write_enum(pmpcfg_csr, reg_val);
	return 0;
}

#ifdef CONFIG_RISCV_PMP
static int riscv_pmp_get(unsigned int index, uint8_t *cfg_val, unsigned long *addr_val)
{
	unsigned long reg_val;
	unsigned long shift, mask;
	int pmpcfg_csr;
	int pmpaddr_csr;

	if (index >= get_num_regions()) {
		return -1;
	}

	/* Calculate PMP config/addr register, shift */
	pmpcfg_csr = CSR_PMPCFG0 + PMPCFG_NUM(index);
	pmpaddr_csr = CSR_PMPADDR0 + index;
	shift = PMPCFG_SHIFT(index);
	mask = 0xFFUL << shift;

	reg_val = csr_read_enum(pmpcfg_csr);
	*cfg_val = (reg_val & mask) >> shift;

	if (addr_val) {
		pmpaddr_csr = CSR_PMPADDR0 + index;
		*addr_val = csr_read_enum(pmpaddr_csr);
	}

	return 0;
}
#endif

/*
 * @brief Configure the range of pmpcfg CSRs.
 *
 * Configure pmpcfg array to range of pmpcfg CSRs from min_index to max_index.
 * If u8_pmpcfg is NULL, this function will clear range of $pmpcfg CSRs to 0.
 *
 * @param min_index	First number of the targeted PMP entry.
 * @param max_index	Last number of the targeted PMP entry.
 * @param u8_pmpcfg	Array of PMP configuration value. NULL means clearing PMPCFG.
 */
void riscv_pmpcfg_set_range(uint8_t min_index, uint8_t max_index, uint8_t *u8_pmpcfg)
{
	unsigned long cfg_mask = 0;
	unsigned long new_pmpcfg = 0;
	unsigned long shift;

	for (int index = min_index; index <= max_index; index++) {
		shift = PMPCFG_SHIFT(index);
		cfg_mask |= (0xFFUL << shift);

		/* If u8_pmpcfg is NULL, new_pmpcfg is always 0 to clear pmpcfg CSR. */
		if (u8_pmpcfg != NULL) {
			new_pmpcfg |= ((unsigned long) u8_pmpcfg[index]) << shift;
		}

		/* If index+1 is new CSR or it's last index, apply new_pmpcfg value to the CSR. */
		if ((PMPCFG_SHIFT(index+1) == 0) || (index == max_index)) {
			int pmpcfg_csr = CSR_PMPCFG0 + PMPCFG_NUM(index);

			/* Update pmpcfg CSR */
			if (cfg_mask == -1UL) {
				/* Update whole CSR */
				csr_write_enum(pmpcfg_csr, new_pmpcfg);
			} else {
				/* Only update cfg_mask bits of CSR */
				unsigned long pmpcfg = csr_read_enum(pmpcfg_csr);

				pmpcfg &= ~cfg_mask;
				pmpcfg |= (cfg_mask & new_pmpcfg);
				csr_write_enum(pmpcfg_csr, pmpcfg);
			}

			/* Clear variables for next pmpcfg */
			cfg_mask = 0;
			new_pmpcfg = 0;
		}
	}
}

#ifdef CONFIG_RISCV_PMP
static int riscv_pmp_get_user_pmp(struct k_pmp *pmp, unsigned int index, uint8_t *cfg_val, unsigned long *addr_val)
{
	uint8_t last_static_index = max_dynamic_region() + 1;

	if (index >= get_num_regions()) {
		return -1;
	}

	if (index >= last_static_index) {
		/* This PMP index is static PMP region, check PMP CSRs. */
		riscv_pmp_get(index, cfg_val, addr_val);
	} else {
		uint8_t *u8_pmpcfg = (uint8_t *) pmp->u_pmpcfg;
		unsigned long *pmpaddr = pmp->u_pmpaddr;

		*cfg_val = u8_pmpcfg[index];

		if (addr_val) {
			*addr_val = pmpaddr[index];
		}
	}

	return 0;
}
#endif

static int riscv_pmp_region_translate(int index,
	const struct riscv_pmp_region *region, bool to_csr,
	unsigned long pmpcfg[], unsigned long pmpaddr[])
{
	int result, pmp_mode;

	if ((region->start == 0) && (region->size == 0)) {
		/* special case: set whole memory as single PMP region.
		 *   RV32: 0 ~ (2**32 - 1)
		 *   RV64: 0 ~ (2**64 - 1)
		 */
		if (index >= CONFIG_PMP_SLOT) {
			return -28;
		}

		pmp_mode = PMP_NAPOT;

		uint8_t cfg_val = PMP_NAPOT | region->perm;
#ifdef CONFIG_64BIT
		unsigned long addr_val = 0x1FFFFFFFFFFFFFFF;
#else
		unsigned long addr_val = 0x1FFFFFFF;
#endif

		if (to_csr) {
			riscv_pmp_set(index, cfg_val, addr_val);
		} else {
			uint8_t *u8_pmpcfg = (uint8_t *)pmpcfg;

			u8_pmpcfg[index] = cfg_val;
			pmpaddr[index] = addr_val;
		}

		result = index+1;
	} else if (region->mode == PMP_MODE_TOR) {
		if ((index+1) >= CONFIG_PMP_SLOT) {
			return -28;
		}

		pmp_mode = PMP_TOR;

		uint8_t cfg_val1 = PMP_NA4 | region->perm;
		unsigned long addr_val1 = TO_PMP_ADDR(region->start);
		uint8_t cfg_val2 = PMP_TOR | region->perm;
		unsigned long addr_val2 = TO_PMP_ADDR(region->start + region->size);

		if (to_csr) {
			riscv_pmp_set(index, cfg_val1, addr_val1);
			riscv_pmp_set(index+1, cfg_val2, addr_val2);
		} else {
			uint8_t *u8_pmpcfg = (uint8_t *)pmpcfg;

			u8_pmpcfg[index] = cfg_val1;
			pmpaddr[index] = addr_val1;
			u8_pmpcfg[index+1] = cfg_val2;
			pmpaddr[index+1] = addr_val2;
		}

		result = index+2;
	} else {
		if (index >= CONFIG_PMP_SLOT) {
			return -28;
		}

		if ((region->mode == PMP_MODE_NA4) || (region->size == 4)) {
			pmp_mode = PMP_NA4;
		} else {
			pmp_mode = PMP_NAPOT;
		}

		uint8_t cfg_val = pmp_mode | region->perm;
		unsigned long addr_val = TO_PMP_NAPOT(region->start, region->size);

		if (to_csr) {
			riscv_pmp_set(index, cfg_val, addr_val);
		} else {
			uint8_t *u8_pmpcfg = (uint8_t *)pmpcfg;

			u8_pmpcfg[index] = cfg_val;
			pmpaddr[index] = addr_val;
		}

		result = index+1;
	}

	if (to_csr) {
		LOG_DBG("Set PMP region %d: (" PR_ADDR ", " PR_ADDR
			", %s%s%s, %s)\n", index, region->start, region->size,
			((region->perm & PMP_R) ? "R" : " "),
			((region->perm & PMP_W) ? "W" : " "),
			((region->perm & PMP_X) ? "X" : " "),
			((pmp_mode == PMP_TOR) ? "TOR" :
			(pmp_mode == PMP_NAPOT) ? "NAPOT" : "NA4"));
	} else {
		LOG_DBG("PMP context " PR_ADDR " add region %d: (" PR_ADDR
			", " PR_ADDR ", %s%s%s, %s)\n", (unsigned long) pmpcfg, index,
			region->start, region->size,
			((region->perm & PMP_R) ? "R" : " "),
			((region->perm & PMP_W) ? "W" : " "),
			((region->perm & PMP_X) ? "X" : " "),
			((pmp_mode == PMP_TOR) ? "TOR" :
			(pmp_mode == PMP_NAPOT) ? "NAPOT" : "NA4"));
	}

	if (pmp_mode == PMP_TOR) {
		LOG_DBG("TOR mode region also use entry %d\n", index+1);
	}

	return result;
}

#ifdef CONFIG_RISCV_PMP
static int riscv_pmp_regions_translate(int start_reg_index,
	const struct riscv_pmp_region regions[], uint8_t regions_num,
	unsigned long pmpcfg[], unsigned long pmpaddr[])
{
	int reg_index = start_reg_index;

	for (int i = 0; i < regions_num; i++) {
		/*
		 * Empty region.
		 *
		 * Note: start = size = 0 is valid region (special case).
		 */
		if ((regions[i].size == 0U) && (regions[i].start != 0)) {
			continue;
		}

		/* Non-empty region. */
		reg_index = riscv_pmp_region_translate(reg_index, &regions[i],
			false, pmpcfg, pmpaddr);

		if (reg_index == -28) {
			LOG_ERR("no free PMP entry\n");
			return -28;
		}
	}

	return reg_index;
}
#endif

static int riscv_pmp_region_set(int index, const struct riscv_pmp_region *region)
{
	/* Don't check special case: start = size = 0 */
	if (!((region->start == 0) && (region->size == 0))) {
		/* Check 4 bytes alignment */
		if(!(((region->start & 0x3) == 0) &&
			((region->size & 0x3) == 0) &&
			(region->size))) {
			LOG_ERR("PMP address/size are not 4 bytes aligned\n");
			return -22;
		}
	}

	return riscv_pmp_region_translate(index, region, true, NULL, NULL);
}

static int riscv_pmp_regions_set_from_last(int last_reg_index,
	const struct riscv_pmp_region regions[], uint8_t regions_num)
{
	int reg_index = last_reg_index;

	for (int i = 0; i < regions_num; i++) {
		/*
		 * Empty region.
		 *
		 * Note: start = size = 0 is valid region (special case).
		 */
		if ((regions[i].size == 0U) && (regions[i].start != 0)) {
			continue;
		}

		/* Non-empty region. */
		int used_entry = 1;

		if (regions[i].mode == PMP_MODE_TOR) {
			used_entry = 2;
		}

		/* Update reg_index to next free entry (from last). */
		reg_index -= used_entry;

		/* Use reg_index+1 PMP entry. */
		riscv_pmp_region_set(reg_index+1, &regions[i]);
	}

	return reg_index;
}

void z_riscv_pmp_clear_config(void)
{
	uint8_t min_index = 0;
	uint8_t max_index = max_dynamic_region();
	uint8_t mprv_index = get_num_regions() - mprv_regions_num;

	LOG_DBG("Clear all dynamic PMP regions: (%d, %d) index\n",
		min_index, max_index);

	/* Clear all dynamic PMP regions (from min_index to max_index). */
	riscv_pmpcfg_set_range(min_index, max_index, NULL);

	/* Clear MPRV region which is also a dynamic region.
	 * It would be reconfigured when configuring M-mode dynamic region.
	 */
	riscv_pmpcfg_set_range(mprv_index, get_num_regions() - 1, NULL);
}

#ifdef CONFIG_RISCV_PMP
void z_riscv_init_user_accesses(struct k_pmp *pmp)
{
	struct riscv_pmp_region dynamic_regions[] = {
		{
			.start = pmp->stack_info.start,
			.size = pmp->stack_info.size,
			.perm = PMP_R ,
			.mode = PMP_MODE_DEFAULT,
		},
	};

	uint8_t index = 0;

	riscv_pmp_regions_translate(index, dynamic_regions,
		ARRAY_SIZE(dynamic_regions), pmp->u_pmpcfg,
		pmp->u_pmpaddr);
}


void z_riscv_configure_user_allowed_stack(struct k_pmp *pmp)
{
	uint8_t min_index = 0;
	uint8_t max_index = max_dynamic_region();

	z_riscv_pmp_clear_config();

	/* Set all dynamic PMP regions (from min_index to max_index). */
	for (uint8_t i = min_index; i <= max_index; i++){
		csr_write_enum(CSR_PMPADDR0 + i, pmp->u_pmpaddr[i]);
    }

	riscv_pmpcfg_set_range(min_index, max_index, (uint8_t *)pmp->u_pmpcfg);

	LOG_DBG("Apply user PMP context " PR_ADDR " to dynamic PMP regions: "
		"(%d, %d) index\n", (unsigned long) pmp->u_pmpcfg,
		min_index, max_index);
}

int z_riscv_pmp_add_dynamic(struct k_pmp *pmp,
			unsigned long addr,
			unsigned long size,
			unsigned char flags)
{
	unsigned char index = 0U;
	unsigned char *uchar_pmpcfg;
	unsigned char max_index = max_dynamic_region();
	int ret = 0;

	/* Check 4 bytes alignment */
	if(!(((addr & 0x3) == 0) && ((size & 0x3) == 0) && size)) {
		LOG_ERR("address/size are not 4 bytes aligned\n");
		ret = -22;
		goto out;
	}

	struct riscv_pmp_region pmp_region = {
		.start = addr,
		.size = size,
		.perm = flags,
	};

	/* Get next free entry */
	uchar_pmpcfg = (unsigned char *) pmp->u_pmpcfg;

	index = (1 + (1 * PMP_USED_ENTRY_DEFAULT));

	while ((index <= max_index) && uchar_pmpcfg[index]) {
		index++;
	}

	/* Select the best mode */
	if (size == 4) {
		pmp_region.mode = PMP_MODE_NA4;
	} else {
		pmp_region.mode = PMP_MODE_DEFAULT;
	}

	index = riscv_pmp_region_translate(index, &pmp_region, false,
		pmp->u_pmpcfg, pmp->u_pmpaddr);

	if (index == -28) {
		ret = -28;
	}

out:
	return ret;
}

/*
 * Check the 1st bit zero of value from the least significant bit to most one.
 */
static inline unsigned long count_trailing_one(unsigned long value)
{
	int shift = 0;

	while ((value >> shift) & 0x1) {
		shift++;
	}

	return shift;

}

/**
 * This internal function checks if the given buffer is in the region.
 *
 * Note:
 *   The caller must provide a valid region number.
 */
static inline int is_in_region(struct k_pmp *pmp, uint32_t index, uint32_t start, uint32_t size)
{
	unsigned long addr_start;
	unsigned long addr_end;
	unsigned long end;
	uint8_t pmpcfg = 0;
	unsigned long pmpaddr = 0;

	riscv_pmp_get_user_pmp(pmp, index, &pmpcfg, &pmpaddr);

	if ((pmpcfg & PMP_TYPE_MASK) == PMP_NA4) {
		addr_start = FROM_PMP_ADDR(pmpaddr);
		addr_end = addr_start + 4UL - 1UL;
	} else if ((pmpcfg & PMP_TYPE_MASK) == PMP_NAPOT) {
		unsigned long shift = count_trailing_one(pmpaddr);
		unsigned long bitmask = (1UL << (shift+1)) - 1UL;
		unsigned long size = FROM_PMP_ADDR(bitmask + 1UL);

		addr_start = FROM_PMP_ADDR(pmpaddr & ~bitmask);
		addr_end = addr_start - 1UL + size;
	} else if ((pmpcfg & PMP_TYPE_MASK) == PMP_TOR) {
		if (index == 0) {
			addr_start = 0;
			addr_end = FROM_PMP_ADDR(pmpaddr) - 1UL;
		} else {
			unsigned long pmpaddr_prev = csr_read_enum(CSR_PMPADDR0 + index - 1);

			addr_start = FROM_PMP_ADDR(pmpaddr_prev);
			addr_end = FROM_PMP_ADDR(pmpaddr) - 1UL;
		}
	} else {
		/* PMP_OFF: PMP region isn't enabled. */
		return 0;
	}

	size = size == 0U ? 0U : size - 1U;

	if (__builtin_add_overflow(start, size, (uint32_t *)&end)) {
		return 0;
	}


	if ((start >= addr_start) && (end <= addr_end)) {
		return 1;
	}

	return 0;
}

/**
 * This internal function checks if the region is user accessible or not.
 *
 * Note:
 *   The caller must provide a valid region number.
 */
static inline int is_user_accessible_region(struct k_pmp *pmp, uint32_t index, int write)
{
	uint8_t pmpcfg = 0;

	riscv_pmp_get_user_pmp(pmp, index, &pmpcfg, NULL);

	if (write != 0) {
		return (pmpcfg & PMP_W) == PMP_W;
	}
	return (pmpcfg & PMP_R) == PMP_R;
}

/**
 * This function validates whether a given memory buffer
 * is user accessible or not.
 */
int arch_buffer_validate(struct k_pmp *pmp, void *addr, size_t size, int write)
{
	uint32_t index;

	/* Iterate all PMP regions */
	for (index = 0; index < get_num_regions(); index++) {
		if (!is_in_region(pmp, index, (unsigned long)addr, size)) {
			continue;
		}

		/*
		 * For RISC-V PMP, low region number takes priority.
		 * Since we iterate all PMP regions, we can stop the iteration
		 * immediately once we find the matched region that
		 * grants permission or denies access.
		 */

		if (is_user_accessible_region(pmp, index, write)) {
			return 0;
		} else {
			return -1;
		}
	}

	return -1;
}

void z_riscv_pmp_init(struct k_pmp *pmp)
{
	unsigned char i;
	unsigned long *pmpcfg;

	pmpcfg = pmp->u_pmpcfg;
	for (i = 0U; i < RISCV_PMP_CFG_NUM; i++)
		pmpcfg[i] = 0;

}
#endif  /* CONFIG_RISCV_PMP */

void z_riscv_configure_static_pmp_regions(void)
{
	/* Max dynamic PMP entry is also next free static PMP entry (from last). */
	int index = max_dynamic_region();

    static_regions[0].size = __rom_region_end - __rom_region_start;

	int new_index = riscv_pmp_regions_set_from_last(index,
		static_regions, ARRAY_SIZE(static_regions));

	static_regions_num += index - new_index;
}
