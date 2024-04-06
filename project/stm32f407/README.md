### 1. Chip

#### 1.1 Chip Info

Chip Name: STM32F407ZGT6.

Extern Oscillator: 8MHz.

UART Pin: TX/RX PA9/PA10.

IIC Pin: SCL/SDA PB8/PB9.

UART2 Pin:  TX/RX PA2/PA3.

### 2. Development and Debugging

#### 2.1 Integrated Development Environment

LibDriver provides both Keil and IAR integrated development environment projects.

MDK is the Keil ARM project and your Keil version must be 5 or higher.Keil ARM project needs STMicroelectronics STM32F4 Series Device Family Pack and you can download from https://www.keil.com/dd2/stmicroelectronics/stm32f407zgtx.

EW is the IAR ARM project and your IAR version must be 9 or higher.

#### 2.2 Serial Port Parameter

Baud Rate: 115200.

Data Bits : 8.

Stop Bits: 1.

Parity: None.

Flow Control: None.

#### 2.3 Serial Port Assistant

We use '\n' to wrap lines.If your serial port assistant displays exceptions (e.g. the displayed content does not divide lines), please modify the configuration of your serial port assistant or replace one that supports '\n' parsing.

### 3. SFA30

#### 3.1 Command Instruction

1. Show sfa30 chip and driver information.

   ```shell
   sfa30 (-i | --information)
   ```

2. Show sfa30 help.

   ```shell
   sfa30 (-h | --help)
   ```

3. Show sfa30 pin connections of the current board.

   ```shell
   sfa30 (-p | --port)
   ```

5. Run sfa30 read test, num means the test times.

   ```shell
   sfa30 (-t read | --test=read) [--interface=<iic | uart>] [--times=<num>]
   ```

5. Run sfa30 basic read function, num means the read times.

   ```shell
   sfa30 (-e read | --example=read) [--interface=<iic | uart>] [--times=<num>]
   ```

6. Run sfa30 basic get sn function.

   ```shell
   sfa30 (-e sn | --example=sn) [--interface=<iic | uart>]
   ```

#### 3.2 Command Example

```shell
sfa30 -i

sfa30: chip is Sensirion SFA30.
sfa30: manufacturer is Sensirion.
sfa30: interface is UART IIC.
sfa30: driver version is 1.0.
sfa30: min supply voltage is 3.2V.
sfa30: max supply voltage is 5.5V.
sfa30: max current is 5.00mA.
sfa30: max temperature is 40.0C.
sfa30: min temperature is 0.0C.
```

```shell
sfa30 -p

sfa30: IIC interface SCL connected to GPIOB PIN8.
sfa30: IIC interface SDA connected to GPIOB PIN9.
sfa30: UART interface TX connected to GPIOA PIN3.
sfa30: UART interface RX connected to GPIOA PIN2.
```

```shell
sfa30 -t read --interface=uart --times=3

sfa30: chip is Sensirion SFA30.
sfa30: manufacturer is Sensirion.
sfa30: interface is UART IIC.
sfa30: driver version is 1.0.
sfa30: min supply voltage is 3.2V.
sfa30: max supply voltage is 5.5V.
sfa30: max current is 5.00mA.
sfa30: max temperature is 40.0C.
sfa30: min temperature is 0.0C.
sfa30: start read test.
sfa30: device information is 2126E29FFF073B15.
sfa30: formaldehyde is 2.60ppb.
sfa30: humidity is 52.85%.
sfa30: temperature is 27.98C.
sfa30: formaldehyde is 2.60ppb.
sfa30: humidity is 52.35%.
sfa30: temperature is 27.97C.
sfa30: formaldehyde is 2.60ppb.
sfa30: humidity is 52.40%.
sfa30: temperature is 28.00C.
sfa30: finish read test.
```

```shell
sfa30 -e read --interface=uart --times=3

sfa30: 1/3.
sfa30: formaldehyde is 2.60ppb.
sfa30: humidity is 52.10%.
sfa30: temperature is 28.00C.
sfa30: 2/3.
sfa30: formaldehyde is 2.60ppb.
sfa30: humidity is 51.95%.
sfa30: temperature is 28.02C.
sfa30: 3/3.
sfa30: formaldehyde is 2.60ppb.
sfa30: humidity is 52.21%.
sfa30: temperature is 28.02C.
```

```shell
sfa30 -e sn --interface=uart 

sfa30: sn is 2126E29FFF073B15.
```

```shell
sfa30 -h

Usage:
  sfa30 (-i | --information)
  sfa30 (-h | --help)
  sfa30 (-p | --port)
  sfa30 (-t read | --test=read) [--interface=<iic | uart>] [--times=<num>]
  sfa30 (-e read | --example=read) [--interface=<iic | uart>] [--times=<num>]
  sfa30 (-e sn | --example=sn) [--interface=<iic | uart>]

Options:
  -e <read | sn>, --example=<read | sn>
                                          Run the driver example.
  -h, --help                              Show the help.
  -i, --information                       Show the chip information.
      --interface=<iic | uart>            Set the chip interface.([default: iic])
  -p, --port                              Display the pin connections of the current board.
  -t <read>, --test=<read>                Run the driver test.
      --times=<num>                       Set the running times.([default: 3])
```

