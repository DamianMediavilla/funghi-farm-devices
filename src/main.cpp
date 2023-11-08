#include <Arduino.h>
#include <Ethernet.h>
#include <ArduinoJson.h>
#include "dht_nonblocking.h" 
#include "LiquidCrystal.h"
#include <string.h>

// definitions
#define DHT_SENSOR_TYPE DHT_TYPE_11

static const int DHT_SENSOR_PIN = A5;

int fanPin = A4; //led rojo
int button = 2; //boton pulldown
int buttonFlag;
int ledVerde = A3;
int vista;
float temperature;
float humidity;

byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};
//char server[] = "farm.damianmediavilla.com/api/create";
String host = "farm.damianmediavilla.com";
char charhost[] = "farm.damianmediavilla.com";
String path = "/api/create";
String  method = "POST";
EthernetClient client;
int alarma=0;
unsigned long enviarReporte;
unsigned long min1=30000;
unsigned long min5=300000;
unsigned long min15=900000;


// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement

//ethernet toma :digitales 11, 12 y 13 - 10 y 4
LiquidCrystal lcd(7, 8, 9, 5, 6, 3);
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );

//Functions
static bool measure_environment( float *temperature, float *humidity )
{
  static unsigned long measurement_timestamp = millis( );
  /* Measure once every four seconds. */
  if( millis( ) - measurement_timestamp > 3000ul )
  {
    if( dht_sensor.measure( temperature, humidity ) == true )
    {
      measurement_timestamp = millis( );
      return( true );
    }
  }
  return( false );
}

void setup() {
  vista=0;
  pinMode(fanPin, OUTPUT);
  lcd.begin(16, 2);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // no point in carrying on, so do nothing forevermore:
    while (true) {
      delay(1);
    }
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

// give the Ethernet shield a second to initialize:
  delay(1000);
  
}

void ethernetMaintain(){
  switch (Ethernet.maintain()) {
    case 1:
      //renewed fail
      Serial.println("Error: renewed fail");
      break;

    case 2:
      //renewed success
      Serial.println("Renewed success");
      //print your local IP address:
      Serial.print("My IP address: ");
      Serial.println(Ethernet.localIP());
      break;

    case 3:
      //rebind fail
      Serial.println("Error: rebind fail");
      break;

    case 4:
      //rebind success
      Serial.println("Rebind success");
      //print your local IP address:
      Serial.print("My IP address: ");
      Serial.println(Ethernet.localIP());
      break;

    default:
      //nothing happened
      break;
  }

}

void enviarDatosWeb(int device, float temperature, float humidity, String msg){// connect to web server on port 80:
  Serial.println("+++envDat+++");
  StaticJsonDocument<96> doc;
  Serial.print(F("JsonGenerado: "));
  doc["device"] = String(device);
  doc["temperature"] = String(temperature);
  doc["humidity"] = String(humidity);
  doc["msg"] = msg;
  serializeJson(doc, Serial);
  Serial.println();
  if(client.connect(charhost, 80)) {
    // if connected:
    Serial.println("Conectado para enviar datos");
    // make a HTTP request:
    // send HTTP header
    client.println(method + " " + path + " HTTP/1.1");
    client.println("Host: " + String(host));
    client.println("Content-Type: application/json");
    //client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: close");
    client.print(F("Content-Length: ")); //en estas dos lineas medimos el doc
    client.println(measureJsonPretty(doc));

    client.println(); // end HTTP header
    //imprime el archivo json
    serializeJsonPretty(doc, client);

    //Inicia la respuesta del servidor
    Serial.println("Respuesta del server: " + host) ;
    while(client.connected()) {
      if(client.available()){
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        Serial.print(c);
      }
    }

    //´prueba impresion
  int len = client.available();
  if (len > 0) {
    byte buffer[120];
    if (len > 120) len = 120;
    client.read(buffer, len);
    if (true) {
      Serial.write(buffer, len); // show in the serial monitor (slows some boards)
    }
    byteCount = byteCount + len;
  } //fin rpueba impresion
  

    // the server's disconnected, stop the client:
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {// if not connected:
    Serial.println("connection failed");
  }

    Serial.println("---envDat---");

}


void loop( )
{
  ethernetMaintain();
  
  //Measure temperature and humidity.  If the functions returns true, then a measurement is available. 
  if( measure_environment( &temperature, &humidity ) == true )
  {
    Serial.print( "T = " );
    Serial.print( temperature, 1 );
    Serial.print( " deg. C, H = " );
    Serial.print( humidity, 1 );
    Serial.println( "%" );
    lcd.setCursor(0, 1);
    lcd.print( "T=" );
    lcd.print( temperature, 1 );
    lcd.print( "C H=" );
    lcd.print( humidity, 1 );
    lcd.println( "% " );
   
   
  }
  //if (client.connect(server, 80)) {
    
  if(millis()-enviarReporte>min15){
    //reporte de rutina 
    enviarDatosWeb(2,temperature,humidity,"normal");
    enviarReporte=millis();
    alarma=0;
  }
  //reporte de alarma
  if(alarma==0 && humidity>70){
    enviarDatosWeb(2,temperature,humidity,"alarma humedad");
    alarma=1;

  }

  // lcd.print(millis() / 1000.0);
  if(digitalRead(button)==HIGH){
    if(buttonFlag==0){
    vista++;
    buttonFlag=1;
    digitalWrite(ledVerde, HIGH);
    };
  }else{
    if(buttonFlag==1){
    buttonFlag=0;
    digitalWrite(ledVerde, LOW);
    }
  }

  if (humidity > 60){
    digitalWrite(fanPin, HIGH);
  }else{
    digitalWrite(fanPin, LOW);
  }
  

  //Vistas del displayLCD
  vista>4?vista=0:vista=vista;
  switch (vista){
    case 1:
      //Vista 1 
      lcd.setCursor(0, 0);
      lcd.print("P.Eryngii -C11  ");
      break;
    case 2:
      //Vista 2 
      lcd.setCursor(0, 0);
      lcd.print("P.Ostreatus -C11");
      break;
    case 3:
      //Vista 3 
      lcd.setCursor(0, 0);
      lcd.print("P.Eryngii -C88  ");
      break;
    case 4:
      //Vista 4 
      lcd.setCursor(0, 0);
      lcd.print("P.Eryngii -C391 " );
      break;
    
    default:
      lcd.setCursor(0, 0);
      lcd.print("vista defalut   ");
      break;
  }

}