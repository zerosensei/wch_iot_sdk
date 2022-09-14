/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DRIVERS_ADC_ADC_CONTEXT_H
#define DRIVERS_ADC_ADC_CONTEXT_H

#include <drivers/adc.h>
#include <sys/atomic.h>

#ifdef __cplusplus
extern "C" {
#endif

struct adc_context;

/*
 * Each driver should provide implementations of the following two functions:
 * - adc_context_start_sampling() that will be called when a sampling (of one
 *   or more channels, depending on the realized sequence) is to be started
 * - adc_context_update_buffer_pointer() that will be called when the sample
 *   buffer pointer should be prepared for writing of next sampling results,
 *   the "repeat_sampling" parameter indicates if the results should be written
 *   in the same place as before (when true) or as consecutive ones (otherwise).
 */
static void adc_context_start_sampling(struct adc_context *ctx);
static void adc_context_update_buffer_pointer(struct adc_context *ctx,
					      bool repeat_sampling);

struct adc_context {
	atomic_t status;
	struct adc_sequence sequence;
	struct adc_sequence_options options;
	uint16_t sampling_index;
};


static inline int adc_context_status(struct adc_context *ctx)
{
	return atomic_get(&ctx->status);
}

static inline void adc_context_complete(struct adc_context *ctx, int status)
{
    ctx->status = status;
    atomic_set(&ctx->status, status);
}

static inline void adc_context_start_read(struct adc_context *ctx,
					  const struct adc_sequence *sequence)
{
	ctx->sequence = *sequence;
    atomic_set(&ctx->status, 1);

	if (sequence->options) {
		ctx->options = *sequence->options;
		ctx->sequence.options = &ctx->options;
		ctx->sampling_index = 0U;
	}

	adc_context_start_sampling(ctx);
}

/*
 * This function should be called after a sampling (of one or more channels,
 * depending on the realized sequence) is done. It calls the defined callback
 * function if required and takes further actions accordingly.
 */
static inline void adc_context_on_sampling_done(struct adc_context *ctx,
						const struct device *dev)
{
	if (ctx->sequence.options) {
		adc_sequence_callback callback = ctx->options.callback;
		enum adc_action action;
		bool finish = false;
		bool repeat = false;

		if (callback) {
			action = callback(dev,
					  &ctx->sequence,
					  ctx->sampling_index);
		} else {
			action = ADC_ACTION_CONTINUE;
		}

		switch (action) {
		case ADC_ACTION_REPEAT:
			repeat = true;
			break;
		case ADC_ACTION_FINISH:
			finish = true;
			break;
		default: /* ADC_ACTION_CONTINUE */
			if (ctx->sampling_index <
			    ctx->options.extra_samplings) {
				++ctx->sampling_index;
			} else {
				finish = true;
			}
		}

		if (!finish) {
			adc_context_update_buffer_pointer(ctx, repeat);
            adc_context_start_sampling(ctx);

			return;
		}
	}

	adc_context_complete(ctx, 0);
}

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_ADC_ADC_CONTEXT_H */

