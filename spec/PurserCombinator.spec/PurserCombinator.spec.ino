#include "System.h"
#include "Purser.h"
#include "PurserCombinator.h"

namespace
{
	PLEN2::System           system;
	PLEN2::PurserCombinator combinator;
}


namespace PLEN2
{
	bool USER_DEFINED_EVENT_HANDLER(PurserCombinator* combinator_ptr)
	{
		switch (combinator_ptr->m_state)
		{
			case PurserCombinator::COMMAND_INCOMING:
			{
				system.USBSerial().print(F("Header : "));
				system.USBSerial().println(combinator_ptr->m_purser[PurserCombinator::READY]->index());
				system.USBSerial().print(F("Command : "));
				system.USBSerial().println(combinator_ptr->m_purser[PurserCombinator::COMMAND_INCOMING]->index());

				combinator_ptr->m_state = PurserCombinator::ARGUMENTS_INCOMING;
				combinator_ptr->m_store_length = 2;

				return false;
			}

			case PurserCombinator::ARGUMENTS_INCOMING:
			{
				combinator_ptr->m_state = PurserCombinator::READY;
				combinator_ptr->m_store_length = 1;

				return false;
			}
		}

		return true;
	}
}


void setup()
{
	// noop.
}

void loop()
{
	if (system.BLESerial().available())
	{
		combinator.readByte(system.BLESerial().read());

		if (combinator.accept())
		{
			if (PLEN2::USER_DEFINED_EVENT_HANDLER(&combinator))
			{
				combinator.transition();
			}
		}
	}
}
