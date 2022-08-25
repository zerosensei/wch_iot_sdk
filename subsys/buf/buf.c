/* buf.c - Buffer management */

/*
 * Copyright (c) 2015-2019 Intel Corporation
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <logging/log.h>

#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/byteorder.h>

#include <buf/buf.h>

#if defined(CONFIG_BUF_LOG)
#define BUF_DBG(fmt, ...) LOG_DBG(fmt, ##__VA_ARGS__)
#define BUF_ERR(fmt, ...) LOG_ERR(fmt, ##__VA_ARGS__)
#define BUF_WARN(fmt, ...) LOG_WRN(fmt, ##__VA_ARGS__)
#define BUF_INFO(fmt, ...) LOG_INF(fmt, ##__VA_ARGS__)
#else

#define BUF_DBG(fmt, ...)
#define BUF_ERR(fmt, ...)
#define BUF_WARN(fmt, ...)
#define BUF_INFO(fmt, ...)
#endif /* CONFIG_BUF_LOG */

#define BUF_ASSERT(cond, ...) __ASSERT(cond, "" __VA_ARGS__)

void buf_simple_init_with_data(struct buf_simple *buf,
				   void *data, size_t size)
{
    buf->__buf = data;
	buf->data  = data;
	buf->size  = size;
	buf->len   = size;
}

#if defined(CONFIG_BUF_SIMPLE_LOG)
#define BUF_SIMPLE_DBG(fmt, ...) BUF_DBG(fmt, ##__VA_ARGS__)
#define BUF_SIMPLE_ERR(fmt, ...) BUF_ERR(fmt, ##__VA_ARGS__)
#define BUF_SIMPLE_WARN(fmt, ...) BUF_WARN(fmt, ##__VA_ARGS__)
#define BUF_SIMPLE_INFO(fmt, ...) BUF_INFO(fmt, ##__VA_ARGS__)
#else
#define BUF_SIMPLE_DBG(fmt, ...)
#define BUF_SIMPLE_ERR(fmt, ...)
#define BUF_SIMPLE_WARN(fmt, ...)
#define BUF_SIMPLE_INFO(fmt, ...)
#endif /* CONFIG_BUF_SIMPLE_LOG */

void buf_simple_clone(const struct buf_simple *original,
			  struct buf_simple *clone)
{
	memcpy(clone, original, sizeof(struct buf_simple));
}

void *buf_simple_add(struct buf_simple *buf, size_t len)
{
	uint8_t *tail = buf_simple_tail(buf);

	BUF_SIMPLE_DBG("buf %p len %zu", buf, len);

	__ASSERT_NO_MSG(buf_simple_tailroom(buf) >= len);

	buf->len += len;
	return tail;
}

void *buf_simple_add_mem(struct buf_simple *buf, const void *mem,
			     size_t len)
{
	BUF_SIMPLE_DBG("buf %p len %zu", buf, len);

	return memcpy(buf_simple_add(buf, len), mem, len);
}

uint8_t *buf_simple_add_u8(struct buf_simple *buf, uint8_t val)
{
	uint8_t *u8;

	BUF_SIMPLE_DBG("buf %p val 0x%02x", buf, val);

	u8 = buf_simple_add(buf, 1);
	*u8 = val;

	return u8;
}

void buf_simple_add_le16(struct buf_simple *buf, uint16_t val)
{
	BUF_SIMPLE_DBG("buf %p val %u", buf, val);

	sys_put_le16(val, buf_simple_add(buf, sizeof(val)));
}

void buf_simple_add_be16(struct buf_simple *buf, uint16_t val)
{
	BUF_SIMPLE_DBG("buf %p val %u", buf, val);

	sys_put_be16(val, buf_simple_add(buf, sizeof(val)));
}

void buf_simple_add_le24(struct buf_simple *buf, uint32_t val)
{
	BUF_SIMPLE_DBG("buf %p val %u", buf, val);

	sys_put_le24(val, buf_simple_add(buf, 3));
}

void buf_simple_add_be24(struct buf_simple *buf, uint32_t val)
{
	BUF_SIMPLE_DBG("buf %p val %u", buf, val);

	sys_put_be24(val, buf_simple_add(buf, 3));
}

void buf_simple_add_le32(struct buf_simple *buf, uint32_t val)
{
	BUF_SIMPLE_DBG("buf %p val %u", buf, val);

	sys_put_le32(val, buf_simple_add(buf, sizeof(val)));
}

void buf_simple_add_be32(struct buf_simple *buf, uint32_t val)
{
	BUF_SIMPLE_DBG("buf %p val %u", buf, val);

	sys_put_be32(val, buf_simple_add(buf, sizeof(val)));
}

void buf_simple_add_le48(struct buf_simple *buf, uint64_t val)
{
	BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

	sys_put_le48(val, buf_simple_add(buf, 6));
}

void buf_simple_add_be48(struct buf_simple *buf, uint64_t val)
{
	BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

	sys_put_be48(val, buf_simple_add(buf, 6));
}

void buf_simple_add_le64(struct buf_simple *buf, uint64_t val)
{
	BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

	sys_put_le64(val, buf_simple_add(buf, sizeof(val)));
}

void buf_simple_add_be64(struct buf_simple *buf, uint64_t val)
{
	BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

	sys_put_be64(val, buf_simple_add(buf, sizeof(val)));
}

void *buf_simple_remove_mem(struct buf_simple *buf, size_t len)
{
	BUF_SIMPLE_DBG("buf %p len %zu", buf, len);

	__ASSERT_NO_MSG(buf->len >= len);

	buf->len -= len;
	return buf->data + buf->len;
}

uint8_t buf_simple_remove_u8(struct buf_simple *buf)
{
	uint8_t val;
	void *ptr;

	ptr = buf_simple_remove_mem(buf, sizeof(val));
	val = *(uint8_t *)ptr;

	return val;
}

uint16_t buf_simple_remove_le16(struct buf_simple *buf)
{
	uint16_t val;
	void *ptr;

	ptr = buf_simple_remove_mem(buf, sizeof(val));
	val = UNALIGNED_GET((uint16_t *)ptr);

	return sys_le16_to_cpu(val);
}

uint16_t buf_simple_remove_be16(struct buf_simple *buf)
{
	uint16_t val;
	void *ptr;

	ptr = buf_simple_remove_mem(buf, sizeof(val));
	val = UNALIGNED_GET((uint16_t *)ptr);

	return sys_be16_to_cpu(val);
}

uint32_t buf_simple_remove_le24(struct buf_simple *buf)
{
	struct uint24 {
		uint32_t u24 : 24;
	} __packed val;
	void *ptr;

	ptr = buf_simple_remove_mem(buf, sizeof(val));
	val = UNALIGNED_GET((struct uint24 *)ptr);

	return sys_le24_to_cpu(val.u24);
}

uint32_t buf_simple_remove_be24(struct buf_simple *buf)
{
	struct uint24 {
		uint32_t u24 : 24;
	} __packed val;
	void *ptr;

	ptr = buf_simple_remove_mem(buf, sizeof(val));
	val = UNALIGNED_GET((struct uint24 *)ptr);

	return sys_be24_to_cpu(val.u24);
}

uint32_t buf_simple_remove_le32(struct buf_simple *buf)
{
	uint32_t val;
	void *ptr;

	ptr = buf_simple_remove_mem(buf, sizeof(val));
	val = UNALIGNED_GET((uint32_t *)ptr);

	return sys_le32_to_cpu(val);
}

uint32_t buf_simple_remove_be32(struct buf_simple *buf)
{
	uint32_t val;
	void *ptr;

	ptr = buf_simple_remove_mem(buf, sizeof(val));
	val = UNALIGNED_GET((uint32_t *)ptr);

	return sys_be32_to_cpu(val);
}

uint64_t buf_simple_remove_le48(struct buf_simple *buf)
{
	struct uint48 {
		uint64_t u48 : 48;
	} __packed val;
	void *ptr;

	ptr = buf_simple_remove_mem(buf, sizeof(val));
	val = UNALIGNED_GET((struct uint48 *)ptr);

	return sys_le48_to_cpu(val.u48);
}

uint64_t buf_simple_remove_be48(struct buf_simple *buf)
{
	struct uint48 {
		uint64_t u48 : 48;
	} __packed val;
	void *ptr;

	ptr = buf_simple_remove_mem(buf, sizeof(val));
	val = UNALIGNED_GET((struct uint48 *)ptr);

	return sys_be48_to_cpu(val.u48);
}

uint64_t buf_simple_remove_le64(struct buf_simple *buf)
{
	uint64_t val;
	void *ptr;

	ptr = buf_simple_remove_mem(buf, sizeof(val));
	val = UNALIGNED_GET((uint64_t *)ptr);

	return sys_le64_to_cpu(val);
}

uint64_t buf_simple_remove_be64(struct buf_simple *buf)
{
	uint64_t val;
	void *ptr;

	ptr = buf_simple_remove_mem(buf, sizeof(val));
	val = UNALIGNED_GET((uint64_t *)ptr);

	return sys_be64_to_cpu(val);
}

void *buf_simple_push(struct buf_simple *buf, size_t len)
{
	BUF_SIMPLE_DBG("buf %p len %zu", buf, len);

	__ASSERT_NO_MSG(buf_simple_headroom(buf) >= len);

	buf->data -= len;
	buf->len += len;
	return buf->data;
}

void *buf_simple_push_mem(struct buf_simple *buf, const void *mem,
			      size_t len)
{
	BUF_SIMPLE_DBG("buf %p len %zu", buf, len);

	return memcpy(buf_simple_push(buf, len), mem, len);
}

void buf_simple_push_le16(struct buf_simple *buf, uint16_t val)
{
	BUF_SIMPLE_DBG("buf %p val %u", buf, val);

	sys_put_le16(val, buf_simple_push(buf, sizeof(val)));
}

void buf_simple_push_be16(struct buf_simple *buf, uint16_t val)
{
	BUF_SIMPLE_DBG("buf %p val %u", buf, val);

	sys_put_be16(val, buf_simple_push(buf, sizeof(val)));
}

void buf_simple_push_u8(struct buf_simple *buf, uint8_t val)
{
	uint8_t *data = buf_simple_push(buf, 1);

	*data = val;
}

void buf_simple_push_le24(struct buf_simple *buf, uint32_t val)
{
	BUF_SIMPLE_DBG("buf %p val %u", buf, val);

	sys_put_le24(val, buf_simple_push(buf, 3));
}

void buf_simple_push_be24(struct buf_simple *buf, uint32_t val)
{
	BUF_SIMPLE_DBG("buf %p val %u", buf, val);

	sys_put_be24(val, buf_simple_push(buf, 3));
}

void buf_simple_push_le32(struct buf_simple *buf, uint32_t val)
{
	BUF_SIMPLE_DBG("buf %p val %u", buf, val);

	sys_put_le32(val, buf_simple_push(buf, sizeof(val)));
}

void buf_simple_push_be32(struct buf_simple *buf, uint32_t val)
{
	BUF_SIMPLE_DBG("buf %p val %u", buf, val);

	sys_put_be32(val, buf_simple_push(buf, sizeof(val)));
}

void buf_simple_push_le48(struct buf_simple *buf, uint64_t val)
{
	BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

	sys_put_le48(val, buf_simple_push(buf, 6));
}

void buf_simple_push_be48(struct buf_simple *buf, uint64_t val)
{
	BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

	sys_put_be48(val, buf_simple_push(buf, 6));
}

void buf_simple_push_le64(struct buf_simple *buf, uint64_t val)
{
	BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

	sys_put_le64(val, buf_simple_push(buf, sizeof(val)));
}

void buf_simple_push_be64(struct buf_simple *buf, uint64_t val)
{
	BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

	sys_put_be64(val, buf_simple_push(buf, sizeof(val)));
}

void *buf_simple_pull(struct buf_simple *buf, size_t len)
{
	BUF_SIMPLE_DBG("buf %p len %zu", buf, len);

	__ASSERT_NO_MSG(buf->len >= len);

	buf->len -= len;
	return buf->data += len;
}

void *buf_simple_pull_mem(struct buf_simple *buf, size_t len)
{
	void *data = buf->data;

	BUF_SIMPLE_DBG("buf %p len %zu", buf, len);

	__ASSERT_NO_MSG(buf->len >= len);

	buf->len -= len;
	buf->data += len;

	return data;
}

uint8_t buf_simple_pull_u8(struct buf_simple *buf)
{
	uint8_t val;

	val = buf->data[0];
	buf_simple_pull(buf, 1);

	return val;
}

uint16_t buf_simple_pull_le16(struct buf_simple *buf)
{
	uint16_t val;

	val = UNALIGNED_GET((uint16_t *)buf->data);
	buf_simple_pull(buf, sizeof(val));

	return sys_le16_to_cpu(val);
}

uint16_t buf_simple_pull_be16(struct buf_simple *buf)
{
	uint16_t val;

	val = UNALIGNED_GET((uint16_t *)buf->data);
	buf_simple_pull(buf, sizeof(val));

	return sys_be16_to_cpu(val);
}

uint32_t buf_simple_pull_le24(struct buf_simple *buf)
{
	struct uint24 {
		uint32_t u24:24;
	} __packed val;

	val = UNALIGNED_GET((struct uint24 *)buf->data);
	buf_simple_pull(buf, sizeof(val));

	return sys_le24_to_cpu(val.u24);
}

uint32_t buf_simple_pull_be24(struct buf_simple *buf)
{
	struct uint24 {
		uint32_t u24:24;
	} __packed val;

	val = UNALIGNED_GET((struct uint24 *)buf->data);
	buf_simple_pull(buf, sizeof(val));

	return sys_be24_to_cpu(val.u24);
}

uint32_t buf_simple_pull_le32(struct buf_simple *buf)
{
	uint32_t val;

	val = UNALIGNED_GET((uint32_t *)buf->data);
	buf_simple_pull(buf, sizeof(val));

	return sys_le32_to_cpu(val);
}

uint32_t buf_simple_pull_be32(struct buf_simple *buf)
{
	uint32_t val;

	val = UNALIGNED_GET((uint32_t *)buf->data);
	buf_simple_pull(buf, sizeof(val));

	return sys_be32_to_cpu(val);
}

uint64_t buf_simple_pull_le48(struct buf_simple *buf)
{
	struct uint48 {
		uint64_t u48:48;
	} __packed val;

	val = UNALIGNED_GET((struct uint48 *)buf->data);
	buf_simple_pull(buf, sizeof(val));

	return sys_le48_to_cpu(val.u48);
}

uint64_t buf_simple_pull_be48(struct buf_simple *buf)
{
	struct uint48 {
		uint64_t u48:48;
	} __packed val;

	val = UNALIGNED_GET((struct uint48 *)buf->data);
	buf_simple_pull(buf, sizeof(val));

	return sys_be48_to_cpu(val.u48);
}

uint64_t buf_simple_pull_le64(struct buf_simple *buf)
{
	uint64_t val;

	val = UNALIGNED_GET((uint64_t *)buf->data);
	buf_simple_pull(buf, sizeof(val));

	return sys_le64_to_cpu(val);
}

uint64_t buf_simple_pull_be64(struct buf_simple *buf)
{
	uint64_t val;

	val = UNALIGNED_GET((uint64_t *)buf->data);
	buf_simple_pull(buf, sizeof(val));

	return sys_be64_to_cpu(val);
}

size_t buf_simple_headroom(struct buf_simple *buf)
{
	return buf->data - buf->__buf;
}

size_t buf_simple_tailroom(struct buf_simple *buf)
{
	return buf->size - buf_simple_headroom(buf) - buf->len;
}

uint16_t buf_simple_max_len(struct buf_simple *buf)
{
	return buf->size - buf_simple_headroom(buf);
}
