#ifndef INCLUDE_DRIVERS_ADC_H
#define INCLUDE_DRIVERS_ADC_H

/**
 * @brief ADC driver APIs
 * @defgroup adc_interface ADC driver APIs
 * @ingroup io_interfaces
 * @{
 */

/** Acquisition time is expressed in microseconds. */
#define ADC_ACQ_TIME_MICROSECONDS  (1)
/** Acquisition time is expressed in nanoseconds. */
#define ADC_ACQ_TIME_NANOSECONDS   (2)
/** Acquisition time is expressed in ADC ticks. */
#define ADC_ACQ_TIME_TICKS         (3)
/** Macro for composing the acquisition time value in given units. */
#define ADC_ACQ_TIME(unit, value)  (((unit) << 14) | ((value) & BIT_MASK(14)))
/** Value indicating that the default acquisition time should be used. */
#define ADC_ACQ_TIME_DEFAULT       0
#define ADC_ACQ_TIME_MAX           BIT_MASK(14)

#define ADC_ACQ_TIME_UNIT(time)    (((time) >> 14) & BIT_MASK(2))
#define ADC_ACQ_TIME_VALUE(time)   ((time) & BIT_MASK(14))

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
 * @brief Structure for specifying the configuration of an ADC channel.
 */
struct adc_channel_cfg {
	/** Gain selection. */
	enum adc_gain gain;

	/**
	 * Acquisition time.
	 * Use the ADC_ACQ_TIME macro to compose the value for this field or
	 * pass ADC_ACQ_TIME_DEFAULT to use the default setting for a given
	 * hardware (e.g. when the hardware does not allow to configure the
	 * acquisition time).
	 * Particular drivers do not necessarily support all the possible units.
	 * Value range is 0-16383 for a given unit.
	 */
	uint16_t acquisition_time;

	/**
	 * Perform calibration before the reading is taken if requested.
	 *
	 * The impact of channel configuration on the calibration
	 * process is specific to the underlying hardware.  ADC
	 * implementations that do not support calibration should
	 * ignore this flag.
	 */
	bool calibrate;

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

/* Forward declaration of the adc_sequence structure. */
struct adc_sequence;

/**
 * @brief Action to be performed after a sampling is done.
 */
enum adc_action {
	/** The sequence should be continued normally. */
	ADC_ACTION_CONTINUE = 0,

	/**
	 * The sampling should be repeated. New samples or sample should be
	 * read from the ADC and written in the same place as the recent ones.
	 */
	ADC_ACTION_REPEAT,

	/** The sequence should be finished immediately. */
	ADC_ACTION_FINISH,
};

/**
 * @brief Type definition of the optional callback function to be called after
 *        a requested sampling is done.
 *
 * @param dev             Pointer to the device structure for the driver
 *                        instance.
 * @param sequence        Pointer to the sequence structure that triggered
 *                        the sampling. This parameter points to a copy of
 *                        the structure that was supplied to the call that
 *                        started the sampling sequence, thus it cannot be
 *                        used with the CONTAINER_OF() macro to retrieve
 *                        some other data associated with the sequence.
 *                        Instead, the adc_sequence_options::user_data field
 *                        should be used for such purpose.
 *
 * @param sampling_index  Index (0-65535) of the sampling done.
 *
 * @returns Action to be performed by the driver. See @ref adc_action.
 */
typedef enum adc_action (*adc_sequence_callback)(const struct device *dev,
						 const struct adc_sequence *sequence,
						 uint16_t sampling_index);

/**
 * @brief Structure defining additional options for an ADC sampling sequence.
 */
struct adc_sequence_options {

	/**
	 * Callback function to be called after each sampling is done.
	 * Optional - set to NULL if it is not needed.
	 */
	adc_sequence_callback callback;

	/**
	 * Pointer to user data. It can be used to associate the sequence
	 * with any other data that is needed in the callback function.
	 */
	void *user_data;

	/**
	 * Number of extra samplings to perform (the total number of samplings
	 * is 1 + extra_samplings).
	 */
	uint16_t extra_samplings;
};

/**
 * @brief Structure defining an ADC sampling sequence.
 */
struct adc_sequence {
	/**
	 * Pointer to a structure defining additional options for the sequence.
	 * If NULL, the sequence consists of a single sampling.
	 */
	const struct adc_sequence_options *options;
	
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
};

/**
 * @brief Type definition of ADC API function for configuring a channel.
 * See adc_channel_setup() for argument descriptions.
 */
typedef int (*adc_api_channel_setup)(const struct device *dev,
				     const struct adc_channel_cfg *channel_cfg);


/**
 * @brief Type definition of ADC API function for getting the channel configuration.
 * See adc_channel_config_get() for argument descriptions.
 */
typedef struct adc_channel_cfg (*adc_api_channel_config_get)(
					const struct device *dev);

/**
 * @brief Type definition of ADC API function for setting a read request.
 * See adc_read() for argument descriptions.
 */
typedef int (*adc_api_read)(const struct device *dev,
			    const struct adc_sequence *sequence);

/**
 * @brief Type definition of ADC API function for checking 
 * if the read request is complete.
 * See adc_read_is_completed() for argument descriptions.
 */
typedef bool (*adc_api_is_completed)(const struct device *dev);

typedef int (*adc_api_convert)(const struct device *dev,
				const struct adc_channel_cfg *channel_cfg, 
				int32_t *pval);

/**
 * @brief ADC driver API
 *
 * This is the mandatory API any ADC driver needs to expose.
 */
struct adc_driver_api {
	adc_api_channel_setup 		channel_setup;
	adc_api_channel_config_get	get_config;
	adc_api_read          		read;
	adc_api_is_completed  		is_completed;
	adc_api_convert		  		convert;
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
 * @brief Get the ADC channel configuration.
 * 
 * @param dev 	Pointer to the device structure for the driver instance.
 * 
 * @return 		Channel configuration.
 */
static inline struct adc_channel_cfg adc_channel_config_get(const struct device *dev)
{
	const struct adc_driver_api *api =
				(const struct adc_driver_api *)dev->api;

	return api->get_config(dev);
}

/**
 * @brief Set a read request.
 *
 * @param dev       Pointer to the device structure for the driver instance.
 * @param sequence  Structure specifying requested sequence of samplings.
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
 * @brief Check if the read request is complete.
 * 
 * @param dev 		Pointer to the device structure for the driver instance.
 * 
 * @return true 	The read request completed.
 * @return false 	The read request not completed.
 */
static inline bool adc_read_is_completed(const struct device *dev)
{
	const struct adc_driver_api *api =
				(const struct adc_driver_api *)dev->api;

	return api->is_completed(dev);
} 

/**
 * @brief Convert a raw ADC value to millivolts or celsius.
 *
 *
 * @retval 0 on successful conversion
 * @retval -EINVAL if the gain is not reversible
 */
static inline int adc_convert(const struct device *dev,
					const struct adc_channel_cfg *channel_cfg, 
					int32_t *pval)
{
	const struct adc_driver_api *api =
				(const struct adc_driver_api *)dev->api;

	return api->convert(dev, channel_cfg, pval);
}

#endif /* INCLUDE_DRIVERS_ADC_H */