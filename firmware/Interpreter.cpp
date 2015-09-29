/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#include "Arduino.h"

#include "System.h"
#include "Interpreter.h"
#include "MotionController.h"

#define _DEBUG false


namespace
{
	PLEN2::System system;

	inline unsigned char getIndex(unsigned char value)
	{
		return (value & (PLEN2::Interpreter::QUEUE_SIZE() - 1));
	}
}


PLEN2::Interpreter::Interpreter(MotionController& motion_crtl)
{
	_queue_begin = 0;
	_queue_end   = 0;
	_p_motion_ctrl = &motion_crtl;
}

bool PLEN2::Interpreter::pushCode(const Code& code)
{
	#if _DEBUG
		outputSerial().println(F("=== in function : Interpreter::pushCode()"));
	#endif

	if (getIndex(_queue_end + 1) == _queue_begin)
	{
		#if _DEBUG
			outputSerial().println(F(">>> error : Queue overflow!"));
		#endif

		return false;
	}

	_code_queue[_queue_end] = code;
	_queue_end = getIndex(_queue_end + 1);

	return true;
}

bool PLEN2::Interpreter::popCode()
{
	#if _DEBUG
		outputSerial().println(F("=== in function : Interpreter::popCode()"));
	#endif

	if (!ready())
	{
		#if _DEBUG
			outputSerial().println(F(">>> error : This is not ready!"));
		#endif

		return false;
	}

	Code& doing = _code_queue[_queue_begin];
	_queue_begin = getIndex(_queue_begin + 1);

	_p_motion_ctrl->play(doing.slot);

	if (doing.loop_count != 0)
	{
		if (_p_motion_ctrl->_header.use_loop == 0)
		{
			_p_motion_ctrl->_header.use_loop = 1;

			_p_motion_ctrl->_header.loop_begin = 0;
			_p_motion_ctrl->_header.loop_end   = _p_motion_ctrl->_header.frame_length - 1;
		}
	}
	else
	{
		_p_motion_ctrl->_header.use_loop = 0;
	}

	_p_motion_ctrl->_header.loop_count = doing.loop_count;

	return true;
}

bool PLEN2::Interpreter::ready()
{
	#if _DEBUG
		outputSerial().println(F("=== in function : Interpreter::ready()"));
	#endif

	return (_queue_begin != _queue_end);
}

void PLEN2::Interpreter::reset()
{
	#if _DEBUG
		outputSerial().println(F("=== in function : Interpreter::reset()"));
	#endif

	_queue_begin = 0;
	_queue_end   = 0;
}