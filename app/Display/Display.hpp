#pragma once

#include <stddef.h>
#include <stdint.h>

class Display
{

public:
    Display() {}
    virtual ~Display() {}

    virtual void init() = 0;

    virtual void clear() = 0;
    virtual void home() = 0;
    virtual void set_cursor(uint8_t col, uint8_t row) = 0;

    virtual void enable_display(bool en)  = 0;
    virtual void enable_backlight(bool en) = 0;
    virtual void enable_autoscroll(bool en) = 0;

    virtual void write(uint8_t c) = 0;

    virtual size_t print(const char* text, uint8_t len) = 0;
    virtual size_t print(const char* text) = 0;
    virtual size_t print(int32_t number) = 0;
    virtual size_t print(char character) = 0;
};