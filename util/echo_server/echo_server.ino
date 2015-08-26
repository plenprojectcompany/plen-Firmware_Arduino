void setup()
{
	Serial.begin(2000000);
	Serial1.begin(2000000);
}

void loop()
{
	if (Serial1.available())
	{
		Serial.write(Serial1.read());
	}

	if (Serial.available())
	{
		Serial.write(Serial.read());
	}
}