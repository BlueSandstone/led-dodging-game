// Include Arduino Wire library for I2C
#include <Wire.h>
 
// Define Slave I2C Address
#define SLAVE_ADDR 9
 
// Analog pin for potentiometer
//int analogPin = 0;
// Integer to hold potentiometer value
//int val = 0;
int gameStatus = 0;
int playerPositionPin = 6; //The first position is controlled by pin 6

//a variable to hold random number from 1-4
long randomNumber;

//a variable for starting delay time difficulty
int difficultyDelayTime = 1000;

//setup button Pin variables for left and right button
int buttonPin_L = 2;
int buttonPin_R = 3;

//setup variable for declaring the LED "obstacle"'s position 
int obstaclePosition;

struct DataPacket {
  byte dataByte;
  int playerPosVar;
  byte gameStatusByte;
  byte gameResetTimersVar;
};

DataPacket dataToSend; //object of type datapacket

//Checks left button for interrupt service routine
void checkButton_L() {
  if (digitalRead(buttonPin_L) == LOW) {
    if(gameStatus == 0) {
      difficultyDelayTime = 1000;
      digitalWrite(6, HIGH);
      playerPositionPin = 6;
      gameStatus = 1;
      dataToSend.playerPosVar = playerPositionPin;
      dataToSend.gameStatusByte = 1;
      dataToSend.gameResetTimersVar = 1;
    }
    if(playerPositionPin != 6) {
      //Move green LED to different LED lane if not already in leftmost lane 
      digitalWrite(playerPositionPin, LOW);
      digitalWrite(playerPositionPin - 1, HIGH);
      playerPositionPin = playerPositionPin - 1;
      dataToSend.playerPosVar = playerPositionPin;
      dataToSend.gameResetTimersVar = 2;
    
    }
  }
}

//Checks right button for interrupt service routine
void checkButton_R() {
  if (digitalRead(buttonPin_R) == LOW) {
    if(gameStatus == 0) {
      digitalWrite(4, LOW);
      difficultyDelayTime = 1000; // reset delay time difficulty
      digitalWrite(9, HIGH);
      playerPositionPin = 9;
      gameStatus = 1;
      dataToSend.playerPosVar = playerPositionPin;
      dataToSend.gameStatusByte = 1;
      dataToSend.gameResetTimersVar = 1; //tell slave arduino to reset timer var when starting button pressed
      delay(250);
    }
    if(playerPositionPin != 9) {
      //Move green LED to right LED lane if not already in right most lane
      digitalWrite(playerPositionPin, LOW);
      digitalWrite(playerPositionPin + 1, HIGH);
      playerPositionPin = playerPositionPin + 1;
      dataToSend.playerPosVar = playerPositionPin;
      dataToSend.gameResetTimersVar = 2;
    }
  }
}

void setup() {
 
  // Initialize I2C communications as Slave
  Wire.begin(9);
  Wire.onRequest(requestEvent);

  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  pinMode(A3, OUTPUT);
  pinMode(8, OUTPUT);

  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

  pinMode(A0, INPUT);

  //Attach Interrupts to ISR
  attachInterrupt(digitalPinToInterrupt(buttonPin_L), checkButton_L, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin_R), checkButton_R, FALLING);

  Serial.begin(9600);
}


void gamePlay() { // Turn off LCD (communicate with Master Arduino that the game has started and that the LCD can now display a timer)
  randomSeed(analogRead(A0));
  randomNumber = random(1, 5); // Random number that decides which lane gets the obstacle
  if (randomNumber == 1) 
  {
    digitalWrite(5, HIGH); 
    delay(difficultyDelayTime); 
    digitalWrite(5, LOW);
    delay(25); 
    digitalWrite(A1, HIGH); 
    delay(difficultyDelayTime); 
    digitalWrite(A1, LOW);
    delay(25); 
    digitalWrite(A2, HIGH); 
    delay(difficultyDelayTime); 
    digitalWrite(A2, LOW);
    delay(25);
    digitalWrite(A3, HIGH); 
    delay(difficultyDelayTime); 
    digitalWrite(A3, LOW);
    delay(25);

    obstaclePosition = 6; 
    if (obstaclePosition == playerPositionPin) 
    {
      gameStatus = 0; // Call Master Arduino to print out game over, along with the total time "survived." 
      dataToSend.gameStatusByte = 2;
      //Unabble to use noInterrupts() function as this breaks I2C communication in some way unfortunately so unable to disable user input while viewing score on display
      detachInterrupt(digitalPinToInterrupt(buttonPin_L));
      detachInterrupt(digitalPinToInterrupt(buttonPin_R));
      delay(10000);
      attachInterrupt(digitalPinToInterrupt(buttonPin_L), checkButton_L, FALLING);
      attachInterrupt(digitalPinToInterrupt(buttonPin_R), checkButton_R, FALLING);
    } 
  } 
  if (randomNumber == 2) 
  {
    digitalWrite(13, HIGH); 
    delay(difficultyDelayTime); 
    digitalWrite(13, LOW);
    delay(25); //Small delay to prevent LED light skipping
    digitalWrite(12, HIGH); 
    delay(difficultyDelayTime); 
    digitalWrite(12, LOW);
    delay(25); 
    digitalWrite(11, HIGH); 
    delay(difficultyDelayTime); 
    digitalWrite(11, LOW);
    delay(25);
    digitalWrite(10, HIGH); 
    delay(difficultyDelayTime); 
    digitalWrite(10, LOW);
    delay(25);
    obstaclePosition = 7; 
    if (obstaclePosition == playerPositionPin) 
    {
      gameStatus = 0; 
      dataToSend.gameStatusByte = 2; //Call Master Arduino to print out game over, along with the total time "survived." 
      //noInterrupts() see note above regarding noInterrupts()
      detachInterrupt(digitalPinToInterrupt(buttonPin_L));
      detachInterrupt(digitalPinToInterrupt(buttonPin_R));
      delay(10000);
      attachInterrupt(digitalPinToInterrupt(buttonPin_L), checkButton_L, FALLING);
      attachInterrupt(digitalPinToInterrupt(buttonPin_R), checkButton_R, FALLING);
    } 
  }
  if (randomNumber == 3)
  {
    dataToSend.dataByte = 3;
    delay(1000);
  }
  if (randomNumber == 4)
  {
    dataToSend.dataByte = 4;
    delay(1000);
  }

  int redSensorValue;
  redSensorValue = analogRead(A0);
  delay(250);
  // print out the values to the Serial Monitor
  Serial.println("raw sensor Values red: ");
  Serial.print(redSensorValue);
  if (redSensorValue > 120) //if red game-over LED lights up. Threshold configured to dim rooms in the evening
  {
    gameStatus = 0;
    dataToSend.gameStatusByte = 2; //Call Master Arduino to print out game over, along with the total time "survived."
    //noInterrupts() see note above regarding noInterrupts()
    detachInterrupt(digitalPinToInterrupt(buttonPin_L));
    detachInterrupt(digitalPinToInterrupt(buttonPin_R));
    delay(10000);
    attachInterrupt(digitalPinToInterrupt(buttonPin_L), checkButton_L, FALLING);
    attachInterrupt(digitalPinToInterrupt(buttonPin_R), checkButton_R, FALLING);
  }
  if(difficultyDelayTime >= 100) //Maximum speed difficulty of "LED" obstacle speed
  {
    difficultyDelayTime = difficultyDelayTime - 20; // Increase speed of game after each loop finishes
  }
}

void loop() {
  delay(250); //To give the arduino some realization time that the game started after pushing a button. THis needs to be included for the starting LEDs to light up

  if(gameStatus == 0) {
    gameStatus = 0;
    digitalWrite(6, LOW); digitalWrite(7, LOW); digitalWrite(8, LOW); digitalWrite(9, LOW);
    //interrupts() see note above regarding noInterrupts() and interrupts()
  }
  if(gameStatus == 1) {
    gamePlay();
  }
 
}

void requestEvent()
{
  Wire.write((byte*)&dataToSend, sizeof(dataToSend));
}

