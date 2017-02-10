#line 2 "JointController.speed.spec.ino"


#include <Arduino.h>
#include <EEPROM.h>

#include "Pin.h"
#include "System.h"
#include "JointController.h"


#define PLEN2_JOINTCONTROLLER_PWM_OUT_00_07_REGISTER OCR1C
#define PLEN2_JOINTCONTROLLER_PWM_OUT_08_15_REGISTER OCR1B
#define PLEN2_JOINTCONTROLLER_PWM_OUT_16_23_REGISTER OCR1A


namespace
{
    using namespace PLEN2;

    void dummy_timer1_ovf_vect()
    {
        volatile static uint8_t output_select = 0;
        volatile static uint8_t joint_select  = 1;

        digitalWrite(Pin::MULTIPLEXER_SELECT0, bitRead(output_select, 0));
        digitalWrite(Pin::MULTIPLEXER_SELECT1, bitRead(output_select, 1));
        digitalWrite(Pin::MULTIPLEXER_SELECT2, bitRead(output_select, 2));

        PLEN2_JOINTCONTROLLER_PWM_OUT_00_07_REGISTER = JointController::m_pwms[
            joint_select + 0 * JointController::Multiplexer::SELECTABLE_LINES
        ];

        PLEN2_JOINTCONTROLLER_PWM_OUT_08_15_REGISTER = JointController::m_pwms[
            joint_select + 1 * JointController::Multiplexer::SELECTABLE_LINES
        ];

        PLEN2_JOINTCONTROLLER_PWM_OUT_16_23_REGISTER = JointController::m_pwms[
            joint_select + 2 * JointController::Multiplexer::SELECTABLE_LINES
        ];

        (++output_select) &= (JointController::Multiplexer::SELECTABLE_LINES - 1);
        (++joint_select)  &= (JointController::Multiplexer::SELECTABLE_LINES - 1);

        (joint_select == 0)? (JointController::m_1cycle_finished = true) : false;
    }
}


/*!
    @brief The application entry point
*/
void setup()
{
    PLEN2::System::begin();

    while (!Serial);

    Serial.print(F("# Test : "));
    Serial.println(__FILE__);
}

void loop()
{
    uint32_t begin_us, end_us;

    begin_us = micros();
    dummy_timer1_ovf_vect();
    end_us   = micros();

    PLEN2::System::debugSerial().print(F("Running time[us]: "));
    PLEN2::System::debugSerial().println(end_us - begin_us);

    while (true);
}
