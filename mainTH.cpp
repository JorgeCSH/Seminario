#include <Arduino.h>
#include <StarterKitNB.h>
#include <SparkFun_SHTC3.h>
// ####################################################################################################################
// Conexiones iniciales ###############################################################################################
// ####################################################################################################################

//Definiciones (copiado)
#define PIN_VBAT WB_A0
#define VBAT_MV_PER_LSB (0.73242188F)
#define VBAT_DIVIDER_COMP (1.73)
#define REAL_VBAT_MV_PER_LSB (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)
#define WIRE_PORT   Wire
#define WIRE_SPEED  800000

// Objetos (creo, debo repasar OOP...atte: posible dcc)
StarterKitNB sk;    // Para el starter kit
SHTC3 mySHTC3;    // objetos de temperatura y calor

// Mensajes, este mensaje es el que aparecia en el video de ejemplo
//String temperatura = "28.1";
//String humedad = "44.8";
//String msg = "{\"temperatura\": "+temperatura+", \"humedad\": "+humedad+"}";

//Datos de conexion entel
String apn = "m2m.entel.cl";
String user = "entelpcs";
String password = "entelpcs";
String band = "B28 LTE"; // -|---> los copie  literal
String Network = "NB";   // -|

// Datos del Adafruit
String clientId = "leonardo";
String userBroker = "jorgechs";
String passwordBroker = "aio_rwyw70KVB0OowBXN7QXUNC1YToKb";
String broker = "io.adafruit.com";
int port = 1883;

// Direccion de los topicos en adafruit
String topic1 = "jorgechs/feeds/test 1/json";
String topic2 = "jorgechs/feeds/test 2/json";
String topic3 = "jorgechs/feeds/bateria/json";
float batery;

// ####################################################################################################################
// Codigo ## ##########################################################################################################
// ####################################################################################################################

// INICIO Exclusivo: sensor temperatura y calor -----------------------------------------------------------------------
// Definimos los valores que nos importan
String hum = "";    // humedad
String temp = "";   // temperatura

// Funcion que obtiene los resultados, error es porque si hay un problema intenta ver cual
void errorDecoder(SHTC3_Status_TypeDef message)
{
  switch(message)                                                   // --|
  {                                                                 //   |
    case SHTC3_Status_Nominal : Serial.print("Nominal"); break;     //   |
    case SHTC3_Status_Error : Serial.print("Error"); break;         //   |---> Es claro que yo no hice esta funcion xd
    case SHTC3_Status_CRC_Fail : Serial.print("CRC Fail"); break;   //   |
    default : Serial.print("Unknown return code"); break;           //   |
  }                                                                 // --|
}

// Funncion para seleccionar la informacion que queremos enviar\printear (?)
void printInfo()
{
  if(mySHTC3.lastStatus == SHTC3_Status_Nominal)    // Selecciona el ultimo valor registrado
  {
    Serial.print("RH = ");    // Para enviar dato humedad
    Serial.print(mySHTC3.toPercent());    // El sensor puede medir la humedad como porcentaje, aca se selecciona
    Serial.print("%, T = ");
    Serial.print(mySHTC3.toDegC());   // Para seleccionar la unidad de medida de temperatura, C = celsius
    Serial.println(" deg C");
  }
  else
  {
    Serial.print("Update failed, error: ");   // Este es el caso de que no detecte nada, eso significa que fallo
    errorDecoder(mySHTC3.lastStatus);   // Fallo => funcion error
    Serial.println();
  }
}
// FIN Exclusivo: sensor temperatura y calor --------------------------------------------------------------------------


// Setup del sistema
void setup()
{
  //primer set up, para el arduino como tal, sk = starter kit
  sk.Setup();
  delay(500);
  sk.UserAPN(apn, user, password);
  sk.Connect(apn, band, Network);

  //Serial, es decir "permite interactuar con el arduino" (creo)
  Serial.begin(115200);   // Iniciar, importante, no se que es el numero xd
  while(Serial == false){};   // Esperar a la conexion a iniciar
  Serial.println("SHTC3 Example 1 - Basic Readings");   // CASO SENSOR T H, printear
  Wire.begin();   // Iniciamos wire por si algun sensor hace uso de el
  Serial.print("Beginning sensor. Result = ");    // Esatus de la variable ejecutada
  errorDecoder(mySHTC3.begin());    // Se enciende el sensor
  Serial.println();                                                       //--|
  Serial.println("\n\n");                                                 //  |--> printear lineas y sigue ejecutando despues de 5 segs
  Serial.println("Waiting for 5 seconds so you can read this info ^^^");  //--|
  delay(5000);
}


// Instrucciones que estan ejecutandose constantemente
void loop()
{
  // Iniciamos conexion al starter kit (es como la llave de encendido xd)
  if (!sk.ConnectionStatus()) // Si no hay conexion a Narrow Band
    {
        sk.Reconnect(apn, band, Network);  // Se intenta reconecta
        delay(2000);
  }

  //Sensor de humedad y temperatura
  SHTC3_Status_TypeDef result = mySHTC3.update();   // Va actualizando en un loop la la info del sensor
  printInfo();    // Funcion usada para printear una linda linea de datos
  delay(190);

  //Aca enviamos la informacion que se va midiendo
  hum = mySHTC3.toPercent();    // Definimos humedad como el valor detectado por el medidor
  temp =mySHTC3.toDegC();   // Definimos temperatura como el valor detectado por el medidor
  batery = (analogRead(PIN_VBAT) * REAL_VBAT_MV_PER_LSB /1000);   // Sensor de bateria
  Serial.println(batery);   // Se envia el valor de la bateria
  if (!sk.LastMessageStatus)  // Se conecta al broker, en este caso, adafruit
  {
    sk.ConnectBroker(clientId, userBroker, passwordBroker, 0, broker, port);
    delay(2000);
  }
  sk.SendMessage(temp, topic1, 0, 0, 0, 0, 10000);    // Envia temperatura
  delay(2000);
  sk.SendMessage(hum, topic2, 0, 0, 0, 0, 10000);    // Envia humedad
  delay(2000);
  sk.SendMessage(String(batery), topic3, 0, 0, 0, 0, 10000);    // Envia bateria
  delay(2000);
}




