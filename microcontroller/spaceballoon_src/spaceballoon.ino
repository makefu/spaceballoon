#include <DHT22.h>
// Only used for sprintf
#include <stdio.h>
// Data wire is plugged into port 7 on the Arduino
// 
//
// 
//
#define DHT22_PIN 2
DHT22 myDHT22(DHT22_PIN);

// https://www.arduino.cc/en/Tutorial/Datalogger
#include <SPI.h>
#include <SD.h>
const int chipSelect = 4;
const uint32_t SAMPLE_INTERVAL_MS = 2000;
uint32_t logTime;


void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("DHT22 Library Demo");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("Card initialized.");
  logTime = micros()/(1000UL*SAMPLE_INTERVAL_MS) + 1;
  logTime *= 1000UL*SAMPLE_INTERVAL_MS;
}

void loop(void)
{
  DHT22_ERROR_t errorCode;
  // The sensor can only be read from every 1-2s, and requires a minimum
  // 2s warm-up after power-on.
  logTime += 1000UL*SAMPLE_INTERVAL_MS;

  // Wait for log time.
  int32_t diff;
  do {
    diff = micros() - logTime;
  } while (diff < 0);


  String dataString = "S:";
  dataString += logTime;
  dataString += "\n";

  Serial.print("Requesting data...");
  errorCode = myDHT22.readData();
  switch(errorCode)
  {
    case DHT_ERROR_NONE:
      Serial.println("Got Data");
      dataString += "T:";
			dataString += myDHT22.getTemperatureC();
			dataString += ",";
      dataString += myDHT22.getHumidity();
      dataString += "\n";
      break;
    case DHT_ERROR_CHECKSUM:
      Serial.print("check sum error ");
			break;
    case DHT_BUS_HUNG:
      Serial.println("BUS Hung ");
      break;
    case DHT_ERROR_NOT_PRESENT:
      Serial.println("Not Present ");
      break;
    case DHT_ERROR_ACK_TOO_LONG:
      Serial.println("ACK time out ");
      break;
    case DHT_ERROR_SYNC_TIMEOUT:
      Serial.println("Sync Timeout ");
      break;
    case DHT_ERROR_DATA_TIMEOUT:
      Serial.println("Data Timeout ");
      break;
    case DHT_ERROR_TOOQUICK:
      Serial.println("Polled to quick ");
      break;
  }
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  Serial.print(dataString);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println("Wrote Data");
    dataFile.close();
    // print to the serial port too:
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

}
