/** @file
 *  @brief Buffer management.
 */

/*
 * Copyright (c) 2015 Intel Corporation
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef INCLUDE_BUF_BUF_H
#define INCLUDE_BUF_BUF_H

#include <kernel.h>

/**
 * @brief Simple buffer representation.
 *
 * This is a simpler variant of the buf object (in fact buf uses
 * buf_simple internally). It doesn't provide any kind of reference
 * counting, user data, dynamic allocation, or in general the ability to
 * pass through kernel objects such as FIFOs.
 *
 * The main use of this is for scenarios where the meta-data of the normal
 * buf isn't needed and causes too much overhead. This could be e.g.
 * when the buffer only needs to be allocated on the stack or when the
 * access to and lifetime of the buffer is well controlled and constrained.
 */
struct buf_simple {
	/** Pointer to the start of data in the buffer. */
	uint8_t *data;

	/**
	 * Length of the data behind the data pointer.
	 *
	 * To determine the max length, use buf_simple_max_len(), not #size!
	 */
	uint16_t len;

	/** Amount of data that buf_simple#__buf can store. */
	uint16_t size;

	/** Start of the data storage. Not to be accessed directly
	 *  (the data pointer should be used instead).
	 */
	uint8_t *__buf;
};

/* Alignment needed for various parts of the buffer definition */
#define __buf_align __aligned(sizeof(void *))

/**
 *  @brief Define a buf_simple stack variable.
 *
 *  This is a helper macro which is used to define a buf_simple object
 *  on the stack.
 *
 *  @param _name Name of the buf_simple object.
 *  @param _size Maximum data storage for the buffer.
 */
#define BUF_SIMPLE_DEFINE(_name, _size)     \
	uint8_t buf_data_##_name[_size];       \
	struct buf_simple _name = {         \
		.data   = buf_data_##_name, \
		.len    = 0,                    \
		.size   = _size,                \
		.__buf  = buf_data_##_name, \
	}


/**
 *
 * @brief Define a static net_buf_simple variable.
 *
 * This is a helper macro which is used to define a static net_buf_simple
 * object.
 *
 * @param _name Name of the net_buf_simple object.
 * @param _size Maximum data storage for the buffer.
 */
#define BUF_SIMPLE_DEFINE_STATIC(_name, _size)        \
	static uint8_t buf_data_##_name[_size]; \
	static struct buf_simple _name = {            \
		.data   = buf_data_##_name,           \
		.len    = 0,                              \
		.size   = _size,                          \
		.__buf  = buf_data_##_name,           \
	}

/**
 *
 * @brief Define a buf_simple stack variable and get a pointer to it.
 *
 * This is a helper macro which is used to define a net_buf_simple object on
 * the stack and the get a pointer to it as follows:
 *
 * struct buf_simple *my_buf = BUF_SIMPLE(10);
 *
 * After creating the object it needs to be initialized by calling
 * buf_simple_init().
 *
 * @param _size Maximum data storage for the buffer.
 *
 * @return Pointer to stack-allocated buf_simple object.
 */
#define BUF_SIMPLE(_size)                        \
	((struct buf_simple *)(&(struct {        \
		struct buf_simple buf;           \
		uint8_t data[_size];                 \
	}) {                                         \
		.buf.size = _size,                   \
	}))

/**
 * @brief Initialize a buf_simple object.
 *
 * This needs to be called after creating a buf_simple object using
 * the BUF_SIMPLE macro.
 *
 * @param buf Buffer to initialize.
 * @param reserve_head Headroom to reserve.
 */
static inline void buf_simple_init(struct buf_simple *buf,
				       size_t reserve_head)
{
	if (!buf->__buf) {
		buf->__buf = (uint8_t *)buf + sizeof(*buf);
	}

	buf->data = buf->__buf + reserve_head;
	buf->len = 0U;
}

/**
 * @brief Initialize a buf_simple object with data.
 *
 * Initialized buffer object with external data.
 *
 * @param buf Buffer to initialize.
 * @param data External data pointer
 * @param size Amount of data the pointed data buffer if able to fit.
 */
void buf_simple_init_with_data(struct buf_simple *buf,
				   void *data, size_t size);

/**
 * @brief Reset buffer
 *
 * Reset buffer data so it can be reused for other purposes.
 *
 * @param buf Buffer to reset.
 */
static inline void buf_simple_reset(struct buf_simple *buf)
{
	buf->len  = 0U;
	buf->data = buf->__buf;
}


/**
 * Clone buffer state, using the same data buffer.
 *
 * Initializes a buffer to point to the same data as an existing buffer.
 * Allows operations on the same data without altering the length and
 * offset of the original.
 *
 * @param original Buffer to clone.
 * @param clone The new clone.
 */
void buf_simple_clone(const struct buf_simple *original,
			  struct buf_simple *clone);

/**
 * @brief Prepare data to be added at the end of the buffer
 *
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param len Number of bytes to increment the length with.
 *
 * @return The original tail of the buffer.
 */
void *buf_simple_add(struct buf_simple *buf, size_t len);

/**
 * @brief Copy given number of bytes from memory to the end of the buffer
 *
 * Increments the data length of the  buffer to account for more data at the
 * end.
 *
 * @param buf Buffer to update.
 * @param mem Location of data to be added.
 * @param len Length of data to be added
 *
 * @return The original tail of the buffer.
 */
void *buf_simple_add_mem(struct buf_simple *buf, const void *mem,
			     size_t len);

/**
 * @brief Add (8-bit) byte at the end of the buffer
 *
 * Increments the data length of the  buffer to account for more data at the
 * end.
 *
 * @param buf Buffer to update.
 * @param val byte value to be added.
 *
 * @return Pointer to the value added
 */
uint8_t *buf_simple_add_u8(struct buf_simple *buf, uint8_t val);

/**
 * @brief Add 16-bit value at the end of the buffer
 *
 * Adds 16-bit value in little endian format at the end of buffer.
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param val 16-bit value to be added.
 */
void buf_simple_add_le16(struct buf_simple *buf, uint16_t val);

/**
 * @brief Add 16-bit value at the end of the buffer
 *
 * Adds 16-bit value in big endian format at the end of buffer.
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param val 16-bit value to be added.
 */
void buf_simple_add_be16(struct buf_simple *buf, uint16_t val);

/**
 * @brief Add 24-bit value at the end of the buffer
 *
 * Adds 24-bit value in little endian format at the end of buffer.
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param val 24-bit value to be added.
 */
void buf_simple_add_le24(struct buf_simple *buf, uint32_t val);

/**
 * @brief Add 24-bit value at the end of the buffer
 *
 * Adds 24-bit value in big endian format at the end of buffer.
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param val 24-bit value to be added.
 */
void buf_simple_add_be24(struct buf_simple *buf, uint32_t val);

/**
 * @brief Add 32-bit value at the end of the buffer
 *
 * Adds 32-bit value in little endian format at the end of buffer.
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param val 32-bit value to be added.
 */
void buf_simple_add_le32(struct buf_simple *buf, uint32_t val);

/**
 * @brief Add 32-bit value at the end of the buffer
 *
 * Adds 32-bit value in big endian format at the end of buffer.
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param val 32-bit value to be added.
 */
void buf_simple_add_be32(struct buf_simple *buf, uint32_t val);

/**
 * @brief Add 48-bit value at the end of the buffer
 *
 * Adds 48-bit value in little endian format at the end of buffer.
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param val 48-bit value to be added.
 */
void buf_simple_add_le48(struct buf_simple *buf, uint64_t val);

/**
 * @brief Add 48-bit value at the end of the buffer
 *
 * Adds 48-bit value in big endian format at the end of buffer.
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param val 48-bit value to be added.
 */
void buf_simple_add_be48(struct buf_simple *buf, uint64_t val);

/**
 * @brief Add 64-bit value at the end of the buffer
 *
 * Adds 64-bit value in little endian format at the end of buffer.
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param val 64-bit value to be added.
 */
void buf_simple_add_le64(struct buf_simple *buf, uint64_t val);

/**
 * @brief Add 64-bit value at the end of the buffer
 *
 * Adds 64-bit value in big endian format at the end of buffer.
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param val 64-bit value to be added.
 */
void buf_simple_add_be64(struct buf_simple *buf, uint64_t val);

/**
 * @brief Remove data from the end of the buffer.
 *
 * Removes data from the end of the buffer by modifying the buffer length.
 *
 * @param buf Buffer to update.
 * @param len Number of bytes to remove.
 *
 * @return New end of the buffer data.
 */
void *buf_simple_remove_mem(struct buf_simple *buf, size_t len);

/**
 * @brief Remove a 8-bit value from the end of the buffer
 *
 * Same idea as with buf_simple_remove_mem(), but a helper for operating
 * on 8-bit values.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return The 8-bit removed value
 */
uint8_t buf_simple_remove_u8(struct buf_simple *buf);

/**
 * @brief Remove and convert 16 bits from the end of the buffer.
 *
 * Same idea as with buf_simple_remove_mem(), but a helper for operating
 * on 16-bit little endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 16-bit value converted from little endian to host endian.
 */
uint16_t buf_simple_remove_le16(struct buf_simple *buf);

/**
 * @brief Remove and convert 16 bits from the end of the buffer.
 *
 * Same idea as with buf_simple_remove_mem(), but a helper for operating
 * on 16-bit big endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 16-bit value converted from big endian to host endian.
 */
uint16_t buf_simple_remove_be16(struct buf_simple *buf);

/**
 * @brief Remove and convert 24 bits from the end of the buffer.
 *
 * Same idea as with buf_simple_remove_mem(), but a helper for operating
 * on 24-bit little endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 24-bit value converted from little endian to host endian.
 */
uint32_t buf_simple_remove_le24(struct buf_simple *buf);

/**
 * @brief Remove and convert 24 bits from the end of the buffer.
 *
 * Same idea as with buf_simple_remove_mem(), but a helper for operating
 * on 24-bit big endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 24-bit value converted from big endian to host endian.
 */
uint32_t buf_simple_remove_be24(struct buf_simple *buf);

/**
 * @brief Remove and convert 32 bits from the end of the buffer.
 *
 * Same idea as with buf_simple_remove_mem(), but a helper for operating
 * on 32-bit little endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 32-bit value converted from little endian to host endian.
 */
uint32_t buf_simple_remove_le32(struct buf_simple *buf);

/**
 * @brief Remove and convert 32 bits from the end of the buffer.
 *
 * Same idea as with buf_simple_remove_mem(), but a helper for operating
 * on 32-bit big endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 32-bit value converted from big endian to host endian.
 */
uint32_t buf_simple_remove_be32(struct buf_simple *buf);

/**
 * @brief Remove and convert 48 bits from the end of the buffer.
 *
 * Same idea as with buf_simple_remove_mem(), but a helper for operating
 * on 48-bit little endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 48-bit value converted from little endian to host endian.
 */
uint64_t buf_simple_remove_le48(struct buf_simple *buf);

/**
 * @brief Remove and convert 48 bits from the end of the buffer.
 *
 * Same idea as with buf_simple_remove_mem(), but a helper for operating
 * on 48-bit big endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 48-bit value converted from big endian to host endian.
 */
uint64_t buf_simple_remove_be48(struct buf_simple *buf);

/**
 * @brief Remove and convert 64 bits from the end of the buffer.
 *
 * Same idea as with buf_simple_remove_mem(), but a helper for operating
 * on 64-bit little endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 64-bit value converted from little endian to host endian.
 */
uint64_t buf_simple_remove_le64(struct buf_simple *buf);

/**
 * @brief Remove and convert 64 bits from the end of the buffer.
 *
 * Same idea as with buf_simple_remove_mem(), but a helper for operating
 * on 64-bit big endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 64-bit value converted from big endian to host endian.
 */
uint64_t buf_simple_remove_be64(struct buf_simple *buf);

/**
 * @brief Prepare data to be added to the start of the buffer
 *
 * Modifies the data pointer and buffer length to account for more data
 * in the beginning of the buffer.
 *
 * @param buf Buffer to update.
 * @param len Number of bytes to add to the beginning.
 *
 * @return The new beginning of the buffer data.
 */
void *buf_simple_push(struct buf_simple *buf, size_t len);

/**
 * @brief Copy given number of bytes from memory to the start of the buffer.
 *
 * Modifies the data pointer and buffer length to account for more data
 * in the beginning of the buffer.
 *
 * @param buf Buffer to update.
 * @param mem Location of data to be added.
 * @param len Length of data to be added.
 *
 * @return The new beginning of the buffer data.
 */
void *buf_simple_push_mem(struct buf_simple *buf, const void *mem,
			      size_t len);

/**
 * @brief Push 16-bit value to the beginning of the buffer
 *
 * Adds 16-bit value in little endian format to the beginning of the
 * buffer.
 *
 * @param buf Buffer to update.
 * @param val 16-bit value to be pushed to the buffer.
 */
void buf_simple_push_le16(struct buf_simple *buf, uint16_t val);

/**
 * @brief Push 16-bit value to the beginning of the buffer
 *
 * Adds 16-bit value in big endian format to the beginning of the
 * buffer.
 *
 * @param buf Buffer to update.
 * @param val 16-bit value to be pushed to the buffer.
 */
void buf_simple_push_be16(struct buf_simple *buf, uint16_t val);

/**
 * @brief Push 8-bit value to the beginning of the buffer
 *
 * Adds 8-bit value the beginning of the buffer.
 *
 * @param buf Buffer to update.
 * @param val 8-bit value to be pushed to the buffer.
 */
void buf_simple_push_u8(struct buf_simple *buf, uint8_t val);

/**
 * @brief Push 24-bit value to the beginning of the buffer
 *
 * Adds 24-bit value in little endian format to the beginning of the
 * buffer.
 *
 * @param buf Buffer to update.
 * @param val 24-bit value to be pushed to the buffer.
 */
void buf_simple_push_le24(struct buf_simple *buf, uint32_t val);

/**
 * @brief Push 24-bit value to the beginning of the buffer
 *
 * Adds 24-bit value in big endian format to the beginning of the
 * buffer.
 *
 * @param buf Buffer to update.
 * @param val 24-bit value to be pushed to the buffer.
 */
void buf_simple_push_be24(struct buf_simple *buf, uint32_t val);

/**
 * @brief Push 32-bit value to the beginning of the buffer
 *
 * Adds 32-bit value in little endian format to the beginning of the
 * buffer.
 *
 * @param buf Buffer to update.
 * @param val 32-bit value to be pushed to the buffer.
 */
void buf_simple_push_le32(struct buf_simple *buf, uint32_t val);

/**
 * @brief Push 32-bit value to the beginning of the buffer
 *
 * Adds 32-bit value in big endian format to the beginning of the
 * buffer.
 *
 * @param buf Buffer to update.
 * @param val 32-bit value to be pushed to the buffer.
 */
void buf_simple_push_be32(struct buf_simple *buf, uint32_t val);

/**
 * @brief Push 48-bit value to the beginning of the buffer
 *
 * Adds 48-bit value in little endian format to the beginning of the
 * buffer.
 *
 * @param buf Buffer to update.
 * @param val 48-bit value to be pushed to the buffer.
 */
void buf_simple_push_le48(struct buf_simple *buf, uint64_t val);

/**
 * @brief Push 48-bit value to the beginning of the buffer
 *
 * Adds 48-bit value in big endian format to the beginning of the
 * buffer.
 *
 * @param buf Buffer to update.
 * @param val 48-bit value to be pushed to the buffer.
 */
void buf_simple_push_be48(struct buf_simple *buf, uint64_t val);

/**
 * @brief Push 64-bit value to the beginning of the buffer
 *
 * Adds 64-bit value in little endian format to the beginning of the
 * buffer.
 *
 * @param buf Buffer to update.
 * @param val 64-bit value to be pushed to the buffer.
 */
void buf_simple_push_le64(struct buf_simple *buf, uint64_t val);

/**
 * @brief Push 64-bit value to the beginning of the buffer
 *
 * Adds 64-bit value in big endian format to the beginning of the
 * buffer.
 *
 * @param buf Buffer to update.
 * @param val 64-bit value to be pushed to the buffer.
 */
void buf_simple_push_be64(struct buf_simple *buf, uint64_t val);

/**
 * @brief Remove data from the beginning of the buffer.
 *
 * Removes data from the beginning of the buffer by modifying the data
 * pointer and buffer length.
 *
 * @param buf Buffer to update.
 * @param len Number of bytes to remove.
 *
 * @return New beginning of the buffer data.
 */
void *buf_simple_pull(struct buf_simple *buf, size_t len);

/**
 * @brief Remove data from the beginning of the buffer.
 *
 * Removes data from the beginning of the buffer by modifying the data
 * pointer and buffer length.
 *
 * @param buf Buffer to update.
 * @param len Number of bytes to remove.
 *
 * @return Pointer to the old location of the buffer data.
 */
void *buf_simple_pull_mem(struct buf_simple *buf, size_t len);

/**
 * @brief Remove a 8-bit value from the beginning of the buffer
 *
 * Same idea as with buf_simple_pull(), but a helper for operating
 * on 8-bit values.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return The 8-bit removed value
 */
uint8_t buf_simple_pull_u8(struct buf_simple *buf);

/**
 * @brief Remove and convert 16 bits from the beginning of the buffer.
 *
 * Same idea as with buf_simple_pull(), but a helper for operating
 * on 16-bit little endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 16-bit value converted from little endian to host endian.
 */
uint16_t buf_simple_pull_le16(struct buf_simple *buf);

/**
 * @brief Remove and convert 16 bits from the beginning of the buffer.
 *
 * Same idea as with buf_simple_pull(), but a helper for operating
 * on 16-bit big endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 16-bit value converted from big endian to host endian.
 */
uint16_t buf_simple_pull_be16(struct buf_simple *buf);

/**
 * @brief Remove and convert 24 bits from the beginning of the buffer.
 *
 * Same idea as with buf_simple_pull(), but a helper for operating
 * on 24-bit little endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 24-bit value converted from little endian to host endian.
 */
uint32_t buf_simple_pull_le24(struct buf_simple *buf);

/**
 * @brief Remove and convert 24 bits from the beginning of the buffer.
 *
 * Same idea as with buf_simple_pull(), but a helper for operating
 * on 24-bit big endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 24-bit value converted from big endian to host endian.
 */
uint32_t buf_simple_pull_be24(struct buf_simple *buf);

/**
 * @brief Remove and convert 32 bits from the beginning of the buffer.
 *
 * Same idea as with buf_simple_pull(), but a helper for operating
 * on 32-bit little endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 32-bit value converted from little endian to host endian.
 */
uint32_t buf_simple_pull_le32(struct buf_simple *buf);

/**
 * @brief Remove and convert 32 bits from the beginning of the buffer.
 *
 * Same idea as with buf_simple_pull(), but a helper for operating
 * on 32-bit big endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 32-bit value converted from big endian to host endian.
 */
uint32_t buf_simple_pull_be32(struct buf_simple *buf);

/**
 * @brief Remove and convert 48 bits from the beginning of the buffer.
 *
 * Same idea as with buf_simple_pull(), but a helper for operating
 * on 48-bit little endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 48-bit value converted from little endian to host endian.
 */
uint64_t buf_simple_pull_le48(struct buf_simple *buf);

/**
 * @brief Remove and convert 48 bits from the beginning of the buffer.
 *
 * Same idea as with buf_simple_pull(), but a helper for operating
 * on 48-bit big endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 48-bit value converted from big endian to host endian.
 */
uint64_t buf_simple_pull_be48(struct buf_simple *buf);

/**
 * @brief Remove and convert 64 bits from the beginning of the buffer.
 *
 * Same idea as with buf_simple_pull(), but a helper for operating
 * on 64-bit little endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 64-bit value converted from little endian to host endian.
 */
uint64_t buf_simple_pull_le64(struct buf_simple *buf);

/**
 * @brief Remove and convert 64 bits from the beginning of the buffer.
 *
 * Same idea as with buf_simple_pull(), but a helper for operating
 * on 64-bit big endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 64-bit value converted from big endian to host endian.
 */
uint64_t buf_simple_pull_be64(struct buf_simple *buf);

/**
 * @brief Get the tail pointer for a buffer.
 *
 * Get a pointer to the end of the data in a buffer.
 *
 * @param buf Buffer.
 *
 * @return Tail pointer for the buffer.
 */
static inline uint8_t *buf_simple_tail(struct buf_simple *buf)
{
	return buf->data + buf->len;
}

/**
 * @brief Check buffer headroom.
 *
 * Check how much free space there is in the beginning of the buffer.
 *
 * buf A valid pointer on a buffer
 *
 * @return Number of bytes available in the beginning of the buffer.
 */
size_t buf_simple_headroom(struct buf_simple *buf);

/**
 * @brief Check buffer tailroom.
 *
 * Check how much free space there is at the end of the buffer.
 *
 * @param buf A valid pointer on a buffer
 *
 * @return Number of bytes available at the end of the buffer.
 */
size_t buf_simple_tailroom(struct buf_simple *buf);

/**
 * @brief Check maximum buf_simple::len value.
 *
 * This value is depending on the number of bytes being reserved as headroom.
 *
 * @param buf A valid pointer on a buffer
 *
 * @return Number of bytes usable behind the buf_simple::data pointer.
 */
uint16_t buf_simple_max_len(struct buf_simple *buf);

/**
 * @brief Parsing state of a buffer.
 *
 * This is used for temporarily storing the parsing state of a buffer
 * while giving control of the parsing to a routine which we don't
 * control.
 */
struct buf_simple_state {
	/** Offset of the data pointer from the beginning of the storage */
	uint16_t offset;
	/** Length of data */
	uint16_t len;
};

/**
 * @brief Save the parsing state of a buffer.
 *
 * Saves the parsing state of a buffer so it can be restored later.
 *
 * @param buf Buffer from which the state should be saved.
 * @param state Storage for the state.
 */
static inline void buf_simple_save(struct buf_simple *buf,
				       struct buf_simple_state *state)
{
	state->offset = buf_simple_headroom(buf);
	state->len = buf->len;
}

/**
 * @brief Restore the parsing state of a buffer.
 *
 * Restores the parsing state of a buffer from a state previously stored
 * by buf_simple_save().
 *
 * @param buf Buffer to which the state should be restored.
 * @param state Stored state.
 */
static inline void buf_simple_restore(struct buf_simple *buf,
					  struct buf_simple_state *state)
{
	buf->data = buf->__buf + state->offset;
	buf->len = state->len;
}





#endif /* INCLUDE_BUF_BUF_H */
