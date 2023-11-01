#include <Arduino.h>
#include "dht_nonblocking.h"
#include "LiquidCrystal.h"

// definitions
#define DHT_SENSOR_TYPE DHT_TYPE_11

static const int DHT_SENSOR_PIN = 2;

int fanPin = 3; //led rojo
int button = 4; //boton pulldown
int buttonFlag;
int ledVerde = 5;
int vista;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
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
 // lcd.print("P.Ostreatus -C23");
  //lcd.print("P.Eryngii -C17");
  lcd.print("P.Djamor -C08");
  Serial.begin(9600);
}

void loop( )
{
  float temperature;
  float humidity;
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
  // lcd.setCursor(0, 1);
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
  
  vista>4?vista=0:vista=vista;
  
  switch (vista)
  {
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