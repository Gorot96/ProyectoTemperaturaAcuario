#include "RestClient.h"
#include "ArduinoJson.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino.h>

int test_delay = 1000; //so we don't spam the API
boolean describe_tests = true;

RestClient client = RestClient("192.168.103.186", 8080);

#define STASSID "realme 6i"
#define STAPSK  "wifi0304"
//declariacion de variablaes calidad agua
#define TdsSensorPin A0
#define VREF 3.0 // analog reference voltage(Volt) of the ADC
#define SCOUNT 30 // sum of sample point
int analogBuffer[SCOUNT]; // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 25;

///////////////////////

const char* mqtt_server="192.168.103.186";

WiFiClient espClient;
PubSubClient cliente(espClient);
long lastMsg = 0;
char msg[50];

char* thisTopic=  "topic_1";
char* thisTopic2= "topic_2";

const int pinDatosDQ = 2;
OneWire oneWireObjeto(pinDatosDQ);
DallasTemperature sensorDS18B20(&oneWireObjeto);

void setup_wifi() {
 
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(STASSID);
 
  WiFi.begin(STASSID, STAPSK);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
 
  if ((char)payload[0] == '0') {
    digitalWrite(14, LOW);
    Serial.println("calefactor apagado");
  } else {
    digitalWrite(14, HIGH);  // Turn the LED off by making the voltage HIGH
    Serial.println("calefactor encendido");
  }
 
}
void callback2(char* topic2, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic2);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
 
  if ((char)payload[0] == '0') {
    digitalWrite(12, LOW);
    Serial.println("filtro apagado");
  } else {
    digitalWrite(12, HIGH);  // Turn the LED off by making the voltage HIGH
    Serial.println("filtro encendido");
  }
 
}

//Setup
void setup()
{
  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(STASSID);
  //pin D5
  pinMode(14, OUTPUT);
  //pin D6
  pinMode(12, OUTPUT);
  pinMode(TdsSensorPin,INPUT);


  setup_wifi();
  cliente.setServer(mqtt_server,1883);
  cliente.setCallback(callback);
  cliente.setCallback(callback2);


  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Setup!");



// Iniciamos el bus 1-Wire
sensorDS18B20.begin(); 

}


 


void reconnect() {
  while (!cliente.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (cliente.connect("ESP8266Client2")) {
      Serial.println("connected");
      //cliente.publish("casa/despacho/temperatura", "Enviando el primer mensaje");
      cliente.subscribe(thisTopic);
      cliente.subscribe(thisTopic2);
    } else {
      Serial.print("failed, rc=");
      Serial.print(cliente.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


String response;

String serializeBody(/*int idtemperatura,*/ int idSensor, double temperatura /*,long mTemporal*/)
{
  StaticJsonDocument<200> doc;

  // StaticJsonObject allocates memory on the stack, it can be
  // replaced by DynamicJsonDocument which allocates in the heap.
  //
  // DynamicJsonDocument  doc(200);

  // Add values in the document
  //
  //doc["idtemperatura"] = idtemperatura;
  doc["idSensor"] = idSensor;
  doc["temperatura"]= temperatura;
  //doc["Mtemporal"]= mTemporal;
  //doc["temperatura"] = temperatura;
  //doc["Mtemporal"] = Mtemporal;

  // Add an array.
  //
  /*
  JsonArray data = doc.createNestedArray("data");
  data.add(temperatura);
  data.add(Mtemporal);
*/
  // Generate the minified JSON and send it to the Serial port.
  //
  String output;
  serializeJson(doc, output);
  // The above line prints:
  // {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}

  // Start a new line
  Serial.println(output);

  // Generate the prettified JSON and send it to the Serial port.
  //
  //serializeJsonPretty(doc, output);
  // The above line prints:
  // {
  //   "sensor": "gps",
  //   "time": 1351824120,
  //   "data": [
  //     48.756080,
  //     2.302038
  //   ]
  // }
  return output;
}

String serializeBody2( int idSensorcad, double calidadAgua)
{
  StaticJsonDocument<200> doc;

  // StaticJsonObject allocates memory on the stack, it can be
  // replaced by DynamicJsonDocument which allocates in the heap.
  //
  // DynamicJsonDocument  doc(200);

  // Add values in the document
  //
  //doc["idtemperatura"] = idtemperatura;
  doc["idSensorcad"] = idSensorcad;
  doc["calidadAgua"]= calidadAgua;
  

  // Add an array.
  //
 
  // Generate the minified JSON and send it to the Serial port.
  //
  String output;
  serializeJson(doc, output);
  // The above line prints:
  

  // Start a new line
  Serial.println(output);

  
  return output;
}

void test_status(int statusCode)
{
  delay(test_delay);
  if (statusCode == 200 || statusCode == 201)
  {
    Serial.print("TEST RESULT: ok (");
    Serial.print(statusCode);
    Serial.println(")");
  }
  else
  {
    Serial.print("TEST RESULT: fail (");
    Serial.print(statusCode);
    Serial.println(")");
  }
}

void deserializeBody(String responseJson){
  if (responseJson != "")
  {
    StaticJsonDocument<200> doc;

    //char json[] =
    //    "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, responseJson);

    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    // Fetch values.
    //
    // Most of the time, you can rely on the implicit casts.
    // In other case, you can do doc["time"].as<long>();
   // int idtemperatura = doc["idtemperatura"];
    int idSensor = doc["idSensor"];
    double temperaturadat = doc["temperaturadat"];
   // long mTemporal = doc["mTemporal"];

    // Print values.
    //Serial.println(idtemperatura);
    Serial.println(idSensor);
    Serial.println(temperaturadat);
    //Serial.println(mTemporal);

    
  }
}

void deserializeBody2(String responseJson){
  if (responseJson != "")
  {
    StaticJsonDocument<200> doc;

    //char json[] =
    //    "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, responseJson);

    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    // Fetch values.
    //
    // Most of the time, you can rely on the implicit casts.
    // In other case, you can do doc["time"].as<long>();
   
    int idSensorcad = doc["idSensorcad"];
    double calidadAgua = doc["calidadAgua"];
    // Print values.
    Serial.println(idSensorcad);
    Serial.println(calidadAgua);
  }
}

void test_response()
{
  Serial.println("TEST RESULT: (response body = " + response + ")");
  response = "";
}

void describe(char *description)
{
  if (describe_tests)
    Serial.println(description);
}

void GET_tests()
{
  describe("Test GET with path");
  test_status(client.get("/api/temperatura/:id", &response));
  test_response();

  describe("Test GET with path and response");
  test_status(client.get("/api/temperatura/nombre_-1809557539", &response));
  test_response();

  
}


void POST_tem()
{
  String post_body = serializeBody( 1, sensorDS18B20.getTempCByIndex(0));
  describe("prueva de post tabla addsensorTemperatura");
  test_status(client.post("/api/addsensorTemp", post_body.c_str(), &response));
  test_response();
}



// funciones para manejo de sensores

///////////////////////////////// sensor temperatura///////////////////////////////////
void SenTemp()
{

  // Mandamos comandos para toma de temperatura a los sensores
    Serial.println("Mandando comandos a los sensores");
    sensorDS18B20.requestTemperatures();
    

}

//////////////////////////////////////// sensor calidad agua ////////////////////////////
void POST_cad()
{
  String post_body = serializeBody2( 1, tdsValue);
  describe("prueva de post tabla addsensorCalidad");
  test_status(client.post("/api/addsensorCad", post_body.c_str(), &response));
  test_response();
}

// funcion para calidadAgua
int getMedianNum(int bArray[], int iFilterLen)
{
int bTab[iFilterLen];
for (byte i = 0; i<iFilterLen; i++)
bTab[i] = bArray[i];
int i, j, bTemp;
for (j = 0; j < iFilterLen - 1; j++)
{
for (i = 0; i < iFilterLen - j - 1; i++)
{
if (bTab[i] > bTab[i + 1])
{
bTemp = bTab[i];
bTab[i] = bTab[i + 1];
bTab[i + 1] = bTemp;
}
}
}
if ((iFilterLen & 1) > 0)
bTemp = bTab[(iFilterLen - 1) / 2];
else
bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
return bTemp;
}

//////////////////////
void SendCad()
{
static unsigned long analogSampleTimepoint = millis();
if(millis()-analogSampleTimepoint > 40U) //every 40 milliseconds,read the analog value from the ADC
{
analogSampleTimepoint = millis();
analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin); //read the analog value and store into the buffer
analogBufferIndex++;
if(analogBufferIndex == SCOUNT)
analogBufferIndex = 0;
}
static unsigned long printTimepoint = millis();
if(millis()-printTimepoint > 800U)
{
printTimepoint = millis();
for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF/ 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
float compensationCoefficient=1.0+0.02*(temperature-25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
float compensationVolatge=averageVoltage/compensationCoefficient; //temperature compensation
tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
Serial.println(tdsValue);

}
}



/////////////////////////////////////////// loop ("main")////////////////////////////////////////////
void loop()
{
  //GET_tests();
  //POST_tem();
  POST_cad();
  SendCad();
  //SenTemp();

  
  if (!cliente.connected()) {
    reconnect();
  }
  cliente.loop();
 
  long now = millis();
  
  
}
