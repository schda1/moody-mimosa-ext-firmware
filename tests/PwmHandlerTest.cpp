#include <common.hpp>
#include <CppUTest/TestHarness.h>

#include <TimerMock.hpp>
#include <PwmHandler.hpp>
#include <DigitalOutputMock.hpp>

TimerMock* timer;
PwmHandler* pwm;
DigitalOutputMock* output_1;
DigitalOutputMock* output_2;

TEST_GROUP(PwmHandler)
{
    void setup() {
        output_1 = new DigitalOutputMock(GPIOA, 0);
        output_2 = new DigitalOutputMock(GPIOB, 0);
        output_1->init();
        output_2->init();

        timer = new TimerMock(TIM1);
        timer->set_period(10);
        timer->set_prescaler(1);
        timer->init();

        pwm = new PwmHandler(timer);
        pwm->attach(output_1, 50);
        pwm->attach(output_2, 10);
        pwm->init();
        pwm->enable(true);
    }

    void teardown() {
        delete pwm;
        delete timer;
        delete output_1;
        delete output_2;
    }
};

TEST(PwmHandler, test_init)
{
    CHECK_EQUAL(0, output_1->get());
    CHECK_EQUAL(0, output_2->get());
    CHECK_EQUAL(0, timer->get_interrupt_counter());

    timer->tick(10);
    
    CHECK_EQUAL(1, output_1->get());
    CHECK_EQUAL(1, timer->get_interrupt_counter());
}

TEST(PwmHandler, test_pwm_functionality)
{
    timer->tick(10);
    CHECK_EQUAL(1, timer->get_interrupt_counter());
    CHECK_EQUAL(1, output_1->get());
    CHECK_EQUAL(1, output_2->get());

    timer->tick(10*10);
    CHECK_EQUAL(11, timer->get_interrupt_counter());
    CHECK_EQUAL(1, output_1->get());
    CHECK_EQUAL(0, output_2->get());

    timer->tick(40*10);
    CHECK_EQUAL(51, timer->get_interrupt_counter());
    CHECK_EQUAL(0, output_1->get());
    CHECK_EQUAL(0, output_2->get());

    timer->tick(50*10);
    CHECK_EQUAL(101, timer->get_interrupt_counter());
    CHECK_EQUAL(1, output_1->get());
    CHECK_EQUAL(1, output_2->get());
}

TEST(PwmHandler, test_set_duty_cycle)
{
    pwm->set_duty_cycle(0, 20);
    pwm->set_duty_cycle(1, 40);

    timer->tick(10);
    CHECK_EQUAL(1, output_1->get());
    CHECK_EQUAL(1, output_2->get());

    timer->tick(20*10);
    CHECK_EQUAL(0, output_1->get());
    CHECK_EQUAL(1, output_2->get());

    timer->tick(20*10);
    CHECK_EQUAL(0, output_1->get());
    CHECK_EQUAL(0, output_2->get());
}

TEST(PwmHandler, test_enable)
{
    bool are_off = true;
    pwm->enable(false);

    for (uint8_t i = 0; i < 200; i++) {
        timer->tick(10);
        are_off = are_off && (output_1->get() == 0);
        are_off = are_off && (output_2->get() == 0);
    }

    CHECK_EQUAL(true, are_off);
}

TEST(PwmHandler, test_attach_channels)
{
    DigitalOutputMock output(GPIOC, 0);

    CHECK_EQUAL(2, pwm->attached_channels());
    pwm->attach(&output, 50);
    CHECK_EQUAL(3, pwm->attached_channels());
}

TEST(PwmHandler, test_attach_too_many_channels)
{
    DigitalOutputMock* additional_outputs[10];
    uint8_t attached_channels = 2;

    for (uint8_t i = 0; i < 10; i++) {
        additional_outputs[i] = new DigitalOutputMock(GPIOC, i);
    }

    for (uint8_t i = 0; i < 10; i++) {
        pwm->attach(additional_outputs[i], 50);
        attached_channels++;

        if (attached_channels < N_PWM_CHANNELS) {
            CHECK_EQUAL(attached_channels, pwm->attached_channels());
        } else {
            CHECK_EQUAL(N_PWM_CHANNELS, pwm->attached_channels());
        }
    }
    
    for (uint8_t i = 0; i < 10; i++) {
        delete additional_outputs[i];
    }
}

TEST(PwmHandler, test_attach_same_channel_twice) 
{
    DigitalOutputMock output(GPIOC, 0);

    /* Attach channel */
    pwm->attach(&output, 50);
    CHECK_EQUAL(3, pwm->attached_channels());

    /* Attach channel again */
    pwm->attach(&output, 50);
    CHECK_EQUAL(3, pwm->attached_channels());
} 