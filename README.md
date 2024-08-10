[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver SFA30

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/sfa30/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

SFA30 is a formaldehyde detection module based on electrochemical sensing technology optimized for lowest false alarm rates. The SFA30 HCHO sensor has a low cross-sensitivity to ethanol and is optimized to operate in the low parts-per-billion (ppb) range to accurately detect low HCHO concentrations around the WHO reference value for indoor exposure.Temperature and humidity compensation via an integrated Sensirion RHT sensor and an integrated microprocessor ensures reliable performance in changing indoor environments. The sensor can withstand adverse environmental conditions during storage, handling and transportation.The robust sensor has a lifetime of 6 years due to its advanced anti-dry technology. Each SFA30 is factory-calibrated and can be easily integrated via I2C or UART interface into your device. The low power consumption allows for battery operation. This makes SFA30 a reliable, easy-to-use and high quality HCHO&RHT combo sensing solution.

LibDriver SFA30 is the full function driver of SFA30 launched by LibDriver. It provides formaldehyde concentration reading, temperature reading, humidity reading, sn reading and other functions. LibDriver is MISRA compliant.

### Table of Contents

  - [Instruction](#Instruction)
  - [Install](#Install)
  - [Usage](#Usage)
    - [example basic](#example-basic)
  - [Document](#Document)
  - [Contributing](#Contributing)
  - [License](#License)
  - [Contact Us](#Contact-Us)

### Instruction

/src includes LibDriver SFA30 source files.

/interface includes LibDriver SFA30 IIC, UART platform independent template.

/test includes LibDriver SFA30 driver test code and this code can test the chip necessary function simply.

/example includes LibDriver SFA30 sample code.

/doc includes LibDriver SFA30 offline document.

/datasheet includes SFA30 datasheet.

/project includes the common Linux and MCU development board sample code. All projects use the shell script to debug the driver and the detail instruction can be found in each project's README.md.

/misra includes the LibDriver MISRA code scanning results.

### Install

Reference /interface IIC, UART platform independent template and finish your platform IIC, UART driver. 

Add the /src directory, the interface driver for your platform, and your own drivers to your project, if you want to use the default example drivers, add the /example directory to your project.

### Usage

You can refer to the examples in the /example directory to complete your own driver. If you want to use the default programming examples, here's how to use them.

#### example basic

```C
#include "driver_sfa30_basic.h"

uint8_t res;
char sn[32];
uint32_t i;

/* init */
res = sfa30_basic_init(interface);
if (res != 0)
{
    return 1;
}

/* get the sn */
res = sfa30_basic_get_device_information((char *)sn);
if (res != 0)
{
    (void)sfa30_basic_deinit();

    return 1;
}

/* print */
sfa30_interface_debug_print("sfa30: sn is %s.\n", sn);

/* delay 2000 ms */
sfa30_interface_delay_ms(2000);

/* loop */
for (i = 0; i < times; i++)
{
    sfa30_data_t data;

    /* read data */
    res = sfa30_basic_read(&data);
    if (res != 0)
    {
        (void)sfa30_basic_deinit();

        return 1;
    }

    /* print */
    sfa30_interface_debug_print("sfa30: %d/%d.\n", i + 1, times);
    sfa30_interface_debug_print("sfa30: formaldehyde is %0.2fppb.\n", data.formaldehyde);
    sfa30_interface_debug_print("sfa30: humidity is %0.2f%%.\n", data.humidity);
    sfa30_interface_debug_print("sfa30: temperature is %0.2fC.\n", data.temperature);

    /* delay 2000 ms */
    sfa30_interface_delay_ms(2000);
}

/* deinit */
(void)sfa30_basic_deinit();

return 0;
```

### Document

Online documents: [https://www.libdriver.com/docs/sfa30/index.html](https://www.libdriver.com/docs/sfa30/index.html).

Offline documents: /doc/html/index.html.

### Contributing

Please refer to CONTRIBUTING.md.

### License

Copyright (c) 2015 - present LibDriver All rights reserved



The MIT License (MIT) 



Permission is hereby granted, free of charge, to any person obtaining a copy

of this software and associated documentation files (the "Software"), to deal

in the Software without restriction, including without limitation the rights

to use, copy, modify, merge, publish, distribute, sublicense, and/or sell

copies of the Software, and to permit persons to whom the Software is

furnished to do so, subject to the following conditions: 



The above copyright notice and this permission notice shall be included in all

copies or substantial portions of the Software. 



THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR

IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,

FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE

AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER

LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,

OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE

SOFTWARE. 

### Contact Us

Please send an e-mail to lishifenging@outlook.com.