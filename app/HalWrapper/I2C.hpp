#pragma once 

#include <common.hpp>
#include <HalWrapper/II2C.hpp>

class I2C : public II2C
{
public:
    I2C(I2C_TypeDef* hspi);
    virtual ~I2C() {}

    void init() override;
    void deinit() override;

    void set_frequency(uint32_t frequency) override;
    void set_reg_address_len(uint8_t length) override;

    void write(uint8_t dev_address, const uint8_t* data_tx, uint16_t len) override;
    void read(uint8_t dev_address, uint8_t* data_rx, uint16_t len) override;

    void write_reg(uint8_t dev_address, uint16_t reg_address, const uint8_t* data_tx, uint16_t len) override;
    void read_reg(uint8_t dev_address, uint16_t reg_address, uint8_t* data_rx, uint16_t len) override;

    I2C_HandleTypeDef* get_handle() { return &hi2c; }

private:

    uint32_t calc_timing_register();

    I2C_TypeDef* instance;
    I2C_HandleTypeDef hi2c;
    
    uint8_t reg_address_len;
    uint32_t frequency;
};