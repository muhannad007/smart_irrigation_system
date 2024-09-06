#include <dht.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define dataPin 11 // Defines pin number to which the sensor is connected
#define RE 8
#define DE 7

const byte nitro[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};
byte values[11];

dht DHT; // Creats a DHT object
SoftwareSerial mod(2, 3);

int pump1 = 13;
int pump2 = 12;

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  mod.begin(9600);
  pinMode(pump1, OUTPUT);
  pinMode(pump2, OUTPUT);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  delay(500);
}

// the loop routine runs over and over again forever:
void loop() {
  byte val1,val2,val3;
  val1 = nitrogen();
  delay(250);
  val2 = phosphorous();
  delay(250);
  val3 = potassium();
  delay(250);
  int readData = DHT.read22(dataPin); // DHT22/AM2302

  float t = DHT.temperature; // Gets the values of the temperature
  float h = DHT.humidity; // Gets the values of the humidity

  // read the input on analog pin 0:
  int soilMoisture = analogRead(A0);
  int moist = map(soilMoisture, 1000, 0, 0, 100);

  float nitro = val1 * 10;
  float phos = val2 * 10;
  float potas = val3 * 10;
  float nuts_sum = (val1 + val2 + val3) / 3;
  float nuts = map(nuts_sum, 0, 255, 0, 100);

  String str = String(moist)+"&"+String(t)+"&"+String(h)+"&"+String(nitro)+"&"+String(phos)+"&"+String(potas);
  Serial.write(str.c_str());

  String order = Serial.readString();
  if (order == "1, 1") {
      digitalWrite(pump1, HIGH);
      digitalWrite(pump2, HIGH);
    } else if (order == "1, 0") {
      digitalWrite(pump1, HIGH);
      digitalWrite(pump2, LOW);
    } else if (order == "0, 1") {
      digitalWrite(pump1, LOW);
      digitalWrite(pump2, HIGH);
    } else if (order == "0, 0") {
      digitalWrite(pump1, LOW);
      digitalWrite(pump2, LOW);
    } else {
      if (order.substring(0, 4) == "2, 1" || order.substring(0, 4) == "2, 0") {
        String water_refstr = order.substring(6, 8);
        float water_ref = water_refstr.toFloat();
        if (water_ref < moist) {
          digitalWrite(pump1, HIGH);
        } else {
          digitalWrite(pump1, LOW);
        }
      } else if (order.substring(0, 4) == "1, 2" || order.substring(0, 4) == "0, 2") {
        String soil_refstr = order.substring(6, 8);
        float soil_ref = soil_refstr.toFloat();
        if (soil_ref < nuts) {
          digitalWrite(pump2, HIGH);
        } else {
          digitalWrite(pump2, LOW);
        }     
      } else if (order.substring(0, 4) == "2, 2") {
        String water_refstr = order.substring(6, 8);
        String soil_refstr = order.substring(10, 12);
        float water_ref = water_refstr.toFloat();
        float soil_ref = soil_refstr.toFloat();
        if (soil_ref < nuts && water_ref >= moist) {
          digitalWrite(pump1, HIGH);
          digitalWrite(pump2, LOW);
        } else if (soil_ref > nuts && water_ref < moist) {
          digitalWrite(pump1, LOW);
          digitalWrite(pump2, HIGH);
        } else if (soil_ref < nuts && water_ref < moist) {
          digitalWrite(pump1, HIGH);
          digitalWrite(pump2, HIGH);
        } else {
          digitalWrite(pump1, LOW);
          digitalWrite(pump2, LOW);
        }
      }
    }

  delay(1000);        // delay in between reads for stability
}


byte nitrogen(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(nitro,sizeof(nitro))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
      //Serial.print(mod.read(),HEX);
      values[i] = mod.read();
//      Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
 
byte phosphorous(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(phos,sizeof(phos))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
      //Serial.print(mod.read(),HEX);
      values[i] = mod.read();
//      Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
 
byte potassium(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(mod.write(pota,sizeof(pota))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
      //Serial.print(mod.read(),HEX);
      values[i] = mod.read();
//      Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
