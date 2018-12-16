#include <LiquidCrystal.h>

#include <Key.h>
#include <Keypad.h>

#include <Ethernet.h>



// ==========================
// == MOTOR CONFIG
// ==========================s

int const MOTOR_POLES = 4;
int const MOTOR_STEPS = 4;

int const STEP [ MOTOR_STEPS ][ MOTOR_POLES ] = {
  {1, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 1},
  {1, 0, 0, 1}
};

// Sensibilidad de giro
int const GEAR_RATIO = 16;
int const SHAFT_STEPS = 8;

int const SHAFT_MOTOR_STEPS_RATIO = GEAR_RATIO * MOTOR_STEPS;
int const MOTOR_TURN_DELAY = 1;

int const INITIAL_SHAFT_POSITION_X = 4;
int const INITIAL_SHAFT_POSITION_Y = 4;

boolean turnRight, turnLeft;
int shaftPositionX = INITIAL_SHAFT_POSITION_X;
int motorStepX = 0;

const int m1 = 53;
const int m2 = 51;
const int m3 = 49;
const int m4 = 47;

boolean turnUp, turnDown;
int shaftPositionY = INITIAL_SHAFT_POSITION_Y;
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


//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
LiquidCrystal lcd(A11, A10, A15, A14, A13, A12);

//LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

/*
const int D4 = A13;
const int D5 = A12;
const int D6 = A11;
const int D7 = A10;

const int RS = A9;
const int EN = A8;

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
*/

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
const int CALIBRATION_MODE = 2;
const int NORMAL_MODE = 4;

const char TEST_KEY = '7';
const char CALIBRATION_KEY = '0';
const char NORMAL_KEY = '*';
const char CONFIRMAR_KEY = '5';


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
}

void setupSerial() {
  Serial.begin(9600);
  while (!Serial) {}
  Serial.println("...Iniciando...");
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
  Serial.println("Motor configurado");
}

void setupLCD() {
  lcd.begin(16, 2);
  lcd.print("Bienvenidos");
  lcd.setCursor(0, 1);
  lcd.print("Configurando...");
  Serial.println("LCD configurado");
}

void setupEthernet() {
  Serial.println("Configurando Ethernet");
  Ethernet.begin(mac);
  Serial.print("Conectado a la red. La IP asignada es ");
  Serial.println(Ethernet.localIP());
  delay(100);
  Serial.println("Ethernet configurado");
}

// ==========================
// == LOOP
// ==========================

int loopCount = 0;

void loop() {
  
  if (MODE == 0) MODE = print_set_mode();

  if (MODE == TEST_MODE) test_mode();
  if (MODE == CALIBRATION_MODE) calibration_mode();
  if (MODE == NORMAL_MODE) normal_mode();
}

void test_mode() {
  Serial.println("Iniciando pruebas del modo test");
  lcd.clear();
  lcd.print("Test Ping");
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

  lcd.setCursor(5, 0);
  lcd.print("Movimiento");
  
  lcd.setCursor(0, 1);
  lcd.print("Eje x  ");
  Serial.println("Prueba de motor eje X");

  print_shafts_position_in_lcd();
  for (turnNo = INITIAL_SHAFT_POSITION_X; turnNo < SHAFT_STEPS; turnNo++) {
    move_from_command(KEY_RIGHT);
    print_shafts_position_in_lcd();
  }

  for (turnNo; turnNo > 0; turnNo--) {
    move_from_command(KEY_LEFT);
    print_shafts_position_in_lcd();
  }

  for (turnNo; turnNo < SHAFT_STEPS/2; turnNo++) {
    move_from_command(KEY_RIGHT);
    print_shafts_position_in_lcd();
  }

  lcd.setCursor(0, 1);
  lcd.print("Eje y  ");
  Serial.println("Prueba de motor eje Y");

  print_shafts_position_in_lcd();
  for (turnNo = INITIAL_SHAFT_POSITION_Y; turnNo < SHAFT_STEPS; turnNo++) {
    move_from_command(KEY_UP);
    print_shafts_position_in_lcd();
  }

  for (turnNo; turnNo > 0; turnNo--) {
    move_from_command(KEY_DOWN);
    print_shafts_position_in_lcd();
  }

  for (turnNo; turnNo < SHAFT_STEPS/2; turnNo++) {
    move_from_command(KEY_UP);
    print_shafts_position_in_lcd();
  }

  Serial.println("Fin pruebas del motor");
  MODE = SELECT_MODE;
}

void calibration_mode() {
  lcd.clear();
  lcd.print("Modo calibracion");
  for(;;){
    print_shafts_position_in_lcd();
    char key = wait_for_key();
    if(key == CONFIRMAR_KEY) {
      MODE = SELECT_MODE;
      set_shafts_offset();
      Serial.println("Confirmando nuevo offset para los servos");
      break;
    } else {
      move_from_command(key); 
    }
  }
}

void set_shafts_offset(){
  shaftPositionX = INITIAL_SHAFT_POSITION_X;
  shaftPositionY = INITIAL_SHAFT_POSITION_Y;
}

void normal_mode() {
  lcd.clear();
  lcd.print("Operativo!");
  print_shafts_position_in_lcd();
  for(;;) {
    String command = make_request("/api/command?x=" + String(shaftPositionX) + "&y=" + String(shaftPositionY));
    
    lcd.setCursor(0, 1);

    if (command == RESPONSE_SNAP) {      
      lcd.print("Mandar ");
      snap_photo();
    } else if (command == RESPONSE_NOP) {
      Serial.println("Aletargando frecuencia de polling");
      lcd.print("Espera ");
      delay(3000);
    } else {
      lcd.print("Mover  ");
      move_from_response(command);
      print_shafts_position_in_lcd();
    }
  }
}

void snap_photo(){
  Serial.println("Sacando foto");
  String filename = String(shaftPositionX) + "_" + String(shaftPositionY) + ".jpg";
  notify_new_photo("/api/photo", filename);
  delay(2000);
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
  Serial.println("Esperando seleccion de modo");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ingrese modo");
  lcd.setCursor(0, 1);
  lcd.print(String(TEST_KEY) + "=T " + String(CALIBRATION_KEY) + "=C " + String(NORMAL_KEY) + "=N");
  
  char key;
  do {
    key = wait_for_key();
  } while (key != TEST_KEY && key != CALIBRATION_KEY && key != NORMAL_KEY);

  if (key == TEST_KEY) {
    return TEST_MODE;
  }
  if (key == CALIBRATION_KEY) {
    return CALIBRATION_MODE;
  }
  if (key == NORMAL_KEY) {
    return NORMAL_MODE;
  }
}

char wait_for_key(){
  char pressed_key;
  do {
    pressed_key = keypad.getKey();
  } while (pressed_key == NO_KEY);
  return pressed_key;
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
    delay(150);
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
          Serial.println("Haciendo request a " + endpoint);
          client.println("GET " + endpoint + " HTTP/1.1");
          client.println("Host: " + String(SERVER[0]) + "." + String(SERVER[1]) + "."  + String(SERVER[2]) + "." + String(SERVER[3]));
          client.println("Connection:close");
          client.println();
        } else {
          Serial.println("Fallo la request");
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
