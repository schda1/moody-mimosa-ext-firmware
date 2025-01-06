# Background

In order to test and reconstruct the internal mimosa behaviour, I use an additional microcontroller to continously read the input and output pins of the ASIC (ui_in, uo_out etc). It also provides the clock signal (configurable via UART) and allows to reset the ASIC. Its main responsibility is to provide a consistent data set of all read pin values via UART. 

The receiver (laptop, PyQt application) feeds the data into the PyVerilator simulation and thus allows to get full insight into the internal, hidden information such as neurotransmitter levels or emotion and to log data. Moreover, the logged data allows to directly compare the ASIC behaviour with the intended behaviour, as simulated with the PyVerilator package. 

#  Firmware
Since I had a [nucleo with a STM32G474RE](https://www.st.com/en/evaluation-tools/nucleo-g474re.html) in a handy LQFP64 at hand, I decided to use this microcontroller, even though it is clearly overpowered for the task at hand. 

There are tons of options how to actually set up the application for the microcontroller.

1. Developing the entire application with the [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) (Eclipse-based)
2. Developing a [Zephyr-application](https://github.com/zephyrproject-rtos/zephyr) (e.g. from VS Code), using the west build utilities.
3. [Platform-IO](https://platformio.org/): Developing an application for a nucleo board, with e.g. an Arduino-like "framework".
4. Setting up the application with STM32CubeMX (exporting with Makefile) but independently developing it inside VS Code and using the arm-none-aebi-gcc compiler. You can also debug it from VSCode, even you work inside WSL.

I ruled out option 1 and 3. I also decided not to set up a Zephyr application, mainly because I did not want to blow up the Dockercontainer with [installing the Zephyr SDK](https://github.com/zephyrproject-rtos/docker-image) and keep the procedure comprehensible for a larger audience not familiar with e.g. device trees. Instead, I decided to initially set up the application with STM32CubeMX and then develop the application independently from within VSCode. The driver files are now available from ST on [github](https://github.com/STMicroelectronics/STM32CubeG4) and I thus added them as a git submodule. In this way, the build process can be automated without violating license agreements. The arm gcc compiler is installed in the Dockerfile, so no additional manual steps are needed.

In order to build the firmware, just type `make` or e.g. `make -j16` to run several jobs in parallel. 

# CppUTest

In order to test the message parser module, I added some simple unit tests using [CppUTest](http://cpputest.github.io/). It is added as a submodule and installed in the `scripts/set_up_dependencies.sh` script. No manual steps are required. I follow the [suggested usage](https://github.com/jwgrenning/tddec-code) by J. W. Grenning, the maintainer of CppUTest and author of "Test-Driven Development for Embedded C". I added a Makefile to the tests folder in which the sources and additional folders (mocks, tests) are handled manually. Of course, for a more modern approach, you could also integrate it into CMake.

There are only few tests and only for the msg_parser module. This is a typical piece of code where you just know that there are bugs hidden or incorrectly handled edge cases. Although it relies on incoming uart data, it is pretty simple to test.

# Comments

- By default, the firmware is built in release configuration. If you want to build with debug symbols, you have to change the configuration in the makefile. However, building in debug configuration takes surprisingly long. 
- For advanced debugging, you probably should use the STM32CubeIDE or use professional tools such as a Segger J-LINK.
- There are various tools to flash the .hex file. You can e.g. use the [Stm32 ST-Link Utility]() or J-Flash, if you use a Segger J-Link. If you don't use a nucleo or have a custom PCB with a STM32 you can still use the ST-Link on a nucleo board in order to flash and debug your application with SWD.

# Pinout MCU

A STM32G474RETx in an LQFP64 package is used, with the following pin assignment. 26 of the pins are used to track the state of all in- and outputs of the Mimosa ASIC.

| Pin  | Fct         | Pin  | Fct           | Pin  | Fct            | Pin  | Fct  | 
|----  | ----------- | ---- | ------------- | ---- | -------------- | ---- | ---- |
| PA0  | EXTI, M clk | PB0  | Inp, UI_IN[0] | PC0  | Inp, UO_OUT[0] | PF0  |      |
| PA1  | Inp, M-NRST | PB1  | Inp, UI_IN[1] | PC1  | Inp, UO_OUT[1] | PF1  |      |
| PA2  | LPUART1 TX  | PB2  | Inp, UI_IN[2] | PC2  | Inp, UO_OUT[2] | PG10 | NRST |
| PA3  | LPUART1 RX  | PB3  | Inp, UI_IN[3] | PC3  | Inp, UO_OUT[3] |      |      |
| PA4  | Out, M-NRST | PB4  | Inp, UI_IN[4] | PC4  | Inp, UO_OUT[4] |      |      |
| PA5  | PWM 0       | PB5  | Inp, UI_IN[5] | PC5  | Inp, UO_OUT[5] |      |      |
| PA6  | PWM 1       | PB6  | Inp, UI_IN[6] | PC6  | Inp, UO_OUT[6] |      |      |
| PA7  | PWM 2       | PB7  | Inp, UI_IN[7] | PC7  | Inp, UO_OUT[7] |      |      | 
| PA8  | I2C2 SDA    | PB8  | Boot-0        | PC8  | Inp, UIO[0]    |      |      | 
| PA9  | I2C2 SCL    | PB9  | Out, Clk      | PC9  | Inp, UIO[1]    |      |      |
| PA10 | PWM 3       | PB10 | USART3 TX     | PC10 | Inp, UIO[2]    |      |      |
| PA11 | PWM 4       | PB11 | USART3 RX     | PC11 | Inp, UIO[3]    |      |      |
| PA12 | PWM 5       | PB12 | FRAM CS       | PC12 | Inp, UIO[4]    |      |      |
| PA13 | SWDIO       | PB13 | SPI2 SCK      | PC13 | Inp, UIO[5]    |      |      |
| PA14 | SWCLK       | PB14 | SPI2 MISO     | PC14 | Inp, UIO[6]    |      |      |
| PA15 |             | PB15 | SPI2 MOSI     | PC15 | Inp, UIO[7]    |      |      |

Brief description of several key peripherals:

- **EXTI, M-clk:** A state change of the model clock (prescaled ASIC input clock) triggers an update of the pin observer. All the ASIC pins are read and sent to the host computer via LPUART1.
- **PWM0-5:** These signals correspond to pwm-ed outputs that are fed into a low-pass RC filter, defining the comparator levels. They thus define cold, hot, quiet, loud, dark and bright.
- **I2C2:** Used for accessing the LCD display. The UART data from the ASIC is presented on the LCD. It turned out to be too space-consuming, letting the ASIC directly print to the LCD. We thus need the MCU to display the UART messages from the ASIC on the display.
- **SWDIO, SWCLK:** Serial wire debugging. 
- **TIM4 CH4:** Creates the model input clock (set to 19200 Hz, so that it matches a convenient UART baud rate).
- **USART3:** Receives the UART messages transmitted by the ASIC. These are the phrases the ASIC emits depending on its mood and development stage. The received data is buffered and complete messages are displayed on the I2C LCD.
- **SPI2:** Used to preload the FRAM with phrases. The ASIC then reads those phrases depending on its mood. Attention: The messages can only be loaded when the ASIC is in reset mode. 
- **UI_IN, UO_OUT, UIO**: Input, output and bidirectional pins of the ASIC. The MCU reads all those pins and sends the condensed information to the host computer via LPUART1. In this way, the host can feed this information into the simulation and compare the behaviour of the ASIC with the simulation. 