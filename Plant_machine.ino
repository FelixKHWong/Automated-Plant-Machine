
#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11

//equipment initialization
LiquidCrystal lcd(13,12,11,10,9,8);
DHT dht(DHTPIN, DHTTYPE);

// variables
int sensortemp = 2;
int sensorsoil = A0;
int digisoil = 3;
int pump = 6;
int lock0 = 0;
int lock1 = 0;
int moist = 0;
int rtime =0;
unsigned long timer = 0;
float hum = 0;
float temp = 0;
float HtId = 0;

// functions
void printdata();
void screen();
unsigned long bigtime(float,float);
int water(int);

void setup() {                                                      // initiate serial communications, sensors and pins to control
  Serial.begin(9600);
  lcd.begin(16,2);  
  dht.begin();
  pinMode(sensortemp,INPUT);
  pinMode(sensorsoil,INPUT); 
  pinMode(digisoil,OUTPUT);
  pinMode(pump,OUTPUT);
  digitalWrite(pump,HIGH);                                           // For some reason high means off for relay
  digitalWrite(digisoil,LOW);
  lcd.println("INITIATING");
  delay(3000);
  lcd.clear();
}

void loop() {


  // temperature
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  HtId = dht.computeHeatIndex(temp,hum, false);
  printdata();
/*  if (isnan(hum) || isnan(temp)) {                                  going to hope it doesnt break
   Serial.println("Failed to read from DHT sensor!");
   return;
  }*/
  delay(bigtime(hum,temp));
   //soil
  digitalWrite(digisoil,HIGH);                                       // only check soil after certain amount of time
  delay(100);
  moist = analogRead(sensorsoil);
  digitalWrite(digisoil,LOW);
  water(moist);                                                     // water depending on moisture reading
  screen();
  delay(2000);
}

unsigned long bigtime(float temp, float hum){
  if (temp >= 26){                                                     // if its hot set timer to 30 min [1800*1000 = 30 min]
    timer = 1800UL*1000UL;
  }
  else if(temp < 26){                                                  // not so hot, dehyrates at slower rate, check in an hour
    timer = 3600UL*1000UL;                                            //[3600*1000 = 60 min]
  }
  if(hum > 95){                                                       //100% humidity means raining so dont water, set next check in 3 hours
    timer = 10800UL*1000UL;                                           //[10800*1000 = 180 min]
  }
  rtime = timer/60000;                                                //Converting to minute
  return timer;
}

int water(int moist){                                                   //if soil is dry then water
  if(moist > 750){
    digitalWrite(pump,LOW);
    lcd.clear();
    lcd.home();
    lcd.print("WATERING");                                            //pump for 7 seconds
    delay(3500);    
    lcd.clear();
    digitalWrite(pump,HIGH);    
  }
   else if(400 < moist < 750){                                          //soil is at good level, dont water
    digitalWrite(pump,HIGH);
    Serial.println("HYDRATED");
  }
  else if(moist < 400){                                                 //soil is at damp level dont water
    digitalWrite(pump,HIGH);
    Serial.println("VERY HYDRATED!");
  }
}

void printdata(){                                                       // prints all the necessary data
  Serial.print(moist);                                                // moisture, humidity,temperature, timer
  Serial.print(" Moisture Reading ");
  Serial.println();
  Serial.print(hum);
  Serial.print(" % humidity ");
  Serial.println();
  Serial.print(temp);
  Serial.print(" Degrees C ");
  Serial.println();
  Serial.print(HtId);
  Serial.print(" actual Degrees C ");
  Serial.println();
  Serial.print("Timer is: "); 
  Serial.print(rtime); 
  Serial.print(" Minutes");
}


void screen(){                                                       // should print out two lines at a time then reset to print next lines
  lcd.clear();
  lcd.home();
/*  switch (lock0) {                                                    // displays next check and timer
      case 0:
         lcd.print("Last check at " //time);                            !!! extra feature may need real time clock module
         Serial.println("LC " //time);
         break;
      case 1:
         lcd.print("LW " //time);
         Serial.println("LW " //time);
         lock0 = 0;
         break;
  }*/
  lcd.print(moist);
  lcd.print("Moisture ");
  lcd.print(hum);
  lcd.print("%Humidity ");
  lcd.print(temp);
  lcd.print("Deg C ");
  lcd.setCursor(0,1);
  lcd.print("Timer set to " );  
  lcd.print(rtime,DEC);
  lcd.setCursor(0,0); 
  delay(1000);
  for(int j =0;j<80;j++){
   lcd.scrollDisplayLeft();
   delay(400);
  }
}


