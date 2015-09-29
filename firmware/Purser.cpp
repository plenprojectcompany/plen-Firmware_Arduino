/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

// #include <ArduinoJSON.h>

// 独自ライブラリ関連
#include "Purser.h"


namespace
{
	class AbstractToken
	{
	public:
		virtual bool incomingFook()
		{
			return true;
		}

		virtual bool outgoingFook()
		{
			return true;
		}

		virtual int  end() = 0;
	};

	class SetterToken : public AbstractToken
	{
	public:
		typedef enum
		{
			HOME_ANGLE,
			JOINT_SETTINGS,
			MAX_ANGLE,
			MOTION_FRAME,
			MOTION_HEADER,
			MIN_ANGLE,
			EOE
		} Value;

		/*!
			@note
			事前にアルファベット順でソートしておく。
		*/
		const char* SYMBOL[] = {
			"HO",
			"JS",
			"MA",
			"MF",
			"MH",
			"MI"
		};

		int end()
		{
			return EOE;
		}
	};


	class GetterToken : public AbstractToken
	{
	public:
		typedef enum
		{
			JOINT_SETTINGS,
			MOTION,
			VERSION_INFORMATION,
			EOE
		} Value;

		/*!
			@note
			事前にアルファベット順でソートしておく。
		*/
		const char* SYMBOL[] = {
			"JS",
			"MO",
			"VI"
		};

		int end()
		{
			return EOE;
		}
	};

	class ControllerToken : public AbstractToken
	{
		typedef enum
		{
			ANGLE_DIFF,
			ANGLE,
			HOME_POSITION,
			PLAY_MOTION,
			STOP_MOTION,
			EOE
		} Value;

		/*!
			@note
			事前にアルファベット順でソートしておく。
		*/
		const char* SYMBOL[] = {
			"AD",
			"AN",
			"HP",
			"MP",
			"MS"
		};

		int end()
		{
			return EOE;
		}
	};

	class InterpreterToken : public AbstractToken
	{
		typedef enum 
		{
			RESET_INTERPRETER,
			SET_FUNCTION,
			EOE
		} Value;

		/*!
			@note
			事前にアルファベット順でソートしておく。
		*/
		const char* SYMBOL[] = {
			"RI",
			"SF"
		};

		int end()
		{
			return EOE;
		}
	};

	class HeaderToken : public AbstractToken
	{
		typedef enum
		{
			CONTROLLER,
			INTERPRETER,
			SETTER,
			GETTER,
			EOE
		} VALUE;

		/*!
			@note
			事前に処理の優先度順でソートしておく。
		*/
		const char SYMBOL[] = {
			'$',
			'#',
			'<',
			'>'
		};

		int end()
		{
			return EOE;
		}
	};

	HeaderToken      header_token;
	ControllerToken  controller_token;
	InterpreterToken interpreter_token;
	SetterToken      setter_token;
	GetterToken      getter_token;

	AbstractToken* token_array[] = {
		&controller_token,
		&interpreter_token,
		&setter_token,
		&getter_token
	};

	StaticJsonBuffer<200> json;
}


PLEN2::Purser::Purser()
{
	_header_token = HeaderToken::EOE;
}

void PLEN2::Purser::readByte(char byte)
{
	switch (_state)
	{
		case INIT:
		{
			for (int token = 0; token < HeaderToken::EOE; token++)
			{
				if (byte == HeaderToken::SYMBOL[token])
				{
					_header_token = token;
					_token = token_array[token];

					break;
				}
			}

			break;
		}

		case HEADER_INCOMING:
		{
			for ()
		}

		case COMMAND_INCOMING:
		{

		}

		case ARGUMENTS_INCOMING:
		{

		}
	}
}