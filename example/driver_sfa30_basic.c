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
 * @file      driver_sfa30_basic.c
 * @brief     driver sfa30 basic source file
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

#include "driver_sfa30_basic.h"

static sfa30_handle_t gs_handle;        /**< sfa30 handle */

/**
 * @brief     basic example init
 * @param[in] interface is the chip interface
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 * @note      none
 */
uint8_t sfa30_basic_init(sfa30_interface_t interface)
{
    uint8_t res;
    
    /* link functions */
    DRIVER_SFA30_LINK_INIT(&gs_handle, sfa30_handle_t);
    DRIVER_SFA30_LINK_UART_INIT(&gs_handle, sfa30_interface_uart_init);
    DRIVER_SFA30_LINK_UART_DEINIT(&gs_handle, sfa30_interface_uart_deinit);
    DRIVER_SFA30_LINK_UART_READ(&gs_handle, sfa30_interface_uart_read);
    DRIVER_SFA30_LINK_UART_WRITE(&gs_handle, sfa30_interface_uart_write);
    DRIVER_SFA30_LINK_UART_FLUSH(&gs_handle, sfa30_interface_uart_flush);
    DRIVER_SFA30_LINK_IIC_INIT(&gs_handle, sfa30_interface_iic_init);
    DRIVER_SFA30_LINK_IIC_DEINIT(&gs_handle, sfa30_interface_iic_deinit);
    DRIVER_SFA30_LINK_IIC_WRITE_COMMAND(&gs_handle, sfa30_interface_iic_write_cmd);
    DRIVER_SFA30_LINK_IIC_READ_COMMAND(&gs_handle, sfa30_interface_iic_read_cmd);
    DRIVER_SFA30_LINK_DELAY_MS(&gs_handle, sfa30_interface_delay_ms);
    DRIVER_SFA30_LINK_DEBUG_PRINT(&gs_handle, sfa30_interface_debug_print);
    
    /* set the interface */
    res = sfa30_set_interface(&gs_handle, interface);
    if (res != 0)
    {
        sfa30_interface_debug_print("sfa30: set interface failed.\n");
    
        return 1;
    }
    
    /* init the chip */
    res = sfa30_init(&gs_handle);
    if (res != 0)
    {
        sfa30_interface_debug_print("sfa30: init failed.\n");
    
        return 1;
    }
    
    /* start measurement */
    res = sfa30_start_measurement(&gs_handle);
    if (res != 0)
    {
        sfa30_interface_debug_print("sfa30: start measurement failed.\n");
        (void)sfa30_deinit(&gs_handle);
        
        return 1;
    }

    return 0;
}

/**
 * @brief  basic example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t sfa30_basic_deinit(void)
{
    uint8_t res;
    
    /* stop measurement */
    res = sfa30_stop_measurement(&gs_handle);
    if (res != 0)
    {
        return 1;
    }
    
    /* deinit */
    res = sfa30_deinit(&gs_handle);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      basic example read
 * @param[out] *data points to a sfa30_data_t structure
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t sfa30_basic_read(sfa30_data_t *data)
{
    uint8_t res;
    
    /* read data */
    res = sfa30_read(&gs_handle, data);
    if (res != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  basic example reset
 * @return status code
 *         - 0 success
 *         - 1 reset failed
 * @note   none
 */
uint8_t sfa30_basic_reset(void)
{
    if (sfa30_reset(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      basic example get device information
 * @param[out] *info points to a info buffer
 * @return     status code
 *             - 0 success
 *             - 1 get device information failed
 * @note       none
 */
uint8_t sfa30_basic_get_device_information(char info[32])
{
    if (sfa30_get_device_information(&gs_handle, info) != 0)
    {
        return 1;
    }
    
    return 0;
}
