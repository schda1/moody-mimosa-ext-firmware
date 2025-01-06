#pragma once

#include <stdint.h>

class II2C
{

public:
    II2C() {}
    virtual ~II2C() {}

    virtual void init() = 0;
    virtual void deinit() = 0;

    virtual void set_frequency(uint32_t frequency) = 0;
    virtual void set_reg_address_len(uint8_t length) = 0;

    virtual void write(uint8_t dev_address, const uint8_t* data_tx, uint16_t len) = 0;
    virtual void read(uint8_t dev_address,uint8_t* data_rx, uint16_t len) = 0;
    
    virtual void write_reg(uint8_t dev_address, uint16_t reg_address, const uint8_t* data_tx, uint16_t len) = 0;
    virtual void read_reg(uint8_t dev_address, uint16_t reg_address, uint8_t* data_rx, uint16_t len) = 0;
};