// This #include statement was automatically added by the Particle IDE.
#include "OneWire/OneWire.h"
// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature/spark-dallas-temperature.h"

//#define DEBUG 0
//#define DEBUG 1
#define PORT 48879
#define MAX_DATA_BYTES 128

//D7 LED Flash Example
#define BLUE D2//D7

char *ipAddress;

int pinSetPWM(String command);
int pinPWM = A4;//WKP;//D0;
int valPWM = 0;

int pinToggle(String command);
int pinOnOff = D7;//D1;
int pinOnOffState = 0;

/* Street light leds! */
int setStrLight(String light);
String streetLight = "green";
//int streetLight = 1;
int pinRed = A2;
int pinYellow = A1;
int pinGreen = A0;

/* Motion detection */
int pinMotionRead = D3;
int pinMotionLed = D2;
// buttons helper vars
int state = 0; // variable to store the read value
bool motion = false;


/*
 *
 */
int readSensors(String sensor);

/* Photo resistor */
int photoresistor = A3;
int photoPower = A5;
int photoValue = 0;


/*
 * DALLAS DS18x20 sensors
*/

#define ONE_WIRE_BUS_PIN D6
// oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS_PIN);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature tempSensors(&oneWire);
//
int readTemp = 1;
double tempC1 = 0;
double tempC2 = 0;
// Devices
DeviceAddress Probe01 = { 0x10, 0x5C, 0x78, 0x5E, 0x02, 0x08, 0x00, 0x0F };
DeviceAddress Probe02 = { 0x28, 0x5D, 0x45, 0x9F, 0x03, 0x00, 0x00, 0xDE };

/*
 * SETUP FUNCTION
 */
void setup() {
    setupOutputPins();


    setupPWMpins();

    setupTogglePins();

    setupInputPins();

    //Share davide IP
    getPhotonsIP();

    // Dallas One-wire temp sensors
//    setupTempSensors();

//    setupStreetLights();

    setupPhotoresistor();


    // Cloud interactions
    Particle.function("lampSetVal", pinSetPWM); // 0-255
    Particle.function("lampToggle", pinToggle); // on-off
    Particle.function("setStrLight", setStrLight); // red-green

    Particle.function("readSensors", readSensors);


    Particle.variable("ipAddress", ipAddress);
    Particle.variable("pwmval", valPWM);
    Particle.variable("stateOnOff", pinOnOffState);

    Particle.variable("readTemp", readTemp);
    Particle.variable("temp1", tempC1);
    Particle.variable("temp2", tempC2);

    Particle.variable("strLight", streetLight);
    Particle.variable("photoValue", photoValue);


    Particle.publish("motion-detected", "online");
    Particle.publish("sensors", "online");
//    Particle.subscribe("streetLightChange", setStrLight);

    #if DEBUG
        Serial.begin(9600);
        while(!Serial.available()) Particle.process();
    #endif
}

/*
 * THE LOOP
 */
int run = 0;
void loop() {
    run++;
//    heartbeatBlink();
//    motionDetection();
//    heartbeatBlink(BLUE, 15);
    blinkLed(BLUE, 100);

    if(run == 150 && readTemp == 1){ // about every 30 sec
        readSensors("once");
//        getTemperature();
//        getPhotoValue();
        delay(500);
        run = 0;
        #if DEBUG
            Serial.println("Get sensors");
        #endif
    }
}


/*
 * FUNCTIONS
 */

void blinkLed(int pin, int freq) {
    digitalWrite(pin, HIGH);
    delay(freq);
    digitalWrite(pin, LOW);
    delay(freq);
}

//void heartbeatBlink(int pin, int pauseTime) {
void heartbeatBlink(int pin) {
    int pauseTime = 5;
    int i = 0;
    for( i = 0; i < 256; i++ ) {
        analogWrite(pin, i);
        delay(pauseTime);
    }
    for( i = 254; i > -1; i-- ) {
        analogWrite(pin, i);
        delay(pauseTime);
    }
    #if DEBUG
         Serial.print("statusLed: "); Serial.println(statusLed);
    #endif
}

// Get photons IP and set cloud varitable
void getPhotonsIP() {
    IPAddress ip = WiFi.localIP();
    static char ipAddress[24] = "";
    // https://community.particle.io/t/network-localip-to-string-to-get-it-via-spark-variable/2581/5
    sprintf(ipAddress, "%d.%d.%d.%d:%d", ip[0], ip[1], ip[2], ip[3], PORT);
    #if DEBUG
        Serial.print("Local ipAddress: ");
        Serial.println(ipAddress);
    #endif
}

int readSensors(String sensor) {
    if(sensor == "on") {
        readTemp = 1;
        getTemperature();
        getPhotoValue();
        return 1;
    }
    if(sensor == "off") {
        readTemp = 1;
        return 1;
    }
    if(sensor == "once") {
        getTemperature();
        getPhotoValue();
        return 1;
    }
    return -1;
}


//void setupOutputPins(string pins[], int array_size) {
//    for(int i = 0; i < array_size; i++)
//        pinMode(pins[i], OUTPUT);
//}
void setupOutputPins() {
    pinMode(BLUE, OUTPUT);
    analogWrite(BLUE, LOW);

    pinMode(pinMotionLed, OUTPUT);
    digitalWrite(pinMotionLed, LOW);
}

//void setupInputPins(string pins[], int array_size) {
//    for(int i = 0; i < array_size; i++)
//        pinMode(pins[i], INPUT_PULLDOWN);
//}
void setupInputPins() {
    pinMode(pinMotionRead, INPUT);
}

void setupTogglePins() {
    pinMode(pinOnOff, OUTPUT);
    digitalWrite(pinOnOff, LOW);
//    analogWrite(pinOnOff, 0);
}

int pinToggle(String command) {
//    if(command == "on") {
    if(pinOnOffState == 0) {
        digitalWrite(pinOnOff, HIGH);
        analogWrite(pinPWM, 255);
        pinOnOffState = 1;
        return 1;
    } else {
        digitalWrite(pinOnOff, LOW);
        analogWrite(pinPWM, 0);
        pinOnOffState = 0;
        return 1;
//        return -1;
    }
}

void setupPWMpins() {
//    pinMode(pinPWM, OUTPUT);
////    digitalWrite(pinPWM, LOW);
//    analogWrite(pinPWM, 0);

    pinMode(DAC, OUTPUT);
    analogWrite(DAC, 0);
}

int pinSetPWM(String command) {
    int val = command.toInt();
    if(val >= 0) {
        analogWrite(pinPWM, val);
        analogWrite(DAC, val); //1024/4095 * 3.3V = 0.825V
        valPWM = val;
        return 1;
    }
    else return -1;
}

/*
 * Dallas onewire temperature detection
 */

void setupTempSensors() {
    //Dallas termo
    tempSensors.begin();
    // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
    tempSensors.setResolution(Probe01, 9);
    tempSensors.setResolution(Probe02, 9);
    #if DEBUG
        discoverOneWireDevices();
    #endif
}

void getTemperature() {
    //double tempC1 = 0;
    //double tempC2 = 0;

    tempSensors.requestTemperatures();
    tempC1 = tempSensors.getTempCByIndex( 0 );
    tempC2 = tempSensors.getTempCByIndex( 1 );

//    tempC1 = tempSensors.getTempC(Probe01);
//    tempC2 = tempSensors.getTempC(Probe02);

    String t1 = String("t1:"); //String(tempC1);
    String t2 = String(";t2:"); //String(tempC2);
    String tm = String("");
    t1.concat(String(tempC1));
    t2.concat(String(tempC2));
    tm.concat(t1);
    tm.concat(t2);
    tm.concat(";");

    Particle.publish("sensors", tm);

    #if DEBUG
        Serial.print("Temperature: "); Serial.println(tempC1);
//        Serial.print("Temperature: "); Serial.println(tempSensors.getTempC(Probe01));
        Serial.print("Temperature: "); Serial.println(tempC2);
//        Serial.print("Temperature: "); Serial.println(tempSensors.getTempC(Probe02));
    #endif
}

void discoverOneWireDevices(void) {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];

  Serial.print("Looking for 1-Wire devices...\n\r");
  while(oneWire.search(addr)) {
    Serial.print("\n\rFound \'1-Wire\' device with address:\n\r");
    for( i = 0; i < 8; i++) {
      Serial.print("0x");
      if (addr[i] < 16) {
        Serial.print('0');
      }
      Serial.print(addr[i], HEX);
      if (i < 7) {
        Serial.print(", ");
      }
    }
    if ( OneWire::crc8( addr, 7) != addr[7]) {
        Serial.print("CRC is not valid!\n");
        return;
    }
  }
  Serial.print("\n\r\n\rThat's it.\r\n");
  oneWire.reset_search();
  return;
}


/*
 * Motion detection
 */

void motionDetection() {
    //mode
    state = digitalRead(pinMotionRead);       // read the input pin
    if(state == HIGH){
        Particle.publish("motion-detected", "true");
        motion = true;
    }
    // blink the LED as long as the button is pressed
    while(digitalRead(pinMotionRead) == HIGH) {
        blinkLed(pinMotionLed, 50);
    }
    // unset motion when loop is done
    if(motion){
        Particle.publish("motion-detected", "false");
        motion = false;
    }
}


/*
 * Photo resistor detection
 */
void setupPhotoresistor() {
    // First, declare all of our pins. This lets our device know which ones will be used for outputting voltage, and which ones will read incoming voltage.
    pinMode(photoresistor,INPUT);  // Our photoresistor pin is input (reading the photoresistor)
//    pinMode(photoPower,OUTPUT); // The pin powering the photoresistor is output (sending out consistent power)

    // Next, write the power of the photoresistor to be the maximum possible, so that we can use this for power.
//    digitalWrite(photoPower,HIGH);
}

void getPhotoValue() {
    photoValue  = analogRead(photoresistor);
    String msg = String("brightness:");
    msg.concat(String(photoValue));
    msg.concat(";");
    Particle.publish("sensors", msg);
}

/*
 * Street light leds!
 */
void setupStreetLights() {
    pinMode(pinRed, OUTPUT);
    digitalWrite(pinRed, LOW);

    pinMode(pinYellow, OUTPUT);
    digitalWrite(pinYellow, LOW);

    pinMode(pinGreen, OUTPUT);
    digitalWrite(pinGreen, LOW);

    initBlinkLights();
}

void initBlinkLights() {
    digitalWrite(pinRed, HIGH);
    delay(100);
    digitalWrite(pinYellow, HIGH);
    delay(100);
    digitalWrite(pinGreen, HIGH);
    delay(100);

    digitalWrite(pinRed, LOW);
    delay(100);
    digitalWrite(pinYellow, LOW);
}

void greenLight(){
    streetLight = "green";
//    streetLight = 1;
    digitalWrite(pinRed, HIGH);
    delay(100);
    digitalWrite(pinYellow, HIGH);
    delay(1000);
    digitalWrite(pinRed, LOW);
    digitalWrite(pinYellow, LOW);
    digitalWrite(pinGreen, HIGH);
}

void redLight(){
    streetLight = "red";
//    streetLight = 0;

    digitalWrite(pinGreen, HIGH);
    delay(100);
    digitalWrite(pinGreen, LOW);
    digitalWrite(pinYellow, HIGH);
    delay(1000);
    digitalWrite(pinYellow, LOW);
    digitalWrite(pinRed, HIGH);
}

int setStrLight(String light) {
    if(light == "green") {
        greenLight();
        return 1;
    }
    if(light == "red") {
        redLight();
        return 1;
    }
    else {
        return -1;
    }
//  Particle.publish("streetLightChange", "online");
}
int changeLights() {
    return 1;
}
