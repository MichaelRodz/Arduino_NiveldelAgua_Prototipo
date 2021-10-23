#define SensorPin A0
unsigned int sensorValue;
//int valvePin = 13;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(valvePin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  sensorValue = analogRead(A0);
  Serial.println(sensorValue);

//  if(sensorValue < 200){
//    digitalWrite(valvePin, HIGH);
//  }
//  else{
//    digitalWrite(valvePin, LOW);
//  }
}
