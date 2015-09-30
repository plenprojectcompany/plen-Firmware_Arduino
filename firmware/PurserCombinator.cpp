/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

// 標準ライブラリ
#include <ctype.h>
#include <string.h>

// 独自ライブラリ
#include "PurserCombinator.h"

#define _DEBUG false


namespace PLEN2
{
	class AbstractPurser;
	class CharGroupPurser;
	class StringGroupPurser;
	class HexStringPurser;
}


class PLEN2::AbstractPurser
{
protected:
	int m_index;

public:
	AbstractPurser()
		: m_index(-1)
	{
		// noop.
	}

	virtual ~AbstractPurser() {}

	virtual bool purse(const char* input) = 0;
	virtual int index()
	{
		return m_index;
	}
};


class PLEN2::CharGroupPurser : public PLEN2::AbstractPurser
{
private:
	const char* m_accept_chars;

public:
	CharGroupPurser(const char* accept_chars)
		: m_accept_chars(accept_chars)
	{
		// noop.
	}

	~CharGroupPurser()
	{
		// noop.
	}

	bool purse(const char* input)
	{
		const char* it = m_accept_chars;

		m_index = 0;
		while (it[m_index] != '\0')
		{
			if (it[m_index] == input[0])
			{
				return true;
			}

			m_index++;
		}

		m_index = -1;
		return false;
	}
};


class PLEN2::StringGroupPurser : public PLEN2::AbstractPurser
{
private:
	const char* m_accept_strs[];
	const int   m_size;

public:
	StringGroupPurser(const char* accept_strs[], const int size)
		: m_accept_strs(accept_strs)
		, m_size(size)
	{
		// noop.
	}

	~StringGroupPurser()
	{
		// noop.
	}
	
	bool purse(const char* input)
	{
		int begin  = 0;
		int middle = m_size / 2;
		int end    = m_size;

		while (begin < end)
		{
			int result = strncasecmp(input, m_accept_strs[middle], 2);

			if (result == 0)
			{
				m_index = middle;
				return true;
			}

			if (result > 0)
			{
				begin  = middle + 1;
				middle = (begin + end) / 2;

				continue;
			}

			if (result < 0)
			{
				end    = middle - 1;
				middle = (begin + end) / 2;

				continue;
			}
		}

		m_index = -1;
		return false;
	}
};


class PLEN2::HexStringPurser : public PLEN2::AbstractPurser
{
public:
	HexStringPurser()
	{
		// noop.
	}

	~HexStringPurser()
	{
		// noop.
	}

	bool purse(const char* input)
	{
		const char* it = input;

		while (*it != '\0')
		{
			if (isxdigit(*it++) == 0)
			{
				m_index = -1;
				return false;
			}
		}

		m_index = 0;
		return true;
	}
};


namespace
{
	/*!
		@note
		CharGroupPurserは内部でリニアサーチを行うため、
		優先度の高いヘッダほど先頭に配置する。

		$ : Controller
		# : Interpreter
		> : Setter
		< : Getter
	*/
	PLEN2::CharGroupPurser header_purser("$#><");

	/*!
		@attention
		コマンドを増やす場合、必ずソートした状態で配列に与える。
		(StringGroupPurserは内部でバイナリサーチを行うため。)
	*/
	const char* CONTROLLER_SYMBOL[] = {
		"AD", // ANGLE DIFF
		"AN", // ANGLE
		"HP", // HOME POSITION
		"MP", // PLAY MOTION, @todo "PM"に変更する
		"MS"  // STOP MOTION, @todo "SM"に変更する
	};
	const int CONTROLLER_SYMBOL_LENGTH = sizeof(CONTROLLER_SYMBOL) / sizeof(CONTROLLER_SYMBOL[0]);
	PLEN2::StringGroupPurser controller_purser(CONTROLLER_SYMBOL, CONTROLLER_SYMBOL_LENGTH);

	const char* INTERPRETER_SYMBOL[] = {
		"RI", // RESET INTERPRETER
		"SF"  // SET FUNCTION
	};
	const int INTERPRETER_SYMBOL_LENGTH = sizeof(INTERPRETER_SYMBOL) / sizeof(INTERPRETER_SYMBOL[0]);
	PLEN2::StringGroupPurser interpreter_purser(INTERPRETER_SYMBOL, INTERPRETER_SYMBOL_LENGTH);

	const char* SETTER_SYMBOL[] = {
		"HO", // HOME ANGLE
		"JS", // JOINT SETTINGS
		"MA", // MAX ANGLE
		"MF", // MOTION FRAME
		"MH", // MOTION HEADER
		"MI"  // MIN ANGLE
	};
	const int SETTER_SYMBOL_LENGTH = sizeof(SETTER_SYMBOL) / sizeof(SETTER_SYMBOL[0]);
	PLEN2::StringGroupPurser setter_purser(SETTER_SYMBOL, SETTER_SYMBOL_LENGTH);

	const char* GETTER_SYMBOL[] = {
		"JS", // JOINT SETTINGS
		"MO", // MOTION
		"VI"  // VERSION INFORMATION
	};
	const int GETTER_SYMBOL_LENGTH = sizeof(GETTER_SYMBOL) / sizeof(GETTER_SYMBOL[0]);
	PLEN2::StringGroupPurser getter_purser(GETTER_SYMBOL, GETTER_SYMBOL_LENGTH);

	PLEN2::AbstractPurser* command_purser[] = {
		&controller_purser,
		&interpreter_purser,
		&setter_purser,
		&getter_purser
	};


	PLEN2::HexStringPurser args_purser;
}


void PLEN2::PurserCombinator::m_abort()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : PurserCombinator::m_abort()"));
	#endif

	m_state = READY;
	m_buffer.position = 0;
}


PLEN2::PurserCombinator::PurserCombinator()
	: m_state(READY)
{
	m_purser[READY]              = &header_purser;
	m_purser[COMMAND_INCOMING]   = command_purser[0];
	m_purser[ARGUMENTS_INCOMING] = &args_purser;
}


void PLEN2::PurserCombinator::readByte(char byte)
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : PurserCombinator::readByte()"));
	#endif

	m_buffer[position] = byte;
	(++position) &= (LENGTH() - 1);

	m_buffer[position] = '\0';
}


bool PLEN2::PurserCombinator::accept()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : PurserCombinator::accept()"));
	#endif

	return m_purser[m_state].purse(m_buffer.data);
}


void transition()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : PurserCombinator::transition()"));
	#endif
}