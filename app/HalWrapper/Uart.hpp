#pragma once

#include <common.hpp>
#include <HalWrapper/IUart.hpp>

class Uart : public IUart
{

public:
    Uart(USART_TypeDef* instance);
    virtual ~Uart() {}

    void init() override;
    void deinit() override;

    void set_baudrate(uint32_t baudrate) override;

    void write(uint8_t data) override;
    void write(const uint8_t* data, uint16_t len) override;
    uint16_t read(uint8_t* data, uint16_t len) override;

    void start_read_async() override;
    void stop_read_async() override;

    void rx_callback() override;
    void tx_callback() override;

protected:
    void init_uart();

    uint8_t rx_data;
    bool async_read;
};