/*
 * File      : rtdevice.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-01-08     bernard      first version.
 */

#ifndef __RT_DEVICE_H__
#define __RT_DEVICE_H__

#include <rtthread.h>

#define RT_DEVICE(device)            ((rt_device_t)device)

/* completion flag */
struct rt_completion
{
    rt_uint32_t flag;

    /* suspended list */
    rt_list_t suspended_list;
};

/* ring buffer */
struct rt_ringbuffer
{
    rt_uint8_t *buffer_ptr;
    /* use the msb of the {read,write}_index as mirror bit. You can see this as
     * if the buffer adds a virtual mirror and the pointers point either to the
     * normal or to the mirrored buffer. If the write_index has the same value
     * with the read_index, but in differenct mirro, the buffer is full. While
     * if the write_index and the read_index are the same and within the same
     * mirror, the buffer is empty. The ASCII art of the ringbuffer is:
     *
     *          mirror = 0                    mirror = 1
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Full
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     *  read_idx-^                   write_idx-^
     *
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Empty
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * read_idx-^ ^-write_idx
     *
     * The tradeoff is we could only use 32KiB of buffer for 16 bit of index.
     * But it should be enough for most of the cases.
     *
     * Ref: http://en.wikipedia.org/wiki/Circular_buffer#Mirroring */
    rt_uint16_t read_mirror : 1;
    rt_uint16_t read_index : 15;
    rt_uint16_t write_mirror : 1;
    rt_uint16_t write_index : 15;
    /* as we use msb of index as mirror bit, the size should be signed and
     * could only be positive. */
    rt_int16_t buffer_size;
};
typedef struct rt_ringbuffer* rt_ringbuffer_t;

/** return the size of data in rb */
rt_inline rt_uint16_t RT_RINGBUFFER_SIZE(struct rt_ringbuffer *rb)
{
    if (rb->read_index == rb->write_index)
    {
        if (rb->read_mirror == rb->write_mirror)
            /* we are in the same side, the ringbuffer is empty. */
            return 0;
        else
            return rb->buffer_size;
    }
    else
    {
        if (rb->write_index > rb->read_index)
            return rb->write_index - rb->read_index;
        else
            return rb->buffer_size - (rb->read_index - rb->write_index);
    }
}

/** return the size of empty space in rb */
#define RT_RINGBUFFER_EMPTY(rb) ((rb)->buffer_size - RT_RINGBUFFER_SIZE(rb))

/* pipe device */
#define PIPE_DEVICE(device)          ((struct rt_pipe_device*)(device))
struct rt_pipe_device
{
    struct rt_device parent;

    /* ring buffer in pipe device */
    struct rt_ringbuffer ringbuffer;

    /* suspended list */
    rt_list_t suspended_read_list;
    rt_list_t suspended_write_list;
};

#define RT_DATAQUEUE_EVENT_UNKNOWN   0x00
#define RT_DATAQUEUE_EVENT_POP       0x01
#define RT_DATAQUEUE_EVENT_PUSH      0x02
#define RT_DATAQUEUE_EVENT_LWM       0x03

struct rt_data_item;
#define RT_DATAQUEUE_SIZE(dq)        ((dq)->put_index - (dq)->get_index)
#define RT_DATAQUEUE_EMPTY(dq)       ((dq)->size - RT_DATAQUEUE_SIZE(dq))
/* data queue implementation */
struct rt_data_queue
{
    rt_uint16_t size;
    rt_uint16_t lwm;
    rt_bool_t   waiting_lwm;

    rt_uint16_t get_index;
    rt_uint16_t put_index;

    struct rt_data_item *queue;

    rt_list_t suspended_push_list;
    rt_list_t suspended_pop_list;

    /* event notify */
    void (*evt_notify)(struct rt_data_queue *queue, rt_uint32_t event);
};

/**
 * Completion
 */
void rt_completion_init(struct rt_completion *completion);
rt_err_t rt_completion_wait(struct rt_completion *completion,
                            rt_int32_t            timeout);
void rt_completion_done(struct rt_completion *completion);

/**
 * RingBuffer for DeviceDriver
 *
 * Please note that the ring buffer implementation of RT-Thread
 * has no thread wait or resume feature.
 */
void rt_ringbuffer_init(struct rt_ringbuffer *rb,
                        rt_uint8_t           *pool,
                        rt_int16_t            size);
rt_size_t rt_ringbuffer_put(struct rt_ringbuffer *rb,
                            const rt_uint8_t     *ptr,
                            rt_uint16_t           length);
rt_size_t rt_ringbuffer_putchar(struct rt_ringbuffer *rb,
                                const rt_uint8_t      ch);
rt_size_t rt_ringbuffer_get(struct rt_ringbuffer *rb,
                            rt_uint8_t           *ptr,
                            rt_uint16_t           length);
rt_size_t rt_ringbuffer_getchar(struct rt_ringbuffer *rb, rt_uint8_t *ch);
rt_inline rt_uint16_t rt_ringbuffer_get_size(struct rt_ringbuffer *rb)
{
    RT_ASSERT(rb != RT_NULL);
    return rb->buffer_size;
}

/**
 * Pipe Device
 */
rt_err_t rt_pipe_create(const char *name, rt_size_t size);
void rt_pipe_destroy(struct rt_pipe_device *pipe);

/**
 * DataQueue for DeviceDriver
 */
rt_err_t rt_data_queue_init(struct rt_data_queue *queue,
                            rt_uint16_t           size,
                            rt_uint16_t           lwm,
                            void (*evt_notify)(struct rt_data_queue *queue, rt_uint32_t event));
rt_err_t rt_data_queue_push(struct rt_data_queue *queue,
                            const void           *data_ptr,
                            rt_size_t             data_size,
                            rt_int32_t            timeout);
rt_err_t rt_data_queue_pop(struct rt_data_queue *queue,
                           const void          **data_ptr,
                           rt_size_t            *size,
                           rt_int32_t            timeout);
rt_err_t rt_data_queue_peak(struct rt_data_queue *queue,
                            const void          **data_ptr,
                            rt_size_t            *size);
void rt_data_queue_reset(struct rt_data_queue *queue);

#ifdef RT_USING_RTC
#include "drivers/rtc.h"
#ifdef RT_USING_ALARM
#include "drivers/alarm.h"
#endif
#endif /* RT_USING_RTC */

#ifdef RT_USING_SPI
#include "drivers/spi.h"
#endif /* RT_USING_SPI */

#ifdef RT_USING_MTD_NOR
#include "drivers/mtd_nor.h"
#endif /* RT_USING_MTD_NOR */

#ifdef RT_USING_MTD_NAND
#include "drivers/mtd_nand.h"
#endif /* RT_USING_MTD_NAND */

#ifdef RT_USING_USB_DEVICE
#include "drivers/usb_device.h"
#endif /* RT_USING_USB_DEVICE */

#ifdef RT_USING_USB_HOST
#include "drivers/usb_host.h"
#endif /* RT_USING_USB_HOST */

#ifdef RT_USING_SERIAL
#include "drivers/serial.h"
#endif /* RT_USING_SERIAL */

#ifdef RT_USING_I2C
#include "drivers/i2c.h"
#include "drivers/i2c_dev.h"

#ifdef RT_USING_I2C_BITOPS
#include "drivers/i2c-bit-ops.h"
#endif /* RT_USING_I2C_BITOPS */
#endif /* RT_USING_I2C */

#ifdef RT_USING_SDIO
#include "drivers/mmcsd_core.h"
#include "drivers/sd.h"
#include "drivers/sdio.h"
#endif

#endif /* __RT_DEVICE_H__ */
