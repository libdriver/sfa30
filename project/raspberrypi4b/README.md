### 1. Board

#### 1.1 Board Info

Board Name: Raspberry Pi 4B.

IIC Pin: SCL/SDA GPIO3/GPIO2.

UART Pin: TX/RX GPIO14/GPIO15.

### 2. Install

#### 2.1 Dependencies

Install the necessary dependencies.

```shell
sudo apt-get install libgpiod-dev pkg-config cmake -y
```
#### 2.2 Configuration

Enable serial port.

Disable serial console.

#### 2.3 Makefile

Build the project.

```shell
make
```

Install the project and this is optional.

```shell
sudo make install
```

Uninstall the project and this is optional.

```shell
sudo make uninstall
```

#### 2.4 CMake

Build the project.

```shell
mkdir build && cd build 
cmake .. 
make
```

Install the project and this is optional.

```shell
sudo make install
```

Uninstall the project and this is optional.

```shell
sudo make uninstall
```

Test the project and this is optional.

```shell
make test
```

Find the compiled library in CMake. 

```cmake
find_package(sfa30 REQUIRED)
```


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
./sfa30 -i

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
./sfa30 -p

sfa30: IIC interface SCL connected to GPIO3(BCM).
sfa30: IIC interface SDA connected to GPIO2(BCM).
sfa30: UART interface TX connected to GPIO15(BCM).
sfa30: UART interface RX connected to GPIO14(BCM).
```

```shell
./sfa30 -t read --interface=uart --times=3

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
sfa30: humidity is 48.55%.
sfa30: temperature is 26.99C.
sfa30: formaldehyde is 2.60ppb.
sfa30: humidity is 48.70%.
sfa30: temperature is 26.94C.
sfa30: formaldehyde is 2.60ppb.
sfa30: humidity is 49.00%.
sfa30: temperature is 26.91C.
sfa30: finish read test.
```

```shell
./sfa30 -e read --interface=uart --times=3

sfa30: 1/3.
sfa30: formaldehyde is 2.60ppb.
sfa30: humidity is 50.98%.
sfa30: temperature is 26.16C.
sfa30: 2/3.
sfa30: formaldehyde is 2.60ppb.
sfa30: humidity is 51.03%.
sfa30: temperature is 26.14C.
sfa30: 3/3.
sfa30: formaldehyde is 2.60ppb.
sfa30: humidity is 51.16%.
sfa30: temperature is 26.06C.
```

```shell
./sfa30 -e sn --interface=uart 

sfa30: sn is 2126E29FFF073B15.
```

```shell
./sfa30 -h

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

