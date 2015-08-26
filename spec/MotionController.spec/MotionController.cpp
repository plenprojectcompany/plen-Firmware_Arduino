// Arduinoライブラリ関連
#include "Arduino.h"

// 独自ライブラリ関連
#include "System.h"
#include "MotionController.h"


namespace {
	PLEN2::System         system;
	PLEN2::ExternalEEPROM exteeprom;


	inline static const int PRECISION() { return 16; }
	
	inline long getFixedPoint(int value)
	{
		return (value << PRECISION());
	}
}


PLEN2::MotionController::MotionController(JointController& joint_ctrl)
{

}


bool PLEN2::MotionController::setHeader(const Header* p_header)
{

}


bool PLEN2::MotionController::getHeader(Header* p_header)
{

}


bool PLEN2::MotionController::setFrame(unsigned char slot, const Frame* p_frame)
{

}


bool PLEN2::MotionController::getFrame(unsigned char slot, Frame* p_frame)
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : MotionController::getFrame()"));
	#endif

	char* filler = (char*)p_frame;

	int read_count = sizeof(Frame) / (ExternalEEPROM::SLOT_SIZE() - 2);
	int read_size_sup = sizeof(Frame) - read_count * (ExternalEEPROM::SLOT_SIZE() - 2);

	int ret = exteeprom.readSlot((int)slot * )
}


bool PLEN2::MotionController::playing()
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : MotionController::playing()"));
	#endif

	return _playing;
}


bool PLEN2::MotionController::frameUpdatable()
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : MotionController::frameUpdatable()"));
	#endif

	volatile static unsigned char before_overflow_count = 0;
	
	bool result = (before_overflow_count != _p_joint_ctrl->_overflow_count);
	before_overflow_count = _p_joint_ctrl->_overflow_count;

	return result;
}


bool PLEN2::MotionController::frameUpdateFinished()
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : MotionController::frameUpdateFinished()"));
	#endif

	return (_transition_count == 0);
}


bool PLEN2::MotionController::nextFrameLoadable()
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : MotionController::nextFrameLoadable()"));
	#endif

	if (_header.codes[0] != 0) return true;

	return ((_p_frame_next->number + 1) < _header.frame_num);
}


void PLEN2::MotionController::play(unsigned char slot)
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : MotionController::play()"));
	#endif

	if (slot >= SLOT_MAX())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> bad argment : slot = "));
			system.outputSerial().println((int)slot);
		#endif

		return;
	}

	_header.slot = slot;
	getHeader(&_header);

	_p_frame_next->number = 0;
	getFrame(_header.slot, _p_frame_next);

	if (_header.frame_num > Header::FRAMENUM_MIN())
	{
		_p_frame_back->number = 1;
		getFrame(_header.slot, _p_frame_back);
	}

	_transition_count = _p_frame_next->transition_time_ms / Frame::UPDATE_INTERVAL_MS();
	for (int joint_id = 0; joint_id < JointController::SUM(); joint_id++)
	{
		_now_fixed_points[joint_id] = getFixedPoint(_p_frame_now->joint_angle[joint_id]);
		
		_diff_fixed_points[joint_id] =  getFixedPoint(_p_frame_next->joint_angle[joint_id]) - _now_fixed_points[joint_id];
		_diff_fixed_points[joint_id] /= _transition_count;
	}

	_playing = true;
}


void PLEN2::MotionController::stop()
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : MotionController::stop()"));
	#endif

	_header.codes[0] = 0;
}


void PLEN2::MotionController::frameBuffering()
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : MotionController::frameBuffering()"));
	#endif

	Frame* p_frame_temp = _p_frame_now;
	_p_frame_now  = _p_frame_next;
	_p_frame_next = _p_frame_back;
	_p_frame_back = p_frame_temp;
}


void PLEN2::MotionController::dump(unsigned char slot)
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : MotionController::dump()"));
	#endif

	if (slot >= SLOT_MAX())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> bad argment : slot = "));
			system.outputSerial().println((int)slot);
		#endif

		return;
	}

	Header header;
	header.slot = slot;
	getHeader(&header);

	system.outputSerial().println(F("{"));

		system.outputSerial().print(F("\t\"slot\": "));
		system.outputSerial().print(header.slot, HEX);
		system.outputSerial().println(F(","));

		system.outputSerial().print(F("\t\"name\": "));
		system.outputSerial().print(header.name);
		system.outputSerial().println(F(","));

		system.outputSerial().println(F("\t\"codes\": ["));
	
			system.outputSerial().print(F("\t\t\"function\": "));
			system.outputSerial().print(header.codes[0], HEX);
			system.outputSerial().println(F(","));

			system.outputSerial().print(F("\t\t\"arguments\": ["));
	
				system.outputSerial().print(header.codes[1], HEX);
				system.outputSerial().println(F(","));

				system.outputSerial().print(header.codes[2], HEX);
	
			system.outputSerial().println(F("\t\t]"));
	
		system.outputSerial().println(F("\t],"));

		system.outputSerial().println(F("\t\"frames\": ["));

		for (int frame_index = 0; frame_index < header.frame_num; frame_index++)
		{
			Frame frame;
			frame.number = frame_index;
			getFrame(header.slot, &frame);

			system.outputSerial().println(F("\t\t{"));

			system.outputSerial().print(F("\t\t\t\"transition_time_ms\": "));
			system.outputSerial().print(frame.transition_time_ms);
			system.outputSerial().println(F(","));

			system.outputSerial().println(F("\t\t\t\"outputs\": ["));

			for (int device_index = 0; device_index < JointController::SUM(); device_index++)
			{
				system.outputSerial().println(F("\t\t\t\t{"));

					system.outputSerial().print(F("\t\t\t\t\t\"device:\" "));
					system.outputSerial().println(device_index);

					system.outputSerial().print(F("\t\t\t\t\t\"value:\" "));
					system.outputSerial().println(frame.joint_angle[device_index]);

				if ((device_index + 1) == JointController::SUM())
				{
				system.outputSerial().println(F("\t\t\t\t},"));
				}
				else
				{
				system.outputSerial().println(F("\t\t\t\t}"));
				}
			}

				system.outputSerial().println(F("\t\t\t]"));

			if ((frame_index + 1) == header.frame_num)
			{
			system.outputSerial().println(F("\t\t}"));
			}
			else
			{
			system.outputSerial().println(F("\t\t},"));
			}
		}

		system.outputSerial().println(F("\t]"));
	
	system.outputSerial().println(F("}"));
}