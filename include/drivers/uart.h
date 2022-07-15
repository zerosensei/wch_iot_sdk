#ifndef INCLUDE_DRIVERS_UART_H
#define INCLUDE_DRIVERS_UART_H

#include <errno.h>
#include <device.h>

/**
 * @brief Reception stop reasons.
 *
 * Values that correspond to events or errors responsible for stopping
 * receiving.
 */
enum uart_rx_stop_reason {
	/** @brief Overrun error */
	UART_ERROR_OVERRUN = (1 << 0),
	/** @brief Parity error */
	UART_ERROR_PARITY  = (1 << 1),
	/** @brief Framing error */
	UART_ERROR_FRAMING = (1 << 2),
	/**
	 * @brief Break interrupt
	 *
	 * A break interrupt was received. This happens when the serial input
	 * is held at a logic '0' state for longer than the sum of
	 * start time + data bits + parity + stop bits.
	 */
	UART_BREAK = (1 << 3),
	/**
	 * @brief Collision error
	 *
	 * This error is raised when transmitted data does not match
	 * received data. Typically this is useful in scenarios where
	 * the TX and RX lines maybe connected together such as
	 * RS-485 half-duplex. This error is only valid on UARTs that
	 * support collision checking.
	 */
	UART_ERROR_COLLISION = (1 << 4),
};

/** @brief Parity modes */
enum uart_config_parity {
	UART_CFG_PARITY_NONE,
	UART_CFG_PARITY_ODD,
	UART_CFG_PARITY_EVEN,
	UART_CFG_PARITY_MARK,
	UART_CFG_PARITY_SPACE,
};

/** @brief Number of stop bits. */
enum uart_config_stop_bits {
	UART_CFG_STOP_BITS_0_5,
	UART_CFG_STOP_BITS_1,
	UART_CFG_STOP_BITS_1_5,
	UART_CFG_STOP_BITS_2,
};

/** @brief Number of data bits. */
enum uart_config_data_bits {
	UART_CFG_DATA_BITS_5,
	UART_CFG_DATA_BITS_6,
	UART_CFG_DATA_BITS_7,
	UART_CFG_DATA_BITS_8,
	UART_CFG_DATA_BITS_9,
};

/**
 * @brief Hardware flow control options.
 *
 * flow control is managed by hardware/driver.
 */
enum uart_config_flow_control {
	UART_CFG_FLOW_CTRL_NONE,
	UART_CFG_FLOW_CTRL_RTS_CTS,
	UART_CFG_FLOW_CTRL_DTR_DSR,
};

/**
 * @brief UART controller configuration structure
 *
 * @param baudrate  Baudrate setting in bps
 * @param parity    Parity bit, use @ref uart_config_parity
 * @param stop_bits Stop bits, use @ref uart_config_stop_bits
 * @param data_bits Data bits, use @ref uart_config_data_bits
 * @param flow_ctrl Flow control setting, use @ref uart_config_flow_control
 */
struct uart_config {
	uint32_t baudrate;
	uint8_t parity;
	uint8_t stop_bits;
	uint8_t data_bits;
	uint8_t flow_ctrl;
};

/**
 * @defgroup uart_interrupt Interrupt-driven UART API
 * @{
 */

/**
 * @brief Define the application callback function signature for
 * uart_irq_callback_user_data_set() function.
 *
 * @param dev UART device instance.
 * @param user_data Arbitrary user data.
 */
typedef void (*uart_irq_callback_user_data_t)(const struct device *dev,
					      void *user_data);

/**
 * @}
 */

/**
 * @cond INTERNAL_HIDDEN
 *
 * For internal driver use only, skip these in public documentation.
 */

/** @brief Driver API structure. */
struct uart_driver_api {
	/** Console I/O function */
	int (*poll_in)(const struct device *dev, unsigned char *p_char);
	void (*poll_out)(const struct device *dev, unsigned char out_char);

	/** Console I/O function */
	int (*err_check)(const struct device *dev);

	/** UART configuration functions */
	int (*configure)(const struct device *dev,
			 const struct uart_config *cfg);
	int (*config_get)(const struct device *dev, struct uart_config *cfg);

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	/** Interrupt driven FIFO fill function */
	int (*fifo_fill)(const struct device *dev, const uint8_t *tx_data,
			 int len);

	/** Interrupt driven FIFO read function */
	int (*fifo_read)(const struct device *dev, uint8_t *rx_data,
			 const int size);

	/** Interrupt driven transfer enabling function */
	void (*irq_tx_enable)(const struct device *dev);

	/** Interrupt driven transfer disabling function */
	void (*irq_tx_disable)(const struct device *dev);

	/** Interrupt driven transfer ready function */
	int (*irq_tx_ready)(const struct device *dev);

	/** Interrupt driven receiver enabling function */
	void (*irq_rx_enable)(const struct device *dev);

	/** Interrupt driven receiver disabling function */
	void (*irq_rx_disable)(const struct device *dev);

	/** Interrupt driven transfer complete function */
	int (*irq_tx_complete)(const struct device *dev);

	/** Interrupt driven receiver ready function */
	int (*irq_rx_ready)(const struct device *dev);

	/** Interrupt driven error enabling function */
	void (*irq_err_enable)(const struct device *dev);

	/** Interrupt driven error disabling function */
	void (*irq_err_disable)(const struct device *dev);

	/** Interrupt driven pending status function */
	int (*irq_is_pending)(const struct device *dev);

	/** Interrupt driven interrupt update function */
	int (*irq_update)(const struct device *dev);

	/** Set the irq callback function */
	void (*irq_callback_set)(const struct device *dev,
				 uart_irq_callback_user_data_t cb,
				 void *user_data);

#endif

};

/** @endcond */

/**
 * @brief Check whether an error was detected.
 *
 * @param dev UART device instance.
 *
 * @retval 0 If no error was detected.
 * @retval err Error flags as defined in @ref uart_rx_stop_reason
 * @retval -ENOSYS If not implemented.
 */
static inline int uart_err_check(const struct device *dev)
{
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->err_check == NULL) {
		return -ENOSYS;
	}

	return api->err_check(dev);
}

/**
 * @defgroup uart_polling Polling UART API
 * @{
 */

/**
 * @brief Read a character from the device for input.
 *
 * This routine checks if the receiver has valid data.  When the
 * receiver has valid data, it reads a character from the device,
 * stores to the location pointed to by p_char, and returns 0 to the
 * calling thread. It returns -1, otherwise. This function is a
 * non-blocking call.
 *
 * @param dev UART device instance.
 * @param p_char Pointer to character.
 *
 * @retval 0 If a character arrived.
 * @retval -1 If no character was available to read (i.e. the UART
 *            input buffer was empty).
 * @retval -ENOSYS If the operation is not implemented.
 * @retval -EBUSY If async reception was enabled using @ref uart_rx_enable
 */
static inline int uart_poll_in(const struct device *dev, unsigned char *p_char)
{
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->poll_in == NULL) {
		return -ENOSYS;
	}

	return api->poll_in(dev, p_char);
}

/**
 * @brief Read a 16-bit datum from the device for input.
 *
 * This routine checks if the receiver has valid data.  When the
 * receiver has valid data, it reads a 16-bit datum from the device,
 * stores to the location pointed to by p_u16, and returns 0 to the
 * calling thread. It returns -1, otherwise. This function is a
 * non-blocking call.
 *
 * @param dev UART device instance.
 * @param p_u16 Pointer to 16-bit data.
 *
 * @retval 0  If data arrived.
 * @retval -1 If no data was available to read (i.e., the UART
 *            input buffer was empty).
 * @retval -ENOTSUP If API is not enabled.
 * @retval -ENOSYS If the function is not implemented.
 * @retval -EBUSY If async reception was enabled using @ref uart_rx_enable
 */
static inline int uart_poll_in_u16(const struct device *dev, uint16_t *p_u16)
{
#ifdef CONFIG_UART_WIDE_DATA
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->poll_in_u16 == NULL) {
		return -ENOSYS;
	}

	return api->poll_in_u16(dev, p_u16);
#else
	ARG_UNUSED(dev);
	ARG_UNUSED(p_u16);
	return -ENOTSUP;
#endif
}

/**
 * @brief Write a character to the device for output.
 *
 * This routine checks if the transmitter is full.  When the
 * transmitter is not full, it writes a character to the data
 * register. It waits and blocks the calling thread, otherwise. This
 * function is a blocking call.
 *
 * To send a character when hardware flow control is enabled, the handshake
 * signal CTS must be asserted.
 *
 * @param dev UART device instance.
 * @param out_char Character to send.
 */
static inline void uart_poll_out(const struct device *dev,
			     unsigned char out_char)
{
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	api->poll_out(dev, out_char);
}

/**
 * @brief Write a 16-bit datum to the device for output.
 *
 * This routine checks if the transmitter is full. When the
 * transmitter is not full, it writes a 16-bit datum to the data
 * register. It waits and blocks the calling thread, otherwise. This
 * function is a blocking call.
 *
 * To send a datum when hardware flow control is enabled, the handshake
 * signal CTS must be asserted.
 *
 * @param dev UART device instance.
 * @param out_u16 Wide data to send.
 */
static inline void uart_poll_out_u16(const struct device *dev, uint16_t out_u16)
{
#ifdef CONFIG_UART_WIDE_DATA
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	api->poll_out_u16(dev, out_u16);
#else
	ARG_UNUSED(dev);
	ARG_UNUSED(out_u16);
#endif
}

/**
 * @}
 */

/**
 * @brief Set UART configuration.
 *
 * Sets UART configuration using data from *cfg.
 *
 * @param dev UART device instance.
 * @param cfg UART configuration structure.
 *
 * @retval 0 If successful.
 * @retval -errno Negative errno code in case of failure.
 * @retval -ENOSYS If configuration is not supported by device
 *                  or driver does not support setting configuration in runtime.
 */
static inline int uart_configure(const struct device *dev,
			     const struct uart_config *cfg)
{
	const struct uart_driver_api *api =
				(const struct uart_driver_api *)dev->api;

	if (api->configure == NULL) {
		return -ENOSYS;
	}
	return api->configure(dev, cfg);
}

/**
 * @brief Get UART configuration.
 *
 * Stores current UART configuration to *cfg, can be used to retrieve initial
 * configuration after device was initialized using data from DTS.
 *
 * @param dev UART device instance.
 * @param cfg UART configuration structure.
 *
 * @retval 0 If successful.
 * @retval -errno Negative errno code in case of failure.
 * @retval -ENOSYS If driver does not support getting current configuration.
 */
static inline int uart_config_get(const struct device *dev,
			      struct uart_config *cfg)
{
	const struct uart_driver_api *api =
				(const struct uart_driver_api *)dev->api;

	if (api->config_get == NULL) {
		return -ENOSYS;
	}

	return api->config_get(dev, cfg);
}

/**
 * @addtogroup uart_interrupt
 * @{
 */

/**
 * @brief Fill FIFO with data.
 *
 * @details This function is expected to be called from UART
 * interrupt handler (ISR), if uart_irq_tx_ready() returns true.
 * Result of calling this function not from an ISR is undefined
 * (hardware-dependent). Likewise, *not* calling this function
 * from an ISR if uart_irq_tx_ready() returns true may lead to
 * undefined behavior, e.g. infinite interrupt loops. It's
 * mandatory to test return value of this function, as different
 * hardware has different FIFO depth (oftentimes just 1).
 *
 * @param dev UART device instance.
 * @param tx_data Data to transmit.
 * @param size Number of bytes to send.
 *
 * @return Number of bytes sent.
 * @retval -ENOSYS  if this function is not supported
 * @retval -ENOTSUP If API is not enabled.
 */
static inline int uart_fifo_fill(const struct device *dev,
				 const uint8_t *tx_data,
				 int size)
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->fifo_fill == NULL) {
		return -ENOSYS;
	}

	return api->fifo_fill(dev, tx_data, size);
#else
	ARG_UNUSED(dev);
	ARG_UNUSED(tx_data);
	ARG_UNUSED(size);
	return -ENOTSUP;
#endif
}

/**
 * @brief Fill FIFO with wide data.
 *
 * @details This function is expected to be called from UART
 * interrupt handler (ISR), if uart_irq_tx_ready() returns true.
 * Result of calling this function not from an ISR is undefined
 * (hardware-dependent). Likewise, *not* calling this function
 * from an ISR if uart_irq_tx_ready() returns true may lead to
 * undefined behavior, e.g. infinite interrupt loops. It's
 * mandatory to test return value of this function, as different
 * hardware has different FIFO depth (oftentimes just 1).
 *
 * @param dev UART device instance.
 * @param tx_data Wide data to transmit.
 * @param size Number of datum to send.
 *
 * @return Number of datum sent.
 * @retval -ENOSYS If this function is not implemented
 * @retval -ENOTSUP If API is not enabled.
 */
static inline int uart_fifo_fill_u16(const struct device *dev,
				     const uint16_t *tx_data,
				     int size)
{
#if defined(CONFIG_UART_INTERRUPT_DRIVEN) && defined(CONFIG_UART_WIDE_DATA)
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->fifo_fill_u16 == NULL) {
		return -ENOSYS;
	}

	return api->fifo_fill_u16(dev, tx_data, size);
#else
	ARG_UNUSED(dev);
	ARG_UNUSED(tx_data);
	ARG_UNUSED(size);
	return -ENOTSUP;
#endif
}

/**
 * @brief Read data from FIFO.
 *
 * @details This function is expected to be called from UART
 * interrupt handler (ISR), if uart_irq_rx_ready() returns true.
 * Result of calling this function not from an ISR is undefined
 * (hardware-dependent). It's unspecified whether "RX ready"
 * condition as returned by uart_irq_rx_ready() is level- or
 * edge- triggered. That means that once uart_irq_rx_ready() is
 * detected, uart_fifo_read() must be called until it reads all
 * available data in the FIFO (i.e. until it returns less data
 * than was requested).
 *
 * Note that the calling context only applies to physical UARTs and
 * no to the virtual ones found in USB CDC ACM code.
 *
 * @param dev UART device instance.
 * @param rx_data Data container.
 * @param size Container size.
 *
 * @return Number of bytes read.
 * @retval -ENOSYS If this function is not implemented.
 * @retval -ENOTSUP If API is not enabled.
 */
static inline int uart_fifo_read(const struct device *dev, uint8_t *rx_data,
				 const int size)
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->fifo_read == NULL) {
		return -ENOSYS;
	}

	return api->fifo_read(dev, rx_data, size);
#else
	ARG_UNUSED(dev);
	ARG_UNUSED(rx_data);
	ARG_UNUSED(size);
	return -ENOTSUP;
#endif
}

/**
 * @brief Read wide data from FIFO.
 *
 * @details This function is expected to be called from UART
 * interrupt handler (ISR), if uart_irq_rx_ready() returns true.
 * Result of calling this function not from an ISR is undefined
 * (hardware-dependent). It's unspecified whether "RX ready"
 * condition as returned by uart_irq_rx_ready() is level- or
 * edge- triggered. That means that once uart_irq_rx_ready() is
 * detected, uart_fifo_read() must be called until it reads all
 * available data in the FIFO (i.e. until it returns less data
 * than was requested).
 *
 * Note that the calling context only applies to physical UARTs and
 * no to the virtual ones found in USB CDC ACM code.
 *
 * @param dev UART device instance.
 * @param rx_data Wide data container.
 * @param size Container size.
 *
 * @return Number of datum read.
 * @retval -ENOSYS If this function is not implemented.
 * @retval -ENOTSUP If API is not enabled.
 */
static inline int uart_fifo_read_u16(const struct device *dev,
				     uint16_t *rx_data,
				     const int size)
{
#if defined(CONFIG_UART_INTERRUPT_DRIVEN) && defined(CONFIG_UART_WIDE_DATA)
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->fifo_read_u16 == NULL) {
		return -ENOSYS;
	}

	return api->fifo_read_u16(dev, rx_data, size);
#else
	ARG_UNUSED(dev);
	ARG_UNUSED(rx_data);
	ARG_UNUSED(size);
	return -ENOTSUP;
#endif
}

/**
 * @brief Enable TX interrupt in IER.
 *
 * @param dev UART device instance.
 */
static inline void uart_irq_tx_enable(const struct device *dev)
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->irq_tx_enable != NULL) {
		api->irq_tx_enable(dev);
	}
#else
	ARG_UNUSED(dev);
#endif
}

/**
 * @brief Disable TX interrupt in IER.
 *
 * @param dev UART device instance.
 */
static inline void uart_irq_tx_disable(const struct device *dev)
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->irq_tx_disable != NULL) {
		api->irq_tx_disable(dev);
	}
#else
	ARG_UNUSED(dev);
#endif
}

/**
 * @brief Check if UART TX buffer can accept a new char
 *
 * @details Check if UART TX buffer can accept at least one character
 * for transmission (i.e. uart_fifo_fill() will succeed and return
 * non-zero). This function must be called in a UART interrupt
 * handler, or its result is undefined. Before calling this function
 * in the interrupt handler, uart_irq_update() must be called once per
 * the handler invocation.
 *
 * @param dev UART device instance.
 *
 * @retval 1 If TX interrupt is enabled and at least one char can be
 *           written to UART.
 * @retval 0 If device is not ready to write a new byte.
 * @retval -ENOSYS If this function is not implemented.
 * @retval -ENOTSUP If API is not enabled.
 */
static inline int uart_irq_tx_ready(const struct device *dev)
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->irq_tx_ready == NULL) {
		return -ENOSYS;
	}

	return api->irq_tx_ready(dev);
#else
	ARG_UNUSED(dev);
	return -ENOTSUP;
#endif
}

/**
 * @brief Enable RX interrupt.
 *
 * @param dev UART device instance.
 */
static inline void uart_irq_rx_enable(const struct device *dev)
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->irq_rx_enable != NULL) {
		api->irq_rx_enable(dev);
	}
#else
	ARG_UNUSED(dev);
#endif
}

/**
 * @brief Disable RX interrupt.
 *
 * @param dev UART device instance.
 */
static inline void uart_irq_rx_disable(const struct device *dev)
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->irq_rx_disable != NULL) {
		api->irq_rx_disable(dev);
	}
#else
	ARG_UNUSED(dev);
#endif
}

/**
 * @brief Check if UART TX block finished transmission
 *
 * @details Check if any outgoing data buffered in UART TX block was
 * fully transmitted and TX block is idle. When this condition is
 * true, UART device (or whole system) can be power off. Note that
 * this function is *not* useful to check if UART TX can accept more
 * data, use uart_irq_tx_ready() for that. This function must be called
 * in a UART interrupt handler, or its result is undefined. Before
 * calling this function in the interrupt handler, uart_irq_update()
 * must be called once per the handler invocation.
 *
 * @param dev UART device instance.
 *
 * @retval 1 If nothing remains to be transmitted.
 * @retval 0 If transmission is not completed.
 * @retval -ENOSYS If this function is not implemented.
 * @retval -ENOTSUP If API is not enabled.
 */
static inline int uart_irq_tx_complete(const struct device *dev)
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->irq_tx_complete == NULL) {
		return -ENOSYS;
	}
	return api->irq_tx_complete(dev);
#else
	ARG_UNUSED(dev);
	return -ENOTSUP;
#endif
}

/**
 * @brief Check if UART RX buffer has a received char
 *
 * @details Check if UART RX buffer has at least one pending character
 * (i.e. uart_fifo_read() will succeed and return non-zero). This function
 * must be called in a UART interrupt handler, or its result is undefined.
 * Before calling this function in the interrupt handler, uart_irq_update()
 * must be called once per the handler invocation. It's unspecified whether
 * condition as returned by this function is level- or edge- triggered (i.e.
 * if this function returns true when RX FIFO is non-empty, or when a new
 * char was received since last call to it). See description of
 * uart_fifo_read() for implication of this.
 *
 * @param dev UART device instance.
 *
 * @retval 1 If a received char is ready.
 * @retval 0 If a received char is not ready.
 * @retval -ENOSYS If this function is not implemented.
 * @retval -ENOTSUP If API is not enabled.
 */
static inline int uart_irq_rx_ready(const struct device *dev)
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->irq_rx_ready == NULL) {
		return -ENOSYS;
	}
	return api->irq_rx_ready(dev);
#else
	ARG_UNUSED(dev);
	return -ENOTSUP;
#endif
}
/**
 * @brief Enable error interrupt.
 *
 * @param dev UART device instance.
 */
static inline void uart_irq_err_enable(const struct device *dev)
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->irq_err_enable) {
		api->irq_err_enable(dev);
	}
#else
	ARG_UNUSED(dev);
#endif
}

/**
 * @brief Disable error interrupt.
 *
 * @param dev UART device instance.
 */
static inline void uart_irq_err_disable(const struct device *dev)
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->irq_err_disable) {
		api->irq_err_disable(dev);
	}
#else
	ARG_UNUSED(dev);
#endif
}

/**
 * @brief Check if any IRQs is pending.
 *
 * @param dev UART device instance.
 *
 * @retval 1 If an IRQ is pending.
 * @retval 0 If an IRQ is not pending.
 * @retval -ENOSYS If this function is not implemented.
 * @retval -ENOTSUP If API is not enabled.
 */
static inline int uart_irq_is_pending(const struct device *dev)
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->irq_is_pending == NULL) {
		return -ENOSYS;
	}
	return api->irq_is_pending(dev);
#else
	ARG_UNUSED(dev);
	return -ENOTSUP;
#endif
}

/**
 * @brief Start processing interrupts in ISR.
 *
 * This function should be called the first thing in the ISR. Calling
 * uart_irq_rx_ready(), uart_irq_tx_ready(), uart_irq_tx_complete()
 * allowed only after this.
 *
 * The purpose of this function is:
 *
 * * For devices with auto-acknowledge of interrupt status on register
 *   read to cache the value of this register (rx_ready, etc. then use
 *   this case).
 * * For devices with explicit acknowledgment of interrupts, to ack
 *   any pending interrupts and likewise to cache the original value.
 * * For devices with implicit acknowledgment, this function will be
 *   empty. But the ISR must perform the actions needs to ack the
 *   interrupts (usually, call uart_fifo_read() on rx_ready, and
 *   uart_fifo_fill() on tx_ready).
 *
 * @param dev UART device instance.
 *
 * @retval 1 On success.
 * @retval -ENOSYS If this function is not implemented.
 * @retval -ENOTSUP If API is not enabled.
 */
static inline int uart_irq_update(const struct device *dev)
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->irq_update == NULL) {
		return -ENOSYS;
	}
	return api->irq_update(dev);
#else
	ARG_UNUSED(dev);
	return -ENOTSUP;
#endif
}

/**
 * @brief Set the IRQ callback function pointer.
 *
 * This sets up the callback for IRQ. When an IRQ is triggered,
 * the specified function will be called with specified user data.
 * See description of uart_irq_update() for the requirements on ISR.
 *
 * @param dev UART device instance.
 * @param cb Pointer to the callback function.
 * @param user_data Data to pass to callback function.
 */
static inline void uart_irq_callback_user_data_set(const struct device *dev,
						   uart_irq_callback_user_data_t cb,
						   void *user_data)
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if ((api != NULL) && (api->irq_callback_set != NULL)) {
		api->irq_callback_set(dev, cb, user_data);
	}
#else
	ARG_UNUSED(dev);
	ARG_UNUSED(cb);
	ARG_UNUSED(user_data);
#endif
}

/**
 * @brief Set the IRQ callback function pointer (legacy).
 *
 * This sets up the callback for IRQ. When an IRQ is triggered,
 * the specified function will be called with the device pointer.
 *
 * @param dev UART device instance.
 * @param cb Pointer to the callback function.
 */
static inline void uart_irq_callback_set(const struct device *dev,
					 uart_irq_callback_user_data_t cb)
{
	uart_irq_callback_user_data_set(dev, cb, NULL);
}


/**
 * @brief Send extra command to driver.
 *
 * Implementation and accepted commands are driver specific.
 * Refer to the drivers for more information.
 *
 * @param dev UART device instance.
 * @param cmd Command to driver.
 * @param p Parameter to the command.
 *
 * @retval 0 If successful.
 * @retval -ENOSYS If this function is not implemented.
 * @retval -ENOTSUP If API is not enabled.
 * @retval -errno Other negative errno value in case of failure.
 */
static inline int uart_drv_cmd(const struct device *dev, uint32_t cmd, uint32_t p)
{
#ifdef CONFIG_UART_DRV_CMD
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->drv_cmd == NULL) {
		return -ENOSYS;
	}
	return api->drv_cmd(dev, cmd, p);
#else
	ARG_UNUSED(dev);
	ARG_UNUSED(cmd);
	ARG_UNUSED(p);
	return -ENOTSUP;
#endif
}


#endif /* INCLUDE_DRIVERS_UART_H */