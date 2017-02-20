#line 2 "MotionPlayer.operation.spec.ino"


#include <avr/wdt.h>

#include <Wire.h>
#include <EEPROM.h>

#include "System.h"
#include "ExternalEEPROM.h"
#include "Parser.h"
#include "Protocol.h"
#include "JointController.h"
#include "Motion.h"
#include "MotionPlayer.h"


namespace
{
    PLEN2::JointController joint_ctrl;
    PLEN2::MotionPlayer    motion_player(joint_ctrl);

    PLEN2_MOTION_PLAYER_CREATE_DEFAULT_THREAD_HANDLER(motion_player);

    class OperationTest: public PLEN2::Protocol
    {
    private:
        void playMotion()
        {
            motion_player.play(
                Utility::hexbytes2uint16<2>(m_buffer.data)
            );
        }

        void stopMotion()
        {
            motion_player.willStop();
        }

        void getMotion()
        {
            motion_player.dump(
                Utility::hexbytes2uint16<2>(m_buffer.data)
            );
        }

    public:
        virtual void afterHook()
        {
            if (m_state == READY)
            {
                uint8_t header_id = m_parser[HEADER_INCOMING]->index();
                uint8_t cmd_id    = m_parser[COMMAND_INCOMING]->index();

                if ((header_id == 0 /* := Controller */) && (cmd_id == 5 /* := PLAY MOTION */))
                {
                    playMotion();
                }

                if ((header_id == 0 /* := Controller */) && (cmd_id == 6 /* := STOP MOTION */))
                {
                    stopMotion();
                }

                if ((header_id == 3 /* := Getter     */) && (cmd_id == 1 /* := MOTION      */))
                {
                    getMotion();
                }
            }
        }
    };

    OperationTest test_core;
}


/*!
    @brief The application entry point
*/
void setup()
{
    PLEN2::System::begin();
    PLEN2::ExternalEEPROM::begin();

    joint_ctrl.loadSettings();
    motion_player.setThreadHandler(PLEN2_MOTION_PLAYER_DEFAULT_THREAD_HANDLER);

    while (!Serial);

    PLEN2::System::outputSerial().print(F("# Test : "));
    PLEN2::System::outputSerial().println(F(__FILE__));
}


/*!
    @brief Main polling loop
*/
void loop()
{
    using namespace PLEN2;

    if (System::USBSerial().available())
    {
        test_core.readByte(System::USBSerial().read());

        if (test_core.accept())
        {
            test_core.transitState();
        }
    }

    if (System::BLESerial().available())
    {
        test_core.readByte(System::BLESerial().read());

        if (test_core.accept())
        {
            test_core.transitState();
        }
    }

}