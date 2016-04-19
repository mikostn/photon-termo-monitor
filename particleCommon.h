#ifndef particleCommon_h
#define particleCommon_h

char *ipAddress;

int BLUE = D7;

String deviceName = "";
String deviceID = "";

//int antenna = 1;


void blinkLed(int pin, int blinkTime, int blinks = 1);


int getInfo(String command);

#endif //particleCommon_h
