/*
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_PM_DEVICE_H_
#define ZEPHYR_INCLUDE_PM_DEVICE_H_

#include <device.h>
#include <kernel.h>
#include <sys/atomic.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Device Power Management API
 * @defgroup subsys_pm_device Device
 * @ingroup subsys_pm
 * @{
 */

/** @cond INTERNAL_HIDDEN */

struct device;

/** @brief Device PM flags. */
enum pm_device_flag {

	PM_DEVICE_FLAG_WS_CAPABLE,
	/** Indicates if the device is being used as wakeup source. */
	PM_DEVICE_FLAG_WS_ENABLED,
};


/**
 * @brief Device PM info
 */
struct pm_device {

	/* Device PM status flags. */
	atomic_t flags;
};


//TODO: pm deivce define

/**
 * @brief Utility macro to initialize #pm_device.
 *
 * @note #DT_PROP_OR is used to retrieve the wakeup_source property because
 * it may not be defined on all devices.
 *
 * @param obj Name of the #pm_device structure being initialized.
 * @param node_id Devicetree node for the initialized device (can be invalid).
 * @param pm_action_cb Device PM control callback function.
 */
#define Z_PM_DEVICE_INIT(obj, node_id, pm_action_cb)			\
	{								\
		Z_PM_DEVICE_RUNTIME_INIT(obj)				\
		.action_cb = pm_action_cb,				\
		.state = PM_DEVICE_STATE_ACTIVE,			\
		.flags = ATOMIC_INIT(COND_CODE_1(			\
				DT_NODE_EXISTS(node_id),		\
				((DT_PROP_OR(node_id, wakeup_source, 0) \
				  << PM_DEVICE_FLAG_WS_CAPABLE) |	\
				 (DT_NODE_HAS_COMPAT(node_id, power_domain) << \
				  PM_DEVICE_FLAG_PD)), (0))),		\
		Z_PM_DEVICE_POWER_DOMAIN_INIT(node_id)			\
	}

/**
 * Get the name of device PM resources.
 *
 * @param dev_name Device name.
 */
#define Z_PM_DEVICE_NAME(dev_name) _CONCAT(__pm_device__, dev_name)

/**
 * @brief Define device PM slot.
 *
 * This macro defines a pointer to a device in the z_pm_device_slots region.
 * When invoked for each device with PM, it will effectively result in a device
 * pointer array with the same size of the actual devices with PM enabled. This
 * is used internally by the PM subsystem to keep track of suspended devices
 * during system power transitions.
 *
 * @param dev_name Device name.
 */
#define Z_PM_DEVICE_DEFINE_SLOT(dev_name)				\
	static const Z_DECL_ALIGN(struct device *)			\
	_CONCAT(Z_PM_DEVICE_NAME(dev_name), _slot) __used		\
	__attribute__((__section__(".z_pm_device_slots")))

#ifdef CONFIG_PM_DEVICE
/**
 * Define device PM resources for the given node identifier.
 *
 * @param node_id Node identifier (DT_INVALID_NODE if not a DT device).
 * @param dev_name Device name.
 * @param pm_action_cb PM control callback.
 */
#define Z_PM_DEVICE_DEFINE(node_id, dev_name, pm_action_cb)		\
	Z_PM_DEVICE_DEFINE_SLOT(dev_name);				\
	static struct pm_device Z_PM_DEVICE_NAME(dev_name) =		\
	Z_PM_DEVICE_INIT(Z_PM_DEVICE_NAME(dev_name), node_id,		\
			 pm_action_cb)

/**
 * Get a reference to the device PM resources.
 *
 * @param dev_name Device name.
 */
#define Z_PM_DEVICE_GET(dev_name) (&Z_PM_DEVICE_NAME(dev_name))

#else
#define Z_PM_DEVICE_DEFINE(node_id, dev_name, pm_action_cb)
#define Z_PM_DEVICE_GET(dev_name) NULL
#endif /* CONFIG_PM_DEVICE */

/** @endcond */

/**
 * Define device PM resources for the given device name.
 *
 * @note This macro is a no-op if @kconfig{CONFIG_PM_DEVICE} is not enabled.
 *
 * @param dev_name Device name.
 * @param pm_action_cb PM control callback.
 *
 * @see #PM_DEVICE_DT_DEFINE, #PM_DEVICE_DT_INST_DEFINE
 */
#define PM_DEVICE_DEFINE(dev_name, pm_action_cb) \
	Z_PM_DEVICE_DEFINE(DT_INVALID_NODE, dev_name, pm_action_cb)

/**
 * Define device PM resources for the given node identifier.
 *
 * @note This macro is a no-op if @kconfig{CONFIG_PM_DEVICE} is not enabled.
 *
 * @param node_id Node identifier.
 * @param pm_action_cb PM control callback.
 *
 * @see #PM_DEVICE_DT_INST_DEFINE, #PM_DEVICE_DEFINE
 */
#define PM_DEVICE_DT_DEFINE(node_id, pm_action_cb)			\
	Z_PM_DEVICE_DEFINE(node_id, Z_DEVICE_DT_DEV_NAME(node_id),	\
			   pm_action_cb)

/**
 * Define device PM resources for the given instance.
 *
 * @note This macro is a no-op if @kconfig{CONFIG_PM_DEVICE} is not enabled.
 *
 * @param idx Instance index.
 * @param pm_action_cb PM control callback.
 *
 * @see #PM_DEVICE_DT_DEFINE, #PM_DEVICE_DEFINE
 */
#define PM_DEVICE_DT_INST_DEFINE(idx, pm_action_cb)			\
	Z_PM_DEVICE_DEFINE(DT_DRV_INST(idx),				\
			   Z_DEVICE_DT_DEV_NAME(DT_DRV_INST(idx)),	\
			   pm_action_cb)

/**
 * @brief Obtain a reference to the device PM resources for the given device.
 *
 * @param dev_name Device name.
 *
 * @return Reference to the device PM resources (NULL if device
 * @kconfig{CONFIG_PM_DEVICE} is disabled).
 */
#define PM_DEVICE_GET(dev_name) \
	Z_PM_DEVICE_GET(dev_name)

/**
 * @brief Obtain a reference to the device PM resources for the given node.
 *
 * @param node_id Node identifier.
 *
 * @return Reference to the device PM resources (NULL if device
 * @kconfig{CONFIG_PM_DEVICE} is disabled).
 */
#define PM_DEVICE_DT_GET(node_id) \
	PM_DEVICE_GET(Z_DEVICE_DT_DEV_NAME(node_id))

/**
 * @brief Obtain a reference to the device PM resources for the given instance.
 *
 * @param idx Instance index.
 *
 * @return Reference to the device PM resources (NULL if device
 * @kconfig{CONFIG_PM_DEVICE} is disabled).
 */
#define PM_DEVICE_DT_INST_GET(idx) \
	PM_DEVICE_DT_GET(DT_DRV_INST(idx))

#ifdef CONFIG_PM_DEVICE
/**
 * @brief Enable or disable a device as a wake up source.
 *
 * A device marked as a wake up source will not be suspended when the system
 * goes into low-power modes, thus allowing to use it as a wake up source for
 * the system.
 *
 * @param dev Device instance.
 * @param enable @c true to enable or @c false to disable
 *
 * @retval true If the wakeup source was successfully enabled.
 * @retval false If the wakeup source was not successfully enabled.
 */
bool pm_device_wakeup_enable(const struct device *dev, bool enable);

/**
 * @brief Check if a device is enabled as a wake up source.
 *
 * @param dev Device instance.
 *
 * @retval true if the wakeup source is enabled.
 * @retval false if the wakeup source is not enabled.
 */
bool pm_device_wakeup_is_enabled(const struct device *dev);

/**
 * @brief Check if a device is wake up capable
 *
 * @param dev Device instance.
 *
 * @retval true If the device is wake up capable.
 * @retval false If the device is not wake up capable.
 */
bool pm_device_wakeup_is_capable(const struct device *dev);

#else
static inline bool pm_device_wakeup_enable(const struct device *dev,
					   bool enable)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(enable);
	return false;
}
static inline bool pm_device_wakeup_is_enabled(const struct device *dev)
{
	ARG_UNUSED(dev);
	return false;
}
static inline bool pm_device_wakeup_is_capable(const struct device *dev)
{
	ARG_UNUSED(dev);
	return false;
}

#endif /* CONFIG_PM_DEVICE */

/** @} */

#ifdef __cplusplus
}
#endif

#endif
