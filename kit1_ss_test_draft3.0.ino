#include <LiquidCrystal.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

SoftwareSerial s(19,18);
// 19 a D5 18 a D6

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2; // LCD pins 
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

unsigned char ok_flag;
unsigned char fail_flag;
unsigned short lenth_val = 0;
unsigned char i2c_rx_buf[16];
unsigned char dirsend_flag=0;

const int alarm_button = A0;
//const int echo_pin = 8;
//const int trig_pin = 9;
const int alarm = 13;
const int overflow_pin = 7;
const int tankEmpty_pin = 6;
bool overflow_emergency = false;
bool tank_empty = false;
int travel_time;
float distance;
float distance_to_target;
// bool tank_is_empty; //  tank_is_empty stores container fill state
float average_measurement;
float tankDepth;
float percent;
char average_measurement_string[5];


void setup() {

lcd.begin(16, 2);
//pinMode(trig_pin, OUTPUT);
//pinMode(echo_pin, INPUT);
pinMode(alarm, OUTPUT);
pinMode(alarm_button, INPUT);
pinMode(overflow_pin, INPUT);
pinMode(tankEmpty_pin, INPUT_PULLUP);

Wire.begin(); 
Serial.begin(9600,SERIAL_8N1);  
s.begin(9600);
printf_begin();

delay(1000);

tankDepth = average_reading(10);

}

/*
  getDistance() returns the conversion from speed of sound to feet 
*/

//float getDistance(float travel_time){
//  distance = (travel_time*765.*5280.*12.)/(3600.*1000000.);
//  distance_to_target = distance/2;
//  return distance_to_target;
//}

/*
getReading() returns a reading from the sensor 
*/

//unsigned long getReading(){
//  digitalWrite(trig_pin,LOW);
//  delayMicroseconds(10);
//  digitalWrite(trig_pin,HIGH);
//  delayMicroseconds(10);
//  digitalWrite(trig_pin,LOW);
//
//  return pulseIn(echo_pin,HIGH);
//}

/*
average_reading() returns an average of every 100 readings from the sound sensor
*/

void handle_nodemcu_connection(){
sprintf(average_measurement_string, "%f", average_measurement);;
  if (s.available() > 0) {
    s.write(average_measurement_string);
  }  
}

float average_reading(int n){
  average_measurement = 0;
  for(int i = 0; i < n; i++){
    //travel_time = getReading();
    average_measurement += ReadDistance();
  }
  average_measurement = average_measurement / n;
  average_measurement = average_measurement / 25.4; //convierte la distancia promedio de mm a pulgadas.
  
   handle_nodemcu_connection();
   return average_measurement;
}

void handle_valve() {
  if (calculate_percent() <= 50) {
     digitalWrite(alarm, HIGH);
  }
  else if (calculate_percent() >= 90){
    digitalWrite(alarm, LOW);
  }
}

/*
  check_if_tank_is_empty checks if the tank is less than half full, toggles the alarm variable, and calls the toggle_alarm() function
*/

/*
bool check_if_tank_is_empty(){
   if (average_measurement >= 6) {
     tank_is_empty = true;
   }
    if (average_measurement < 6) {      
     tank_is_empty = false;
   }
}
*/

/* 
toggle_alarm() toggles the alarm on and off pending the tank being empty and the switch status
*/
// 9 6 4.5 3 
float calculate_percent() {
  float temp = tankDepth-average_measurement;
  percent = (temp/tankDepth)*100;
  return percent;
}
void display_WL(){
  lcd.setCursor(0,0);
  if (!(average_measurement > (tankDepth))) {
    lcd.print("Wl: ");
    lcd.print(calculate_percent());
    lcd.print("%");
  }
}

void display_warning() {
   lcd.setCursor(0,0);
  if (average_measurement > (tankDepth)){
    lcd.print("WL: CHKSYS  "); 
    digitalWrite(alarm, HIGH);
  }
}

void display_reading() {
  lcd.setCursor(0,1);
  lcd.print(average_reading(10));
}

void handle_display() {
  display_reading();
  display_warning();
  display_WL();
}

void check_overflow(){
  int overflow_value = digitalRead(overflow_pin);
  
  if (overflow_value == 1){
    overflow_alarm();
  }
  else{
    overflow_emergency = false;
  }
}

void overflow_alarm() {
  digitalWrite(alarm, LOW);
  lcd.setCursor(0,0);
  lcd.print("OVERFLOW  ");
  Serial.print("OVERFLOW  ");
  overflow_emergency = true;
  
//  delay(500);
}

void tank_empty_alarm() {
  digitalWrite(alarm, HIGH);
  lcd.setCursor(0,0);
  lcd.print("TANK EMPTY");
  Serial.print("TANK EMPTY");
  tank_empty = true;
  
//  delay(500);
}


void handleJson() {
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
//  root["waterlevel"] = average_measurement;
//  root["overflow_f"] = overflow_emergency;
if (isnan(average_measurement)) {
    return;
  }
  root["waterlevel"] = average_measurement;
  root["overflow_f"] = overflow_emergency;
  root["tank_empty"] = tank_empty;
  root["tank_depth"] = tankDepth;
    
  root.printTo(s);
  jsonBuffer.clear();

  delay(2000);
}


void check_tank_empty() {
  int tank_empty_value = digitalRead(tankEmpty_pin);
  
  if (tank_empty_value == 1){
    tank_empty_alarm();
  }
  else{
    tank_empty = false;
  }
}

int serial_putc( char c, struct __file * )
{
  Serial.write( c );
  return c;
}

void printf_begin(void)
{
  fdevopen( &serial_putc, 0 );
}



void SensorRead(unsigned char addr,unsigned char* datbuf,unsigned char cnt) 
{
  unsigned short result=0;
  // step 1: instruct sensor to read echoes
  Wire.beginTransmission(82); // transmit to device #82 (0x52)
  // the address specified in the datasheet is 164 (0xa4)
  // but i2c adressing uses the high 7 bits so it's 82
  Wire.write(byte(addr));      // sets distance data address (addr)
  Wire.endTransmission();      // stop transmitting
  // step 2: wait for readings to happen
  delay(1);                   // datasheet suggests at least 30uS
  // step 3: request reading from sensor
  Wire.requestFrom(82, cnt);    // request cnt bytes from slave device #82 (0x52)
  // step 5: receive reading from sensor
  if (cnt <= Wire.available()) { // if two bytes were received
    *datbuf++ = Wire.read();  // receive high byte (overwrites previous reading)
    *datbuf++ = Wire.read(); // receive low byte as lower 8 bits
  }
}

int ReadDistance(){
    SensorRead(0x00,i2c_rx_buf,2);
    lenth_val=i2c_rx_buf[0];
    lenth_val=lenth_val<<8;
    lenth_val|=i2c_rx_buf[1];
    delay(300); 
    return lenth_val;
}


void loop() {
  check_overflow();
  if (!overflow_emergency){
    Serial.println(average_measurement);
    handle_display();
    handle_valve();
  }
  check_tank_empty();
  handleJson();
}
