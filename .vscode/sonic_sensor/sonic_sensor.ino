#include <LiquidCrystal.h>


const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2; // LCD pins 
const int alarm_button = A0;
const int echo_pin = 8;
const int trig_pin = 9;
const int alarm = 13;
int travel_time;
float distance;
float distance_to_target;
bool tank_is_empty; //  tank_is_empty stores container fill state
float average_measurement;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void setup() {

lcd.begin(16, 2);
pinMode(trig_pin, OUTPUT);
pinMode(echo_pin, INPUT);
pinMode(alarm, OUTPUT);
pinMode(alarm_button, INPUT);

Serial.begin(9600);

}

/*
  getDistance() returns the conversion from speed of sound to feet 
*/

float getDistance(float travel_time){
  distance = (travel_time*765.*5280.*12.)/(3600.*1000000.);
  distance_to_target = distance/2;
  return distance_to_target;
}

/*
getReading() returns a reading from the sensor 
*/

unsigned long getReading(){
  digitalWrite(trig_pin,LOW);
  delayMicroseconds(10);
  digitalWrite(trig_pin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin,LOW);

  return pulseIn(echo_pin,HIGH);
}

/*
average_reading() returns an average of every 100 readings from the sound sensor
*/

float average_reading(){
    average_measurement = 0;
  for(int i = 0; i < 100; i++){
    travel_time = getReading();
    average_measurement += getDistance(travel_time);
  }
  average_measurement = average_measurement / 100;
  return (average_measurement);
}

/*
  check_if_tank_is_empty checks if the tank is less than half full, toggles the alarm variable, and calls the toggle_alarm() function
*/

bool check_if_tank_is_empty(){
   if (average_measurement >= 6) {
     tank_is_empty = true;
   }
    if (average_measurement < 6) {      
     tank_is_empty = false;
   }
}

/* 
toggle_alarm() toggles the alarm on and off pending the tank being empty and the switch status
*/

void toggle_alarm(){
  /* toggle_alarm() checks if the alarm is empty, then checks if the alarm break switch is on/off to toggle the alarm on or off */
  int alarmStatus = digitalRead(A0);
  lcd.setCursor(0,0);
  if (tank_is_empty) {
    lcd.print("Tank is Empty!");
    if (alarmStatus == 1){
      digitalWrite(alarm,LOW);  
    }
    else {
      digitalWrite(alarm,HIGH);
    }
  }
  else {
    digitalWrite(alarm,LOW);  
    lcd.print("Tank is Full!");
  }

}

void loop() {
  lcd.setCursor(0,1);
  lcd.print(average_reading());
  check_if_tank_is_empty();
  toggle_alarm();
}