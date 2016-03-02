#ifndef particleCommon_h
#define particleCommon_h

#define DEBUG 1

char *ipAddress;

int BLUE = D7;


void blinkLed(int pin, int blinkTime, int blinks = 1);


int getInfo(String command);

#endif //particleCommon_h
