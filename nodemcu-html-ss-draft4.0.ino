/* For 5.0 .....
 *  Create an online login where users enter their nodemcu product id, wifi credentials, email, etc. 
 *  nodemcu connects through bluetooth to phone, user logs in through their phone
 *  User connects to local host server and inputs their wifi credentials, email, etc. 
 *  Localhost server sends that data back to nodemcu, setting them up as variables
 *  variables are then used in the rest of the code....
 *  HTTP GET/POST
 *  Send JSON object with user data to begin with / receive 
 *  
 */

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <ESP_Mail_Client.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>

//change the two lines below with your custom information.
//#define WIFI_SSID "LIB-4935961"
//#define WIFI_PASSWORD "c8sPtPgmfmvn"

#define WIFI_SSID "LIB-5954763"
#define WIFI_PASSWORD "dx9pvYksVyhm"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

//change the two lines below with OASIS custom information.
/* The sign in credentials */
#define AUTHOR_EMAIL "oasisemailsender@gmail.com"
#define AUTHOR_PASSWORD "O@$i$_123"

//change the line below with your custom information.
/* Recipient's email*/
#define RECIPIENT_EMAIL "jonatin23@gmail.com"

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

/* Declare the session config data */
ESP_Mail_Session session;

/* Declare the message class */
SMTP_Message message;

SoftwareSerial s(D6,D5);

const char* ssid     = "LIB-4935961";
const char* password = "c8sPtPgmfmvn";
//float measurement; // turn this into an int and convert it to float here 
//bool overflow_flag;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

// Assign output variables to GPIO pins
//const int output5 = 5;
//const int output4 = 4;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

bool overflow_data;
bool tankEmpty_data;
int water_level;
int tankDepth;

void setup(){
  Serial.begin(9600);
  s.begin(9600);
  while (!Serial) continue;
  
  Serial.println();
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }

  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1
  */
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
//  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  /* Declare the message class */
//  SMTP_Message message;

//  /* Set the message headers */
//  message.sender.name = "OASIS";
//  message.sender.email = AUTHOR_EMAIL;
//  message.subject = "tank overflow";
//  message.addRecipient("Granjero", RECIPIENT_EMAIL);

  /*
  //Send HTML message
  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Hello World!</h1><p>- Sent from ESP board</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  */
  
  /*
  //Send raw text message
  String textMsg = "Hello World! - Sent from ESP board";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;*/

  /* Set the custom message header */
  //message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

  /* Connect to server with the session config */
//  if (!smtp.connect(&session))
//    return;
//
//  /* Start sending Email and close the session */
//  if (!MailClient.sendMail(&smtp, &message))
//    Serial.println("Error sending Email, " + smtp.errorReason());
}

void autoconnect_to_client() {
  
}

void set_email_info(char data[]){
    message.sender.name = "OASIS";
    message.sender.email = AUTHOR_EMAIL;
    message.addRecipient("Granjero", RECIPIENT_EMAIL);
  if (data == "overflow"){
    message.subject = "Tank overflow.";
  }
  else if (data == "tankEmpty"){
    message.subject = "Tank empty.";
  }
  else if (data == "check system"){
    message.subject = "Our sensors have detected a problem with your system. ";
  }
}

void send_email() {
  if (smtp.connect(&session)) {
    if (!MailClient.sendMail(&smtp, &message)){
      Serial.println("Error sending Email, " + smtp.errorReason());
        return;
      }
  }
  delay(10000);
}


void handle_emails(){
  if (overflow_data){
    set_email_info("overflow");
  }
  
  else if (tankEmpty_data){
    set_email_info("tankEmpty");
  }

  else if (water_level > tankDepth){
    set_email_info("check system");
  }
  else {
    return;
  }
  send_email();
}

void loop(){
  
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);
  if (root == JsonObject::invalid()){
    jsonBuffer.clear();
    return;
  }

  root.prettyPrintTo(Serial);

  overflow_data = root["overflow_f"];
  tankEmpty_data = root["tank_empty"];
  water_level = root["measurement"];
  tankDepth = root["tank_depth"];
  

  handle_emails();

  //float measurement = root["waterlevel"];
  //Serial.println(measurement);
  //bool overflow_state = root["overflow_f"];
  //Serial.println(overflow_state);
  
//  int overflow_flag = root["overflow_f"];
//  Serial.println(measurement);
  
//  WiFiClient client = server.available();   // Listen for incoming clients
//
//  if (client) {                             // If a new client connects,
//    Serial.println("New Client.");          // print a message out in the serial port
//    String currentLine = "";                // make a String to hold incoming data from the client
//    currentTime = millis();
//    previousTime = currentTime;
//    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
//      currentTime = millis();         
//      if (client.available()) {             // if there's bytes to read from the client,
//        char c = client.read();             // read a byte, then
//        Serial.write(c);                    // print it out the serial monitor
//        header += c;
//        if (c == '\n') {                    // if the byte is a newline character
//          // if the current line is blank, you got two newline characters in a row.
//          // that's the end of the client HTTP request, so send a response:
//          if (currentLine.length() == 0) {
//            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
//            // and a content-type so the client knows what's coming, then a blank line:
//            client.println("HTTP/1.1 200 OK");
//            client.println("Content-type:text/html");
//            client.println("Connection: close");
//            client.println();
//            
//            // turns the GPIOs on and off
//            if (header.indexOf("GET /5/on") >= 0) {
//              Serial.println("GPIO 5 on");
////              output5State = "on";
////              digitalWrite(output5, HIGH);
//            } else if (header.indexOf("GET /5/off") >= 0) {
//              Serial.println("GPIO 5 off");
////              output5State = "off";
////              digitalWrite(output5, LOW);
//            } else if (header.indexOf("GET /4/on") >= 0) {
//              Serial.println("GPIO 4 on");
////              output4State = "on";
////              digitalWrite(output4, HIGH);
//            } else if (header.indexOf("GET /4/off") >= 0) {
//              Serial.println("GPIO 4 off");
////              output4State = "off";
////              digitalWrite(output4, LOW);
//            }
//            
//            // Display the HTML web page
//            client.println("<!DOCTYPE html><html>");
//            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
//            client.println("<link rel=\"icon\" href=\"data:,\">");
//            // CSS to style the on/off buttons 
//            // Feel free to change the background-color and font-size attributes to fit your preferences
//            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
//            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
//            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
//            client.println(".button2 {background-color: #77878A;}</style></head>");
//            
//            // Web Page Heading
//            client.println("<body><h1>Data display page</h1>");
//            
//            // Display current state, and ON/OFF buttons for GPIO 5  
//            client.println("<p>Average measurement = </p>");
//            // If the output5State is off, it displays the ON button       
////            if (output5State=="off") {
////              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
////            } else {
////              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
////            } 
//               
//            // Display current state, and ON/OFF buttons for GPIO 4  
////            client.println("<p>GPIO 4 - State " + output4State + "</p>");
//            // If the output4State is off, it displays the ON button       
////            if (output4State=="off") {
////              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
////            } else {
////              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
////            }
//            client.println("</body></html>");
//            
//            // The HTTP response ends with another blank line
//            client.println();
//            // Break out of the while loop
//            break;
//          } else { // if you got a newline, then clear currentLine
//            currentLine = "";
//          }
//        } else if (c != '\r') {  // if you got anything else but a carriage return character,
//          currentLine += c;      // add it to the end of the currentLine
//        }
//      }
//    }
//    // Clear the header variable
//    header = "";
//    // Close the connection
//    client.stop();
//    Serial.println("Client disconnected.");
//    Serial.println("");
//  } 
}

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");
  }
}
