#pragma once

#include <common.hpp>

class IDigitalOutput
{
public:
    IDigitalOutput() {}
    virtual ~IDigitalOutput() {}

    virtual void init() = 0;
    virtual void deinit() = 0;

    virtual void set(uint8_t state) = 0;
    virtual uint8_t get() const = 0;
    virtual void toggle() = 0;
    virtual void invert(bool invert) = 0;
};