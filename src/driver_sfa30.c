/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      driver_sfa30.c
 * @brief     driver sfa30 source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2023-07-30
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2023/07/30  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_sfa30.h"

/**
 * @brief chip information definition
 */
#define CHIP_NAME                 "Sensirion SFA30"        /**< chip name */
#define MANUFACTURER_NAME         "Sensirion"              /**< manufacturer name */
#define SUPPLY_VOLTAGE_MIN        3.15f                    /**< chip min supply voltage */
#define SUPPLY_VOLTAGE_MAX        5.50f                    /**< chip max supply voltage */
#define MAX_CURRENT               5.0f                     /**< chip max current */
#define TEMPERATURE_MIN           0.0f                     /**< chip min operating temperature */
#define TEMPERATURE_MAX           40.0f                    /**< chip max operating temperature */
#define DRIVER_VERSION            1000                     /**< driver version */

/**
 * @brief chip address definition
 */
#define SFA30_ADDRESS             (0x5D << 1)              /**< chip iic address */

/**
 * @brief chip command definition
 */
#define SFA30_IIC_COMMAND_START_MEASUREMENT                        0x0006U        /**< start measurement command */
#define SFA30_IIC_COMMAND_STOP_MEASUREMENT                         0x0104U        /**< stop measurement command */
#define SFA30_IIC_COMMAND_READ_MEASURED_VALUES                     0x0327U        /**< read measured values command */
#define SFA30_IIC_COMMAND_READ_DEVICE_INFORMATION                  0xD060U        /**< read product type command */
#define SFA30_IIC_COMMAND_RESET                                    0xD304U        /**< reset command */
#define SFA30_UART_COMMAND_START_MEASUREMENT                       0x00           /**< start measurement command */
#define SFA30_UART_COMMAND_STOP_MEASUREMENT                        0x01           /**< stop measurement command */
#define SFA30_UART_COMMAND_READ_MEASURED_VALUES                    0x03           /**< read measured values command */
#define SFA30_UART_COMMAND_READ_DEVICE_INFORMATION                 0xD0           /**< read product type command */
#define SFA30_UART_COMMAND_RESET                                   0xD3           /**< reset command */

/**
 * @brief     generate the crc
 * @param[in] *handle points to an sfa30 handle structure
 * @param[in] *data points to a data buffer
 * @param[in] count is the data length
 * @return    crc
 * @note      none
 */
static uint8_t a_sfa30_generate_crc(sfa30_handle_t *handle, uint8_t* data, uint8_t count)
{
    if (handle->iic_uart == SFA30_INTERFACE_IIC)
    {
        uint8_t current_byte;
        uint8_t crc = 0xFF;
        uint8_t crc_bit;

        for (current_byte = 0; current_byte < count; ++current_byte)        /* calculate crc */
        {
            crc ^= (data[current_byte]);                                    /* xor data */
            for (crc_bit = 8; crc_bit > 0; --crc_bit)                       /* 8 bit */
            {
                if ((crc & 0x80) != 0)                                      /* if 7th bit is 1 */
                {
                    crc = (crc << 1) ^ 0x31;                                /* xor */
                }
                else
                {
                    crc = crc << 1;                                         /* left shift 1 */
                }
            }
        }

        return crc;                                                         /* return crc */
    }
    else
    {
        uint8_t i;
        uint32_t crc = 0x00;

        for (i = 0; i < count; i++)                                         /* sum */
        {
            crc += data[i];                                                 /* sum */
        }

        return (uint8_t)(~(crc & 0xFF));                                    /* take the least significant byte of the result and invert it */
    }
}

/**
 * @brief      read bytes
 * @param[in]  *handle points to an sfa30 handle structure
 * @param[in]  addr is the iic device address
 * @param[in]  reg is the iic register address
 * @param[out] *data points to a data buffer
 * @param[in]  len is the data length
 * @param[in]  delay_ms is the delay time in ms
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
static uint8_t a_sfa30_iic_read(sfa30_handle_t *handle, uint8_t addr, uint16_t reg, uint8_t *data, uint16_t len, uint16_t delay_ms)
{
    uint8_t buf[2];

    buf[0] = (reg >> 8) & 0xFF;                                  /* set msb */
    buf[1] = (reg >> 0) & 0xFF;                                  /* set lsb */
    if (handle->iic_write_cmd(addr, (uint8_t *)buf, 2) != 0)     /* write data */
    {
        return 1;                                                /* return error */
    }
    handle->delay_ms(delay_ms);                                  /* delay ms */
    if (handle->iic_read_cmd(addr, (uint8_t *)data, len) != 0)   /* read data */
    {
        return 1;                                                /* return error */
    }

    return 0;                                                    /* success return 0 */
}

/**
 * @brief     write bytes
 * @param[in] *handle points to an sfa30 handle structure
 * @param[in] addr is the iic device address
 * @param[in] reg is the iic register address
 * @param[in] *data points to a data buffer
 * @param[in] len is the data length
 * @param[in] delay_ms is the delay time in ms
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
static uint8_t a_sfa30_iic_write(sfa30_handle_t *handle, uint8_t addr, uint16_t reg, uint8_t *data, uint16_t len, uint16_t delay_ms)
{
    uint8_t buf[16];

    if (len > 14)                                                    /* check length */
    {
        return 1;                                                    /* return error */
    }
    buf[0] = (reg >> 8) & 0xFF;                                      /* set msb */
    buf[1] = (reg >> 0) & 0xFF;                                      /* set lsb */
    memcpy((uint8_t *)&buf[2], data, len);                           /* copy data */
    if (handle->iic_write_cmd(addr, (uint8_t *)buf, len + 2) != 0)   /* write data */
    {
        return 1;                                                    /* return error */
    }
    handle->delay_ms(delay_ms);                                      /* delay ms */

    return 0;                                                        /* success return 0 */
}

/**
 * @brief      uart get the rx frame
 * @param[in]  *handle points to an sfa30 handle structure
 * @param[in]  len is the buffer length
 * @param[out] *output points to an output buffer
 * @param[in]  out_len is the output length
 * @return     status code
 *             - 0 success
 *             - 1 uart get rx frame failed
 * @note       none
 */
static uint8_t a_sfa30_uart_get_rx_frame(sfa30_handle_t *handle, uint16_t len, uint8_t *output, uint16_t out_len)
{
    uint16_t i, point;

    output[0] = handle->buf[0];                                                  /* set buf[0] */
    point = 1;                                                                   /* set point 1 */
    for (i = 1; i < (len - 1); i++)                                              /* run n -2 times */
    {
        if (point >= (out_len - 1))                                              /* check length */
        {
            return 1;                                                            /* return error */
        }
        if (((handle->buf[i] == 0x7D) && (handle->buf[i + 1]) == 0x5E) ||        /* check buffer */
            ((handle->buf[i] == 0x7D) && (handle->buf[i + 1]) == 0x5D) ||
            ((handle->buf[i] == 0x7D) && (handle->buf[i + 1]) == 0x31) ||
            ((handle->buf[i] == 0x7D) && (handle->buf[i + 1]) == 0x33)
           )
        {
            switch (handle->buf[i + 1])                                          /* judge */
            {
                case 0x5E :                                                      /* 0x5E */
                {
                    output[point] = 0x7E;                                        /* set output */
                    point++;                                                     /* point++ */
                    i++;                                                         /* i++ */

                    break;                                                       /* break */
                }
                case 0x5D :                                                      /* 0x5D */
                {
                    output[point] = 0x7D;                                        /* set output */
                    point++;                                                     /* point++ */
                    i++;                                                         /* i++ */

                    break;                                                       /* break */
                }
                case 0x31 :                                                      /* 0x31 */
                {
                    output[point] = 0x11;                                        /* set output */
                    point++;                                                     /* point++ */
                    i++;                                                         /* i++ */

                    break;                                                       /* break */
                }
                case 0x33 :                                                      /* 0x33 */
                {
                    output[point] = 0x13;                                        /* set output */
                    point++;                                                     /* point++ */
                    i++;                                                         /* i++ */

                    break;                                                       /* break */
                }
                default :
                {
                    break;                                                       /* break */
                }
            }
        }
        else
        {
            output[point] = handle->buf[i];                                      /* set output */
            point++;                                                             /* point++ */
        }
    }
    output[point] = handle->buf[len - 1];                                        /* set the end part */
    point++;                                                                     /* point++ */
    if (point != out_len)                                                        /* check point */
    {
        return 1;                                                                /* return error */
    }

    return 0;                                                                    /* success return 0 */
}

/**
 * @brief      uart set the tx frame
 * @param[in]  *handle points to an sfa30 handle structure
 * @param[in]  *input points to an input buffer
 * @param[in]  in_len is the input length
 * @param[out] *out_len points to an output length buffer
 * @return     status code
 *             - 0 success
 *             - 1 uart set tx frame failed
 * @note       none
 */
static uint8_t a_sfa30_uart_set_tx_frame(sfa30_handle_t *handle, uint8_t *input, uint16_t in_len, uint16_t *out_len)
{
    uint16_t i;

    memset(handle->buf, 0, sizeof(uint8_t) * 256);             /* clear buffer */
    handle->buf[0] = input[0];                                 /* set buf[0] */
    *out_len = 1;                                              /* set output length */
    for (i = 1; i < (in_len - 1); i++)
    {
        if ((*out_len) >= 255)                                 /* check output length */
        {
            return 1;                                          /* return error */
        }
        if ((input[i] == 0x7E) || (input[i] == 0x7D) ||        /* check data */
            (input[i] == 0x11) || (input[i] == 0x13)
           )
        {
            switch (input[i])                                  /* judge input */
            {
                case 0x7E :                                    /* 0x7E */
                {
                    handle->buf[*out_len] = 0x7D;              /* 0x7D */
                    (*out_len)++;                              /* output length++ */
                    handle->buf[*out_len] = 0x5E;              /* 0x5E */
                    (*out_len)++;                              /* output length++ */

                    break;                                     /* break */
                }
                case 0x7D :                                    /* 0x7D */
                {
                    handle->buf[*out_len] = 0x7D;              /* set 0x7D */
                    (*out_len)++;                              /* output length++ */
                    handle->buf[*out_len] = 0x5D;              /* set 0x5D */
                    (*out_len)++;                              /* output length++ */

                    break;                                     /* break */
                }
                case 0x11 :                                    /* 0x11 */
                {
                    handle->buf[*out_len] = 0x7D;              /* set 0x7D */
                    (*out_len)++;                              /* output length++ */
                    handle->buf[*out_len] = 0x31;              /* set 0x31 */
                    (*out_len)++;                              /* output length++ */

                    break;                                     /* break */
                }
                case 0x13 :                                    /* 0x13 */
                {
                    handle->buf[*out_len] = 0x7D;              /* set 0x7D */
                    (*out_len)++;                              /* output length++ */
                    handle->buf[*out_len] = 0x33;              /* set 0x33 */
                    (*out_len)++;                              /* output length++ */

                    break;                                     /* break */
                }
                default :
                {
                    break;                                     /* break */
                }
            }
        }
        else
        {
            handle->buf[*out_len] = input[i];                  /* set the buffer */
            (*out_len)++;                                      /* output length++ */
        }
    }
    handle->buf[*out_len] = input[in_len - 1];                 /* copy the end frame */
    (*out_len)++;                                              /* length++ */

    return 0;                                                  /* success return 0 */
}

/**
 * @brief      write read bytes
 * @param[in]  *handle points to an sfa30 handle structure
 * @param[in]  *input points to an input buffer
 * @param[in]  in_len is the input length
 * @param[in]  delay_ms is the delay time in ms
 * @param[out] *output points to an output buffer
 * @param[in]  out_len is the output length
 * @return     status code
 *             - 0 success
 *             - 1 write read failed
 * @note       none
 */
static uint8_t a_sfa30_uart_write_read(sfa30_handle_t *handle, uint8_t *input, uint16_t in_len,
                                       uint16_t delay_ms, uint8_t *output, uint16_t out_len)
{
    uint16_t len;

    if (a_sfa30_uart_set_tx_frame(handle, input, in_len, (uint16_t *)&len) != 0)        /* set tx frame */
    {
        return 1;                                                                       /* return error */
    }
    if (handle->uart_flush() != 0)                                                      /* uart flush */
    {
        return 1;                                                                       /* return error */
    }
    if (handle->uart_write(handle->buf, len) != 0)                                      /* write data */
    {
        return 1;                                                                       /* return error */
    }
    handle->delay_ms(delay_ms);                                                         /* delay ms */
    len = handle->uart_read(handle->buf, 256);                                          /* read data */
    if (a_sfa30_uart_get_rx_frame(handle, len, output, out_len) != 0)                   /* get rx frame */
    {
        return 1;                                                                       /* return error */
    }

    return 0;                                                                           /* success return 0 */
}

/**
 * @brief     print error
 * @param[in] *handle points to an sfa30 handle structure
 * @param[in] e is the error code
 * @return    error code
 * @note      none
 */
static uint8_t a_sfa30_uart_error(sfa30_handle_t *handle, uint8_t e)
{
    switch (e)
    {
        case 0x00 :
        {
            break;
        }
        case 0x01 :
        {
            handle->debug_print("sfa30: wrong data length for this command error.\n");        /* wrong data length for this command error */

            break;
        }
        case 0x02 :
        {
            handle->debug_print("sfa30: unknown command.\n");                                 /* unknown command */

            break;
        }
        case 0x03 :
        {
            handle->debug_print("sfa30: no access right for command.\n");                     /* no access right for command */

            break;
        }
        case 0x04 :
        {
            handle->debug_print("sfa30: illegal command parameter or parameter "
                                "out of allowed range.\n");                                   /* illegal command parameter or parameter out of allowed range */

            break;
        }
        case 0x20 :
        {
            handle->debug_print("sfa30: no measurement data available.\n");                   /* no measurement data available */

            break;
        }
        case 0x43 :
        {
            handle->debug_print("sfa30: command not allowed in current state.\n");            /* command not allowed in current state */

            break;
        }
        case 0x44 :
        {
            handle->debug_print("sfa30: internal error.\n");                                  /* internal error */

            break;
        }
        case 0x7F :
        {
            handle->debug_print("sfa30: general error.\n");                                   /* general error */

            break;
        }
        default :
        {
            handle->debug_print("sfa30: unknown code.\n");                                    /* unknown code */

            break;
        }
    }

    return e;                                                                                 /* return error code */
}

/**
 * @brief     set the chip interface
 * @param[in] *handle points to an sfa30 handle structure
 * @param[in] interface is the chip interface
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 * @note      none
 */
uint8_t sfa30_set_interface(sfa30_handle_t *handle, sfa30_interface_t interface)
{
    if (handle == NULL)                           /* check handle */
    {
        return 2;                                 /* return error */
    }

    handle->iic_uart = (uint8_t)interface;        /* set interface */

    return 0;                                     /* success return 0 */
}

/**
 * @brief      get the chip interface
 * @param[in]  *handle points to an sfa30 handle structure
 * @param[out] *interface points to a chip interface buffer
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t sfa30_get_interface(sfa30_handle_t *handle, sfa30_interface_t *interface)
{
    if (handle == NULL)                                        /* check handle */
    {
        return 2;                                              /* return error */
    }

    *interface = (sfa30_interface_t)(handle->iic_uart);        /* get interface */

    return 0;                                                  /* success return 0 */
}

/**
 * @brief     start the measurement
 * @param[in] *handle points to an sfa30 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 start measurement failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sfa30_start_measurement(sfa30_handle_t *handle)
{
    uint8_t res;

    if (handle == NULL)                                                                                         /* check handle */
    {
        return 2;                                                                                               /* return error */
    }
    if (handle->inited != 1)                                                                                    /* check handle initialization */
    {
        return 3;                                                                                               /* return error */
    }

    if (handle->iic_uart != 0)                                                                                  /* uart */
    {
        uint8_t input_buf[6 + 1];
        uint8_t out_buf[7];

        input_buf[0] = 0x7E;                                                                                    /* set start */
        input_buf[1] = 0x00;                                                                                    /* set addr */
        input_buf[2] = SFA30_UART_COMMAND_START_MEASUREMENT;                                                    /* set command */
        input_buf[3] = 0x01;                                                                                    /* set length */
        input_buf[4] = 0x00;                                                                                    /* set subcommand */
        input_buf[5] = a_sfa30_generate_crc(handle, (uint8_t *)&input_buf[1], 4);                               /* set crc */
        input_buf[6] = 0x7E;                                                                                    /* set stop */
        memset(out_buf, 0, sizeof(uint8_t) * 7);                                                                /* clear the buffer */
        res = a_sfa30_uart_write_read(handle, (uint8_t *)input_buf, 7, 10, (uint8_t *)out_buf, 7);              /* write read frame */
        if (res != 0)                                                                                           /* check result */
        {
            handle->debug_print("sfa30: write read failed.\n");                                                 /* write read failed */

            return 1;                                                                                           /* return error */
        }
        if (out_buf[5] != a_sfa30_generate_crc(handle, (uint8_t *)&out_buf[1], 4))                              /* check crc */
        {
            handle->debug_print("sfa30: crc check error.\n");                                                   /* crc check error */

            return 1;                                                                                           /* return error */
        }
        if (a_sfa30_uart_error(handle, out_buf[3]) != 0)                                                        /* check status */
        {
            return 1;                                                                                           /* return error */
        }
    }
    else                                                                                                        /* iic */
    {
        res = a_sfa30_iic_write(handle, SFA30_ADDRESS, SFA30_IIC_COMMAND_START_MEASUREMENT, NULL, 0, 1);        /* start measurement command */
        if (res != 0)                                                                                           /* check result */
        {
            handle->debug_print("sfa30: start measurement failed.\n");                                          /* start measurement failed */

            return 1;                                                                                           /* return error */
        }
    }

    return 0;                                                                                                   /* success return 0 */
}

/**
 * @brief     stop the measurement
 * @param[in] *handle points to an sfa30 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 stop measurement failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sfa30_stop_measurement(sfa30_handle_t *handle)
{
    uint8_t res;

    if (handle == NULL)                                                                                        /* check handle */
    {
        return 2;                                                                                              /* return error */
    }
    if (handle->inited != 1)                                                                                   /* check handle initialization */
    {
        return 3;                                                                                              /* return error */
    }

    if (handle->iic_uart != 0)                                                                                 /* uart */
    {
        uint8_t input_buf[6 + 0];
        uint8_t out_buf[7];

        input_buf[0] = 0x7E;                                                                                   /* set start */
        input_buf[1] = 0x00;                                                                                   /* set addr */
        input_buf[2] = SFA30_UART_COMMAND_STOP_MEASUREMENT;                                                    /* set command */
        input_buf[3] = 0x00;                                                                                   /* set length */
        input_buf[4] = a_sfa30_generate_crc(handle, (uint8_t *)&input_buf[1], 3);                              /* set crc */
        input_buf[5] = 0x7E;                                                                                   /* set stop */
        memset(out_buf, 0, sizeof(uint8_t) * 7);                                                               /* clear the buffer */
        res = a_sfa30_uart_write_read(handle, (uint8_t *)input_buf, 6, 10, (uint8_t *)out_buf, 7);             /* write read frame */
        if (res != 0)                                                                                          /* check result */
        {
            handle->debug_print("sfa30: write read failed.\n");                                                /* write read failed */

            return 1;                                                                                          /* return error */
        }
        if (out_buf[5] != a_sfa30_generate_crc(handle, (uint8_t *)&out_buf[1], 4))                             /* check crc */
        {
            handle->debug_print("sfa30: crc check error.\n");                                                  /* crc check error */

            return 1;                                                                                          /* return error */
        }
        if (a_sfa30_uart_error(handle, out_buf[3]) != 0)                                                       /* check status */
        {
            return 1;                                                                                          /* return error */
        }
    }
    else                                                                                                       /* iic */
    {
        res = a_sfa30_iic_write(handle, SFA30_ADDRESS, SFA30_IIC_COMMAND_STOP_MEASUREMENT, NULL, 0, 50);       /* stop measurement command */
        if (res != 0)                                                                                          /* check result */
        {
            handle->debug_print("sfa30: stop measurement failed.\n");                                          /* stop measurement failed */

            return 1;                                                                                          /* return error */
        }
    }

    return 0;                                                                                                  /* success return 0 */
}

/**
 * @brief      get device information
 * @param[in]  *handle points to an sfa30 handle structure
 * @param[out] *info points to an info buffer
 * @return     status code
 *             - 0 success
 *             - 1 get device information failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t sfa30_get_device_information(sfa30_handle_t *handle, char info[32])
{
    uint8_t res;

    if (handle == NULL)                                                                                                   /* check handle */
    {
        return 2;                                                                                                         /* return error */
    }
    if (handle->inited != 1)                                                                                              /* check handle initialization */
    {
        return 3;                                                                                                         /* return error */
    }


    if (handle->iic_uart != 0)                                                                                            /* uart */
    {
        uint8_t input_buf[6 + 1];
        uint8_t out_buf[7 + 17];

        input_buf[0] = 0x7E;                                                                                              /* set start */
        input_buf[1] = 0x00;                                                                                              /* set addr */
        input_buf[2] = SFA30_UART_COMMAND_READ_DEVICE_INFORMATION;                                                        /* set command */
        input_buf[3] = 0x01;                                                                                              /* set length */
        input_buf[4] = 0x06;                                                                                              /* set subcommand */
        input_buf[5] = a_sfa30_generate_crc(handle, (uint8_t *)&input_buf[1], 4);                                         /* set crc */
        input_buf[6] = 0x7E;                                                                                              /* set stop */
        memset(info, 0, sizeof(char) * 32);                                                                               /* clear info */
        memset(out_buf, 0, sizeof(uint8_t) * 24);                                                                         /* clear the buffer */
        res = a_sfa30_uart_write_read(handle, (uint8_t *)input_buf, 7, 10, (uint8_t *)out_buf, 24);                       /* write read frame */
        if (res != 0)                                                                                                     /* check result */
        {
            handle->debug_print("sfa30: write read failed.\n");                                                           /* write read failed */

            return 1;                                                                                                     /* return error */
        }
        if (out_buf[21] != a_sfa30_generate_crc(handle, (uint8_t *)&out_buf[1], 22))                                      /* check crc */
        {
            handle->debug_print("sfa30: crc check error.\n");                                                             /* crc check error */

            return 1;                                                                                                     /* return error */
        }
        if (a_sfa30_uart_error(handle, out_buf[3]) != 0)                                                                  /* check status */
        {
            return 1;                                                                                                     /* return error */
        }
        memcpy((uint8_t *)info, (uint8_t *)&out_buf[5], 17);                                                              /* copy data*/
    }
    else                                                                                                                  /* iic */
    {
        uint8_t i;
        uint8_t buf[48];

        memset(info, 0, sizeof(char) * 32);                                                                               /* clear info */
        memset(buf, 0, sizeof(uint8_t) * 48);                                                                             /* clear the buffer */
        res = a_sfa30_iic_read(handle, SFA30_ADDRESS, SFA30_IIC_COMMAND_READ_DEVICE_INFORMATION, (uint8_t *)buf,
                               48, 2);                                                                                    /* read measured values command */
        if (res != 0)                                                                                                     /* check result */
        {
            handle->debug_print("sfa30: read measured values failed.\n");                                                 /* read measured values failed */

            return 1;                                                                                                     /* return error */
        }
        for (i = 0; i < 16; i++)                                                                                          /* check crc */
        {
            if (buf[i * 3 + 2] != a_sfa30_generate_crc(handle, (uint8_t *)&buf[i * 3], 2))                                /* check crc */
            {
                handle->debug_print("sfa30: crc is error.\n");                                                            /* crc is error */

                return 1;                                                                                                 /* return error */
            }
        }
        for (i = 0; i < 16; i++)                                                                                          /* copy info */
        {
            info[i * 2 + 0] = (char)buf[i * 3 + 0];                                                                       /* copy info */
            info[i * 2 + 1] = (char)buf[i * 3 + 1];                                                                       /* copy info */
        }
    }

    return 0;                                                                                                             /* success return 0 */
}

/**
 * @brief     reset the chip
 * @param[in] *handle points to an sfa30 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 reset failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sfa30_reset(sfa30_handle_t *handle)
{
    uint8_t res;

    if (handle == NULL)                                                                              /* check handle */
    {
        return 2;                                                                                    /* return error */
    }
    if (handle->inited != 1)                                                                         /* check handle initialization */
    {
        return 3;                                                                                    /* return error */
    }

    if (handle->iic_uart != 0)                                                                       /* uart */
    {
        uint8_t input_buf[6];
        uint8_t out_buf[7];

        input_buf[0] = 0x7E;                                                                         /* set start */
        input_buf[1] = 0x00;                                                                         /* set addr */
        input_buf[2] = SFA30_UART_COMMAND_RESET;                                                     /* set command */
        input_buf[3] = 0x00;                                                                         /* set length */
        input_buf[4] = a_sfa30_generate_crc(handle, (uint8_t *)&input_buf[1], 3);                    /* set crc */
        input_buf[5] = 0x7E;                                                                         /* set stop */
        memset(out_buf, 0, sizeof(uint8_t) * 7);                                                     /* clear the buffer */
        res = a_sfa30_uart_write_read(handle, (uint8_t *)input_buf, 6, 200, (uint8_t *)out_buf, 7);  /* write read frame */
        if (res != 0)                                                                                /* check result */
        {
            handle->debug_print("sfa30: write read failed.\n");                                      /* write read failed */

            return 1;                                                                                /* return error */
        }
        if (out_buf[5] != a_sfa30_generate_crc(handle, (uint8_t *)&out_buf[1], 4))                   /* check crc */
        {
            handle->debug_print("sfa30: crc check error.\n");                                        /* crc check error */

            return 1;                                                                                /* return error */
        }
        if (a_sfa30_uart_error(handle, out_buf[3]) != 0)                                             /* check status */
        {
            return 1;                                                                                /* return error */
        }
    }
    else                                                                                             /* iic */
    {
        res = a_sfa30_iic_write(handle, SFA30_ADDRESS, SFA30_IIC_COMMAND_RESET, NULL, 0, 100);       /* reset command */
        if (res != 0)                                                                                /* check result */
        {
            handle->debug_print("sfa30: reset failed.\n");                                           /* reset failed */

            return 1;                                                                                /* return error */
        }
    }

    return 0;                                                                                        /* success return 0 */
}

/**
 * @brief      read the result
 * @param[in]  *handle points to an sfa30 handle structure
 * @param[out] *data points to an sfa30_data_t structure
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t sfa30_read(sfa30_handle_t *handle, sfa30_data_t *data)
{
    uint8_t res;

    if ((handle == NULL) || (data == NULL))                                                                                 /* check handle */
    {
        return 2;                                                                                                           /* return error */
    }
    if (handle->inited != 1)                                                                                                /* check handle initialization */
    {
        return 3;                                                                                                           /* return error */
    }

    if (handle->iic_uart != 0)                                                                                              /* uart */
    {
        uint8_t input_buf[6 + 1];
        uint8_t out_buf[7 + 6];

        input_buf[0] = 0x7E;                                                                                                /* set start */
        input_buf[1] = 0x00;                                                                                                /* set addr */
        input_buf[2] = SFA30_UART_COMMAND_READ_MEASURED_VALUES;                                                             /* set command */
        input_buf[3] = 0x01;                                                                                                /* set length */
        input_buf[4] = 0x02;                                                                                                /* set subcommand */
        input_buf[5] = a_sfa30_generate_crc(handle, (uint8_t *)&input_buf[1], 4);                                           /* set crc */
        input_buf[6] = 0x7E;                                                                                                /* set stop */
        memset(out_buf, 0, sizeof(uint8_t) * 13);                                                                           /* clear the buffer */
        res = a_sfa30_uart_write_read(handle, (uint8_t *)input_buf, 7, 100, (uint8_t *)out_buf, 13);                        /* write read frame */
        if (res != 0)                                                                                                       /* check result */
        {
            handle->debug_print("sfa30: write read failed.\n");                                                             /* write read failed */

            return 1;                                                                                                       /* return error */
        }
        if (out_buf[11] != a_sfa30_generate_crc(handle, (uint8_t *)&out_buf[1], 10))                                        /* check crc */
        {
            handle->debug_print("sfa30: crc check error.\n");                                                               /* crc check error */

            return 1;                                                                                                       /* return error */
        }
        if (a_sfa30_uart_error(handle, out_buf[3]) != 0)                                                                    /* check status */
        {
            return 1;                                                                                                       /* return error */
        }
        data->formaldehyde_raw = (uint16_t)(((uint16_t)(out_buf[5 + 0]) << 8) | ((uint16_t)(out_buf[5 + 1]) << 0));         /* copy formaldehyde */
        data->humidity_raw = (uint16_t)(((uint16_t)(out_buf[5 + 2]) << 8) | ((uint16_t)(out_buf[5 + 3]) << 0));             /* copy humidity */
        data->temperature_raw = (uint16_t)(((uint16_t)(out_buf[5 + 4]) << 8) | ((uint16_t)(out_buf[5 + 5]) << 0));          /* copy temperature*/
        data->formaldehyde = (float)(data->formaldehyde_raw) / 5.0f;                                                        /* convert formaldehyde */
        data->humidity = (float)(data->humidity_raw) / 100.0f;                                                              /* convert humidity */
        data->temperature = (float)(data->temperature_raw) / 200.0f;                                                        /* convert temperature*/
    }
    else                                                                                                                    /* iic */
    {
        uint8_t i;
        uint8_t buf[9];

        memset(buf, 0, sizeof(uint8_t) * 9);                                                                                /* clear the buffer */
        res = a_sfa30_iic_read(handle, SFA30_ADDRESS, SFA30_IIC_COMMAND_READ_MEASURED_VALUES, (uint8_t *)buf, 9, 5);        /* read measured values command */
        if (res != 0)                                                                                                       /* check result */
        {
            handle->debug_print("sfa30: read measured values failed.\n");                                                   /* read measured values failed */

            return 1;                                                                                                       /* return error */
        }
        for (i = 0; i < 3; i++)                                                                                             /* check crc */
        {
            if (buf[i * 3 + 2] != a_sfa30_generate_crc(handle, (uint8_t *)&buf[i * 3], 2))                                  /* check crc */
            {
                handle->debug_print("sfa30: crc is error.\n");                                                              /* crc is error */

                return 1;                                                                                                   /* return error */
            }
        }
        data->formaldehyde_raw = (uint16_t)(((uint16_t)(buf[0]) << 8) | ((uint16_t)(buf[1]) << 0));                         /* copy formaldehyde */
        data->humidity_raw = (uint16_t)(((uint16_t)(buf[3]) << 8) | ((uint16_t)(buf[4]) << 0));                             /* copy humidity */
        data->temperature_raw = (uint16_t)(((uint16_t)(buf[6]) << 8) | ((uint16_t)(buf[7]) << 0));                          /* copy temperature*/
        data->formaldehyde = (float)(data->formaldehyde_raw) / 5.0f;                                                        /* convert formaldehyde */
        data->humidity = (float)(data->humidity_raw) / 100.0f;                                                              /* convert humidity */
        data->temperature = (float)(data->temperature_raw) / 200.0f;                                                        /* convert temperature*/
    }

    return 0;                                                                                                               /* success return 0 */
}

/**
 * @brief     initialize the chip
 * @param[in] *handle points to an sfa30 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 iic or uart initialization failed
 *            - 2 handle is NULL
 *            - 3 linked functions is NULL
 *            - 4 reset failed
 * @note      none
 */
uint8_t sfa30_init(sfa30_handle_t *handle)
{
    uint8_t res;

    if (handle == NULL)                                                                              /* check handle */
    {
        return 2;                                                                                    /* return error */
    }
    if (handle->debug_print == NULL)                                                                 /* check debug_print */
    {
        return 3;                                                                                    /* return error */
    }
    if (handle->iic_init == NULL)                                                                    /* check iic_init */
    {
        handle->debug_print("sfa30: iic_init is null.\n");                                           /* iic_init is null */

        return 3;                                                                                    /* return error */
    }
    if (handle->iic_deinit == NULL)                                                                  /* check iic_deinit */
    {
        handle->debug_print("sfa30: iic_deinit is null.\n");                                         /* iic_deinit is null */

        return 3;                                                                                    /* return error */
    }
    if (handle->iic_write_cmd == NULL)                                                               /* check iic_write_cmd */
    {
        handle->debug_print("sfa30: iic_write_cmd is null.\n");                                      /* iic_write_cmd is null */

        return 3;                                                                                    /* return error */
    }
    if (handle->iic_read_cmd == NULL)                                                                /* check iic_read_cmd */
    {
        handle->debug_print("sfa30: iic_read_cmd is null.\n");                                       /* iic_read_cmd is null */

        return 3;                                                                                    /* return error */
    }
    if (handle->uart_init == NULL)                                                                   /* check uart_init */
    {
        handle->debug_print("sfa30: uart_init is null.\n");                                          /* uart_init is null */

        return 3;                                                                                    /* return error */
    }
    if (handle->uart_deinit == NULL)                                                                 /* check uart_deinit */
    {
        handle->debug_print("sfa30: uart_deinit is null.\n");                                        /* uart_deinit is null */

        return 3;                                                                                    /* return error */
    }
    if (handle->uart_read == NULL)                                                                   /* check uart_read */
    {
        handle->debug_print("sfa30: uart_read is null.\n");                                          /* uart_read is null */

        return 3;                                                                                    /* return error */
    }
    if (handle->uart_write == NULL)                                                                  /* check uart_write */
    {
        handle->debug_print("sfa30: uart_write is null.\n");                                         /* uart_write is null */

        return 3;                                                                                    /* return error */
    }
    if (handle->uart_flush == NULL)                                                                  /* check uart_flush */
    {
        handle->debug_print("sfa30: uart_flush is null.\n");                                         /* uart_flush is null */

        return 3;                                                                                    /* return error */
    }
    if (handle->delay_ms == NULL)                                                                    /* check delay_ms */
    {
        handle->debug_print("sfa30: delay_ms is null.\n");                                           /* delay_ms is null */

        return 3;                                                                                    /* return error */
    }

    if (handle->iic_uart != 0)                                                                       /* uart */
    {
        uint8_t input_buf[6];
        uint8_t out_buf[7];

        if (handle->uart_init() != 0)                                                                /* uart init */
        {
            handle->debug_print("sfa30: uart init failed.\n");                                       /* uart init failed */

            return 3;                                                                                /* return error */
        }
        input_buf[0] = 0x7E;                                                                         /* set start */
        input_buf[1] = 0x00;                                                                         /* set addr */
        input_buf[2] = SFA30_UART_COMMAND_RESET;                                                     /* set command */
        input_buf[3] = 0x00;                                                                         /* set length */
        input_buf[4] = a_sfa30_generate_crc(handle, (uint8_t *)&input_buf[1], 3);                    /* set crc */
        input_buf[5] = 0x7E;                                                                         /* set stop */
        memset(out_buf, 0, sizeof(uint8_t) * 7);                                                     /* clear the buffer */
        res = a_sfa30_uart_write_read(handle, (uint8_t *)input_buf, 6, 100, (uint8_t *)out_buf, 7);  /* write read frame */
        if (res != 0)                                                                                /* check result */
        {
            handle->debug_print("sfa30: write read failed.\n");                                      /* write read failed */
            (void)handle->uart_deinit();                                                             /* uart deinit */

            return 1;                                                                                /* return error */
        }
        if (out_buf[5] != a_sfa30_generate_crc(handle, (uint8_t *)&out_buf[1], 4))                   /* check crc */
        {
            handle->debug_print("sfa30: crc check error.\n");                                        /* crc check error */
            (void)handle->uart_deinit();                                                             /* uart deinit */

            return 1;                                                                                /* return error */
        }
        if (a_sfa30_uart_error(handle, out_buf[3]) != 0)                                             /* check status */
        {
            (void)handle->uart_deinit();                                                             /* uart deinit */

            return 1;                                                                                /* return error */
        }
    }
    else                                                                                             /* iic */
    {
        if (handle->iic_init() != 0)                                                                 /* iic init */
        {
            handle->debug_print("sfa30: iic init failed.\n");                                        /* iic init failed */

            return 3;                                                                                /* return error */
        }
        res = a_sfa30_iic_write(handle, SFA30_ADDRESS, SFA30_IIC_COMMAND_RESET, NULL, 0, 100);       /* reset command */
        if (res != 0)                                                                                /* check result */
        {
            handle->debug_print("sfa30: reset failed.\n");                                           /* reset failed */
            (void)handle->iic_deinit();                                                              /* iic deinit */

            return 4;                                                                                /* return error */
        }
    }
    handle->inited = 1;                                                                              /* flag finish initialization */

    return 0;                                                                                        /* success return 0 */
}

/**
 * @brief     close the chip
 * @param[in] *handle points to an sfa30 handle structure
 * @return    status code
 *            - 0 success
 *            - 1 iic or uart deinit failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 *            - 4 soft reset failed
 * @note      none
 */
uint8_t sfa30_deinit(sfa30_handle_t *handle)
{
    uint8_t res;

    if (handle == NULL)                                                                              /* check handle */
    {
        return 2;                                                                                    /* return error */
    }
    if (handle->inited != 1)                                                                         /* check handle initialization */
    {
        return 3;                                                                                    /* return error */
    }

    if (handle->iic_uart != 0)                                                                       /* uart */
    {
        uint8_t input_buf[6];
        uint8_t out_buf[7];

        input_buf[0] = 0x7E;                                                                         /* set start */
        input_buf[1] = 0x00;                                                                         /* set addr */
        input_buf[2] = SFA30_UART_COMMAND_RESET;                                                     /* set command */
        input_buf[3] = 0x00;                                                                         /* set length */
        input_buf[4] = a_sfa30_generate_crc(handle, (uint8_t *)&input_buf[1], 3);                    /* set crc */
        input_buf[5] = 0x7E;                                                                         /* set stop */
        memset(out_buf, 0, sizeof(uint8_t) * 7);                                                     /* clear the buffer */
        res = a_sfa30_uart_write_read(handle, (uint8_t *)input_buf, 6, 100, (uint8_t *)out_buf, 7);  /* write read frame */
        if (res != 0)                                                                                /* check result */
        {
            handle->debug_print("sfa30: write read failed.\n");                                      /* write read failed */

            return 4;                                                                                /* return error */
        }
        if (out_buf[5] != a_sfa30_generate_crc(handle, (uint8_t *)&out_buf[1], 4))                   /* check crc */
        {
            handle->debug_print("sfa30: crc check error.\n");                                        /* crc check error */

            return 4;                                                                                /* return error */
        }
        if (a_sfa30_uart_error(handle, out_buf[3]) != 0)                                             /* check status */
        {
            return 4;                                                                                /* return error */
        }
        if (handle->uart_deinit() != 0)                                                              /* uart deinit */
        {
            handle->debug_print("sfa30: uart deinit failed.\n");                                     /* uart deinit failed */

            return 1;                                                                                /* return error */
        }
    }
    else                                                                                             /* iic */
    {
        res = a_sfa30_iic_write(handle, SFA30_ADDRESS, SFA30_IIC_COMMAND_RESET, NULL, 0, 100);       /* reset command */
        if (res != 0)                                                                                /* check result */
        {
            handle->debug_print("sfa30: reset failed.\n");                                           /* reset failed */

            return 4;                                                                                /* return error */
        }
        res = handle->iic_deinit();                                                                  /* iic deinit */
        if (res != 0)                                                                                /* check result */
        {
            handle->debug_print("sfa30: iic deinit failed.\n");                                      /* iic deinit */

            return 1;                                                                                /* return error */
        }
    }

    handle->inited = 0;                                                                              /* flag close initialization */

    return 0;                                                                                        /* success return 0 */
}

/**
 * @brief      set and get the chip register with uart interface
 * @param[in]  *handle points to an sfa30 handle structure
 * @param[in]  *input points to an input buffer
 * @param[in]  in_len is the input length
 * @param[out] *output points to an output buffer
 * @param[in]  out_len is the output length
 * @return     status code
 *             - 0 success
 *             - 1 write read failed
 * @note       none
 */
uint8_t sfa30_set_get_reg_uart(sfa30_handle_t *handle, uint8_t *input, uint16_t in_len, uint8_t *output, uint16_t out_len)
{
    if (handle == NULL)                                                                   /* check handle */
    {
        return 2;                                                                         /* return error */
    }
    if (handle->inited != 1)                                                              /* check handle initialization */
    {
        return 3;                                                                         /* return error */
    }

    if (handle->iic_uart != 0)
    {
        return a_sfa30_uart_write_read(handle, input, in_len, 20, output, out_len);       /* write and read with the uart interface */
    }
    else
    {
        handle->debug_print("sfa30: iic interface is invalid.\n");                        /* iic interface is invalid */

        return 1;                                                                         /* return error */
    }
}

/**
 * @brief     set the chip register with iic interface
 * @param[in] *handle points to an sfa30 handle structure
 * @param[in] reg is the iic register address
 * @param[in] *buf points to a data buffer
 * @param[in] len is the data buffer length
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t sfa30_set_reg_iic(sfa30_handle_t *handle, uint16_t reg, uint8_t *buf, uint16_t len)
{
    if (handle == NULL)                                                           /* check handle */
    {
        return 2;                                                                 /* return error */
    }
    if (handle->inited != 1)                                                      /* check handle initialization */
    {
        return 3;                                                                 /* return error */
    }

    if (handle->iic_uart != 0)
    {
        handle->debug_print("sfa30: uart interface is invalid.\n");               /* uart interface is invalid */

        return 1;                                                                 /* return error */
    }
    else
    {
        return a_sfa30_iic_write(handle, SFA30_ADDRESS, reg, buf, len, 20);       /* write the data */
    }
}

/**
 * @brief      get the chip register with iic interface
 * @param[in]  *handle points to an sfa30 handle structure
 * @param[in]  reg is the iic register address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the data buffer length
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 *             - 2 handle is NULL
 *             - 3 handle is not initialized
 * @note       none
 */
uint8_t sfa30_get_reg_iic(sfa30_handle_t *handle, uint16_t reg, uint8_t *buf, uint16_t len)
{
    if (handle == NULL)                                                          /* check handle */
    {
        return 2;                                                                /* return error */
    }
    if (handle->inited != 1)                                                     /* check handle initialization */
    {
        return 3;                                                                /* return error */
    }

    if (handle->iic_uart != 0)
    {
        handle->debug_print("sfa30: uart interface is invalid.\n");              /* uart interface is invalid */

        return 1;
    }
    else
    {
        return a_sfa30_iic_read(handle, SFA30_ADDRESS, reg, buf, len, 20);       /* read the data */
    }
}

/**
 * @brief      get chip information
 * @param[out] *info points to an sfa30 info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t sfa30_info(sfa30_info_t *info)
{
    if (info == NULL)                                               /* check handle */
    {
        return 2;                                                   /* return error */
    }

    memset(info, 0, sizeof(sfa30_info_t));                          /* initialize sfa30 info structure */
    strncpy(info->chip_name, CHIP_NAME, 32);                        /* copy chip name */
    strncpy(info->manufacturer_name, MANUFACTURER_NAME, 32);        /* copy manufacturer name */
    strncpy(info->interface, "UART IIC", 16);                       /* copy interface name */
    info->supply_voltage_min_v = SUPPLY_VOLTAGE_MIN;                /* set minimal supply voltage */
    info->supply_voltage_max_v = SUPPLY_VOLTAGE_MAX;                /* set maximum supply voltage */
    info->max_current_ma = MAX_CURRENT;                             /* set maximum current */
    info->temperature_max = TEMPERATURE_MAX;                        /* set minimal temperature */
    info->temperature_min = TEMPERATURE_MIN;                        /* set maximum temperature */
    info->driver_version = DRIVER_VERSION;                          /* set driver version */

    return 0;                                                       /* success return 0 */
}
