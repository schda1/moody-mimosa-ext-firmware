# Background

An external PCB with an on-board microcontroller is used in order to interface with the tiny tapeout ASIC. The microcontroller is responsible for the following tasks:

- Select the appropriate tiny-tapeout project through the [3-pin MUX interface](https://www.youtube.com/watch?v=30emLKLpq50&list=PLyynFETmdQDQvd8uO3NWkBZLYGDu6Mr5M&index=1) by Sylvain Munaut 3-pin interface. 
- Provide the clock signal and reset logic for the ASIC
- Provide the comparator analog levels (for defining dark, bright, loud, hot etc.)
- Receive UART data streamed by the mimosa project (the mimosa talks by reading data from an external FRAM and sending the data via UART)
- Display data from the mimosa to an I2C LCD display (did not fit onto the ASIC)
- Read out all in- and outputs from the mimosa and send them to the host computer via UART (for simulation and testing)
- Pre-load the FRAM with phrases for the mimosa to read
- Allowing to permanently store parameters (such as clock frequency, analog comparator values, etc) at the end of the flash
- Provide a interface for communication (custom commands from the host computer via UART)


#  Firmware
Since I had a [nucleo with a STM32G474RE](https://www.st.com/en/evaluation-tools/nucleo-g474re.html) in a handy LQFP64 at hand, I decided to use this microcontroller, even though it is clearly overpowered for the task at hand. 

There are tons of options how to actually set up the application for the microcontroller.

1. Developing the entire application with the [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) (Eclipse-based)
2. Developing a [Zephyr-application](https://github.com/zephyrproject-rtos/zephyr) (e.g. from VS Code), using the west build utilities.
3. [Platform-IO](https://platformio.org/): Developing an application for a nucleo board, with e.g. an Arduino-like "framework".
4. Setting up the application with STM32CubeMX (exporting with Makefile) but independently developing it inside VS Code and using the arm-none-aebi-gcc compiler. You can also debug it from VSCode, even you work inside WSL.

I decided to perform an experiment and try to develop a standalone C++ application with components that are testable. As a test framework, I chose CppUTest. I wrote minimalistic wrapper classes for the main peripherals (GPIO, I2C, UART, SPI, PWM etc). They allow to quickly instantiate the corresponding peripherals in the main application or use derived mocks thereof in unit tests. However, there is a lot of boiler-plate overhead and a so much room for improvements.

In order to build the firmware, just type `make` or e.g. `make -j16` to run several jobs in parallel. Building with CMake is not yet added. 

# CppUTest

In order to test the message parser module, I added some simple unit tests using [CppUTest](http://cpputest.github.io/). It is added as a submodule and installed in the `scripts/set_up_dependencies.sh` script. No manual steps are required. I follow the [suggested usage](https://github.com/jwgrenning/tddec-code) by J. W. Grenning, the maintainer of CppUTest and author of "Test-Driven Development for Embedded C". I added a Makefile to the tests folder in which the sources and additional folders (mocks, tests) are handled manually. Of course, for a more modern approach, you could also integrate it into CMake.

There are only few tests and only for the msg_parser module. This is a typical piece of code where you just know that there are bugs hidden or incorrectly handled edge cases. Although it relies on incoming uart data, it is pretty simple to test.

# Comments

- By default, the firmware is built in release configuration. If you want to build with debug symbols, you have to change the configuration in the makefile. However, building in debug configuration takes surprisingly long. 
- For advanced debugging, you probably should use the STM32CubeIDE or use professional tools such as a Segger J-LINK.
- There are various tools to flash the .hex file. You can e.g. use the [Stm32 ST-Link Utility]() or J-Flash, if you use a Segger J-Link. If you don't use a nucleo or have a custom PCB with a STM32 you can still use the ST-Link on a nucleo board in order to flash and debug your application with SWD.

# Pinout MCU

A STM32G474RETx in an LQFP64 package is used, with the following pin assignment. 26 of the pins are used to track the state of all in- and outputs of the Mimosa ASIC.

| Pin  | Fct         | Pin  | Fct           | Pin  | Fct            | Pin  | Fct            | 
|----  | ----------- | ---- | ------------- | ---- | -------------- | ---- | -------------- |
| PA0  | EXTI, M clk | PB0  | Inp, UI_IN[0] | PC0  | Inp, UO_OUT[0] | PF0  | TT_SEL_RST_N   |
| PA1  | Inp, M-NRST | PB1  | Inp, UI_IN[1] | PC1  | Inp, UO_OUT[1] | PF1  | TT_SEL_INC_PIN |
| PA2  | LPUART1 TX  | PB2  | Inp, UI_IN[2] | PC2  | Inp, UO_OUT[2] | PG10 | NRST           |
| PA3  | LPUART1 RX  | PB3  | Inp, UI_IN[3] | PC3  | Inp, UO_OUT[3] |      |                |
| PA4  | Out, M-NRST | PB4  | Inp, UI_IN[4] | PC4  | Inp, UO_OUT[4] |      |                |
| PA5  | PWM 0       | PB5  | Inp, UI_IN[5] | PC5  | Inp, UO_OUT[5] |      |                |
| PA6  | PWM 1       | PB6  | Inp, UI_IN[6] | PC6  | Inp, UO_OUT[6] |      |                |
| PA7  | PWM 2       | PB7  | Inp, UI_IN[7] | PC7  | Inp, UO_OUT[7] |      |                | 
| PA8  | I2C2 SDA    | PB8  | Boot-0        | PC8  | Inp, UIO[0]    |      |                | 
| PA9  | I2C2 SCL    | PB9  | Out, Clk      | PC9  | Inp, UIO[1]    |      |                |
| PA10 | PWM 3       | PB10 | USART3 TX     | PC10 | Inp, UIO[2]    |      |                |
| PA11 | PWM 4       | PB11 | USART3 RX     | PC11 | Inp, UIO[3]    |      |                |
| PA12 | PWM 5       | PB12 | FRAM CS       | PC12 | Inp, UIO[4]    |      |                |
| PA13 | SWDIO       | PB13 | SPI2 SCK      | PC13 | Inp, UIO[5]    |      |                |
| PA14 | SWCLK       | PB14 | SPI2 MISO     | PC14 | Inp, UIO[6]    |      |                |
| PA15 | TT_SEL_EN   | PB15 | SPI2 MOSI     | PC15 | Inp, UIO[7]    |      |                |

Brief description of several key peripherals:

- **TT_SEL_RST_N, TT_SEL_INC, TT_SEL_EN**: On the tiny-tapeout ASICs, all projects for the corresponding tapeout are on each chip, of course. You have to select the project that should run (i.e. that should be routed to and from the in- and output pins). There is a simple [three-pin interface](https://raw.githubusercontent.com/TinyTapeout/tt-multiplexer/main/docs/diagrams/mux_select_addr_12.png). You basically send 42 pulses if you want to load project number 42.
- **EXTI, M-clk:** A state change of the model clock (prescaled ASIC input clock) triggers an update of the pin observer. All the ASIC pins are read and sent to the host computer via LPUART1.
- **PWM0-5:** These signals correspond to pwm-ed outputs that are fed into a low-pass RC filter, defining the comparator levels. They thus define cold, hot, quiet, loud, dark and bright.
- **I2C2:** Used for accessing the LCD display. The UART data from the ASIC is presented on the LCD. It turned out to be too space-consuming, letting the ASIC directly print to the LCD. We thus need the MCU to display the UART messages from the ASIC on the display.
- **SWDIO, SWCLK:** Serial wire debugging. 
- **TIM4 CH4:** Creates the model input clock (set to 19200 Hz, so that it matches a convenient UART baud rate).
- **USART3:** Receives the UART messages transmitted by the ASIC. These are the phrases the ASIC emits depending on its mood and development stage. The received data is buffered and complete messages are displayed on the I2C LCD.
- **SPI2:** Used to preload the FRAM with phrases. The ASIC then reads those phrases depending on its mood. Attention: The messages can only be loaded when the ASIC is in reset mode. 
- **UI_IN, UO_OUT, UIO**: Input, output and bidirectional pins of the ASIC. The MCU reads all those pins and sends the condensed information to the host computer via LPUART1. In this way, the host can feed this information into the simulation and compare the behaviour of the ASIC with the simulation. 
