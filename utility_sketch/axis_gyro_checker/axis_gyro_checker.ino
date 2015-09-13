void setup()
{
	Serial.begin(2000000);
	Serial1.begin(2000000);
}

void loop()
{
	if (Serial.available())
	{
    Serial.read();
		Serial1.write('>');
	}

  if (Serial1.available())
  {
    Serial.print((int)Serial1.read(), HEX);
    Serial.println("");
  }
}
