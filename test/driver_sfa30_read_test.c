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
 * @file      driver_sfa30_read_test.c
 * @brief     driver sfa30 read test source file
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

#include "driver_sfa30_read_test.h"

static sfa30_handle_t gs_handle;        /**< sfa30 handle */

/**
 * @brief     read test
 * @param[in] interface is the chip interface
 * @param[in] times is the test times
 * @return    status code
 *            - 0 success
 *            - 1 test failed
 * @note      none
 */
uint8_t sfa30_read_test(sfa30_interface_t interface, uint32_t times)
{
    uint8_t res;
    uint32_t i;
    sfa30_info_t info;
    char device_info[32];
    sfa30_data_t data;
    
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
    
    /* get information */
    res = sfa30_info(&info);
    if (res != 0)
    {
        sfa30_interface_debug_print("sfa30: get info failed.\n");
       
        return 1;
    }
    else
    {
        /* print chip info */
        sfa30_interface_debug_print("sfa30: chip is %s.\n", info.chip_name);
        sfa30_interface_debug_print("sfa30: manufacturer is %s.\n", info.manufacturer_name);
        sfa30_interface_debug_print("sfa30: interface is %s.\n", info.interface);
        sfa30_interface_debug_print("sfa30: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        sfa30_interface_debug_print("sfa30: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
        sfa30_interface_debug_print("sfa30: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
        sfa30_interface_debug_print("sfa30: max current is %0.2fmA.\n", info.max_current_ma);
        sfa30_interface_debug_print("sfa30: max temperature is %0.1fC.\n", info.temperature_max);
        sfa30_interface_debug_print("sfa30: min temperature is %0.1fC.\n", info.temperature_min);
    }
    
    /* start read test */
    sfa30_interface_debug_print("sfa30: start read test.\n");
    
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
    
    /* get device information */
    res = sfa30_get_device_information(&gs_handle, device_info);
    if (res != 0)
    {
        sfa30_interface_debug_print("sfa30: get device information failed.\n");
    
        return 1;
    }
    
    /* output */
    sfa30_interface_debug_print("sfa30: device information is %s.\n", device_info);
    
    /* start measurement */
    res = sfa30_start_measurement(&gs_handle);
    if (res != 0)
    {
        sfa30_interface_debug_print("sfa30: start measurement failed.\n");
        (void)sfa30_deinit(&gs_handle);
        
        return 1;
    }
    
    /* delay 2000 ms */
    sfa30_interface_delay_ms(2000);
    
    for (i = 0; i < times; i++)
    {
        res = sfa30_read(&gs_handle, &data);
        if (res != 0)
        {
            sfa30_interface_debug_print("sfa30: read failed.\n");
            (void)sfa30_deinit(&gs_handle);
            
            return 1;
        }
        sfa30_interface_debug_print("sfa30: formaldehyde is %0.2fppb.\n", data.formaldehyde);
        sfa30_interface_debug_print("sfa30: humidity is %0.2f%%.\n", data.humidity);
        sfa30_interface_debug_print("sfa30: temperature is %0.2fC.\n", data.temperature);
        
        /* delay 2000 ms */
        sfa30_interface_delay_ms(2000);
    }
    
    /* stop measurement */
    res = sfa30_stop_measurement(&gs_handle);
    if (res != 0)
    {
        sfa30_interface_debug_print("sfa30: stop measurement failed.\n");
        (void)sfa30_deinit(&gs_handle);
        
        return 1;
    }
    
    /* finish read test */
    sfa30_interface_debug_print("sfa30: finish read test.\n");
    (void)sfa30_deinit(&gs_handle);
    
    return 0;
}
