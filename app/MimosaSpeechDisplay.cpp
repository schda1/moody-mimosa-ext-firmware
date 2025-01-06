#include <MimosaSpeechDisplay.hpp>


MimosaSpeechDisplay::MimosaSpeechDisplay(IUart* uart_receive, IUart* uart_transmit, Display* display) : 
    uart_receive(uart_receive),
    uart_transmit(uart_transmit),
    display(display),
    tail(0),
    head(0), 
    complete_message(false), 
    logging_enabled(true), 
    lcd_display_enabled(true)
{ }

void MimosaSpeechDisplay::init()
{
    /* Uart only needed to get data from the mimosa ASIC, at 19200 baudrate */
    uart_receive->set_baudrate(19200);
    uart_receive->init();
    uart_receive->set_read_callback([this](char c) { this->callback_uart(c); });
    uart_receive->start_read_async();

    display->init();
    display->enable_display(true);
    display->enable_backlight(true);
    display->set_cursor(0, 0);
}

void MimosaSpeechDisplay::enable_logging(bool en)
{
    logging_enabled = en;
}

void MimosaSpeechDisplay::enable_display_on_lcd(bool en)
{
    lcd_display_enabled = en;
}

void MimosaSpeechDisplay::update()
{
    if (complete_message) {

        uint8_t pos = 1;
        
        uint8_t buffer[32];

        buffer[0] = '!';

        while (tail != msg_end) {
            buffer[pos++] = buf[tail];
            tail = (tail + 1) % SPEECH_BUF_LEN;
        }

        uint8_t len = strlen((char*)&buffer[1]);

        if (logging_enabled) {
            uart_transmit->write(buffer, pos);
        }

        if (lcd_display_enabled && len <= 32) {
            display->clear();
            display->set_cursor(0, 0);
            display->print((char*)&buffer[1]);

            /* Only if there is text for the second row */
            if (len > 16) {
                display->set_cursor(0, 1);
                display->print((char*)&buffer[17], len - 16 - 1);
            }
        }

        complete_message = false;
    }
}


void MimosaSpeechDisplay::callback_uart(char c)
{
    buf[head] = c;

    if (c == '\n') {
        msg_end = (head + 1) % SPEECH_BUF_LEN;
        complete_message = true;
    }

    head = (head + 1) % SPEECH_BUF_LEN;
}