#include "Timer.h"

Timer t;

int ledEvent;

typedef struct {
  int day;
  int hour;
  int minute;
  int second;
} time_t;

time_t countdown = {0, 0, 0, 0};

//GPIO 2 is powerPin
const int powerPin = 2;

//GPIO 13 is LED pin
const int ledPin = 13;

//GPIO pin for configuration
const int num_config_bits = 6;
const int bit1_pin = 4;
const int bit2_pin = 5;
const int bit3_pin = 6;
const int bit4_pin = 7;
const int bit5_pin = 8;
const int bit6_pin = 9;

int configuration = 0;

//power switch works for 5 seconds
const int switch_seconds = 5;

void setup()
{
  //initialize countdown to 11 hours at the first boot
  countdown = {0, 11, 0, 0};

  Serial.begin(115200);
  
  //switch on powerPin to indicate the device is working
  //then switch off to indicate the devides is in action
  //start up event would switch on again in 5 seconds
  pinMode(powerPin, OUTPUT);
  
  switchOn();
  delay(1000);
  switchOff();
  
  int tickEvent = t.every(1000, doCountDown);
  Serial.print("1 second tick started id=");
  Serial.println(tickEvent);
  
  pinMode(ledPin, OUTPUT);
  ledEvent = t.oscillate(ledPin, 50, HIGH);
  Serial.print("LED event started id=");
  Serial.println(ledEvent);
  
  int afterEvent = t.after(5*1000, slowDownLed_switchOn);
  Serial.print("After event started id=");
  Serial.println(afterEvent); 

  //set pull down resisters for configuration pins
  pinMode(bit1_pin, OUTPUT);
  pinMode(bit2_pin, OUTPUT);
  pinMode(bit3_pin, OUTPUT);
  pinMode(bit4_pin, OUTPUT);
  pinMode(bit5_pin, OUTPUT);
  pinMode(bit6_pin, OUTPUT);

  digitalWrite(bit1_pin, LOW);
  digitalWrite(bit2_pin, LOW);
  digitalWrite(bit3_pin, LOW);
  digitalWrite(bit4_pin, LOW);
  digitalWrite(bit5_pin, LOW);
  digitalWrite(bit6_pin, LOW);

  pinMode(bit1_pin, INPUT);
  pinMode(bit2_pin, INPUT);
  pinMode(bit3_pin, INPUT);
  pinMode(bit4_pin, INPUT);
  pinMode(bit5_pin, INPUT);
  pinMode(bit6_pin, INPUT);

}

void loop()
{
  t.update();
}

void doCountDown()
{
  time_t *c = &countdown;
  char buf[64] = {0};
  sprintf(buf, "TTR: %d D %d:%d:%d", c->day, c->hour, c->minute, c->second);
  Serial.println(buf); 

  if (countdown.second > 0) {
    countdown.second --;
  }else if (countdown.minute > 0) {
    countdown.minute --;
    countdown.second = 59;
  }else if (countdown.hour > 0) {
    countdown.hour --;
    countdown.minute = 59;
    countdown.second = 59;
  }else if (countdown.day > 0) {
    countdown.day --;
    countdown.hour = 23;
    countdown.minute = 59;
    countdown.second = 59;
  }else{
    //count down is completed, do poweroff action for the consumer
    
    //flash led to indicate action
    ledEvent = t.oscillate(ledPin, 50, HIGH);

    switchOff();

    //read hardware setup to determine cycle here:
    readConfig();

    //slow down led to indicate normal operation after 5 seconds and switch On
    t.after(5*1000, slowDownLed_switchOn);

  }
}


void slowDownLed_switchOn()
{
  Serial.println("stop the led event");
  t.stop(ledEvent);
  t.oscillate(ledPin, 500, HIGH);

  switchOn();
}

void switchOff()
{
  Serial.println("switch Off!!"); 
  digitalWrite(powerPin, LOW);
}

void switchOn()
{
  Serial.println("switch On!"); 
  digitalWrite(powerPin, HIGH);
}

bool is_quick_config(int configuration)
{
  Serial.print("Cofnig:");
  Serial.println(configuration, HEX);
  // this routine check whether there is more
  // than 1 'true' bits there in the configuration
  // return false if more than 1 'true' bits there
  bool found = false;
  for (int i = 0; i<num_config_bits; i++) {
    if ((configuration >> i)  & 0x1) {
      if (found) return false;
      found = true;
    }
  }
  
  return true;
}

void readConfig()
{
  configuration = 0;

  if (digitalRead(bit1_pin)) configuration |= 0x1;
  if (digitalRead(bit2_pin)) configuration |= 0x1 << 1;
  if (digitalRead(bit3_pin)) configuration |= 0x1 << 2;
  if (digitalRead(bit4_pin)) configuration |= 0x1 << 3;
  if (digitalRead(bit5_pin)) configuration |= 0x1 << 4;
  if (digitalRead(bit6_pin)) configuration |= 0x1 << 5;

  if (is_quick_config(configuration)) {
    //quick config:
    if (configuration == 0) {
      countdown = {0, 24, 0, 0};
    }else if (configuration & 0x1) {
      countdown = {0, 1, 0, 0};
    }else if (configuration & 0x1 << 1) {
      countdown = {0, 2, 0, 0};
    }else if (configuration & 0x1 << 2) {
      countdown = {0, 4, 0, 0};
    }else if (configuration & 0x1 << 3) {
      countdown = {0, 6, 0, 0};
    }else if (configuration & 0x1 << 4) {
      countdown = {0, 12, 0, 0};
    }else if (configuration & 0x1 << 5) {
      countdown = {2, 0, 0, 0};
    }
  }
  else{
    //if it is not quick config, one bit equals to 30 minutes
    countdown.day    = (configuration / 24) / 2;
    countdown.hour   = (configuration / 2) % 24;
    countdown.minute = (configuration % 2) * 30;
    countdown.second = 0;
  }
}


