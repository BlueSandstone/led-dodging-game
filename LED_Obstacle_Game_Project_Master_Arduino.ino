// Include Arduino Wire library for I2C
#include <Wire.h>

#include <LiquidCrystal.h>

// initialize the LiquidCrystal library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int difficultyDelayTime = 1000;

int obstaclePosition;

int gameStatus = 0;

struct DataPacket {
  byte dataByte;
  int playerPosVar;
  byte gameStatusByte;
  byte gameResetTimersVar;
};

extern volatile unsigned long timer0_millis; //a value used by the millis() function and can be changed

unsigned long secondsSurvived;

long randomNumber;

// int printScore = 2;

void setup() {
 
  // Initialize I2C communications as Master that gets the data
  Wire.begin();

  //Function to run when data recieved from Master Arduino
  //Wire.onReceive(recieveEvent);

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(1, OUTPUT);

  pinMode(0, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

  //LCD pin setup not needed
  // pinMode(2, INPUT);
  // pinMode(3, INPUT);
  // pinMode(4, OUTPUT);
  // pinMode(5, INPUT);

  pinMode(6, OUTPUT);//LED output pin

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
}

void gamePlay() { // Turn on LCD (communicate with slave Arduino that the game has started and that the LCD can now display a timer) 
  // Random number recieved from master Arduino that decides which lane gets the obstacle: lane 3 or 4

  digitalWrite(A3, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A0, LOW);
  digitalWrite(8, LOW);

  Wire.beginTransmission(9);
  Wire.write(0x06);
  Wire.endTransmission(false);
  Wire.requestFrom(9, sizeof(DataPacket));

  if(Wire.available() == sizeof(DataPacket))
  {
    DataPacket recievedData;
    Wire.readBytes((char*)&recievedData, sizeof(recievedData));

    if(recievedData.gameStatusByte == 1)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Seconds lasted: ");

      int resetTimersVar = recievedData.gameResetTimersVar; //Timer only starts if the player hits any button twice.
      if(resetTimersVar == 1)
      {
        noInterrupts();  // Disable interrupts
        timer0_millis = 0;  // Reset the millis timer
        interrupts();  // Re-enable interrupts
        delay(25);
        difficultyDelayTime = 1000; // reset delay difficulty timer
        delay(50);
      }

      lcd.setCursor(0, 1);
      secondsSurvived = millis()/1000;
      lcd.print(secondsSurvived);

      switch (recievedData.dataByte) //read random # value sent by slave arduino
      {
        case 3:
          //Print to LCD press any button to start game
          digitalWrite(A3, HIGH); 
          delay(difficultyDelayTime); 
          digitalWrite(A3, LOW);
          delay(25); 
          digitalWrite(A1, HIGH); 
          delay(difficultyDelayTime); 
          digitalWrite(A1, LOW);
          delay(25); 
          digitalWrite(A2, HIGH); 
          delay(difficultyDelayTime); 
          digitalWrite(A2, LOW);
          delay(25);
          digitalWrite(A0, HIGH); 
          delay(difficultyDelayTime); 
          digitalWrite(A0, LOW);

          Wire.requestFrom(9, sizeof(DataPacket));
          Wire.readBytes((char*)&recievedData, sizeof(recievedData)); //Need to read player pin position info right before checking condition
          delay(100);
          obstaclePosition = 8;
          if(recievedData.playerPosVar == obstaclePosition)
          {
            digitalWrite(A3, LOW); 
            digitalWrite(A2, LOW);
            digitalWrite(A1, LOW); 
            digitalWrite(A0, LOW);
            digitalWrite(13, LOW); 
            digitalWrite(7, LOW);
            digitalWrite(8, LOW); 
            digitalWrite(9, LOW);
            gameStatus = 0; 
            digitalWrite(6, HIGH);
            lcd.clear();
            lcd.print("You survived: ");
            lcd.setCursor(0,1);
            secondsSurvived = millis()/1000; //retrieve updated time survived in seconds right before printing
            lcd.print(secondsSurvived);
            lcd.print(" seconds!");
            delay(10000); //wait for 10 seconds for user to see score
            digitalWrite(6, LOW); //Turn off the gameOver light
          }
          break;

        case 4:
          digitalWrite(13, HIGH); 
          delay(difficultyDelayTime); 
          digitalWrite(13, LOW);
          delay(25); 
          digitalWrite(7, HIGH); 
          delay(difficultyDelayTime); 
          digitalWrite(7, LOW);
          delay(25); 
          digitalWrite(8, HIGH); 
          delay(difficultyDelayTime); 
          digitalWrite(8, LOW);
          delay(25);
          digitalWrite(9, HIGH); 
          delay(difficultyDelayTime); 
          digitalWrite(9, LOW);

          Wire.requestFrom(9, sizeof(DataPacket));
          Wire.readBytes((char*)&recievedData, sizeof(recievedData)); //Need to read player pin position info right before checking condition
          delay(100);
          obstaclePosition = 9;
          if(recievedData.playerPosVar == obstaclePosition)
          {
            digitalWrite(A3, LOW); 
            digitalWrite(A2, LOW);
            digitalWrite(A1, LOW); 
            digitalWrite(A0, LOW);
            digitalWrite(13, LOW); 
            digitalWrite(7, LOW);
            digitalWrite(8, LOW); 
            digitalWrite(9, LOW);
            gameStatus = 0;

            digitalWrite(6, HIGH);
            lcd.clear();
            lcd.print("You survived: ");
            lcd.setCursor(0,1);
            secondsSurvived = millis()/1000; //retrieve updated time survived in seconds right before printing
            lcd.print(secondsSurvived);
            lcd.print(" seconds!");
            delay(10000); //wait for 10 seconds for user to see score
            digitalWrite(6, LOW); //Turn off the gameOver light
          }
          break;
    
        case 0:
          digitalWrite(A3, LOW); 
          digitalWrite(A2, LOW);
          digitalWrite(A1, LOW); 
          digitalWrite(A0, LOW);
          digitalWrite(13, LOW); 
          digitalWrite(7, LOW);
          digitalWrite(8, LOW); 
          digitalWrite(9, LOW);

        default:
      
          break;
      }
    }
    if(recievedData.gameStatusByte == 2) //If the Slave Arduino Board sent data that the game was lost
    {
      digitalWrite(6, HIGH);
      gameStatus = 0;
      lcd.clear();
      lcd.print("You survived: ");
      lcd.setCursor(0,1);
      secondsSurvived = millis()/1000; //retrieve updated time survived right before printing
      lcd.print(secondsSurvived);
      lcd.print(" seconds!");
      delay(10000);
      digitalWrite(6, LOW); //Turn off the gameOver light
    }
    if(difficultyDelayTime >= 100)
    {
      difficultyDelayTime = difficultyDelayTime - 20;
    }
  }
  else 
  {
    Serial.println("No data recieved");
  }
}

void loop() {
  DataPacket recievedData;
  Wire.requestFrom(9, sizeof(DataPacket));
  Wire.readBytes((char*)&recievedData, sizeof(recievedData)); //Need to read game Status from other arduino board to know when to start playing the game
  if(recievedData.gameStatusByte == 1)
  {
    delay(250);
    digitalWrite(6, LOW);
    gamePlay();
    delay(100);
  }
  else if(gameStatus == 0)
  {
    gameStatus = 0;
    //LCD to prompt user to press any key to start (sending data)
    lcd.setCursor(0, 0);
    lcd.print("Press any key...");
    lcd.setCursor(0, 1);
    lcd.print("to start dodging");
  }
}

