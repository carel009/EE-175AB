// Christopher Arellano code
//***************************************************************************************************************************************
// CA Code begin
#include <Wire.h>
#include "Keypad.h"
#include <Keypad_I2C.h>
//#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h> //i2c
#include <DHT.h>
#define Type DHT11
//***************************************************************************************************************************************
// Global Variables Begin

//***************************************************************************************************************************************
// Pin Assignment Begin
const int red = 3; //pin for red LED
const int green = 4; //pin for green LED
const int pir = 6; //pin for pir
const int fan1 = 8; //pin for fan1
const int fan2 = 9; //pin for fan2
const int pads = 10; //pin for cool/heat spads
const int buzzer = 12; //pin for buzzer
const int sensePin = 22; //pin for temp sensor
const int echoPin = 23; //pin for sonar echo
const int trigPin = 24; //pin for sonar trigger
// Pin Assignment End
//***************************************************************************************************************************************

String keyStr;
String inputStr;
const unsigned int maximum = 5;
const int armed = 1;
const int disarmed = 0;
const int alarmWait = 7000;//7 seconds
const int autoTemp = 1;
const int alwaysActiveTemp = 2;
const int tempOff = 3;
int tempF;
int tempSelect = 75;
const int tempMax = 80;
const int tempMin = 70;
DHT HT(sensePin, Type);
int pirValue = LOW;
int pirState = 0;
int alarmStatus = 0;
int alarmTrigger = 0;
int acTrigger = 0;
int tempStatus = 1;
int strt = 0;
int incorrectInputCount = 0;
const String alarmPin = "1234";
const String tempA = "A";
const String tempAA = "AA";
const String tempAAA = "AAA";
const String displayTemp = "D";
long duration;
int distance;
int trigPic = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2); //i2c
static unsigned int keyPos = 0;
static unsigned int inputPos = 0;
char input;
//raunac variables
String checker = "";
int PLATE = 0;
// Global Variables End
//***************************************************************************************************************************************

//***************************************************************************************************************************************
// Membrane Keypad Begin
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {0, 1, 2, 3};
byte colPins[COLS] = {4, 5, 6, 7};

Keypad_I2C myKeypad = Keypad_I2C(makeKeymap(keys), rowPins, colPins, ROWS, COLS, 0x20);

char key = NO_KEY;
KeyState myKSp = IDLE;

void membraneKeypad()
{
  static char Key[maximum];
  char myKey = myKeypad.getKey();
  KeyState myKS = myKeypad.getState();

  if (myKSp != myKS && myKS != IDLE)
  {
    myKSp = myKS;
    if (myKey)
    {
      key = myKey;
    }
    
    if (myKS == HOLD) {}
    
    if (myKS == RELEASED)
    {
      if (key != '*' && (keyPos < (maximum - 1)))
      {
        Key[keyPos] = key;
        keyPos++;   
      }

      else if (key == '*')
      {
        Key[keyPos] = '\0';
        keyStr = Key;
        keyPos = 0;
      }

      else
      {
        lcd.clear(); //lcd
        lcd.print("Invalid Input"); //lcd
        Serial.print("Invalid Input \n"); //serial
        delay (2000);
        lcd.clear(); //lcd
        keyPos = 0; 
      }
    }
    myKS = IDLE;
  }
}
// Membrane Keypad Setup End
//***************************************************************************************************************************************

//***************************************************************************************************************************************
// Bluetooth Keypad Begin
void bluetoothKeypad()
{
  static char Input[maximum];
  //static unsigned int inputPos = 0;

  if (Serial.available() > 0)  
  { 
    input = Serial.read();

    //raunac change
    if (input == 'k'){
      checker = "";
      PLATE = 1;
    }
    
    else if (input != '*' && (inputPos < (maximum - 1)))
    {
       Input[inputPos] = input;
       inputPos++;   
    }

    else if (input == '*')
    {
      Input[inputPos] = '\0';
      inputStr = Input;
      inputPos = 0;
    }

    //raunac change
    //else if (input == 'k'){
      //checker = "";
      //PLATE = 1;
    //}

    else
    {
      lcd.clear(); //lcd
      lcd.print("Invalid Input"); //lcd
      Serial.print("Invalid Input \n");
      delay (2000);
      lcd.clear(); //lcd
      inputPos = 0; 
    }
  }
}
// Bluetooth Keypad End
//***************************************************************************************************************************************

//***************************************************************************************************************************************
// Input Check Begin
void inputCheck()
{
  if (strt == 0)
  {
    digitalWrite(green, HIGH); //green LED
  }
  
  if (keyStr == alarmPin || inputStr == alarmPin)
  {
    keyStr = "";
    inputStr = "";
    alarmControl();
  }

  if (keyStr == tempA || inputStr == tempA)
  {
    keyStr = "";
    inputStr = "";
    tempStatus = autoTemp;
    tempControl();
  }

  if (keyStr == tempAA || inputStr == tempAA)
  {
    keyStr = "";
    inputStr = "";
    tempStatus = alwaysActiveTemp;
    tempControl();
  }

  if (keyStr == tempAAA || inputStr == tempAAA)
  {
    keyStr = "";
    inputStr = "";
    tempStatus = tempOff;
    tempControl();
  }

  if (keyStr == displayTemp || inputStr == displayTemp)
  {
    keyStr = "";
    inputStr = "";
    lcd.clear(); //lcd
    lcd.print("Current Temp: "); //lcd
    lcd.print(tempF); //lcd
    Serial.print("Current Temp: "); //serial
    Serial.print(tempF); //serial
    Serial.print("\n"); //serial
    delay (2000);
    lcd.clear(); //lcd
  }

  if (keyStr.toInt() >= tempMin && keyStr.toInt() <= tempMax)
  {
    tempSelect = keyStr.toInt();
    keyStr = "";
    lcd.clear(); //lcd
    lcd.print("Temp Select: "); //lcd
    lcd.print(tempSelect); //lcd
    Serial.print("Temp Select: "); //serial
    Serial.print(tempSelect); //serial
    Serial.print("\n"); //serial
    delay (2000);
    lcd.clear(); //lcd
  }

  if (inputStr.toInt() >= tempMin && inputStr.toInt() <= tempMax)
  {
    tempSelect = inputStr.toInt();
    inputStr = "";
    lcd.clear(); //lcd
    lcd.print("Temp Select: "); //lcd
    lcd.print(tempSelect); //lcd
    Serial.print("Temp Select: "); //serial
    Serial.print(tempSelect); //serial
    Serial.print("\n"); //serial
    delay (2000);
    lcd.clear(); //lcd
  }
}
// Input Check End
//***************************************************************************************************************************************

//***************************************************************************************************************************************
// Alarm Control Begin
void alarmControl()
{
  if (alarmStatus == disarmed)
  {
    alarmStatus = armed;
    lcd.clear(); //lcd
    lcd.print("System Armed"); //lcd
    Serial.print("System Armed \n"); //serial
    digitalWrite(red, HIGH); //red LED
    digitalWrite(green, LOW); //green LED
    delay (2000);
    lcd.clear(); //lcd
    strt = 1;
    delay (alarmWait);
    return;                          
  }

  if (alarmStatus == armed)
  {
    alarmStatus = disarmed;
    lcd.clear(); //lcd
    lcd.print("System Disarmed"); //lcd
    Serial.print("System Disarmed \n"); //serial
    digitalWrite(green, HIGH); //green LED
    digitalWrite(red, LOW); //red LED
    delay (2000);
    lcd.clear(); //lcd
    strt = 1;
    incorrectInputCount = 0;
    return;                          
  }
}
// Alarm Control End
//***************************************************************************************************************************************

//***************************************************************************************************************************************
// Alarm Function Begin
void alarmFunction()
{
  if (alarmStatus == armed)
  {
    pirSensor();
    if (alarmTrigger == 1)
    {
      buzzerControl();
    }
  }

  if (alarmStatus == disarmed)
  {
    alarmTrigger = 0;
  }
}
// Alarm Function End
//***************************************************************************************************************************************

//***************************************************************************************************************************************
// Temperature Control Begin
void tempControl()
{
  if (tempStatus == autoTemp)
  {
    lcd.clear(); //lcd
    lcd.print("Auto Temp"); //lcd
    Serial.print("Auto Temp \n"); //serial
    delay (2000);
    lcd.clear(); //lcd
    return;
  }

  if (tempStatus == alwaysActiveTemp)
  {
    acTrigger = 1;
    lcd.clear(); //lcd
    lcd.print("Temp Always On"); //lcd
    Serial.print("Temp Always On \n"); //serial
    delay (2000);
    lcd.clear(); //lcd
    return;
  }

  if (tempStatus == tempOff)
  {
    acTrigger = 0;
    lcd.clear(); //lcd
    lcd.print("Temp Off"); //lcd
    Serial.print("Temp Off \n"); //serial
    delay (2000);
    lcd.clear(); //lcd
    return;
  }
}
// Temperature Control End
//***************************************************************************************************************************************

//***************************************************************************************************************************************
// Temperature Function Begin
void tempFunction()
{
  pirSensor();
  if (acTrigger == 1 && tempSelect < tempF)
  {
    digitalWrite(fan1, LOW); //high orig.
    digitalWrite(fan2, LOW); //high orig.
    digitalWrite(pads, HIGH); //low orig.
  }

  if (acTrigger == 1 && tempSelect > tempF)
  {
    digitalWrite(fan1, LOW); //high orig.
    digitalWrite(fan2, HIGH); //high orig.
    digitalWrite(pads, LOW); //high orig.
  }

  if (acTrigger == 1 && tempSelect == tempF)
  {
    digitalWrite(fan1, HIGH); //low orig.
    digitalWrite(fan2, HIGH); //low orig.
    digitalWrite(pads, HIGH); //low orig.
  }

  if (acTrigger == 0)
  {
    digitalWrite(fan1, HIGH); //low orig.
    digitalWrite(fan2, HIGH); //low orig.
    digitalWrite(pads, HIGH); //low orig.
  }
}
// Temperature Function End
//***************************************************************************************************************************************

//***************************************************************************************************************************************
// Temperature Sense Begin
void tempSense()
{
  tempF = HT.readTemperature(true);
}
// Temperature Sense End
//***************************************************************************************************************************************

//***************************************************************************************************************************************
// Pir Sensor Begin
void pirSensor()
{
  pirValue = digitalRead(pir);

  if (pirValue == HIGH) 
  {
      if (alarmStatus == armed)
      {
        alarmTrigger = 1;
      }

      if (tempStatus == autoTemp)
      {
        acTrigger = 1;
      }
  }
  
  else
  {
    if (tempStatus == autoTemp)
      {
        acTrigger = 0;
      }
  }
}
// Pir Sensor End
//***************************************************************************************************************************************

//***************************************************************************************************************************************
// Buzzer Conrol Begin
void buzzerControl()
{
  tone(buzzer, 2000);
  delay(25);
  noTone(buzzer);
  delay(25);
}
// Buzzer Control End
//***************************************************************************************************************************************

//***************************************************************************************************************************************
// Sonar Begin
void sonar()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  if(distance <= 5 && trigPic == 0)//20 orig.
  {
    Serial.print("Picture \n");
    trigPic = 1;
  }
  if(distance > 6 && trigPic == 1)//60 orig.
  {
    trigPic = 0;
  }
}
// sonar End
//***************************************************************************************************************************************
// CA Code end
//***************************************************************************************************************************************

//Raunac Bhuiyan code
//***************************************************************************************************************************************
// RB Code begin
int Passive_Buzzer = 26;  //WILL USE PIN 26
int Piezo_Sensor = A4;
int Red_light = 27;  //WILL USE PIN 27
int Green_light = 28;     //WILL USE PIN 28
int Blue_light = 29;  //WILL USE PIN 29
unsigned int i;
unsigned int j;
int x;
int y;
int z;
int Array[6] = {1000, 1100, 1200, 1000, 1100, 1200};
int Array_two[6] = {Red_light, Blue_light, Green_light, Blue_light, Red_light, Green_light};

const int Analoginput = A5;
const String comp = "Object detected";
String variable = "";
int place = 0;

void Package_delivery() {
  if (place == 0){
    if (analogRead(Analoginput) <= 800){
      place = 1;
    }
    else{
      place = 0;
    }
  }
  else if (place == 1){
    lcd.print("Object detected");
    delay (2000);
    lcd.clear();
    place = 2;
  }
  else if (place == 2){
    if (analogRead(Analoginput) <= 800){
      place = 2;
    }
    else if (analogRead(Analoginput) >= 800){
      lcd.print("Object removed");
      delay (2000);
      lcd.clear();
      place = 3;
    }
  }
  else if (place == 3){
    place = 0;
  }
}

void Door_knock() {
    //  Serial.println(analogRead(Piezo_Sensor));   //Reading the knock

  if (analogRead(Piezo_Sensor) >= 55 && analogRead(Piezo_Sensor) <= 150){
 
    for (i = 0; i < 6; i++){
      x = Array[i];         //x is carrying the value of Frequency
      y = Array_two[i];     //y is carrying the value of the Color pin
      digitalWrite(y, HIGH);
      delay(500);           //500ms delay
      tone(Passive_Buzzer, x);           //Playing the sound
      digitalWrite(y, LOW);
    }
  noTone(Passive_Buzzer);
  }

}
// RB Code end
//***************************************************************************************************************************************

// Graham Jabeguero code
//***************************************************************************************************************************************
// GJ Code begin
//////////////////////
// FIRE SENSOR
const int senseFire = A9;
const int sprinkler = 50;
int fireVal;
int fire_cnt = 0;
int fire_flag = 0;
int fire_delay = 0;
//////////
/////
// AMBIENT LIGHT
const int indoor_lights = 13;
const int light_sensor = A8;
int outside_light = 0;
// Resistor end of Photoresistor on GND
// Opposite end on 5V
// Middle on A0;
/////////////////////


//****************************************************************************************************
//
//****************************************************************************************************
// Fire Detection System Begin
void fire_sys() {
///////////////////////////////////////////////////
// Function used to sense fire
  int fireVal = analogRead(senseFire);
///////////////////////////////////////////////////
// If system senses fire for more than 5 secs then sprinkler will turn on
  if (fireVal < 970) {
    fire_cnt++;
  }
  else {
    fire_cnt = 0;
  }
///////////////////////////////////////////////////
// Turns on waterpump and displays message
  if (fire_cnt > 10) { // orig 200
    digitalWrite(sprinkler, HIGH); // Originally High
    // Should display message only once
    if (fire_flag == 0) {
      lcd.print("FIRE DETECTED");
      delay (2000);
      lcd.clear();
    }
    //
    fire_flag = 1;
  }
///////////////////////////////////////////////////
// When it stops detecting fire, will delay for 4 secs THEN turn off to make sure no fire
  if (fireVal > 970 && fire_flag == 1 && fire_delay < 1) { 
      fire_delay++;
  }
  else if (fire_delay >= 1) {
    digitalWrite(sprinkler, LOW);
    fire_delay = 0;
    fire_flag = 0;
    lcd.print("Sprinklers stop");
    delay (2000);
    lcd.clear();
  }

  // Notes: the fire_delay and fire_cnt seem to not give accurate results; 1000 does not equal 1 second in this case.
  //        Ex: Having Serial.println(fire_cnt) increments the cnt slower so I would have to decrease the total value of both.
  
  // Debug
  //delay(1000);
  //Serial.println(fireVal);
  //Serial.println(fire_cnt);
}
// Fire Detection System End
//****************************************************************************************************
//
//****************************************************************************************************
// Ambient Light System Begin
void light_sys() {
  outside_light = analogRead(light_sensor);
  //Serial.println(outside_light);

  if (outside_light >= 500 && outside_light <= 600) { // from 500 - 600
    analogWrite(indoor_lights, 10);
  }
  else if (outside_light >= 400 && outside_light <= 500) { // from 400 - 500
    analogWrite(indoor_lights, 50);
  }
  else if (outside_light >= 300 && outside_light <= 400) { // from 300 - 400
    analogWrite(indoor_lights, 128);
  }
  else if (outside_light <= 300) { // from 0 to 300
    analogWrite(indoor_lights, 255);
  }
  else { // more than 600
    analogWrite(indoor_lights, 0);
  }
  //delay(1);
}
// Ambient Light System End
// GJ Code end
//***************************************************************************************************************************************

// Garage code
//***************************************************************************************************************************************
// Garage Code begin
const int ENA = 2;    //WILL USE PIN 2 FOR ARDUINO MEGA
const int ENB = 7;    //WILL USE PIN 7 FOR ARDUINO MEGA
const int IN1 = 46;   //WILL USE PIN 46 FOR ARDUINO MEGA
const int IN2 = 47;   //WILL USE PIN 47 FOR ARDUINO MEGA
const int IN3 = 49;   //WILL USE PIN 49 FOR ARDUINO MEGA
const int IN4 = 51;   //WILL USE PIN 51 FOR ARDUINO MEGA
const int Laser_sensor = 43;  //WILL USE PIN 43 FOR ARDUINO MEGA
const int Laser = 14;
const int Photoresistor = A0; 
int counter = 0;
int state = 0;
char input2;
//String checker = "";
//int PLATE = 0;
int Prev_state;

/*void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ENA, OUTPUT);  //ENA
  pinMode(ENB, OUTPUT);  //ENB
  pinMode(IN1, OUTPUT);  //IN1
  pinMode(IN2, OUTPUT);  //IN2
  pinMode(IN3, OUTPUT);  //IN3
  pinMode(IN4, OUTPUT);  //IN4
  pinMode(Laser, OUTPUT); //Laser
  pinMode(Laser_sensor, INPUT); //Laser sensor
  pinMode(Photoresistor, INPUT); //Photoresistor
}*/

void Garage() {
  analogWrite(ENA, 255);   //ENA = 1
  analogWrite(ENB, 255);   //ENB = 1

  //WILL GET RID OF IN FINAL VERSION
  //if (input == 'k'){
    //checker = "";
    //PLATE = 1;
  //}
  
  if (state == 0){
    digitalWrite(Laser, LOW);
    rotate_off();
    if (analogRead(Photoresistor) >= 500 || (input == 'k' && checker != "NOT")){
      //Serial.println(Photoresistor);
      state = 1;
      counter = 300;
    }
    else{
      state = 0;
    }
  }
  if (state == 1){
    digitalWrite(Laser, HIGH);
    rotate_one();
    counter--;
    state = 1;
    if (digitalRead(Laser_sensor) == HIGH){
      Prev_state = 1;
      state = 15;
    }
    if (counter == 0){
      state = 2;
    }
  }
  if (state == 15){
    rotate_off();
    if (digitalRead(Laser_sensor) == HIGH){
      state = 15;
    }
    else{
      state = Prev_state;
    }
  }
  if (state == 2){
    rotate_off();
    digitalWrite(Laser, LOW);
    if (analogRead(Photoresistor) >= 500){
      state = 2;
    }
    else if (PLATE == 1){
      state = 25;
    }
    else if (analogRead(Photoresistor) <= 500){
      counter = 300;
      state = 3;
    }
  }
  if (state == 25){
    PLATE = 0;
    rotate_off();
    if (analogRead(Photoresistor) >= 500){
      state = 2;
    }
  }
  if (state == 3){
    digitalWrite(Laser, HIGH);
    rotate_two();
    counter--;
    if (digitalRead(Laser_sensor) == HIGH){
      Prev_state = 3;
      state = 15;
    }
    if (counter == 0){
      state = 4;
    }
  }
  if (state == 4){
    checker = "NOT";
    PLATE = 0;
    state = 0;
  }
}
void rotate_one(){
  //Step 0
  analogWrite(IN1, 255);  //Activate IN1
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
  delay(15);            //Motor Speed

  //Step 1
  analogWrite(IN3, 255);  //Activate IN2
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN4, 0);    
  delay(15);            //Motor Speed

  //Step 2
  analogWrite(IN2, 255);  //Activate IN3
  analogWrite(IN1, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
  delay(15);            //Motor Speed

  //Step 3
  analogWrite(IN4, 255);  //Activate IN4
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  delay(15);            //Motor Speed
}
void rotate_two(){
  //Step 0
  analogWrite(IN4, 255);  //Activate IN1
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN1, 0);
  delay(15);            //Motor Speed

  //Step 1
  analogWrite(IN2, 255);  //Activate IN2
  analogWrite(IN1, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);    
  delay(15);            //Motor Speed

  //Step 2
  analogWrite(IN3, 255);  //Activate IN3
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN4, 0);
  delay(15);            //Motor Speed

  //Step 3
  analogWrite(IN1, 255);  //Activate IN4
  analogWrite(IN4, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  delay(15);            //Motor Speed
}
void rotate_off(){
  analogWrite(IN4, 0);  
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
}
// Garage Code end
//***************************************************************************************************************************************

//***************************************************************************************************************************************
// Setup Begin
void setup() 
{
  //CA Setup
  Serial.begin(9600);
  HT.begin(); 
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(fan1, OUTPUT);
  pinMode(fan2, OUTPUT);
  pinMode(pads, OUTPUT);
  pinMode(pir, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Wire.begin();
  myKeypad.begin(makeKeymap(keys));
  //lcd.begin(16, 2); 
  lcd.begin(); //i2c 

  //RB Setup
  pinMode(Analoginput, INPUT); //A5

  pinMode(Passive_Buzzer, OUTPUT);  //The Passive Buzzer
  pinMode(Piezo_Sensor, INPUT);
  pinMode(Red_light, OUTPUT);  //The Red LED
  pinMode(Blue_light, OUTPUT);  //The Blue LED
  pinMode(Green_light, OUTPUT); //The Green LED

  //GJ setup
  pinMode(indoor_lights, OUTPUT);
  pinMode(light_sensor, INPUT); // Senses the outside light and determines the light level
  pinMode(sprinkler, OUTPUT);
  pinMode(senseFire, INPUT);

  //garage setup
  pinMode(ENA, OUTPUT);  //ENA
  pinMode(ENB, OUTPUT);  //ENB
  pinMode(IN1, OUTPUT);  //IN1
  pinMode(IN2, OUTPUT);  //IN2
  pinMode(IN3, OUTPUT);  //IN3
  pinMode(IN4, OUTPUT);  //IN4
  pinMode(Laser, OUTPUT); //Laser
  pinMode(Laser_sensor, INPUT); //Laser sensor
  pinMode(Photoresistor, INPUT); //Photoresistor
}
// Setup End
//***************************************************************************************************************************************

//***************************************************************************************************************************************
// Main Begin
void loop()
{
  //CA functions
  bluetoothKeypad();
  membraneKeypad();
  inputCheck();
  alarmFunction();
  tempSense();
  tempFunction();
  sonar();

  //RB functions
  Door_knock();
  Package_delivery();

  //GJ functions
  fire_sys();
  light_sys();

  //garage
  Garage();
}
// Main End
//***************************************************************************************************************************************
