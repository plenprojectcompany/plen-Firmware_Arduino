/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#include "Arduino.h"
#include "Profiler.h"


namespace Shared
{
	unsigned int m_nest = 0;
}


void Utility::Profiler::m_tabbing()
{
	for (int i = 0; i < Shared::m_nest; i++)
	{
		Serial.write('\t');
	}
}


Utility::Profiler::Profiler(const __FlashStringHelper* fsh_ptr)
{
	m_tabbing();
	Serial.println(F(">>> pushed"));

	m_tabbing();
	Serial.print(F("  | running   : "));
	Serial.println(fsh_ptr);

	Shared::m_nest++;
	m_begin = micros();

	delay(DELAY_MS());
}


Utility::Profiler::~Profiler()
{
	m_end = micros() - m_begin - DELAY_MS();
	Shared::m_nest--;

	m_tabbing();
	Serial.print(F("  | nest      : "));
	Serial.println(Shared::m_nest);

	m_tabbing();
	Serial.print(F("  | exec time : "));
	Serial.println(m_end);

	m_tabbing();
	Serial.println(F("<<< popped"));

	delay(DELAY_MS());
}