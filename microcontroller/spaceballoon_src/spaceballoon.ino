#include <DHT22.h>
// Only used for sprintf
#include <stdio.h>


// PINs in use on PRO Micro

// 18(A0)   -> SD:CS
// 14(MISO) -> SD:MOSI
// 16(MOSI) -> SD:MISO
// 13(SCLK) -> SD:CLK
// 2        -> DHT:DATA
//

#define DHT22_PIN 19
#define CS_PIN 18


DHT22 myDHT22(DHT22_PIN);

// https://www.arduino.cc/en/Tutorial/Datalogger

#include <SPI.h>
#include <SD.h>
const uint32_t SAMPLE_INTERVAL_MS = 5000;
uint32_t logTime;

// software serial
//#include <Adafruit_GPS.h>
//#include <SoftwareSerial.h>


void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("DHT22 Library Demo");
  if (!SD.begin(CS_PIN)) {
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


  String dataString = "";
  dataString += logTime/1000;
  dataString += ",";

  Serial.print("Requesting data...");
  errorCode = myDHT22.readData();
  switch(errorCode)
  {
    case DHT_ERROR_NONE:
      Serial.println("Got Data");
			dataString += myDHT22.getTemperatureC();
			dataString += ",";
      dataString += myDHT22.getHumidity();
      dataString += ",";
      break;
    case DHT_ERROR_CHECKSUM:
      Serial.print("check sum error ");
			dataString += ",,";
			break;
    case DHT_BUS_HUNG:
      Serial.println("BUS Hung ");
			dataString += ",,";
      break;
    case DHT_ERROR_NOT_PRESENT:
      Serial.println("Not Present ");
			dataString += ",,";
      break;
    case DHT_ERROR_ACK_TOO_LONG:
      Serial.println("ACK time out ");
			dataString += ",,";
      break;
    case DHT_ERROR_SYNC_TIMEOUT:
      Serial.println("Sync Timeout ");
			dataString += ",,";
      break;
    case DHT_ERROR_DATA_TIMEOUT:
      Serial.println("Data Timeout ");
			dataString += ",,";
      break;
    case DHT_ERROR_TOOQUICK:
      Serial.println("Polled to quick ");
			dataString += ",,";
      break;
  }

  dataString += "\n";
  Serial.print(dataString);

  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}
