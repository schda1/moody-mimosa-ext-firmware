#include <common.hpp>
#include <App.hpp>

#include <HalWrapper/I2C.hpp>
#include <HalWrapper/Spi.hpp>
#include <HalWrapper/Uart.hpp>
#include <HalWrapper/Timer.hpp>
#include <HalWrapper/ExtiInput.hpp>
#include <HalWrapper/DigitalOutput.hpp>
#include <HalWrapper/PwmOutput.hpp>
#include <HalWrapper/DigitalInput.hpp>

#include <PersistantStorage/FramStorage_Spi.hpp>
#include <PersistantStorage/FlashStorage.hpp>
#include <Parameter/ParameterHandler.hpp>

#include <Display/Lcd_I2C.hpp>

#include <PwmHandler.hpp>
#include <PinObserver.hpp>
#include <MimosaDriver.hpp>
#include <MessageParser.hpp>
#include <ComparatorDriver.hpp>
#include <MimosaSpeechDisplay.hpp>

/* Uart peripherals */
#define MAIN_UART   (LPUART1)
#define MIMOSA_UART (USART3)
#define PWM_TIMER   (TIM3)

/* Mimosa clock signal */
#define MIMOSA_CLK_PORT       (GPIOB)
#define MIMOSA_CLK_PIN        (9)
#define MIMOSA_CLK_TIMER      (TIM4)
#define MIMOSA_CLK_RESOLUTION (16)

/* Mimosa nrst signal */
#define MIMOSA_NRESET_PORT (GPIOA)
#define MIMOSA_NRESET_PIN  (4)

/* Mimosa external interrupt */
#define MIMOSA_EXTI_PORT (GPIOA)
#define MIMOSA_EXTI_PIN  (0)

/* Pin definitions for Pin Observer*/
#define MIMOSA_UI_PORT   (GPIOB) 
#define MIMOSA_UI_MASK   (0x00FF)
#define MIMOSA_UO_PORT   (GPIOC)
#define MIMOSA_UO_MASK   (0x00FF)
#define MIMOSA_UIO_PORT  (GPIOC)
#define MIMOSA_UIO_MASK  (0xFF00)
#define MIMOSA_CTRL_PORT (GPIOA)
#define MIMOSA_CTRL_MASK (0x0050)

/* SPI communication with FRAM */
#define FRAM_SPI         (SPI2)
#define FRAM_SPI_CS_PORT (GPIOB)
#define FRAM_SPI_CS_PIN  (12)

/* External I2C Lcd display*/
#define LCD_I2C_BUS   (I2C2)
#define LCD_ADDRESS   (0x3F)
#define LCD_ROWS      (2)
#define LCD_COLS      (16)

/* Parameter partition in Flash */
#define FLASH_PARAM_START_ADDR (0x0807F800)
#define FLASH_PARAM_SIZE       (0x800)


enum class MsgType : int8_t {NONE = 0, START, STOP, PAUSE, SET_PERIOD, SET_COLD, SET_HOT, 
                             SET_DARK, SET_BRIGHT, SET_QUIET, SET_LOUD, FRAM_INIT, FRAM_WRITE, 
                             FRAM_READ, FRAM_ERASE, LOG_ON, LOG_OFF, UPLOAD, PRINT, RST_MIMOSA, INVALID};

const Command command_table[] = {
    {"START",      (int8_t)MsgType::START,      MsgData::NONE},
    {"STOP",       (int8_t)MsgType::STOP,       MsgData::NONE},
    {"PAUSE",      (int8_t)MsgType::PAUSE,      MsgData::NONE},
    {"SET_PERIOD", (int8_t)MsgType::SET_PERIOD, MsgData::VALUE},
    {"SET_COLD",   (int8_t)MsgType::SET_COLD,   MsgData::VALUE},
    {"SET_HOT",    (int8_t)MsgType::SET_HOT,    MsgData::VALUE},
    {"SET_DARK",   (int8_t)MsgType::SET_DARK,   MsgData::VALUE},
    {"SET_BRIGHT", (int8_t)MsgType::SET_BRIGHT, MsgData::VALUE},
    {"SET_QUIET",  (int8_t)MsgType::SET_QUIET,  MsgData::VALUE},
    {"SET_LOUD",   (int8_t)MsgType::SET_LOUD,   MsgData::VALUE},
    {"FRAM_WRITE", (int8_t)MsgType::FRAM_WRITE, MsgData::STRING},
    {"FRAM_READ",  (int8_t)MsgType::FRAM_READ,  MsgData::VALUE},
    {"FRAM_ERASE", (int8_t)MsgType::FRAM_ERASE, MsgData::NONE},
    {"FRAM_INIT",  (int8_t)MsgType::FRAM_INIT,  MsgData::VALUE},
    {"LOG_ON",     (int8_t)MsgType::LOG_ON,     MsgData::NONE},
    {"LOG_OFF",    (int8_t)MsgType::LOG_OFF,    MsgData::NONE},
    {"UPLOAD",     (int8_t)MsgType::UPLOAD,     MsgData::NONE},
    {"PRINT",      (int8_t)MsgType::PRINT,      MsgData::NONE},
    {"RST_MIMOSA", (int8_t)MsgType::RST_MIMOSA, MsgData::VALUE},
    {NULL,         (int8_t)MsgType::INVALID,    MsgData::NONE}
};

static void process_message(Msg* msg);
static void input_config_fct(GPIO_TypeDef* port, uint8_t pin);

/* Uart for communicating with host computer via FTDI*/
Uart uart(MAIN_UART);
MessageParser parser(&uart);
UART_HandleTypeDef* hlpuart1;

/* Uart over which mimosa ASIC talks */
Uart uart_mimosa(MIMOSA_UART);

/* I2C bus to display data on the 16x2 LCD display */
I2C i2c2(LCD_I2C_BUS);
Lcd_I2C lcd(&i2c2, LCD_ADDRESS, LCD_COLS, LCD_ROWS);

/* SPI bus to communicate with external FRAM, where mimosa phrases are stored */
DigitalOutput fram_cs(FRAM_SPI_CS_PORT, FRAM_SPI_CS_PIN);
Spi fram_spi(FRAM_SPI, &fram_cs);
FramStorage_Spi fram(&fram_spi);

/* Parameter handling (storage in Flash) */
FlashStorage flash(FLASH_PARAM_START_ADDR, FLASH_PARAM_SIZE);
ParameterHandler param_handler(&flash);

/* Timer for PWM of the comparator input levels */
Timer pwm_timer(PWM_TIMER);
PwmHandler pwm_handler(&pwm_timer);
ComparatorDriver cmp_driver(&pwm_handler, &param_handler);

/* Additional GPIO */
PwmOutput mimosa_clk(MIMOSA_CLK_PORT, MIMOSA_CLK_PIN, MIMOSA_CLK_TIMER, MIMOSA_CLK_RESOLUTION);
DigitalOutput mimosa_nreset(MIMOSA_NRESET_PORT, MIMOSA_NRESET_PIN);
ExtiInput mimosa_exti(MIMOSA_EXTI_PORT, MIMOSA_EXTI_PIN);

/* Pin observer to track all in- and output of the mimosa ASIC */
PinObserver pin_observer(input_config_fct);

/* Main mimosa state machine handler */
MimosaDriver mimosa(&mimosa_clk, &mimosa_exti, &mimosa_nreset, &pin_observer, &param_handler);

/* Display of speech data of the mimosa ASIC */
MimosaSpeechDisplay speech_buffer(&uart_mimosa, &uart, &lcd);


void app_init(void)
{
    /* Externally define uart used for printf function */
    hlpuart1 = uart.get_handle();

    /* Initialize Parameters */
    flash.init();
    param_handler.download();

    /* Initialize UART message parser */
    parser.init();
    parser.set_command_table(command_table, sizeof(command_table) / sizeof(Command));

    /* Initialize PwmHandler Timer */
    pwm_timer.set_prescaler(11);
    pwm_timer.set_period(80);

    /* Initialize pin observer */
    pin_observer.configure(PortType::UI_IN, MIMOSA_UI_PORT, MIMOSA_UI_MASK);
    pin_observer.configure(PortType::UO_OUT, MIMOSA_UO_PORT, MIMOSA_UO_MASK);
    pin_observer.configure(PortType::UIO, MIMOSA_UIO_PORT, MIMOSA_UIO_MASK);
    pin_observer.configure(PortType::CTRL, MIMOSA_CTRL_PORT, MIMOSA_CTRL_MASK);
    pin_observer.reset();

    /* Initialize Comparator Driver */
    cmp_driver.init();
    cmp_driver.enable(true);
    cmp_driver.print();

    /* Initialize Mimosa */
    mimosa.init();
    speech_buffer.init();
    speech_buffer.enable_logging(true);

    /* Start mimosa */
    mimosa.start();
}

void app_run(void)
{
    Msg msg;

    while(true) {
        
        /* Process incoming messages */
        parser.update(&msg);
        if (msg.id != (int8_t)MsgType::NONE) {
            process_message(&msg);
        }

        speech_buffer.update();

        /* Process Mimosa Clock and log data */
        mimosa.update();
        
        HAL_Delay(1);
    }
}

static void process_message(Msg* msg)
{
    MsgType type = (MsgType)msg->id;

    switch (type) {
        case MsgType::START:
            mimosa.start();
            break;
        case MsgType::STOP:
            mimosa.stop();
            break;
        case MsgType::PAUSE:
            mimosa.pause();
            break;
        case MsgType::SET_PERIOD:
            mimosa.set_period_ms(msg->value);
            break;
        case MsgType::SET_COLD:
            cmp_driver.set_cold_thresh(msg->value);
            break;
        case MsgType::SET_HOT:
            cmp_driver.set_hot_thresh(msg->value);
            break;
        case MsgType::SET_DARK:
            cmp_driver.set_dark_thresh(msg->value);
            break;
        case MsgType::SET_BRIGHT:
            cmp_driver.set_bright_thresh(msg->value);
            break;
        case MsgType::SET_QUIET:
            cmp_driver.set_quiet_thresh(msg->value);
            break;
        case MsgType::SET_LOUD:
            cmp_driver.set_loud_thresh(msg->value);
            break;
        case MsgType::FRAM_INIT:
            if (msg->value)
                fram.init(); 
            else
                fram.deinit();
            break;
        case MsgType::FRAM_WRITE:
            printf("Write FRAM: %s, len = %d\n", msg->str, strlen(msg->str));
            fram.write(msg->value, (uint8_t*)msg->str, strlen(msg->str));
            break;
        case MsgType::FRAM_READ:
            uint8_t buffer[64];
            fram.read(msg->value, buffer, 32);
            printf("Read FRAM: %s\n", buffer);
            break;
        case MsgType::FRAM_ERASE:
            fram.erase();
            break;
        case MsgType::LOG_ON:
            mimosa.enable_log(true);
            break;
        case MsgType::LOG_OFF:
            mimosa.enable_log(false);
            break;
        case MsgType::UPLOAD:
            flash.erase();
            param_handler.upload();
            break;
        case MsgType::PRINT:
            /* Various print functions */
            break;
        case MsgType::RST_MIMOSA:
            if (msg->value)
                mimosa_nreset.set(0);
            else
                mimosa_nreset.set(1);
            break;
        
        default:
            printf("Invalid message\n");
            break;
    }
}

static void input_config_fct(GPIO_TypeDef* port, uint8_t pin)
{
    DigitalInput input(port, pin);
    input.init();
}
