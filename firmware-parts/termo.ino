// This #include statement was automatically added by the Particle IDE.
#include "OneWire/OneWire.h"

// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature/spark-dallas-temperature.h"

// This #include statement was automatically added by the Particle IDE.
#include "ThingSpeak/ThingSpeak.h"

#define DEBUG 1

/*
  *****************************************************************************************
  **** Setup pins&functions .h
  ***************************************************************************************** */
char *ipAddress;

int led1 = D1;
int BLUE = D7;

void blinkLed(int pin, int blinkTime, int blinks = 1);
int getInfo(String command);

/*
  *****************************************************************************************
  **** Setup Dallas
  ***************************************************************************************** */
// Init Dallas on pin digital pin 3
#define ONE_WIRE_BUS_PIN D5 //A5 //D6

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS_PIN);

// Pass our oneWire reference to Dallas Temperature.
#define MaxSensorCount 8  // max num of sensors to be defined
byte SensorsCount;  // actual num o' sensors read from oneWire bus
byte Address[MaxSensorCount][8];  // 5 addresses of 8 bytes each  <--- Trick 77 for multiple 8-byte addresses
DallasTemperature Sensors(&oneWire); // pointer to Sensors

int NextSensor = 0;  // next Sensor to be read. if nextSensor>SensorCount-1, nextSensor=0;


/*
  *****************************************************************************************
  **** Setup WiFi
  ***************************************************************************************** */
// Connects to a network secured with WPA2 credentials.
STARTUP(WiFi.setCredentials("parti", "stn2244test"));


/*
  *****************************************************************************************
  **** Visit https://www.thingspeak.com to sign up for a free account and create
  **** a channel.  The video tutorial http://community.thingspeak.com/tutorials/thingspeak-channels/
  **** has more information. You need to change this to your channel, and your write API key
  **** IF YOU SHARE YOUR CODE WITH OTHERS, MAKE SURE YOU REMOVE YOUR WRITE API KEY!!
  *****************************************************************************************/
TCPClient client;
IPAddress pingIp(10,1,2,56);
IPAddress localThing(10,1,2,56);
unsigned long thingChannelNumber = 4;
const char * thingWriteAPIKey = "ZJ45KSIFKE1NGPQ5";


/*
  *****************************************************************************************
  **** Setup function
  ***************************************************************************************** */
void setup() {
    #if DEBUG
        Serial.begin(9600);
        while(!Serial.available()) Particle.process();
    #endif

    pinMode(BLUE,OUTPUT);

    Particle.function("getInfo", getInfo);

    Particle.variable("ipAddress", ipAddress, STRING);
    IPAddress ip = WiFi.localIP();
    static char ipAddress[24] = "";
    // https://community.particle.io/t/network-localip-to-string-to-get-it-via-spark-variable/2581/5
    sprintf(ipAddress, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    Particle.variable("ipAddress", ipAddress, STRING);
    #if DEBUG
        Serial.print("Local ipAddress: "); Serial.println(ipAddress);
    #endif


    /*
      *****************************************************************************************
      **** setup DallasTemperature Sensors
      ***************************************************************************************** */
    // start sensor library
    Sensors.begin();

    // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
    // Sensors.setResolution(10);

    // get sensor address by searching the OneWire bus
    discoverOneWireDevices();

    // after search is done...
    SensorsCount = Sensors.getDeviceCount();

    /*
      *****************************************************************************************
      **** Setup ThingSpeak communication
      ***************************************************************************************** */
      ThingSpeak.begin(client, localThing, 3000);

}


/*
  *****************************************************************************************
  **** LOOP
  ***************************************************************************************** */
void loop() {
    int sensorIndex = NextSensor;
    int sensorField = sensorIndex + 1;

    if(sensorIndex == 0 && DEBUG) {
        // locate devices on the bus
        Serial.print("Locating devices...");
        Serial.print("Found ");
        Serial.print(SensorsCount, DEC);
        Serial.println(" devices.");

        // report parasite power requirements
        Serial.print("Parasite power is: ");
        if (Sensors.isParasitePowerMode()) {
            Serial.println("ON");
        } else {
            Serial.println("OFF");
        }
    }

    #if DEBUG
        Serial.print("Address[");
        Serial.print(sensorIndex);
        Serial.print("] = ");
        for (byte n=0; n<8; n++)
        {
            Serial.print(Address[sensorIndex][n], HEX);
        }
    #endif

    /*
      *****************************************************************************************
      **** Get & send temperature data
      ***************************************************************************************** */
    Sensors.requestTemperatures();
    // tempSensors.getAddress(thermometer, cI);
    // double tempC = Sensors.getTempCByIndex( cI );
    float tempC = Sensors.getTempC(Address[sensorIndex]);

    /*
     * ********************************************
     * **** Send to data server
     * -127 and 85 deg C - appear to be error info
     * -127 - DEVICE_DISCONNECTED
     * 85 - communication error (???)
    ***********************************************/
    if(tempC > -127 && tempC != 85) {
        ThingSpeak.setField((sensorField), tempC);
        // Write the fields that you've set all at once.
        ThingSpeak.writeFields(thingChannelNumber, thingWriteAPIKey);
    }

    #if DEBUG
        Serial.print(", temperature: "); Serial.print(tempC); Serial.println(" C");
    #endif

    blinkLed(BLUE, 100, 3);

    NextSensor++;
    if(NextSensor > (SensorsCount - 1)) {
        NextSensor = 0;
    }

    delay(3000);

}


/*
  *****************************************************************************************
  **** Functions
  ***************************************************************************************** */
void discoverOneWireDevices(void) {
    #if DEBUG
        Serial.print("Looking for 1-Wire devices...\n\r");
    #endif

    int i = 0;
    byte ii;
//    byte present = 0;
//    byte data[12];
//    byte addr[8];

    while(oneWire.search(Address[i])) {
        #if DEBUG
        Serial.print("\n\rFound \'1-Wire\' device with address:\n\r");
        for( ii = 0; ii < 8; ii++) {
            Serial.print(Address[i][ii], HEX);
        }
        if ( OneWire::crc8( Address[i], 7) != Address[i][7]) {
            Serial.print("CRC is not valid!\n");
            return;
        }
        #endif
        i++;
    }
    #if DEBUG
        Serial.print("\n\r\n\rThat's it.\r\n");
    #endif

    oneWire.reset_search();
    return;
    /*
    for (byte i=0; i<SensorsCount; i++) {
        // assigns the next address found to Address[i][0..7]
        //if(!Sensors.getAddress(Address[i], i))
        if (!oneWire.search(Address[i]))
        {
          Serial.print("Unable to find address");
          Serial.print("\n");
          oneWire.reset_search();
        }
    }
    */
}

void blinkLed(int pin, int blinkTime, int blinks) {
    while(blinks){
        digitalWrite(pin, HIGH);
        delay(blinkTime/2);
        digitalWrite(pin, LOW);
        delay(blinkTime/2);
        blinks = blinks - 1;
    }
}


/*
 ********************************************************************************
 Cloud functions
 ********************************************************************************/
int getInfo(String command) {
    if(command == "wifi") {
    // EXAMPLE
    WiFiAccessPoint ap[5];
    int found = WiFi.getCredentials(ap, 5);
    for (int i = 0; i < found; i++) {
        Serial.print("ssid: ");
        Serial.println(ap[i].ssid);
        // security is one of WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA, WLAN_SEC_WPA2
        Serial.print("security: ");
        Serial.println(ap[i].security);
        // cipher is one of WLAN_CIPHER_AES, WLAN_CIPHER_TKIP
        Serial.print("cipher: ");
        Serial.println(ap[i].cipher);
    }
    return 1;
    } else{
        return -1;
    }
}
