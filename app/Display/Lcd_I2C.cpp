#include <Display/Lcd_I2C.hpp>

#include <string.h>
#include <stdio.h>
#include "stm32g4xx_hal.h"

/* Flags for display entry mode */
#define LCD_ENTRY_RIGHT     (0x00)
#define LCD_ENTRY_LEFT      (0x02)
#define LCD_ENTRY_SHIFT_INC (0x01)
#define LCD_ENTRY_SHIFT_DEC (0x00)

/* Flags for display control */
#define LCD_DISPLAY_ON   (0x04)
#define LCD_DISPLAY_OFF  (0x00)
#define LCD_CURSOR_ON    (0x02)
#define LCD_CURSOR_OFF   (0x00)
#define LCD_BLINK_ON     (0x01)
#define LCD_BLINK_OFF    (0x00)

/* Flags for display and cursor shift */
#define LCD_DISPLAY_MOVE (0x08)
#define LCD_CURSOR_MOVE  (0x00)
#define LCD_MOVE_RIGHT   (0x04)
#define LCD_MOVE_LEFT    (0x00)

/* Flags for modes */
#define LCD_8BIT_MODE    (0x10)
#define LCD_4BIT_MODE    (0x00)
#define LCD_2LINE        (0x08)
#define LCD_1LINE        (0x00)
#define LCD_5x10_DOTS    (0x04)
#define LCD_5x8_DOTS     (0x00)

/* Flags for backlight control */
#define LCD_BACKLIGHT    (0x08)
#define LCD_NO_BACKLIGHT (0x00)

/* Specific LCD bits */
#define LCD_EN_BIT       (1<<2)
#define LCD_RW_BIT       (1<<1)
#define LCD_RS_BIT       (1<<0)


Lcd_I2C::Lcd_I2C(II2C* hi2c, uint8_t address, uint8_t columns, uint8_t rows) : 
    hi2c(hi2c), 
    address(address), 
    columns(columns), 
    rows(rows), 
    backlight(LCD_NO_BACKLIGHT)
{ }

void Lcd_I2C::init()
{
    hi2c->init();

    display_function = LCD_4BIT_MODE | LCD_1LINE | LCD_5x8_DOTS;

    if (rows > 1) {
        display_function |= LCD_2LINE;
    }

    HAL_Delay(50);

    /* Reset expander, turn backlight off */
    expander_write(backlight);
    HAL_Delay(1000);

    /* Start in 8-bit mode, set it to 4-bit mode */
    write_nibble(0x03 << 4);
    HAL_Delay(5);
    write_nibble(0x03 << 4);
    HAL_Delay(5);
    write_nibble(0x03 << 4);
    HAL_Delay(1);
    write_nibble(0x02 << 4);

    command(LCD_FUNCTIONSET | display_function);

    /* Turn display on. No cursor and no blinking by default */
    display_control = LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF;
    enable_display(true);

    clear();

    /* Default mode: From left to right */
    display_mode = LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DEC;
    command(LCD_ENTRYMODESET | display_mode);

    home();
}

void Lcd_I2C::clear()
{
    command(LCD_CLEARDISPLAY);
    HAL_Delay(2);
}

void Lcd_I2C::home()
{
    command(LCD_RETURNHOME);
    HAL_Delay(2);
}

void Lcd_I2C::set_cursor(uint8_t col, uint8_t row)
{
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row > rows) {
        row = rows - 1;
    }

    command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void Lcd_I2C::enable_cursor(bool en)
{
    if (en) {
        display_control |= LCD_CURSOR_ON;
    } else {
        display_control &= ~LCD_CURSOR_ON;
    }

    command(LCD_DISPLAYCONTROL | display_control);
}

void Lcd_I2C::enable_display(bool en)
{
    if (en) {
        display_control |= LCD_DISPLAY_ON;
    } else {
        display_control &= ~LCD_DISPLAY_ON;
    }
    
    command(LCD_DISPLAYCONTROL | display_control);
}

void Lcd_I2C::enable_blink(bool en)
{
    if (en) {
        display_control |= LCD_BLINK_ON;
    } else {
        display_control &= ~LCD_BLINK_ON;
    }
    
    command(LCD_DISPLAYCONTROL | display_control);
}

void Lcd_I2C::enable_backlight(bool en)
{
    backlight = en ? LCD_BACKLIGHT : LCD_NO_BACKLIGHT;
    expander_write(0);
}

void Lcd_I2C::enable_autoscroll(bool en)
{
    if (en) {
        display_mode |= LCD_ENTRY_SHIFT_INC;
    } else {
        display_mode &= ~LCD_ENTRY_SHIFT_INC;
    }
    
    command(LCD_ENTRYMODESET | display_mode);
}

void Lcd_I2C::set_scroll_left()
{
    command(LCD_CURSORSHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_LEFT);
}
void Lcd_I2C::set_scroll_right()
{
    command(LCD_CURSORSHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_RIGHT);
}

void Lcd_I2C::set_left_to_right()
{
    display_mode |= LCD_ENTRY_LEFT;
    command(LCD_ENTRYMODESET | display_mode);
}

void Lcd_I2C::set_right_to_left()
{
    display_mode &= ~LCD_ENTRY_LEFT;
    command(LCD_ENTRYMODESET | display_mode);
}

void Lcd_I2C::write(uint8_t value)
{
    send(value, LCD_RS_BIT);
}


void Lcd_I2C::command(uint8_t command)
{
    send(command, 0);
}

void Lcd_I2C::send(uint8_t value, uint8_t mode)
{
    uint8_t highnib = value & 0xf0;
    uint8_t lownib = (value << 4) & 0xf0;
    write_nibble((highnib) | mode);
    write_nibble((lownib) | mode);
}

void Lcd_I2C::write_nibble(uint8_t value)
{
    expander_write(value);
    pulse_enable(value);
}

void Lcd_I2C::expander_write(uint8_t data)
{
    data |= backlight;
    hi2c->write(address << 1, &data, 1);
}

void Lcd_I2C::pulse_enable(uint8_t data)
{
    expander_write(data | LCD_EN_BIT);
    expander_write(data & ~LCD_EN_BIT); 
    HAL_Delay(1);
}

size_t Lcd_I2C::print(const char* text)
{
    size_t len = strlen(text);
    
    return print(text, len);
}

size_t Lcd_I2C::print(int32_t number)
{
    char buffer[10];
    sprintf(buffer, "%ld", number);
    return print(buffer);
}

size_t Lcd_I2C::print(char character)
{
    write(character);
    return 1;
}

size_t Lcd_I2C::print(const char* text, uint8_t len) 
{
    for (size_t i = 0; i < len; i++) {
        write(text[i]);
    }

    return len;
}