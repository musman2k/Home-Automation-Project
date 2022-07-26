#include <WiFi.h>                                                // esp32 library
#include <IOXhop_FirebaseESP32.h>                                             // firebase library
#include <DHT.h>   

#define FIREBASE_HOST "dht-11-21772-default-rtdb.firebaseio.com"                         // the project name address from firebase id
#define FIREBASE_AUTH "e0V3jubyw0SahAfC5VaRUXJ6v2QxdNRGUgZElsVD"                    // the secret key generated from firebase
#define WIFI_SSID ""                                          // input your home or public wifi name
#define WIFI_PASSWORD "karachi021"                                    //password of wifi ssid


String fireStatus1 = "";// led status received from firebase
String fireStatus2 = "";//  Fan status received from firebase
String fireStatus3 = "";// auto fan status received from firebase
String fireStatus4 = "";
int DHTPIN = 27;               // Digital pin connected to DHT11
#define DHTTYPE DHT11         // Initialize dht type as DHT 11
DHT dht(DHTPIN, DHTTYPE);                                                    
int led = 25;                                                               
int fan = 26;
int ledPin = 12; // choose pin for the LED (IR SENSOR)
int inputPin = 13; // choose input pin (for Infrared sensor) 
int val = 0; // variable for reading the pin status
#define POWER_PIN     4 /// Water Sensor
#define SIGNAL_PIN    32   /// Water Sensor
float value = 0;
//----------------------
//Walk through
#define ULTRASONIC_TRIG_PIN1     21   // pin TRIG 
#define ULTRASONIC_ECHO_PIN1     22 // pin ECHO 

#define ULTRASONIC_TRIG_PIN2     16   // pin TRIG 
#define ULTRASONIC_ECHO_PIN2     17 // pin ECHO 

int Red_LED = 22;
int Blue_LED = 23;
//----------------------
void IR_Sensor();
void  LED_FAN();
void  Water_Tank();
void  dht11();
void  walk_through();

void setup() {

  Serial.begin(9600);

  delay(1000);

  pinMode(led, OUTPUT); 
  pinMode(fan, OUTPUT);
  pinMode(ledPin, OUTPUT); // declare LED as output 
  pinMode(inputPin, INPUT); // declare Infrared sensor as input               
  pinMode(POWER_PIN, OUTPUT);   // configure 14 pin as an OUTPUT(Water Sensor)
  digitalWrite(POWER_PIN, LOW); // turn the water sensor OFF
    // ultraonic setup 
  pinMode(ULTRASONIC_TRIG_PIN1, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN1, INPUT);
  pinMode(ULTRASONIC_TRIG_PIN2, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN2, INPUT);
  pinMode (Red_LED, OUTPUT);
  pinMode (Blue_LED, OUTPUT);
  dht.begin();                //reads dht sensor data

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                      //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                                      //print local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.setString("LED_STATUS", "OFF"); //send initial string of led status
  Firebase.setString("FAN_STATUS", "OFF");
  Firebase.setString("AUTO_FAN_STATUS", "OFF");
  Firebase.setString("TANK_STATUS", "Empty");
}

void loop() {

  LED_FAN();
  IR_Sensor();
  Water_Tank();
  dht11();
  walk_through();
  delay(1000);
///--------------------
}

void LED_FAN()
{
  fireStatus1 = Firebase.getString("LED_STATUS");                     // get led status input from firebase
  fireStatus2 = Firebase.getString("FAN_STATUS");                    //  get fan status input from firebase
  

  if (fireStatus1 == "ON") {                         // compare the input of led status received from firebase

    Serial.println("Led Turned ON");                 

    digitalWrite(led, HIGH);                                                         // make output led ON

  }

  else if (fireStatus1 == "OFF") {              // compare the input of led status received from firebase

    Serial.println("Led Turned OFF");

    digitalWrite(led, LOW);                                                         // make output led OFF

  }

  else {

    Serial.println("Wrong Credential! Please send ON/OFF");

  }
  if (fireStatus2 == "ON") {                         // compare the input of fan status received from firebase

    Serial.println("FAN Turned ON");                 

    digitalWrite(fan, HIGH);                                                         // make output fan ON

  }

  else if (fireStatus2 == "OFF") {              // compare the input of fan status received from firebase

    Serial.println("FAN Turned OFF");

      digitalWrite(fan, LOW);                                                         // make output fan OFF

  }
  else {

    Serial.println("Wrong Credential! Please send ON/OFF");

  }
}

///------------------------------------------------------
  ///IR Sensor (Parking)
void IR_Sensor()
  {
  val = digitalRead(inputPin); // read input value 
  String firePark1 = String("Empty");
  String firePark2 = String("Full");
   if (val == HIGH)
   { // check if the input is HIGH
      digitalWrite(ledPin, LOW); // turn LED OFF   
//      Firebase.pushString("/Car Parking", firePark1);
          Firebase.setString ("/Car Parking",firePark1);
      
   } 
   else 
   { 
      digitalWrite(ledPin, HIGH); // turn LED ON 
//      Firebase.pushString("/Car Parking", firePark2);
          Firebase.setString ("/Car Parking",firePark2);
   }
  }



  
  ///dht 11
///====================
void dht11()
{
float h = dht.readHumidity();                                 // Read Humidity
float t = dht.readTemperature();                              // Read temperature
fireStatus3 = Firebase.getString("Auto_FAN_STATUS");                    //  get auto fan status input from firebase
  
  if (isnan(h) || isnan(t))                                     // Checking sensor working
  {                                   
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  } 
  Serial.print("Humidity: ");  
  Serial.print(h);
  String fireHumid = String(h) + String("%");                   //Humidity integer to string conversion
  
  Serial.print("%  Temperature: ");  
  Serial.print(t);  
  Serial.println("°C ");
  String fireTemp = String(t) + String("°C");                  //Temperature integer to string conversion
  delay(5000);
//  Firebase.pushString("/DHT11/Humidity", fireHumid);            //setup path to send Humidity readings
//  Firebase.pushString("/DHT11/Temperature", fireTemp); //setup path to send Temperature readings
  Firebase.setString ("/DHT11/Temperature",fireTemp);
  Firebase.setString ("/DHT11/Humidity",fireHumid);
  
  if(t>28)
  {
    if (fireStatus3 == "ON") {                         // compare the input of fan status received from firebase

    Serial.println("FAN Turned ON");                 

    digitalWrite(fan, HIGH);
    Firebase.setString("FAN_STATUS", "ON");// make output fan ON

  }
  }
  if(h>90)
  {
    
  }
    if (Firebase.failed()) 
    {
 
      Serial.print("pushing /logs failed:");
      Serial.println(Firebase.error()); 
      return;
  }
}



void walk_through()
  {
    ///------------------------------------------------------
  //walk_through
  // measure distance for dist1
  float dur1, dist1, dur2, dist2;
  digitalWrite(ULTRASONIC_TRIG_PIN1, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(ULTRASONIC_TRIG_PIN1, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(ULTRASONIC_TRIG_PIN1, LOW);
  dur1 = pulseIn(ULTRASONIC_ECHO_PIN1, HIGH);
  dist1 = (dur1/2) / 29.1;
  Serial.print("**** Ultrasonic Distance: ");
  Serial.print(dist1);
  Serial.println(" cm");
  // measure distance for dist2

  digitalWrite(ULTRASONIC_TRIG_PIN2, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(ULTRASONIC_TRIG_PIN2, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(ULTRASONIC_TRIG_PIN2, LOW);
  dur2 = pulseIn(ULTRASONIC_ECHO_PIN2, HIGH);
  dist2 = dur2*0.034/2;
  Serial.print("**** Ultrasonic Distance: ");
  Serial.print(dist2);
  Serial.println(" cm");
if ((dist1<30) && (dist2 < 30))
{
  digitalWrite (Red_LED,HIGH);
  digitalWrite (Blue_LED,HIGH);
}
 else if ((dist1>30) && (dist2>30))
{
  digitalWrite (Red_LED,LOW); 
  digitalWrite (Blue_LED,LOW);
}
else if (dist1<30)
{

  digitalWrite (Red_LED,HIGH);
   digitalWrite (Blue_LED,LOW);
  
  }
  
else if (dist2 < 30)
{
  
     digitalWrite (Red_LED,LOW); 
     digitalWrite (Blue_LED,HIGH);
}
  }

 ///------------------------------------------------------
  ///Water Sensor
  void Water_Tank()
  {
  fireStatus4 = Firebase.getString("TANK_STATUS");
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
   value = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  Serial.print("Sensor value: ");
  Serial.println(value);
  String fireWaterE = String("Empty");
  String fireWaterL = String("Low");
  String fireWaterM = String("Medium");
  String fireWaterH = String("High");
  if(value<=400)
  {
//  Firebase.pushString("/Water Tank/Level", fireWaterE);
  Firebase.setString ("/TANK_STATUS/Level",fireWaterE);
  }
  else if(value>400 && value <=1000)
  {
//    Firebase.pushString("/Water Tank/Level", fireWaterL);
    Firebase.setString ("/TANK_STATUS/Level",fireWaterL);
  }
  else if(value>1000 && value<=1200)
  {
//    Firebase.pushString("/Water Tank/Level", fireWaterM);
        Firebase.setString ("/TANK_STATUS/Level",fireWaterM);
  }
  else if(value>1200)
  {
//    Firebase.pushString("/Water Tank/Level", fireWaterH);
        Firebase.setString ("/TANK_STATUS/Level",fireWaterH);
  }
    if (Firebase.failed()) 
    {
 
      Serial.print("pushing /logs failed:");
      Serial.println(Firebase.error()); 
      return;
  }
  }
