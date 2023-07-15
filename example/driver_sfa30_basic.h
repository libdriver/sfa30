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
 * @file      driver_sfa30_basic.h
 * @brief     driver sfa30 basic header file
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

#ifndef DRIVER_SFA30_BASIC_H
#define DRIVER_SFA30_BASIC_H

#include "driver_sfa30_interface.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup sfa30_example_driver sfa30 example driver function
 * @brief    sfa30 example driver modules
 * @ingroup  sfa30_driver
 * @{
 */

/**
 * @brief     basic example init
 * @param[in] interface is the chip interface
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 * @note      none
 */
uint8_t sfa30_basic_init(sfa30_interface_t interface);

/**
 * @brief  basic example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t sfa30_basic_deinit(void);

/**
 * @brief      basic example read
 * @param[out] *data points to a sfa30_data_t structure
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t sfa30_basic_read(sfa30_data_t *data);

/**
 * @brief  basic example reset
 * @return status code
 *         - 0 success
 *         - 1 reset failed
 * @note   none
 */
uint8_t sfa30_basic_reset(void);

/**
 * @brief      basic example get device information
 * @param[out] *info points to a info buffer
 * @return     status code
 *             - 0 success
 *             - 1 get device information failed
 * @note       none
 */
uint8_t sfa30_basic_get_device_information(char info[32]);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
