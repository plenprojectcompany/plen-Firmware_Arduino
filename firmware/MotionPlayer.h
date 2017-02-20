/*!
    @file      MotionPlayer.h
    @brief     Management class of a motion.
    @author    Kazuyuki TAKASE
    @copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_MOTION_PLAYER_H
#define PLEN2_MOTION_PLAYER_H


#include <stdint.h>

#include "JointController.h"

namespace PLEN2
{
    namespace Motion
    {
        class Header;
        class Frame;
    }

    class MotionInterpreter;
    class MotionPlayer;
}

/*!
    @brief Management class of a motion
*/
class PLEN2::MotionPlayer
{
friend class MotionInterpreter;

public:
    /*!
        @brief Function pointer of motion playing thread handler

        @attention
        The instance should be a private member normally.
        It is a public member because it is the only way to access it from watchdog timeout interruption vector,
        so you must not access it from other functions basically.
    */
    static void (*m_thread_handler)();

    /*!
        @brief Constructor

        @param [in] joint_ctrl An instance of joint controller.
    */
    explicit MotionPlayer(JointController& joint_ctrl);

    /*!
        @brief Decide if a motion is playing

        @return Result
    */
    bool playing();

    /*!
        @brief Decide if a motion frame is updatable

        @return Result
    */
    bool frameUpdatable();

    /*!
        @brief Decide that updating a frame has finished

        @return Result
    */
    bool updatingFinished();

    /*!
        @brief Decide if there is a loadable frame at the next time

        @return Result
    */
    bool nextFrameLoadable();

    /*
        @brief Set user defined function as the thread handler

        @param [in] handler Function pointer of user defined thread handler
    */
    void setThreadHandler(void (*handler)());

    /*!
        @brief Play a motion

        @param [in] slot Number of a motion.
    */
    void play(uint8_t slot);

    /*!
        @brief Play a frame directly

        @param [in] frame An instance of frame
    */
    void playFrameDirectly(const Motion::Frame& frame);

    /*!
        @brief Will stop playing a motion

        The method doesn't stop playing a motion just after running itself,
        but will stop it when a frame that has the stop flag is discovered.
    */
    void willStop();

    /*!
        @brief Stop playing a motion
    */
    void stop();

    /*
        @brief Add differences between current-frame and next-frame to current-frame
    */
    void updateFrame();

    /*!
        @brief Load next frame
    */
    void loadNextFrame();

    /*!
        @brief Dump a motion with JSON format

        Output JSON string like a "motion.json",
        however, property "device"'s type is not string but integer defined as device id in "device_map.json".

        @sa
        - https://github.com/plenprojectcompany/PLEN2/tree/master/motion
        - https://github.com/plenprojectcompany/plen-ControlServer/blob/master/control_server/device_map.json

        @param [in] slot Slot of a motion.
    */
    void dump(uint8_t slot);

private:
    enum { FRAMEBUFFER_LENGTH = 2 };

    void m_setupFrame(uint8_t index);
    void m_bufferingFrame();


    JointController* m_joint_ctrl_ptr;

    uint8_t m_transition_count;
    bool    m_playing;

    Motion::Header m_header;
    Motion::Frame  m_buffer[FRAMEBUFFER_LENGTH];
    Motion::Frame* m_frame_current_ptr;
    Motion::Frame* m_frame_next_ptr;

    int32_t m_current_fixed_points[JointController::JOINTS_SUM];
    int32_t m_diff_fixed_points[JointController::JOINTS_SUM];
};

#define PLEN2_MOTION_PLAYER_CREATE_DEFAULT_THREAD_HANDLER(instance) \
    struct DefaultThreadHandler \
    { \
        static void core() \
        { \
            if (instance.playing()) \
            { \
                if (instance.frameUpdatable()) \
                { \
                    instance.updateFrame(); \
                } \
                if (instance.updatingFinished()) \
                { \
                    if (instance.nextFrameLoadable()) \
                    { \
                        instance.loadNextFrame(); \
                    } \
                    else \
                    { \
                        instance.stop(); \
                    } \
                } \
            } \
        } \
    }

#define PLEN2_MOTION_PLAYER_DEFAULT_THREAD_HANDLER DefaultThreadHandler::core

#endif // PLEN2_MOTION_PLAYER_H
