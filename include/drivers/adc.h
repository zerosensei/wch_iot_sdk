#ifndef INCLUDE_DRIVERS_ADC_H
#define INCLUDE_DRIVERS_ADC_H

/**
 * @brief ADC driver APIs
 * @defgroup adc_interface ADC driver APIs
 * @ingroup io_interfaces
 * @{
 */

/** @brief ADC channel gain factors. */
enum adc_gain {
	ADC_GAIN_1_6, /**< x 1/6. */
	ADC_GAIN_1_5, /**< x 1/5. */
	ADC_GAIN_1_4, /**< x 1/4. */
	ADC_GAIN_1_3, /**< x 1/3. */
	ADC_GAIN_2_5, /**< x 2/5. */
	ADC_GAIN_1_2, /**< x 1/2. */
	ADC_GAIN_2_3, /**< x 2/3. */
	ADC_GAIN_4_5, /**< x 4/5. */
	ADC_GAIN_1,   /**< x 1. */
	ADC_GAIN_2,   /**< x 2. */
	ADC_GAIN_3,   /**< x 3. */
	ADC_GAIN_4,   /**< x 4. */
	ADC_GAIN_6,   /**< x 6. */
	ADC_GAIN_8,   /**< x 8. */
	ADC_GAIN_12,  /**< x 12. */
	ADC_GAIN_16,  /**< x 16. */
	ADC_GAIN_24,  /**< x 24. */
	ADC_GAIN_32,  /**< x 32. */
	ADC_GAIN_64,  /**< x 64. */
	ADC_GAIN_128, /**< x 128. */
};

/**
 * @brief Invert the application of gain to a measurement value.
 *
 *
 * @param gain the gain used to amplify the input signal.
 *
 * @param value a pointer to a value that initially has the effect of
 * the applied gain but has that effect removed when this function
 * successfully returns.  If the gain cannot be reversed the value
 * remains unchanged.
 *
 * @retval 0 if the gain was successfully reversed
 * @retval -EINVAL if the gain could not be interpreted
 */
int adc_wch_gain_invert(enum adc_gain gain,
		    int32_t *value);

/** @brief ADC references. */
enum adc_reference {
	ADC_REF_VDD_1,     /**< VDD. */
	ADC_REF_VDD_1_2,   /**< VDD/2. */
	ADC_REF_VDD_1_3,   /**< VDD/3. */
	ADC_REF_VDD_1_4,   /**< VDD/4. */
	ADC_REF_INTERNAL,  /**< Internal. */
	ADC_REF_EXTERNAL0, /**< External, input 0. */
	ADC_REF_EXTERNAL1, /**< External, input 1. */
};

/**
 * @brief Structure for specifying the configuration of an ADC channel.
 */
struct adc_channel_cfg {
	/** Gain selection. */
	enum adc_gain gain;

	/** Reference selection. */
	enum adc_reference reference;
	
	/**
	 * Channel identifier.
	 * This value primarily identifies the channel within the ADC API - when
	 * a read request is done, the corresponding bit in the "channels" field
	 * of the "adc_sequence" structure must be set to include this channel
	 * in the sampling.
	 */
	uint8_t channel_id   : 5;

	/** Channel type: single-ended or differential. */
	uint8_t differential : 1;
};

/**
 * @brief Structure defining an ADC sampling sequence.
 */
struct adc_sequence {
	/**
	 * Bit-mask indicating the channels to be included in each sampling
	 * of this sequence.
	 * All selected channels must be configured with adc_channel_setup()
	 * before they are used in a sequence.
	 */
	uint32_t channels;

	/**
	 * Pointer to a buffer where the samples are to be written. Samples
	 * from subsequent samplings are written sequentially in the buffer.
	 * The number of samples written for each sampling is determined by
	 * the number of channels selected in the "channels" field.
	 * The buffer must be of an appropriate size, taking into account
	 * the number of selected channels and the ADC resolution used,
	 * as well as the number of samplings contained in the sequence.
	 */
	void *buffer;

	/**
	 * Specifies the actual size of the buffer pointed by the "buffer"
	 * field (in bytes). The driver must ensure that samples are not
	 * written beyond the limit and it must return an error if the buffer
	 * turns out to be not large enough to hold all the requested samples.
	 */
	size_t buffer_size;

	/**
	 * ADC resolution.
	 * For single-ended channels the sample values are from range:
	 *   0 .. 2^resolution - 1,
	 */
	uint8_t resolution;

	/**
	 * Perform calibration before the reading is taken if requested.
	 *
	 * The impact of channel configuration on the calibration
	 * process is specific to the underlying hardware.  ADC
	 * implementations that do not support calibration should
	 * ignore this flag.
	 */
	bool calibrate;
};

/**
 * @brief Type definition of ADC API function for configuring a channel.
 * See adc_channel_setup() for argument descriptions.
 */
typedef int (*adc_api_channel_setup)(const struct device *dev,
				     const struct adc_channel_cfg *channel_cfg);

/**
 * @brief Type definition of ADC API function for setting a read request.
 * See adc_read() for argument descriptions.
 */
typedef int (*adc_api_read)(const struct device *dev,
			    const struct adc_sequence *sequence);

/**
 * @brief Type definition of ADC API function for setting an asynchronous
 *        read request.
 * See adc_read_async() for argument descriptions.
 */
typedef int (*adc_api_read_async)(const struct device *dev,
				  const struct adc_sequence *sequence,
				  struct k_poll_signal *async);

/**
 * @brief ADC driver API
 *
 * This is the mandatory API any ADC driver needs to expose.
 */
struct adc_driver_api {
	adc_api_channel_setup channel_setup;
	adc_api_read          read;
#ifdef CONFIG_ADC_ASYNC
	adc_api_read_async    read_async;
#endif
	uint16_t ref_internal;	/* mV */
};

/**
 * @brief Configure an ADC channel.
 *
 * It is required to call this function and configure each channel before it is
 * selected for a read request.
 *
 * @param dev          Pointer to the device structure for the driver instance.
 * @param channel_cfg  Channel configuration.
 *
 * @retval 0       On success.
 * @retval -EINVAL If a parameter with an invalid value has been provided.
 */
static inline int adc_channel_setup(const struct device *dev,
					   const struct adc_channel_cfg *channel_cfg)
{
	const struct adc_driver_api *api =
				(const struct adc_driver_api *)dev->api;

	return api->channel_setup(dev, channel_cfg);
}

/**
 * @brief Set a read request.
 *
 * @param dev       Pointer to the device structure for the driver instance.
 * @param sequence  Structure specifying requested sequence of samplings.
 *
 * If invoked from user mode, any sequence struct options for callback must
 * be NULL.
 *
 * @retval 0        On success.
 * @retval -EINVAL  If a parameter with an invalid value has been provided.
 * @retval -ENOMEM  If the provided buffer is to small to hold the results
 *                  of all requested samplings.
 * @retval -ENOTSUP If the requested mode of operation is not supported.
 * @retval -EBUSY   If another sampling was triggered while the previous one
 *                  was still in progress. This may occur only when samplings
 *                  are done with intervals, and it indicates that the selected
 *                  interval was too small. All requested samples are written
 *                  in the buffer, but at least some of them were taken with
 *                  an extra delay compared to what was scheduled.
 */
static inline int adc_read(const struct device *dev,
				  const struct adc_sequence *sequence)
{
	const struct adc_driver_api *api =
				(const struct adc_driver_api *)dev->api;

	return api->read(dev, sequence);
}

/**
 * @brief Set an asynchronous read request.
 *
 * @note This function is available only if @kconfig{CONFIG_ADC_ASYNC}
 * is selected.
 *
 * If invoked from user mode, any sequence struct options for callback must
 * be NULL.
 *
 * @param dev       Pointer to the device structure for the driver instance.
 * @param sequence  Structure specifying requested sequence of samplings.
 * @param async     Pointer to a valid and ready to be signaled struct
 *                  k_poll_signal. (Note: if NULL this function will not notify
 *                  the end of the transaction, and whether it went successfully
 *                  or not).
 *
 * @returns 0 on success, negative error code otherwise.
 *          See adc_read() for a list of possible error codes.
 *
 */
static inline int adc_read_async(const struct device *dev,
					const struct adc_sequence *sequence,
					struct k_poll_signal *async)
{
#ifdef CONFIG_ADC_ASYNC
	const struct adc_driver_api *api =
				(const struct adc_driver_api *)dev->api;

	return api->read_async(dev, sequence, async);
#else
	ARG_UNUSED(dev);
	ARG_UNUSED(sequence);
	ARG_UNUSED(async);
	return -ENOTSUP;
#endif
}

/**
 * @brief Get the internal reference voltage.
 *
 * Returns the voltage corresponding to @ref ADC_REF_INTERNAL,
 * measured in millivolts.
 *
 * @return a positive value is the reference voltage value.  Returns
 * zero if reference voltage information is not available.
 */
static inline uint16_t adc_ref_internal(const struct device *dev)
{
	const struct adc_driver_api *api =
				(const struct adc_driver_api *)dev->api;

	return api->ref_internal;
}

/**
 * @brief Convert a raw ADC value to millivolts.
 *
 * This function performs the necessary conversion to transform a raw
 * ADC measurement to a voltage in millivolts.
 *
 * @param ref_mv the reference voltage used for the measurement, in
 * millivolts.  This may be from adc_ref_internal() or a known
 * external reference.
 *
 * @param gain the ADC gain configuration used to sample the input
 *
 * @param resolution the number of bits in the absolute value of the
 * sample.  For differential sampling this needs to be one less than the
 * resolution in struct adc_sequence.
 *
 * @param valp pointer to the raw measurement value on input, and the
 * corresponding millivolt value on successful conversion.  If
 * conversion fails the stored value is left unchanged.
 *
 * @retval 0 on successful conversion
 * @retval -EINVAL if the gain is not reversible
 */
static inline int adc_raw_to_millivolts(int32_t ref_mv,
					enum adc_gain gain,
					uint8_t resolution,
					int32_t *valp)
{
	int32_t adc_mv = *valp * ref_mv;
	int ret = adc_gain_invert(gain, &adc_mv);

	if (ret == 0) {
		*valp = (adc_mv >> resolution);
	}

	return ret;
}


#endif /* INCLUDE_DRIVERS_ADC_H */