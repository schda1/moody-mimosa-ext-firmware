#pragma once

#include <HalWrapper/IDigitalOutput.hpp>

enum class SpiMode
{
    MODE_0,
    MODE_1,
    MODE_2,
    MODE_3
};

class ISpi
{

public:
    ISpi() {}
    virtual ~ISpi() {}

    virtual void init() = 0;
    virtual void deinit() = 0;
    virtual void add_cs(IDigitalOutput* cs) = 0;

    virtual void set_frequency(uint32_t frequency) = 0;
    virtual void set_mode(SpiMode mode) = 0;

    virtual void write(const uint8_t* data_tx, uint16_t len) = 0;
    virtual void read(uint8_t* data_rx, uint16_t len) = 0;
    virtual void write_and_read(const uint8_t* data_tx, uint8_t* data_rx, uint16_t len) = 0;
    virtual void write_then_read(const uint8_t* data_tx, uint16_t len_tx, uint8_t* data_rx, uint16_t len_rx) = 0;
    virtual void write_then_write(const uint8_t* data_tx1, uint16_t len_tx1, const uint8_t* data_tx2, uint16_t len_tx2) = 0;
};