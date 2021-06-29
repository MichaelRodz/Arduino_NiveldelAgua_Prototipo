// int alarmButton = 10;
int echoPin = 11;
int trigPin = 12;
int alarm = 13;
int travelTime;
float distance;
float distanceToTarget;
// bool buttonToggle;
bool tankIsEmpty; //  tankIsEmpty checks if container is filled or not 
float averageMeasurement;


void setup() {
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
pinMode(alarm, OUTPUT);
// pinMode(alarmButton, INPUT);
// buttonToggle = true;
Serial.begin(9600);

}

/*
  getDistance() returns the 
*/

float getDistance(float travelTime_){
  distance = (travelTime_*765.*5280.*12.)/(3600.*1000000.);
  distanceToTarget = distance/2;
  return distanceToTarget;
}

/*
averageReading() returns an average of every 500 readings from the sound sensor
*/

float averageReading(){
    averageMeasurement = 0;
  for(int i = 0; i < 200; i++){
    digitalWrite(trigPin,LOW);
    delayMicroseconds(10);
    digitalWrite(trigPin,HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin,LOW);
    travelTime = pulseIn(echoPin,HIGH);
    delay(3);
    averageMeasurement += getDistance(travelTime);
  }
  averageMeasurement = averageMeasurement / 200;
  checktankIsEmpty(averageMeasurement);
  return (averageMeasurement);
}

/*
  Check tankIsEmpty checks if the tank is less than half full, toggles the alarm variable, and calls the toggleAlarm() function
*/

bool checktankIsEmpty(float averageMeasurement_){
   if (averageMeasurement_ >= 2) {
     tankIsEmpty = true;
   }
    if (averageMeasurement_ < 2) {      
     tankIsEmpty = false;
   }
   toggleAlarm();
}

/* 
toggleAlarm() toggles the alarm on and off if the tank is empty or full respectively
*/

void toggleAlarm(){
  if (tankIsEmpty/* && buttonToggle*/) {
    Serial.println("Tank is Empty!");
    digitalWrite(alarm,HIGH);
  }
  else {
    digitalWrite(alarm,LOW);  
  }
}

void loop() {
  // Serial.println(buttonToggle);
  Serial.println(averageReading());
  // toggleButton();
}

/*

  toggleButton() checks every tick if the button was pressed and toggles the alarm ON/OFF variable ONLY IF the alarm is already on, preventing accidental button presses 

*/

// void toggleButton() {
//   if (tankIsEmpty) {
//     if (digitalRead(alarmButton) == HIGH) {
//       buttonToggle = !buttonToggle;
//     }
//   }
// }
