//
//    FILE: calibrado_hx711.ino
//  AUTHOR: Felix Moli Grao
// PURPOSE: HX711 demo
//     URL: https://github.com/albalate/hxt11_2022
#include "arduino.h"
#include "HX711.h"
#include <EEPROM.h>
int eeAddress = 0;
#define EEPROM_SIZE 100

HX711 balanza;

#ifdef ESP32
byte buttonTare = 15;     // conect to gnd for tare 4
byte buttonCalibrate = 2; // conect to gnd for calibrate 5
byte ver = 0;             // conect to gnd for send state to serial 5
uint8_t Data = 19;        //for esp32
uint8_t Clock = 18;       //for esp32
#else
byte buttonTare = 3;      // conect to gnd for tare
byte buttonCalibrate = 2; // conect to gnd for for calibrate
byte ver = 4;             // conect to gnd for for send state to serial
uint8_t Data = 6;
uint8_t Clock = 7;
#endif

struct Bascula
{
    float scala;
    long offSet;
};

Bascula bascula;

void SaveStruct(int eeAddress, Bascula bascula)
{
    EEPROM.put(eeAddress, bascula);
#ifdef ESP32
    EEPROM.commit();
#endif
    Serial.println("Save custom object to EEPROM: ");
    Serial.print("guardando scala: ");
    Serial.println(bascula.scala);
    Serial.print("guardando offset: ");
    Serial.println(bascula.offSet);
    delay(8000);
}

Bascula LoadStruct(int eeAddress)
{
    EEPROM.get(eeAddress, bascula);
    Serial.println("Read custom object from EEPROM: ");
    Serial.print("Cargando scale: ");
    Serial.println(bascula.scala);
    Serial.print("cargando offset: ");
    Serial.println(bascula.offSet);
    delay(8000);
    return bascula;
}

void calibrate()
{
    Serial.println("\nclean the scale, press a key to continue");
    while (!Serial.available());
    while (Serial.available())  Serial.read();
    balanza.set_scale();
    balanza.tare();
    Serial.println("\nPut 1000 gr in scale, press a key to continue");
    while (!Serial.available());
    while (Serial.available()) Serial.read();
    balanza.set_scale(balanza.get_units(15) / 1000);
    balanza.set_offset(balanza.get_value(15));
    Serial.println("\nScale is calibrated,clean the scale Scale is calibrated, press a key to continue");
    balanza.tare(15);

    bascula.scala = balanza.get_scale();
    bascula.offSet = balanza.get_offset();
    SaveStruct(0, bascula); //Save to eeprom
}
void setup()
{
    Serial.begin(115200);
    balanza.begin(Data, Clock);
#ifdef ESP32
    if (!EEPROM.begin(1000))
    {
        Serial.println("Failed to initialise EEPROM");
        Serial.println("Restarting...");
        delay(1000);
        ESP.restart();
    }
#endif

    bascula = LoadStruct(0); //load off eeprom
    balanza.set_scale(bascula.scala);
    balanza.set_offset(bascula.offSet);
    Serial.println("Iniciando Scala:  " + String(balanza.get_scale()));
    Serial.println("Iniciando Offset:  " + String(balanza.get_offset()));
    if ((bascula.scala == 0.00) || (bascula.offSet == 0))
    {
        calibrate();
    }
    else
    {
        Serial.println("poniendo tara");
        balanza.tare(25);
    }
    pinMode(buttonTare, INPUT_PULLUP);
    pinMode(buttonCalibrate, INPUT_PULLUP);
    pinMode(ver, INPUT_PULLUP);
}

void loop()
{
    if (digitalRead(buttonTare) == false)
    {

        Serial.println("Scala:  " + String(balanza.get_scale()));
        Serial.println("Offset:  " + String(balanza.get_offset()));
        delay(2000);
        balanza.tare(25);
    }

    if (digitalRead(buttonCalibrate) == false)
    {
        calibrate();
        delay(500);
    }

    if (digitalRead(ver) == false)
    {
        LoadStruct(0);
        delay(2500);
    }

    Serial.print("UNITS: ");
    Serial.println(balanza.get_units(15));
    delay(250);
}
