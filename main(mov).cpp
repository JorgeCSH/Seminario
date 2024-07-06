#include <Arduino.h>
#include <StarterKitNB.h>
//===================================
// Conexiones iniciales             #
//===================================
//Definiciones (copiado)
#define PIN_VBAT WB_A0
#define VBAT_MV_PER_LSB (0.73242188F)
#define VBAT_DIVIDER_COMP (1.73)
#define REAL_VBAT_MV_PER_LSB (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)
#define WIRE_PORT   Wire
#define WIRE_SPEED  800000
StarterKitNB sk;    // Para el starter kit
//Datos de conexion entel
String apn = "m2m.entel.cl";
String user = "entelpcs";
String password = "entelpcs";
String band = "B28 LTE"; // -|---> los copie  literal
String Network = "NB";   // -|

// Datos del Adafruit
String clientId = "peru";
String userBroker = "Benja_F";
String passwordBroker = "aio_tmgC77IaI2Fmx7x9jYXz5F4k4PCx";
String broker = "io.adafruit.com";
int port = 1883;

// Direccion de los topicos en adafruit
String topic1 = "Benja_F/feeds/test 1/json";
String topic2 = "Benja_F/feeds/test 2/json";
String topic3 = "Benja_F/feeds/bateria/json";
float batery;  



#include <Wire.h>
#define SENSOR_PIN  WB_IO6   // Attach AM312 sensor to Arduino Digital Pin WB_IO6

int gCurrentStatus = 0;         // variable for reading the pin current status
int gLastStatus = 0;            // variable for reading the pin last status


// Me parece que es el inicio del programa como tal #####################
void setup() 
{
   sk.Setup();
   delay(500);
   sk.UserAPN(apn, user, password);
   sk.Connect(apn, band, Network);  
   pinMode(SENSOR_PIN, INPUT);   // The Water Sensor is an Input
   pinMode(LED_GREEN, OUTPUT);  // The LED is an Output
   pinMode(LED_BLUE, OUTPUT);   // The LED is an Output   
   Serial.begin(115200);
   time_t timeout = millis();
   while (!Serial)
   {
     if ((millis() - timeout) < 5000)
     {
       delay(100);
     }
     else
     {
       break;
     }
   } 
   Serial.println("========================");
   Serial.println("    RAK12006 test");
   Serial.println("========================");
}
//###############################################################

void loop() {                                       // Este loop toma el estatus de "corriente" que arroja el sensor y lo compara con el actual
                                                   // Si son distintos => hay cambio en el movimiento (estado inicial = 0)
  gCurrentStatus = digitalRead(SENSOR_PIN);        
  if(gLastStatus != gCurrentStatus)                 
  {
    if(gCurrentStatus == 0)
    {//0: detected   1: not detected
      Serial.println("IR detected ...");
     digitalWrite(LED_GREEN,HIGH);   //turn on
     digitalWrite(LED_BLUE,HIGH);
    }
    else
    {
      Serial.println("IR not detected ... yet jejeje");
      digitalWrite(LED_GREEN,LOW);
      digitalWrite(LED_BLUE,LOW);   // turn LED OF
    }   
    gLastStatus = gCurrentStatus;
  }
  else
  {
    delay(100);
  }
  if (!sk.ConnectionStatus())                       // Si no hay conexión, se reconecta
  {
    sk.Reconnect(apn, band, Network);
    delay(2000);
  }
  batery = (analogRead(PIN_VBAT) * REAL_VBAT_MV_PER_LSB /1000); // Sensor de bateria
  if (!sk.LastMessageStatus)  // Se conecta al broker, en este caso, adafruit
  {
    sk.ConnectBroker(clientId, userBroker, passwordBroker, 0, broker, port);
    delay(2000);
  }
  sk.SendMessage(String(gCurrentStatus), topic1, 0, 0, 0, 0, 10000);    // Envia "Movimiento"
  sk.SendMessage(String(batery), topic3, 0, 0, 0, 0, 10000);    // Envia bateria
  delay(2000);
}
