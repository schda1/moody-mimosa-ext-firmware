#pragma once

#include <functional>
#include <common.hpp>

class IUart {
    public: 
        using ReadCallback = std::function<void(uint8_t)>;
        using TransmitCallback = std::function<void(void)>;

        IUart(USART_TypeDef* instance) { this->instance = instance; }
        virtual ~IUart() {}

        virtual void init() = 0;
        virtual void deinit() = 0;

        virtual void set_baudrate(uint32_t baudrate) = 0;

        virtual void write(uint8_t data) = 0;
        virtual void write(const uint8_t* data, uint16_t len) = 0;
        virtual uint16_t read(uint8_t* data, uint16_t len) = 0;

        virtual void start_read_async() = 0;
        virtual void stop_read_async() = 0;

        virtual void rx_callback() = 0;
        virtual void tx_callback() = 0;
        virtual void set_read_callback(ReadCallback callback) { async_read_callback = callback; }
        virtual void set_transmit_callback(TransmitCallback callback) {async_transmit_callback = callback; }

        virtual USART_TypeDef* get_instance() { return instance; }
        virtual UART_HandleTypeDef* get_handle() { return &huart; }

    protected:
        USART_TypeDef* instance;
        UART_HandleTypeDef huart;
        ReadCallback async_read_callback;
        TransmitCallback async_transmit_callback;

};