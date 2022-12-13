/*
 Interface to Shinyei Model PPD42NS Particle Sensor
 Program by Christopher Nafis 
 Written April 2012
 
 http://www.seeedstudio.com/depot/grove-dust-sensor-p-1050.html
 http://www.sca-shinyei.com/pdf/PPD42NS.pdf
 
 JST Pin 1 (Black Wire)  => Arduino GND
 JST Pin 3 (Red wire)    => Arduino 5VDC
 JST Pin 4 (Yellow wire) => Arduino Digital Pin 8
 
 Modifier par Eric HANACEK
 le 11/01/2015
 adapter avec SD Card Shield v4.0
 pin A5 en input digital
 
 */
#include <Arduino.h>

#define SN "PPD42NS Node"
#define SV "2.0"

// Enable debug prints to serial monitor
//#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_RF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

// Enable repeater functionality for this node
#define MY_REPEATER_FEATURE

//Options: RF24_PA_MIN, RF24_PA_LOW, (RF24_PA_HIGH), RF24_PA_MAX
//#define MY_RF24_PA_LEVEL RF24_PA_MAX

//#define MY_OTA_FIRMWARE_FEATURE

//uncomment this line to assign a static ID
//#define MY_NODE_ID AUTO 
#define MY_NODE_ID 150

//MY_RF24_CHANNEL par defaut 76
//Channels: 1 to 126 - 76 = Channel 77
//MY_RF24_CHANNEL (76)
#define MY_RF24_CHANNEL 81

//Define this to use the IRQ pin of the RF24 module
//#define MY_RF24_IRQ_PIN (2) 
//Define this to use the RF24 power pin (optional).
//#define MY_RF24_POWER_PIN (3)

//Placer les #define avant <Mysensors.h>
#include <MySensors.h>

#define CHILD_ID_DUST 0  // sensor number needed in the custom devices set up

// Wait times
#define LONG_WAIT 500
#define LONG_WAIT2 2000
#define SHORT_WAIT 50

//#include <SD.h>

// const int chipSelect = 4;
// // make a string for assembling the data to log:
// String dataString = "";

int pin = A5; // adapter avec SD Card Shield v4.0
unsigned long duration;
unsigned long lowpulseoccupancy = 0;
unsigned long starttime;
unsigned long sampletime_ms = 30000;
float ratio = 0;
float lastconcentration = 0;

//float DateMesure;

// initate and re-use to save memory.
MyMessage msgDUST(CHILD_ID_DUST, V_LEVEL);
//MyMessage msgUNIT(CHILD_ID_DUST, V_UNIT_PREFIX);

void before()
{
  // Optional method - for initialisations that needs to take place before MySensors transport has been setup (eg: SPI devices).
}

void presentation()
{
  Serial.print("===> Envoyer présentation pour noeud : ");
  Serial.println(MY_NODE_ID);
  
  char sNoeud[] = STR(MY_NODE_ID);

  // Send the sketch version information to the gateway and Controller
  Serial.println("=======> Envoyer SketchInfo");
  Serial.print(SN); Serial.print(" "); Serial.println(SV);
  sendSketchInfo(SN, SV );
  wait(LONG_WAIT);

  // Register the sensor to gw
  Serial.println("=======> Présenter les capteurs");
  char sChild[25];
  strcpy(sChild, "myS ");
  strcat(sChild, sNoeud);
  strcat(sChild, " DUST");
  Serial.println(sChild);
  present(CHILD_ID_DUST, S_DUST, sChild); 
  wait(LONG_WAIT);  

}

void setup() 
{
  //pinMode(13, OUTPUT);
  //digitalWrite(13, LOW); //Eteindre la LED
  
  // Serial.begin(9600);
  // Serial.println("Demarrage detecteur");
  // pinMode(pin,INPUT);

  // adapter avec SD Card Shield v4.0
  //digitalWrite(A5, HIGH);  // set pullup on analog pin 5 
  
  /*Serial.println("Initialisation de la carte SD");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
     Serial.println("Problème de carte SD");
     // don't do anything more:
     return;
  }
  Serial.println("Carte intialisee.");
  EcrireLigneSD("Nouveau enregistrement");
  EcrireLigneSD("lowpulse (micro S);ratio (%);concentration ( particules / 0.01 cubic feet (0.28316846592 litre)");
  */

  // starttime = millis();

}

void loop() 
{
  //Pour Home assistant
  static bool first_message_sent = false;
  if (!first_message_sent) {
    //send(msgPrefix.set("custom_lux"));  // Set custom unit.
    Serial.println("======> Sending initial value");
    //La couleur
    Serial.println("Color Message");
    //send(msgRGB.set( "00FF00" ));
    send(msgDUST.set( 0 ));
    wait(LONG_WAIT2); //to check: is it needed

    first_message_sent = true;
  }

  duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy + duration; //en micro secondes temps à zéro

  if ((millis()-starttime) > sampletime_ms)
  {
    ratio = lowpulseoccupancy / (sampletime_ms * 10.0);  // Integer percentage 0=>100
    //pied cube 
    // 1 ft3 = 28.316846592 l
    //litre
    // 1 l = 0.035314666721489 ft3 
    //
    // 0.01 ft3 = 0.28316846592 l (~=  283 ml  1/5 ml)
    float concentration = 1.1 * pow(ratio,3) - 3.8 * pow(ratio,2) + 520 * ratio + 0.62; // using spec sheet curve
    
    if (lastconcentration != concentration )
    {
      lastconcentration = concentration;


      
    }
  //   //DateMesure=millis()/1000; 
  //   //dataString=DateMesure;
  //   //dataString+="s: ";   
  //   //dataString="lowpulse: ";
  //   dataString="";
  //   dataString+=lowpulseoccupancy;
  //   //dataString+=" micro S, ratio: ";
  //   dataString+=";";
  //   dataString+=ratio;
  //   //dataString+=" %, concentration: ";
  //   dataString+=";";
  //   dataString+=concentration;
  //   //dataString+=" particules / 0.01 cubic feet (0.28316846592 litre)";
    
  //   // print to the serial port too:
  //   //Serial.println(dataString);
    
  //   /*if (EcrireLigneSD(dataString)==true) {
  //     EcrireLCD(1,20,"Ecriture Ok");
  //   }
  //   else {
  //     EcrireLCD(1,20,"Erreur ecriture");
  //   }
    
  //   dataString="Test";
  //   EcrireLCD(1,1,dataString);
  //   EcrireLCD(2,1,"Ligne 2 colonne 1");
  //   //EcrireLCD(3,1,"Ligne 3 colonne 1");
  //   //EcrireLCD(4,1,"Ligne 4 colonne 1");
  //   */
  //   lowpulseoccupancy = 0;
  //   starttime = millis();
    
  }
}

/*boolean EcrireLigneSD(String dataString) {
   // open the file. note that only one file can be open at a time,
   // so you have to close this one before opening another.
   //if (SD.exists("particul.txt")) {
   boolean resultat;
   
   File dataFile = SD.open("particul.txt", FILE_WRITE);

   // if the file is available, write to it:
   if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      // print to the serial port too:
      Serial.println(dataString);
      resultat=true;
   }
   // if the file isn't open, pop up an error:
   else {
      Serial.println("error opening particul.txt");
       resultat=false;
   }
    //}
    //else {
    //    Serial.println("carte absente!");
    //}
  
  for (int i=0;i<3;i++){
    digitalWrite(13, HIGH);//Allumer la LED pendant 1 seconde
    delay(300);
    digitalWrite(13, LOW);
    delay(300);
  }
  return resultat;
}
*/

/*void EcrireLCD(int Ligne, int Colonne, String dataString) {

}
*/