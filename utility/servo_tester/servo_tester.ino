#define PRINTOUT_CSV true
#define USE_RAWDATA  true

#include <Arduino.h>
#include <EEPROM.h>

#include "System.h"
#include "JointController.h"


namespace
{
    PLEN2::JointController joint_ctrl;


    enum { DEVICE_ID = 0 };

    enum { PWM_TRIM_BEGIN  = 550 };
    enum { PWM_TRIM_END    = 850 };

    bool isRange(uint16_t pwm)
    {
        return ((PWM_TRIM_BEGIN <= pwm) && (pwm < PWM_TRIM_END));
    }

    uint32_t pwm2usec(uint16_t pwm)
    {
        return ((static_cast<uint32_t>(1023 - pwm) * 4096) / 1023);
    }

    uint32_t analog2angle(uint16_t analog)
    {
        return ((static_cast<uint32_t>(analog) * 3333) / 1023);
    }
}


void setup()
{
    PLEN2::System::begin();
    joint_ctrl.loadSettings();

    pinMode(A0, OUTPUT); digitalWrite(A0, LOW);
    pinMode(A2, OUTPUT); digitalWrite(A2, LOW);

    pinMode(A3, OUTPUT); digitalWrite(A3, LOW);
    pinMode(A5, OUTPUT); digitalWrite(A5, LOW);
}

void loop()
{
    using namespace PLEN2;


    joint_ctrl.m_pwms[0] = JointController::PWM_MIN;
    joint_ctrl.m_pwms[1] = JointController::PWM_MIN;

    System::USBSerial().println(F("=== Waiting key input... ==="));
    while (System::USBSerial().available() <= 0);
    while (System::USBSerial().available()) System::USBSerial().read();

    #if PRINTOUT_CSV
        #if USE_RAWDATA
            System::USBSerial().print(F("DEVICE ID: "));
            System::USBSerial().println(DEVICE_ID);

            System::USBSerial().println(F("PWM1,Analog1,PWM2,Analog2"));
        #else
            System::USBSerial().println(F("PWM1 [us],Angle1 [deg x10],PWM2 [us],Angle2 [deg x10]"));
        #endif
    #endif

    for (uint16_t pwm = JointController::PWM_MIN; pwm <= JointController::PWM_MAX; pwm++)
    {
        joint_ctrl.m_pwms[0] = pwm;
        joint_ctrl.m_pwms[1] = pwm;

        delay(100);

        const uint16_t analog1 = analogRead(1);

        delay(5);

        const uint16_t analog2 = analogRead(4);

        delay(5);

        if (isRange(pwm))
        {
            #if PRINTOUT_CSV
                System::USBSerial().print(USE_RAWDATA ? pwm : pwm2usec(pwm));
                System::USBSerial().print(F(","));
                System::USBSerial().print(USE_RAWDATA ? analog1 : analog2angle(analog1));
                System::USBSerial().print(F(","));
                System::USBSerial().print(USE_RAWDATA ? pwm : pwm2usec(pwm));
                System::USBSerial().print(F(","));
                System::USBSerial().println(USE_RAWDATA ? analog2 : analog2angle(analog2));
            #endif
        }
    }
}