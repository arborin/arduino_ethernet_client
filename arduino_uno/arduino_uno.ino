/*

  PIN DESCRIPTION HERE
  ------------------------------------------------------------------------------------------
  Arduino uses digital pins 10, 11, 12, and 13 (SPI) to comm unicate with the W5100 on the
  ethernet shield. These pins cannot be used for general i/o.
  ------------------------------------------------------------------------------------------

*/

#include <SPI.h>
#include <Ethernet.h>

//------------------------------------------------------------------------------------------
// CONFIGRUATION
// NETWORKING



IPAddress ip(192, 168, 1, 10);
IPAddress subnet(255, 255, 255, 0);
IPAddress gateway(192, 168, 1, 1);
IPAddress myDns(192, 168, 1, 1);


byte server[]           = {192, 168, 1, 100};                           // static server IP
byte mac[]              = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};         // MAC Address

String url              = "/arduino/action.php";                        // server url path
String node_name        = "node_1";                                     // Arduino name

int btn_pin_2           = 2;
int btn_pin_3           = 3;
int btn_pin_5           = 5;
int btn_pin_6           = 6;
int btn_pin_7           = 7;

int btn_state_2         = 1;
int btn_state_3         = 1;
int btn_state_5         = 1;
int btn_state_6         = 1;
int btn_state_7         = 1;

int led                 = 8;
int sensorPin           = A0;                                           // analogue sensor pin
int sensorValue         = 0;                                            // sensor reading value

int period              = 10000;
unsigned long time_now  = 0;
                        
int levels[4]           = {400, 600, 800, 1000};                        // 25%, 50%, 75%, 100% 
int send_level[4]       = {0, 0, 0, 0};

EthernetClient client;

//------------------------------------------------------------------------------------------

void setup()
{
  Serial.begin(9600);
  Serial.println(">> PROGRAM START");
  SPI.begin();

  // disable sd card
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  // enable ethernet
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);

  // BUTTON PINS SETUP
  pinMode(btn_pin_2, INPUT);
  pinMode(btn_pin_3, INPUT);
  pinMode(btn_pin_5, INPUT);
  pinMode(btn_pin_6, INPUT);
  pinMode(btn_pin_7, INPUT);
  
  // LED INDICATOR
  pinMode(led, OUTPUT);


  // ETHERNET CONNECTION CHECK

  Ethernet.begin(mac, ip, gateway, subnet, myDns);
 
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }
    
  Serial.print("Assigned IP ");
  Serial.println(Ethernet.localIP());

  delay(1000);

}


void press_button(int button, int btn_value) {
  int read_value = digitalRead(button);
  delay(100);
  if (read_value == btn_value) {
    String request = "";
    // REQUEST STRING STRUCTURE
    request += "GET " + url;
    request += "?btn=btn_" + String(button);
    request += "&state=" + String(read_value);
    request += "&name=" + node_name;
    request += " HTTP/1.1";

    // CALL SEND REQUEST FUNCTION
    make_request(request);
    // DELAY 1 SECOND AFTER REQUEST
    delay(500);
  }
}


void read_level(){
  // READ ANALOGUE PIN
  sensorValue = analogRead(sensorPin);
  Serial.print("Presure Val: ");
  Serial.println(sensorValue);
  bool send_data = false;

  // 0-25%
  if( sensorValue < levels[0] and send_level[0] == 0 ){
    send_level[0] = 1;
    send_level[1] = 0;
    send_level[2] = 0;
    send_level[3] = 0;
    send_data = true;
    Serial.println("LEVEL 0-25%");
  }
  
  // 25-50%
  if( sensorValue >= levels[0] && sensorValue < levels[1] && send_level[1] == 0){
    send_level[0] = 0;
    send_level[1] = 1;
    send_level[2] = 0;
    send_level[3] = 0;
    send_data = true;
    Serial.println("LEVEL 25-50%");
  }

  // 50-75%
  if( sensorValue >= levels[1] && sensorValue < levels[2] && send_level[2] == 0){
    send_level[0] = 0;
    send_level[1] = 0;
    send_level[2] = 1;
    send_level[3] = 0;
    send_data = true;
    Serial.println("LEVEL 50-75%");
  }

  // 75-100%
  if( sensorValue >= levels[3] && send_level[3] == 0){
    send_level[0] = 0;
    send_level[1] = 0;
    send_level[2] = 0;
    send_level[3] = 1;
    send_data = true;
    Serial.println("LEVEL 75-100%");
  }



  // SEND DATA
  if( send_data == true ){
    // REQUEST STRING STRUCTURE
    String request = "";
    request += "GET " + url;
    request += "?value=" + String(sensorValue);
    request += "&name=" + node_name;
    request += " HTTP/1.1";
    
    // CALL SEND REQUEST FUNCTION
    make_request(request);
    // DELAY 1 SECOND AFTER REQUEST
    delay(500);
  }
}


void status_led(){
  // LED BLINKING 
  // WHEN WEB SERVER RESPONCE IS "#OK"
  for( int i=0; i<2; i++){
    digitalWrite(led, HIGH);
    delay(50);
    digitalWrite(led, LOW);
    delay(50);
  }
}



void loop()
{
  // READ INPUT PINS
  int read_pin_2 = digitalRead(btn_pin_2);
  int read_pin_3 = digitalRead(btn_pin_3);
  int read_pin_5 = digitalRead(btn_pin_5);
  int read_pin_6 = digitalRead(btn_pin_6);
  int read_pin_7 = digitalRead(btn_pin_7);

  if ( read_pin_2 != btn_state_2 ) {
    press_button(2, read_pin_2);
    // CHANGE PIN CURRENT STATUS
    btn_state_2 = read_pin_2;
  }

  if ( read_pin_3 != btn_state_3 ) {
    press_button(3, read_pin_3);
    // CHANGE PIN CURRENT STATUS
    btn_state_3 = read_pin_3;
  }
  
  if ( read_pin_5 != btn_state_5 ) {
    press_button(5, read_pin_5);
    // CHANGE PIN CURRENT STATUS
    btn_state_5 = read_pin_5;
  }

  if ( read_pin_6 != btn_state_6 ) {
    press_button(6, read_pin_6);
    // CHANGE PIN CURRENT STATUS
    btn_state_6 = read_pin_6;
  }

  if ( read_pin_7 != btn_state_7 ) {
    press_button(7, read_pin_7);
    // CHANGE PIN CURRENT STATUS
    btn_state_7 = read_pin_7;
  }

  read_level();
  delay(100);
  
//  if((unsigned long)(millis() - time_now) > period){
//    time_now = millis();
//    read_level();
//  }
}


void make_request(String request)
{
  if (client.connect(server, 80))
  {
    Serial.println(request);

    client.println(request);
    client.println("Host: 192.168.1.100");
    client.println("Connection: close");
    client.println();

    delay(50);

    String responce = "";  // answer variable

    while (true)
    {
      if (client.available())
      {
        char ch = client.read();
        responce += String(ch);
      }
      // if there isn't anything left to be read from the server display the message
      if (!client.connected())
      { client.stop();
        break;
      }
    }

    if(responce.endsWith("#OK")) {
        status_led();
    }

    
    Serial.println(responce);
  } else {
    Serial.println("Can't conect to web server");
  }
}
