#include <SPI.h>
#include <Keypad.h>
#include <Ethernet.h>
#include <LiquidCrystal.h>


// ==========================
// == MOTOR CONFIG
// ==========================

int const MOTOR_POLES = 4;
int const MOTOR_STEPS = 4;

int const STEP [ MOTOR_STEPS ][ MOTOR_POLES ] = {
  {1, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 1},
  {1, 0, 0, 1}
};

int const GEAR_RATIO = 64;
int const SHAFT_STEPS = 8;

int const SHAFT_MOTOR_STEPS_RATIO = GEAR_RATIO * MOTOR_STEPS;
int const MOTOR_TURN_DELAY = 1;

boolean turnRight, turnLeft;
int shaftPositionX = 0;
int motorStepX = 0;

const int m1 = 53;
const int m2 = 51;
const int m3 = 49;
const int m4 = 47;

boolean turnUp, turnDown;
int shaftPositionY = 0;
int motorStepY = 0;

const int w1 = 23;
const int w2 = 25;
const int w3 = 27;
const int w4 = 29;


// ==========================
// == KEYPAD CONFIG
// ==========================

const int ROWS = 4;
const int COLS = 3;

const char keys[ROWS][COLS] = {
  {'*', '0', '#'},
  {'7', '8', '9'},
  {'4', '5', '6'},
  {'1', '2', '3'},
};

byte colPins[COLS] = {40, 42, 44};
byte rowPins[ROWS] = {38, 36, 34, 32};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const char KEY_UP    = '8';
const char KEY_DOWN  = '2';
const char KEY_LEFT  = '6';
const char KEY_RIGHT = '4';

// ==========================
// == LCD CONFIG
// ==========================

const int D4 = A13;
const int D5 = A12;
const int D6 = A11;
const int D7 = A10;

const int RS = A9;
const int EN = A8;

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);


// ==========================
// == ETHERNET CONFIG
// ==========================

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xE1 };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:

byte SERVER[] = {10, 0, 1, 5};
IPAddress serverAddress(SERVER[0], SERVER[1], SERVER[2], SERVER[3]);


EthernetClient client;
EthernetServer server = EthernetServer(80);

const int PORT = 4000;

String RESPONSE_UP    = "@up";
String RESPONSE_DOWN  = "@down";
String RESPONSE_LEFT  = "@left";
String RESPONSE_RIGHT = "@right";
String RESPONSE_NOP = "@nop";
String RESPONSE_SNAP = "@snap";


// ==========================
// == GLOBAL VARS
// ==========================

long time = 0;

const int TEST_MODE = 1;
const int MAINT_MODE = 2;
const int NORMAL_MODE = 4;

const int SELECT_MODE = 0;


int MODE = SELECT_MODE;


// ==========================
// == SETUP
// ==========================

void setup() {
  setupSerial();
  setupLCD();
  setupMotor();
  setupEthernet();
  setupEthernetServer();
}

void setupSerial() {
  Serial.begin(9600);
  while (!Serial) {}
  Serial.println("...Starting...");
}

void setupMotor() {
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(m3, OUTPUT);
  pinMode(m4, OUTPUT);
  pinMode(w1, OUTPUT);
  pinMode(w2, OUTPUT);
  pinMode(w3, OUTPUT);
  pinMode(w4, OUTPUT);
  Serial.println("Motor Configured");
}

void setupLCD() {
  Serial.println("Setup LCD");
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Hello Moto!");
  Serial.println("LCD Started");
}

void setupEthernet() {
  Serial.println("Setup Ethernet");
  Ethernet.begin(mac);
  Serial.print("Connected to network. Assigned IP: ");
  Serial.println(Ethernet.localIP());
  delay(1000);
  Serial.println("Ethernet started");
}

void setupEthernetServer() {
  Serial.println("Setup Ethernet server");
  server.begin();
  Serial.println("Ethernet server started");
}

// ==========================
// == LOOP
// ==========================

int loopCount = 0;

void loop() {
  
  if (MODE == 0) MODE = print_set_mode();

  if (MODE == TEST_MODE) test_mode();
  if (MODE == MAINT_MODE) maint_mode();
  if (MODE == NORMAL_MODE) normal_mode();
}

void test_mode() {
  Serial.println("Iniciando pruebas del modo test");
  Serial.println("Prueba de ping");
  long test_started_time = millis();
  long time_running = 0;
  while (time_running <= 10 * 1000) {
    long start_time = millis();
    make_request("/api/ping");
    long end_time = millis();
    Serial.print("Ping: ");
    Serial.print(end_time - start_time);
    Serial.println("ms");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("Ping: ");
    lcd.print(end_time - start_time);
    lcd.print("ms");
    time_running = millis() - test_started_time;
  }
  int turnNo;

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);

  Serial.println("Prueba de motor eje X");
  for (turnNo = 0; turnNo < SHAFT_STEPS; turnNo++) {
    move_from_command(KEY_RIGHT);
  }

  for (turnNo = 0; turnNo < SHAFT_STEPS; turnNo++) {
    move_from_command(KEY_LEFT);
  }

  Serial.println("Prueba de motor eje Y");
  for (turnNo = 0; turnNo < SHAFT_STEPS; turnNo++) {
    move_from_command(KEY_UP);
  }

  for (turnNo = 0; turnNo < SHAFT_STEPS; turnNo++) {
    move_from_command(KEY_DOWN);
  }

  Serial.println("Fin pruebas del motor");
  MODE = SELECT_MODE;
}

void maint_mode() {
  char key = keypad.getKey();
  move_from_command(key);
}

void normal_mode() {
  String command = make_request("/api/command?x=" + String(shaftPositionX) + "&y=" + String(shaftPositionY));

  move_from_response(command);

  if (command == RESPONSE_SNAP) {
    snap_photo();
  }

  if (command == RESPONSE_NOP) {
    Serial.println("Aletargando frecuencia de polling");
    delay(3000);
  }
}

void snap_photo(){
  Serial.println("Sacando foto");
  /*
  make_request("/photo");
  handlePhotoRequest();
  */
  String filename = String(shaftPositionX) + "_" + String(shaftPositionY) + ".jpg";
  notify_new_photo("/api/photo", filename);
}

char get_char_code_from(String response) {
  if (response == RESPONSE_UP)    return KEY_UP;
  if (response == RESPONSE_DOWN)  return KEY_DOWN;
  if (response == RESPONSE_LEFT)  return KEY_LEFT;
  if (response == RESPONSE_RIGHT) return KEY_RIGHT;
  return '.';
}

void print_shafts_position_in_lcd() {

  lcd.setCursor(8, 1);
  lcd.print(" X:");
  lcd.print(shaftPositionX);
  lcd.print(" Y:");
  lcd.print(shaftPositionY);

  Serial.print(" X:");
  Serial.print(shaftPositionX);
  Serial.print(" Y:");
  Serial.println(shaftPositionY);

}

int print_set_mode() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ingrese modo");
  Serial.println("Ingrese modo");
  char key;
  do {
    key = keypad.getKey();
  } while (key != '#' && key != '0' && key != '*');

  Serial.print("Presionaron: ");
  Serial.println(key);
  lcd.clear();
  lcd.setCursor(0, 0);

  if (key == '#') {
    lcd.print("Test");
    return TEST_MODE;
  }
  if (key == '0') {
    lcd.print("Maint");
    return MAINT_MODE;
  }
  if (key == '*') {
    lcd.print("Normal");
    return NORMAL_MODE;
  }
}

void lcd_print_if(char key, char expected, String message) {
  if (key == expected) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message);
    Serial.println(message);
  }
}

void move_from_command(char key) {

  turnUp    = key == KEY_UP;
  turnDown  = key == KEY_DOWN;
  turnLeft  = key == KEY_LEFT;
  turnRight = key == KEY_RIGHT;

  if (turnLeft || turnRight || turnDown || turnUp) {
    move_shaft();
    print_shafts_position_in_lcd();
  }

}

void move_from_response(String command) {
  Serial.println("Aplicando: " + command);

  turnUp    = command == RESPONSE_UP;
  turnDown  = command == RESPONSE_DOWN;
  turnLeft  = command == RESPONSE_LEFT;
  turnRight = command == RESPONSE_RIGHT;

  if (turnLeft || turnRight || turnDown || turnUp) {
    move_shaft();
    print_shafts_position_in_lcd();
  }

}

void move_shaft() {
  int turnNo;
  if ((turnRight && shaftPositionX <= 7) || (shaftPositionX > 0 && turnLeft) || (turnUp && shaftPositionY <= 7) || (shaftPositionY > 0 && turnDown)) {
    for (turnNo = 0; turnNo < SHAFT_MOTOR_STEPS_RATIO; turnNo++) {
      move_motor();
      delay(2);
    }
    if (turnRight) {
      shaftPositionX++;
    } else if (turnLeft) {
      shaftPositionX--;
    } else if (turnUp) {
      shaftPositionY++;
    } else if (turnDown) {
      shaftPositionY--;
    }
  }
}

void move_motor() {
  digitalWrite( m1, STEP[motorStepX][0] );
  digitalWrite( m2, STEP[motorStepX][1] );
  digitalWrite( m3, STEP[motorStepX][2] );
  digitalWrite( m4, STEP[motorStepX][3] );
  digitalWrite( w1, STEP[motorStepY][0] );
  digitalWrite( w2, STEP[motorStepY][1] );
  digitalWrite( w3, STEP[motorStepY][2] );
  digitalWrite( w4, STEP[motorStepY][3] );

  if (turnRight) {
    motorStepX++;
  } else if (turnLeft) {
    motorStepX--;
  } else if (turnUp) {
    motorStepY++;
  } else if (turnDown) {
    motorStepY--;
  }
  motorStepX = ( motorStepX + 4 ) % 4 ;
  motorStepY = ( motorStepY + 4 ) % 4 ;
}

String make_request(String endpoint) {

  boolean nextIsResponse = false;
  String response = "";

  while (true) {

    long elapsed_time = millis() - time;

    if (elapsed_time >= 1 * 1000) {
      time = millis();
      while (!client.connected()) {
        if (client.connect(serverAddress, PORT)) {
          Serial.println("Making request");
          client.println("GET " + endpoint + " HTTP/1.1");
          client.println("Host: " + String(SERVER[0]) + "." + String(SERVER[1]) + "."  + String(SERVER[2]) + "." + String(SERVER[3]));
          client.println("Connection:close");
          client.println();
        } else {
          Serial.println("Error making the request");
        }
      }

      while (!client.available()) {}

      while (client.available()) {
        char c = client.read();
        nextIsResponse = nextIsResponse || c == '@';
        if (nextIsResponse) {
          response += c;
        }
      }
      client.stop();

      return response;
    }
  }
}

void handlePhotoRequest(){
  EthernetClient clientConnection = server.available();
  while(!clientConnection) {
    Serial.println("Esperando conexion entrante");
    delay(2000);
    clientConnection = server.available();
  }
  
  Serial.println("Conexión detectada");
  Serial.println("Recibiendo request");
  int newLineCount = 0;
  boolean termino_request;
  while (clientConnection.connected()) {
    if (clientConnection.available()) {
      char c = clientConnection.read();

      if (c == '\n') {
        newLineCount++;
      } else if (c != '\r') {
        newLineCount = 0;
      }
      
      termino_request = newLineCount == 2;
      if (termino_request) {
        Serial.println("Request recibida. Enviando foto");

        clientConnection.println("HTTP/1.1 200 OK");
        clientConnection.println("Content-Type: text/plain");
        clientConnection.println("Connection: close");  // the connection will be closed after completion of the response
        clientConnection.println();
        clientConnection.println("Foo");
        break;
      }
    }
  }
  Serial.println("Foto enviada");
  delay(10);
  clientConnection.stop();
  Serial.println("Conexión cerrada");
}

void notify_new_photo(String endpoint, String file_name){
  Serial.println("Notificando nueva foto");
  String data = "filename=" + file_name;

  while (!client.connected()) {
    if (client.connect(serverAddress, PORT)) {
      Serial.println("Enviando nueva foto");
      client.println("PUT " + endpoint + " HTTP/1.1");
      client.println("Host: " + String(SERVER[0]) + "." + String(SERVER[1]) + "."  + String(SERVER[2]) + "." + String(SERVER[3]));
      client.println("User-Agent: Arduino/1.0"); 
      client.println("Connection: close");
      client.println("Content-Type: application/x-www-form-urlencoded");  
      client.print("Content-Length: "); 
      client.println(data.length());
      client.println();
      client.println(data);    
    } else {
      Serial.println("Error making the request");
    }
  }

  while (!client.available()) {}

  while (client.available()) {client.read();}
  Serial.println("Foto enviada");
  
  delay(10);
  client.stop();
}
