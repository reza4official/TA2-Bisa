#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "tugasakhir-02.firebaseio.com"                    //Your Firebase Project URL goes here without "http:" , "\" and "/"
#define FIREBASE_AUTH "7I5IORRKJXuSURNvmjh6C5Sl37uEAEXjQNBaROCm"        //Your Firebase Database Secret goes here
#define WIFI_SSID "Samsung Galaxy Note10+"                                          //Your WiFi Name
#define WIFI_PASSWORD "reza4official_"                                        //Password of your wifi network 
#define timeSeconds 30

/** constant */
String ID = "Alat 1";                                                   // Current device
String child = "Alat 2";                                                // Next device
//int child = NULL;                                                       // If the "CURRENT" device didn't have the NEXT device 
//String parent = "Alat 2";                                               // Previous device
int parent = NULL;                                                      // If the "CURRENT" device didn't have the PREVIOUS device


/** declare inputs */
int relay = 2;                                                          // choose the pin for the RELAY
int pir = 13;                                                           // choose the input pin (for PIR sensor)
int gerak = 0;                                                          // variable for reading the pin status

/** Firebase variable */
int lampu, childLamp, bypass, layer;                                    // variable for reading firebase value
int sensor, lamp, Status, automode;                                     // variable for reading input value

/** Sensor initial state */
int pirStatus = LOW;

/** Setup timer */
unsigned long now = millis();
unsigned long lastTrigger = 0;
bool mulaiTimer = false;

/** Main setup */
void setup() {
  /** Serial Monitor baudrate */
  Serial.begin(9600);

  /** Declare I/O*/
  pinMode(relay, OUTPUT);                                                // declare lamp as output
  pinMode(pir, INPUT_PULLUP);                                            // declare sensor as input

  /** Declare relay initial State */
  digitalWrite(relay, 0);

  /** Call Function */
  wifiConnect();

  /** Firebase setup */
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                          // Begin Firebase connection
  Serial.println("Connected to Firebase");
  layer = Firebase.getInt(ID + "/Layer");                                // Get data for "Layer" variable from Firebase
//  Serial.println(ID + " = Layer " + layer);
}

/** WiFi connect function */
void wifiConnect(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                  // Begin WiFi connection
  Serial.print("connecting");
  while (WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected to wifi");
}

/** Firebase reconnect function */
void firebasereconnect(){
  Serial.println("Trying to reconnect");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

///** Read Firebase value */
//void readFirebase(){
//  Status = Firebase.getInt(ID + "/Status");
//  lampu = Firebase.getInt(parent + "/Lampu");
//  childLamp = Firebase.getInt(child + "/Lampu");
//  bypass = Firebase.getString(ID + "/Bypass").toInt();
//  automode = Firebase.getString(ID + "/Auto").toInt();
//}

/** Set Auto/Manual mode procedure */
//void manualMode(){
//  if (automode == 0){
//      if (bypass == 1) {
//      digitalWrite(relay, HIGH); 
//      Serial.println("Lampu1-1 Menyala");
//      delay(1000);
//    } else {
//      digitalWrite(relay,LOW);
//      Serial.println("Lampu1-1 Mati");
//      delay(1000);
//    }
//  }
//}

/** Main loop */
void loop(){
   if (Firebase.failed()){
    Serial.print("setting number failed:");
    Serial.println(Firebase.error());
    firebasereconnect();
    delay(500);
    return;
  }

  now = millis();
  gerak = digitalRead(pir);

/** Dynamic Sensor Activation process */
  if (gerak == HIGH && pirStatus == LOW){
    /** Read Firebase value */
    Status = Firebase.getInt(ID + "/Status");
    lampu = Firebase.getInt(parent + "/Lampu");
    childLamp = Firebase.getInt(child + "/Lampu");
    bypass = Firebase.getString(ID + "/Bypass").toInt();
    automode = Firebase.getString(ID + "/Auto").toInt();
    /** Turn on the light process */
    if ((layer == 1 && Status == 1) && automode == 1){                                       // Turn on layer 1
      digitalWrite(relay, HIGH);
      Serial.println("Gerakan Terdeteksi!");
      mulaiTimer = true;
      lastTrigger = millis();
      Firebase.setInt(child + "/Status", 1);
      Firebase.setInt(ID + "/Lampu", 1);
    } else if ((layer == 2 && Status == 1) && automode == 1){                               // Turn on layer 2
      if (lampu == 1){
        digitalWrite(relay, HIGH);
        Serial.println("Gerakan Terdeteksi!");
        mulaiTimer = true;
        lastTrigger = millis();
        Firebase.setInt(child + "/Status", 1);
        Firebase.setInt(ID + "/Lampu", 1);
      } 
    } else if ((layer == 3 && Status == 1) && automode == 1){                               // Turn on layer 3
      if (lampu == 1){
        digitalWrite(relay, HIGH);
        Serial.println("Gerakan Terdeteksi!");
        mulaiTimer = true;
        lastTrigger = millis();
        Firebase.setInt(child + "/Status", 1);
        Firebase.setInt(ID + "/Lampu", 1);
      } 
    }
  /** Turn off the light process */
  } else if ((layer == 1 && Status == 1 && childLamp == 0) && (now - lastTrigger > (timeSeconds*1000))){      // Turn off layer 1
      digitalWrite(relay, LOW);
      Serial.println("Gerakan Tidak Terdeteksi");
      mulaiTimer = false;
      Firebase.setInt(child + "/Status", 0); 
      Firebase.setInt(ID + "/Lampu", 0);
  } else if ((layer == 2 && Status == 1 && childLamp == 0) && (now - lastTrigger > (timeSeconds*1000))){    // Turn off layer 2 
      digitalWrite(relay, LOW);
      Serial.println("Gerakan Tidak Terdeteksi");
      mulaiTimer = false;
      Firebase.setInt(child + "/Status", 0); 
      Firebase.setInt(ID + "/Lampu", 0);
  } else if ((layer == 3 && Status == 1) && (now - lastTrigger > (timeSeconds*1000))){                      // Turn off layer 3
      digitalWrite(relay, LOW);
      Serial.println("Gerakan Tidak Terdeteksi");
      mulaiTimer = false; 
      Firebase.setInt(ID + "/Lampu", 0);
  }
}
