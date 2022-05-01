#include <Bridge.h>
#include <HttpClient.h>


HttpClient client;

// --------------------------------------------------------------------------------------------------
// SET CONFIGURATION PARAMETERS
// --------------------------------------------------------------------------------------------------

String server           = "http://192.168.1.100";                       // server address
String route            = "/arduino/action.php";                        // server url path
String node_name        = "node_1";                                     // Arduino name

int btn_pin_2           = 2;                                            // button pin 2
int btn_pin_3           = 3;                                            // button pin 3
int btn_pin_5           = 5;                                            // button pin 5
int btn_pin_6           = 6;                                            // button pin 6
int btn_pin_7           = 7;                                            // button pin 7

int btn_state_2         = 1;                                            // default button states
int btn_state_3         = 1;                                            // ..
int btn_state_5         = 1;                                            // ..
int btn_state_6         = 1;                                            // ..
int btn_state_7         = 1;                                            // ..

int led                 = 8;                                            // indicator led
int sensorPin           = A0;                                           // analogue sensor pin
int sensorValue         = 0;                                            // sensor reading value
                        
int levels[4]           = {400, 600, 800, 1000};                        // 25%, 50%, 75%, 100% 
int send_level[4]       = {0, 0, 0, 0};                                 // send status

// --------------------------------------------------------------------------------------------------
// END CONFIGURATION PARAMETERS
// --------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------
// SETUP
// --------------------------------------------------------------------------------------------------

void setup() {

  // Bridge takes about two seconds to start up
  // it can be helpful to use the on-board LED
  // as an indicator for when it has initialized
  
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  
  Bridge.begin();
  
  digitalWrite(13, HIGH);
  
  // BUTTON PINS SETUP
  pinMode(btn_pin_2, INPUT);
  pinMode(btn_pin_3, INPUT);
  pinMode(btn_pin_5, INPUT);
  pinMode(btn_pin_6, INPUT);
  pinMode(btn_pin_7, INPUT);
  
  // LED INDICATOR
  pinMode(led, OUTPUT);

  SerialUSB.begin(9600);

  while (!Serial); // wait for a serial connection

  SerialUSB.println("PROGRAM IS RUNNING");
} // end setup


// --------------------------------------------------------------------------------------------------
// CUSTOM F UNCTIONS
// --------------------------------------------------------------------------------------------------

void press_button(int button, int btn_value) {
  /*
    button      - button pin number
    btn_value   - button pin state value

    this is debounce funcion for buttons
    it reads button state and send request to server
  */
  
  int read_value = digitalRead(button);
  
  delay(100);
  
  if (read_value == btn_value) {
    // create request url
    String request = server + route + "?name="+node_name+"&btn=btn_" + String(button) + "&state=" + String(read_value);

    // Print request url for debug
    SerialUSB.println(request);
    
    // CALL SEND REQUEST FUNCTION
    make_request(request);
    
    delay(200);
  }
} // end press_button



void read_level(){
  /*
    this is funcion read analogue pin value
    and send request to server in apropriate range

    also sets send_level value to avoid 
    second time sending data, 
    this means, that it will send request in one range only once
  */
  
  // READ ANALOGUE PIN
  sensorValue = analogRead(sensorPin);
  SerialUSB.println(sensorValue);
  bool send_data = false;               // for checking send data or not end of the function

  // 0-25%
  if( sensorValue < levels[0] and send_level[0] == 0 ){
    send_level[0] = 1;
    send_level[1] = 0;
    send_level[2] = 0;
    send_level[3] = 0;
    send_data = true;
    SerialUSB.println("LEVEL 0-25%");
  }
  
  // 25-50%
  if( sensorValue >= levels[0] && sensorValue < levels[1] && send_level[1] == 0){
    send_level[0] = 0;
    send_level[1] = 1;
    send_level[2] = 0;
    send_level[3] = 0;
    send_data = true;
    SerialUSB.println("LEVEL 25-50%");
  }

  // 50-75%
  if( sensorValue >= levels[1] && sensorValue < levels[2] && send_level[2] == 0){
    send_level[0] = 0;
    send_level[1] = 0;
    send_level[2] = 1;
    send_level[3] = 0;
    send_data = true;
    SerialUSB.println("LEVEL 50-75%");
  }

  // 75-100%
  if( sensorValue >= levels[3] && send_level[3] == 0){
    send_level[0] = 0;
    send_level[1] = 0;
    send_level[2] = 0;
    send_level[3] = 1;
    send_data = true;
    SerialUSB.println("LEVEL 75-100%");
  }


  // SEND DATA
  if( send_data == true ){

    // create request url
    String request = server + route + "?name="+node_name+"&value=" + String(sensorValue);
    
    // CALL SEND REQUEST FUNCTION
    make_request(request);
    
    delay(200);
  }
}


void status_led(){
  /* 
     this function blink led pin
     if responce data from server is "#OK"
  */
   
  for( int i=0; i<2; i++){
    digitalWrite(led, HIGH);
    delay(50);
    digitalWrite(led, LOW);
    delay(50);
  }
}



void make_request(String request)
{
  // Make a HTTP request:
  
  client.get(request);

  // if there are incoming bytes available
  // from the server, read them and print them:
  
  String responce = "";
  
  while (client.available()) {
    char c = client.read();
    responce += c;
  }
  
  if( responce != "" && responce.endsWith("#OK") ){
    status_led();
    SerialUSB.println(responce);
  }
  
  SerialUSB.flush();
} // end make_request

// --------------------------------------------------------------------------------------------------
// END CUSTOM FUNCTIONS
// --------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------
// MAIN LOOP FUNCTION
// --------------------------------------------------------------------------------------------------

void loop() {

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
  

  // Read analogue sensor value
  read_level();
  
  SerialUSB.flush();
}
// --------------------------------------------------------------------------------------------------
// END MAIN LOOP FUNCTION
// --------------------------------------------------------------------------------------------------
