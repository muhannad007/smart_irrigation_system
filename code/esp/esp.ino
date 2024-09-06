#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "La Maison";
const char* password = "lamaison99";
const char* sendValues = "http://192.168.1.108:80/irrigation/update_sensor_values.php";
const char* receiveOrders = "http://192.168.1.108:80/irrigation/get_order.php";
const char* water_result =  "http://192.168.1.108:80/irrigation/get_water_result.php";
const char* soil_result =  "http://192.168.1.108:80/irrigation/get_soil_result.php";
const String postData = "soil_moisture=1&temp=1&humid=1&nitro=1&phos=1&potas=1&user_id=4"; // Data to be sent in URL-encoded format

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
unsigned long lastTime = 0;
// Timer set to 10 seconds (10000)
unsigned long timerDelay = 1000;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");

  // Random seed is a number used to initialize a pseudorandom number generator
  randomSeed(analogRead(0));
}

void loop() {
  // Send an HTTP POST request every 10 seconds

  String vals = Serial.readString();
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    String value = String(Serial.read());
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      HTTPClient http_2;
      HTTPClient http_water;
      HTTPClient http_soil;
      String payload_1 = "{}";
      String payload_2 = "{}";
      String payload_water = "{}";
      String payload_soil = "{}";

      // std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

      // // Ignore SSL certificate validation (with host ssl)
      // client->setInsecure();

      http.begin(client, sendValues);
      http_2.begin(client, receiveOrders);
      http_water.begin(client, water_result);
      http_soil.begin(client, soil_result);

    // Set headers
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      http_2.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // http_water.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // http_soil.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
      // JSON data to send with HTTP POST
      String httpRequestData = postData;
      // String httpRequestData = vals + "&user_id = 1";
      String httpRequestData_2 = "user_id=1";
      // String httpRequestData_water = "user_id = 4";
      // String httpRequestData_soil;
      
      int httpResponseCode = http.POST(httpRequestData);
      int httpResponseCode_2 = http_2.POST(httpRequestData_2);

      payload_1 = http.getString();
      // Serial.print(payload_1);
      // Serial.print("  ");

      payload_2 = http_2.getString();
      // Serial.print(payload_2.substring(0,1));
      // Serial.print("  ");

      if (payload_2 == "0, 0") Serial.write(payload_2.c_str());
      else if (payload_2 == "0, 1") Serial.write(payload_2.c_str());
      else if (payload_2 == "1, 0") Serial.write(payload_2.c_str());
      else if (payload_2 == "1, 1") Serial.write(payload_2.c_str());
      else {
        if (payload_2.substring(0, 1) == "2") {
          int httpResponse_water = http_water.GET();
          payload_water = http_water.getString();
          String total = payload_2 + ", " +payload_water;
          Serial.write(total.c_str());
        }
        else if (payload_2.substring(3) == "2") {
          int httpResponse_soil = http_soil.GET();
          payload_soil = http_soil.getString();
          String total = payload_2 + ", " + payload_soil;
          Serial.write(total.c_str());
        }
        else {
          int httpResponse_water = http_water.GET();
          int httpResponse_soil = http_soil.GET();
          payload_water = http_water.getString();
          payload_soil = http_soil.getString();
          String total = payload_2 + ", " + payload_water + ", " +payload_soil;
          Serial.write(total.c_str());
        }
      }

      http.end();
      http_2.end();
      http_water.end();
      http_soil.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}






// #include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>
// #include <WiFiClient.h>

// const char* ssid = "La Maison";
// const char* password = "lamaison99";
// const char* sendValues = "http://192.168.1.108:80/irrigation/get_water_result.php";

// unsigned long lastTime = 0;
// unsigned long timerDelay = 1000;

// void setup() {
//   Serial.begin(115200); 

//   WiFi.begin(ssid, password);
//   Serial.println("Connecting");
//   while(WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("");
//   Serial.print("Connected to WiFi network with IP Address: ");
//   Serial.println(WiFi.localIP());
 
//   Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
// }


// void loop() {
//   // Send an HTTP POST request depending on timerDelay
//   if ((millis() - lastTime) > timerDelay) {
//     //Check WiFi connection status
//     if(WiFi.status()== WL_CONNECTED){
//       WiFiClient client;
//       HTTPClient http;
      
//       // Your Domain name with URL path or IP address with path
//       http.begin(client, sendValues);
//       int httpResponseCode = http.GET();
      
//       if (httpResponseCode>0) {
//         Serial.print("HTTP Response code: ");
//         Serial.println(httpResponseCode);
//         String payload = http.getString();
//         Serial.println(payload);
//       }
//       else {
//         Serial.print("Error code: ");
//         Serial.println(httpResponseCode);
//       }
//       // Free resources
//       http.end();
//     }
//     else {
//       Serial.println("WiFi Disconnected");
//     }
//     lastTime = millis();
//   }
// }