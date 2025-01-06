#include <common.hpp>
#include <CppUTest/TestHarness.h>
#include <PinObserver.hpp>

PinObserver* observer;

void configure_pins(GPIO_TypeDef* port, uint8_t pin) { }


TEST_GROUP(PinObserver)
{
    void setup() {
        observer = new PinObserver(configure_pins);
    }

    void teardown() {
        delete observer;
    }
};

TEST(PinObserver, test_init)
{   
    PinObservation pins = observer->get_pins();

    LONGS_EQUAL(0, pins.cycle);
    LONGS_EQUAL(0, pins.ui_in);
    LONGS_EQUAL(0, pins.uo_out);
    LONGS_EQUAL(0, pins.uoi);
    LONGS_EQUAL(0, pins.ctrl);
}

TEST(PinObserver, test_configure)
{
    GPIO_TypeDef port;

    /* Configure */
    for (int i = 0; i < 4; i++) {
        observer->configure((PortType)i, &port, i+5);
    }

    /* Check configuration */
    for (int i = 0; i < 4; i++) {
        PortConfig config = observer->get_config((PortType)i);
        POINTERS_EQUAL(&port, config.port);
        LONGS_EQUAL(i+5, config.mask);
    }
}

TEST(PinObserver, test_uin)
{
    GPIO_TypeDef port[4];

    /* Set up */
    for (int i = 0; i < 4; i++) {
        observer->configure((PortType)i, &port[i], 0x00FF);
        port[i].IDR = 0x0012 + i;
    }

    /* Update */
    observer->update();

    /* Check */
    PinObservation pins = observer->get_pins();
    LONGS_EQUAL(1, pins.cycle);
    LONGS_EQUAL(port[0].IDR, pins.ui_in);
    LONGS_EQUAL(port[1].IDR, pins.uo_out);
    LONGS_EQUAL(port[2].IDR, pins.uoi);
    LONGS_EQUAL(port[3].IDR, pins.ctrl);
}

/**
 * Test bit compression 
 * 
 * Mask:       0101010101010101
 * Idr:        0101000000000101
 * Compressed  0000000011000011
 */
TEST(PinObserver, test_compress)
{
    GPIO_TypeDef port;

    observer->configure(PortType::UI_IN, &port, 0x5555);
    port.IDR = 0x5005;

    observer->update();

    PinObservation pins = observer->get_pins();
    LONGS_EQUAL(1, pins.cycle);
    LONGS_EQUAL(0x00C3, pins.ui_in);
}