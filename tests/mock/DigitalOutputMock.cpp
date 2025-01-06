#include <DigitalOutputMock.hpp>

DigitalOutputMock::DigitalOutputMock(GPIO_TypeDef* port, uint8_t pin) : 
    port(port), 
    pin(pin), 
    inverted(false), 
    state(0)
{ }

void DigitalOutputMock::init() 
{ 
    this->state = 0;
}

void DigitalOutputMock::deinit() 
{ 
    this->state = 0;
}

void DigitalOutputMock::set(uint8_t state) 
{ 
    if (inverted) {
        state = !state;
    }

    this->state = state;
}

uint8_t DigitalOutputMock::get() const
{ 
    return (inverted) ? !this->state : this->state;
}

void DigitalOutputMock::toggle() 
{ 
    this->state = !this->state;
}

void DigitalOutputMock::invert(bool invert) 
{ 
    this->inverted = invert;
}

