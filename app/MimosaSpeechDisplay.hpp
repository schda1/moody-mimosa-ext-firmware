#pragma once

#include <common.hpp>
#include <HalWrapper/IUart.hpp>
#include <Display/Display.hpp>

#define SPEECH_BUF_LEN (64)

class MimosaSpeechDisplay 
{
public:
    MimosaSpeechDisplay(IUart* uart_receive, IUart* uart_transmit, Display* display);
    virtual ~MimosaSpeechDisplay() {}

    void init();
    void update();

    void enable_logging(bool en);
    void enable_display_on_lcd(bool en);

    void callback_uart(char c);

protected:
    IUart* uart_receive;
    IUart* uart_transmit;
    Display* display;

    uint8_t buf[SPEECH_BUF_LEN];
    uint8_t tail;
    uint8_t head;
    uint8_t msg_end;
    bool complete_message;
    bool logging_enabled;
    bool lcd_display_enabled;
};