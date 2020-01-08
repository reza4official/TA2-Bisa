/** Tugas Akhir */

/** This code is made by :
 *  
 *  Name: Moch. Rheza Wira Z. Hasbi
 *  NIM: 1301154168
 *  Class: IF 39-14
 *  
 *  Final project title:
 *  PERANCANGAN SISTEM PENCAHAYAAN PADA APARTEMEN
 *  MENGGUNAKAN METODE DYNAMIC SENSOR ACTIVATION
 *  
 *  Pembimbing 1:
 *  Name : Dr. Maman Abdurohman, S.T., M.T.
 *  NIP  : 99750010-1
 *  
 *  Pembimbing 2:
 *  Name : Sidik Prabowo, S.T., M.T.
 *  NIP  : 15870072-1
 */

/** Library setup */
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

/** Firebase and WiFi detail setup */
#define FIREBASE_HOST "tugasakhir-02.firebaseio.com"                    //Your Firebase Project URL goes here without "http:" , "\" and "/"
#define FIREBASE_AUTH "7I5IORRKJXuSURNvmjh6C5Sl37uEAEXjQNBaROCm"        //Your Firebase Database Secret goes here
#define WIFI_SSID "Razer WiFi"                                          //Your WiFi Name
#define WIFI_PASSWORD "01454765"                                        //Password of your wifi network 

/** Time setup */
#define timeSeconds 30
#define FirebaseCheckingTime 10

/** Device setup */
String ID = "Alat 1";                                                   // Current device
String child = "Alat 2";                                                // Next device

/** declare inputs */
int relay = 2;                                                          // choose the pin for the RELAY
int pir = 13;                                                           // choose the input pin (for PIR sensor)
int gerak = 0;                                                          // variable for reading the pin status

/** Firebase variable */
int childLamp, bypass, layer;                                           // variable for reading firebase value
int Status, automode;                                                   // variable for reading input value
int pirRead;

/** Sensor initial state */
int pirStatus = LOW;
char prev = LOW;
int triggerOn = 0;
int triggerOff = 0;
int neighbor = 0;

/** Timer setup */
unsigned long checkFirebase = millis();
unsigned long lastTrigger = 0;

/** Main setup */
void setup() {
  /** Serial Monitor baudrate */
  Serial.begin(9600);

  /** Declare I/O*/
  pinMode(relay, OUTPUT);                                              // declare lamp as output
  pinMode(pir, INPUT_PULLUP);                                          // declare sensor as input

  /** Declare relay initial State */
  digitalWrite(relay, 0);

  /** Call Function */
  wifiConnect();

  /** Firebase setup */
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                        // Begin Firebase connection
  Serial.println("Connected to Firebase");
  layer = Firebase.getInt(ID + "/Layer");                              // Get data for "Layer" variable from Firebase

  /** Read Firebase intial value */
  readFirebase();
}

/** WiFi connect function */
void wifiConnect() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                // Begin WiFi connection
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected to wifi");
}

/** Firebase reconnect function */
void firebasereconnect() {
  Serial.println("Trying to reconnect");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

/** Read Firebase value */
void readFirebase() {
  Status = Firebase.getInt(ID + "/Status");
  if ( child != "None" ) {
    childLamp = Firebase.getInt(child + "/Lampu");
  }
  else {
    childLamp = 2;
  }
  //  lampu = Firebase.getInt(parent + "/Lampu");
  //  bypass = Firebase.getString(ID + "/Bypass").toInt();
  //  automode = Firebase.getString(ID + "/Auto").toInt();
}

/** Write Firebase value */
void updateFirebase() {
  if (layer == 1 || layer == 2) {
    Firebase.setInt(ID + "/Lampu", 1);
    Firebase.setInt(child + "/Status", 1);
  }
  else if (layer == 3) {
    Firebase.setInt(ID + "/Lampu", 1);
  }
}

/** Main Loop */
void loop() {
  pirRead = digitalRead(pir);
  
  /** Read PIR current status and previous status */
  if ( pirRead == HIGH ) {
    if ( prev == LOW ) {
      triggerOn = 1;
      prev = HIGH;
    }
  }
  else if ( pirRead == LOW ) {
    if ( prev == HIGH ) {
      triggerOff = 1;
    }
    prev = LOW;
  }

  /** Turn on light process */
  if ( triggerOn == 1 ) {
    readFirebase();
    if (Status == 1) {
      digitalWrite(relay , HIGH);
      Serial.println("Gerakan Terdeteksi!");
      lastTrigger = millis();
      updateFirebase();
      triggerOn = 0;
    }
    else {
      return;
    }
  }
  if ( triggerOff == 1 ) {
    triggerOff = 0;
  }

  /** Check Child Lamp status */
  if ( prev == LOW ) {
    if ( digitalRead(relay) == HIGH ) {
      if ( millis() - checkFirebase >= (FirebaseCheckingTime * 1000) ) {
        readFirebase();
        checkFirebase = millis();
        Serial.println("============");
        Serial.println("childLamp Now = ");
        Serial.println(childLamp);
        Serial.println("============");
        Serial.println();
        if ( childLamp == 0 ) {
          Serial.println("============");
          Serial.println("childLamp = ");
          Serial.println(childLamp);
          Serial.println("============");
          Serial.println();
          neighbor = 1;
          Serial.println("============");
          Serial.println("neighbor = ");
          Serial.println(neighbor);
          Serial.println("============");
          Serial.println();
        }
        else if ( childLamp == 1 ) {
          Serial.println("============");
          Serial.println("childLamp = ");
          Serial.println(childLamp);
          Serial.println("============");
          Serial.println();
          neighbor = 0;
          Serial.println("============");
          Serial.println("neighbor = ");
          Serial.println(neighbor);
          Serial.println("============");
          Serial.println();
        }
        else if ( childLamp == 2 ) {
          neighbor = 2;
          Serial.println("============");
          Serial.println("neighbor = ");
          Serial.println(neighbor);
          Serial.println("============");
          Serial.println();
        }
      }

      /** Turn off light process */
      if ( neighbor == 1 ) {
        if ( millis() - lastTrigger >= (timeSeconds * 1000) ) {
          digitalWrite(relay , LOW);
          Serial.println("Gerakan Tidak Terdeteksi");
          Firebase.setInt(ID + "/Lampu", 0);
          Firebase.setInt(child + "/Status", 0);
          neighbor = 0;
        }
      }
      else if ( neighbor == 2 ) {
        if ( millis() - lastTrigger >= (timeSeconds * 1000) ) {
          digitalWrite(relay , LOW);
          Serial.println("Gerakan Tidak Terdeteksi");
          Firebase.setInt(ID + "/Lampu", 0);
          neighbor = 0;
        }
      }
    }
  }
}
