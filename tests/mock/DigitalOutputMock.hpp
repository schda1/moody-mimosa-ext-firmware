#include <HalWrapper/IDigitalOutput.hpp>
#include "stm32g4xx_hal.h"

class DigitalOutputMock : public IDigitalOutput {
    public:
        DigitalOutputMock(GPIO_TypeDef* port, uint8_t pin);
        virtual ~DigitalOutputMock() {}

        void init() override;
        void deinit() override;

        void set(uint8_t state) override;
        uint8_t get() const override;
        void toggle() override;
        void invert(bool invert) override;

    protected:
        GPIO_TypeDef* port;
        uint8_t pin;
        bool inverted;
        
        bool state;
};