#pragma once

#include <common.hpp>

enum class DigitalInputMode
{
    INPUT,
    INPUT_PULL_UP,
    INPUT_PULL_DOWN
};

class IDigitalInput
{
public:
    IDigitalInput(DigitalInputMode mode){ this->mode = mode; }
    virtual ~IDigitalInput() {}

    virtual void init() = 0;
    virtual void deinit() = 0;

    virtual uint8_t get() const = 0;
    virtual void invert(bool invert) = 0;

    virtual void set_input_mode(DigitalInputMode mode) { this->mode = mode; }

protected:
    DigitalInputMode mode;
};