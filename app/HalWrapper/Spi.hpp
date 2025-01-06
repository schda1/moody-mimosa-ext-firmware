#pragma once 

#include <common.hpp>
#include <HalWrapper/IDigitalOutput.hpp>
#include <HalWrapper/ISpi.hpp>

class Spi : public ISpi
{
public:
    Spi(SPI_TypeDef* instance, IDigitalOutput* cs);
    virtual ~Spi() {}

    void init() override;
    void deinit() override;
    void add_cs(IDigitalOutput* cs) override;

    void set_frequency(uint32_t frequency) override;
    void set_mode(SpiMode mode) override;

    void write(const uint8_t* data_tx, uint16_t len) override;
    void read(uint8_t* data_rx, uint16_t len) override;
    void write_and_read(const uint8_t* data_tx, uint8_t* data_rx, uint16_t len) override;
    void write_then_read(const uint8_t* data_tx, uint16_t len_tx, uint8_t* data_rx, uint16_t len_rx) override;
    void write_then_write(const uint8_t* data_tx1, uint16_t len_tx1, const uint8_t* data_tx2, uint16_t len_tx2) override;

private:
    void set_cs(uint8_t state);

    SPI_TypeDef* instance;
    SPI_HandleTypeDef hspi;
    IDigitalOutput* cs;
    SpiMode mode;
    uint32_t frequency;
};