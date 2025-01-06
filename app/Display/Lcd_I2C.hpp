#pragma once

#include <Display/Display.hpp>
#include <HalWrapper/II2C.hpp>

#define LCD_CLEARDISPLAY   (0x01)
#define LCD_RETURNHOME     (0x02)
#define LCD_ENTRYMODESET   (0x04)
#define LCD_DISPLAYCONTROL (0x08)
#define LCD_CURSORSHIFT    (0x10)
#define LCD_FUNCTIONSET    (0x20)
#define LCD_SETCGRAMADDR   (0x40)
#define LCD_SETDDRAMADDR   (0x80)

class Lcd_I2C : public Display
{
public:
    Lcd_I2C(II2C* hi2c, uint8_t address, uint8_t columns, uint8_t rows);

    void init()  override;

    void clear() override;
    void home() override;
    void set_cursor(uint8_t col, uint8_t row) override;

    void enable_display(bool en) override;
    void enable_backlight(bool en) override;

    void enable_cursor(bool en);
    void enable_blink(bool en);
    void enable_autoscroll(bool en) override;
    
    void set_scroll_left();
    void set_scroll_right();
    void set_left_to_right();
    void set_right_to_left();
    
    size_t print(const char* text, uint8_t len) override;
    size_t print(const char* text) override;
    size_t print(int32_t number) override;
    size_t print(char character) override;
    void command(uint8_t command);
    void write(uint8_t c);

private:
    void send(uint8_t value, uint8_t mode);
    void write_nibble(uint8_t data);
    void expander_write(uint8_t data);
    void pulse_enable(uint8_t data);

    II2C* hi2c;

    uint8_t address;
    uint8_t display_function;
    uint8_t display_control;
    uint8_t display_mode;

    uint8_t columns;
    uint8_t rows;
    uint8_t backlight;
};
