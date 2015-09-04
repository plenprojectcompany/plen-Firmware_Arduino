void setup()
{
	Serial.begin(2000000);

	while (!Serial); // Arduino Micro only.

	Serial.print(F("char  : "));
	Serial.print(sizeof(char));
	Serial.println(F(" byte"));

	Serial.print(F("int   : "));
	Serial.print(sizeof(int));
	Serial.println(F(" byte"));

	Serial.print(F("short : "));
	Serial.print(sizeof(short));
	Serial.println(F(" byte"));

	Serial.print(F("long  : "));
	Serial.print(sizeof(long));
	Serial.println(F(" byte"));

	Serial.print(F("float : "));
	Serial.print(sizeof(float));
	Serial.println(F(" byte"));
}


void loop()
{
	// noop.
}
