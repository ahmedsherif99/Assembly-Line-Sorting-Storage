#include <AccelStepper.h>
#include <MultiStepper.h>

// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
#define dirPin1 7 // metal motor connection
#define stepPin1 8 // metal motor connection
#define motorInterfaceType1 1
#define dirPin2 9 // Non-metal motor connection
#define stepPin2 10 // Non-metal motor connection
#define motorInterfaceType2 1
int IRSensor1 = 3; // connect ir sensor to arduino pin 3
int IRSensor2 = 4; // connect ir sensor to arduino pin 4
int metalsensor = 5; // connect metal sensor to arduino pin 5
int lcdconnect= 6; // connect signal to lcd arduino to pin 6
int yellowsens = 11;  //connect signal sent to assembly conveyor arduino to pin 11
int greensens= 12;    //connect signal sent to assembly conveyor arduino to pin 12
int disconv=2;   //connect signal send to disassembly conveyor arduino to pin 2
int checkermet=0; //variables used to check metal product
int checkernon=0; //variables used to check non-metal product
double nontimer;
  // Create a new instance of the AccelStepper class:
  AccelStepper stepper1 = AccelStepper(motorInterfaceType1, stepPin1, dirPin1);
  AccelStepper stepper2 = AccelStepper(motorInterfaceType2, stepPin2, dirPin2);
void setup() {
  Serial.begin(115200); // begin serial monitor on laptop
  while (!Serial) delay(10); // for safety
  Serial.println("Hello!");
 pinMode (yellowsens, OUTPUT); // Signal pin OUTPUT
 pinMode(greensens,OUTPUT);  // Signal pin OUTPUT
 pinMode(disconv,OUTPUT);   // Signal pin OUTPUT
  digitalWrite(yellowsens,LOW); // send 0 to assembly conveyor
  digitalWrite(greensens,LOW);  // send 0 to assembly conveyor
  pinMode (IRSensor1, INPUT_PULLUP); // ir sensor pin INPUT
  pinMode (IRSensor2, INPUT_PULLUP); // ir sensor pin INPUT
   pinMode (lcdconnect, OUTPUT); // LCD pin Output
  pinMode (metalsensor, INPUT_PULLUP); // metal sensor pin INPUT
  // Set the maximum speed in steps per second:
  stepper1.setMaxSpeed(1000);
  stepper2.setMaxSpeed(1000);
  stepper1.setAcceleration(600);
  stepper2.setAcceleration(600);
   // Set the current position to 0:
  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);
}
    static double productsmetal[10];  // array to hold time for metal products
    static double productsnon[10];    // array to hold time for non-metal products
    static int countmet=-1;     // counter to count the number of metal products
    static int countnon=-1;     // counter to count the number of non-metal products
    static int flagnon[10]={0}; // array to hold if non metal is in or out storage
    static int flagmet[10]={0}; // array to hold if metal is in or out storage
void loop() {   
int metalsens = digitalRead (metalsensor);
int secondsens= digitalRead (IRSensor2);
if(metalsens==0){       //this function is used to make sure if the signal came from metal sensor is correct or noise from other systems
  double metaltime=millis();
  while(millis()-metaltime<2000){
    if(digitalRead(IRSensor1)==0){
      checkermet=1;
      }
    }
  }
if(digitalRead(IRSensor1)==0&&checkermet!=1){   //this function is used to make sure if the signal came from
  checkernon=1;                                // ir sensor is correct or noise from other systems
  nontimer=millis();
  }
if (checkermet==1){
  checkermet=0;
  digitalWrite(greensens,HIGH); // send 1 to assembly conveyor to stop
digitalWrite(lcdconnect,LOW); // send zero to nfc arduino to make sure that it will send a new signal
  delay(150);
  digitalWrite(lcdconnect,HIGH); // send 1 to nfc to let it know that the product is in sorting 
  digitalWrite(greensens,LOW); //send zero to assembly to finish signal
delay(3000);
  stepper1.moveTo(6000); // move the pusher with the product inside storage
   stepper1.runToPosition();
  delay(1000);
  stepper1.moveTo(0); // move pusher to origin
   stepper1.runToPosition();
    delay(1000);
  countmet++; // counter increase to indicate that their is a product entered
    productsmetal[countmet]=millis(); // record the time of entry of the product to calculate the time of storage
    flagmet[countmet]=1;
    
}
if(digitalRead(IRSensor2)==0&&checkernon==1&&(millis()-nontimer>3000)){ // used to check that a non-metal product reached the position
   checkernon=0;
  digitalWrite(yellowsens,HIGH); // send 1 to conveyor to stop
  digitalWrite(lcdconnect,LOW); // send zero to nfc arduino to make sure that it will send a new signal
  delay(150);
  digitalWrite(lcdconnect,HIGH); // send 1 to nfc to let it know that the product is in sorting 
  digitalWrite(yellowsens,LOW); //send zero to assembly to finish signal
  stepper2.moveTo(6000); // move the pusher with the product inside storage
  stepper2.runToPosition();
  delay(1000); 
  stepper2.moveTo(0); // move pusher to origin
  stepper2.runToPosition();
    delay(1000);
  countnon++; // counter increase to indicate that their is a product entered
  productsnon[countnon]=millis(); // record the time of entry of the product to calculate the time of storage
  flagnon[countnon]=1;
  }
  if(flagmet[0]==1&&((millis()-productsmetal[0])>=20000)){ // check if product finished the time of storage
    digitalWrite(disconv,HIGH); // send 1 to disassembly conveyor to stop it
    if(flagmet[1]==1){  //check if their is 2 products inside storage
  stepper1.moveTo(9000);  // move the pusher to move product out of storage
   stepper1.runToPosition();
  delay(1000);
  digitalWrite(disconv,LOW); //send zero to disassembly to finish signal
  stepper1.moveTo(0);  // move pusher to origin
   stepper1.runToPosition();
    delay(1000); 
    flagmet[0]=0; // reset to know that there is no product inside storage
    digitalWrite(lcdconnect,LOW); // finish signal to lcd 
    }else{  //check if only 1 product is inside
  digitalWrite(disconv,HIGH);   // same code as previous
  stepper1.moveTo(10000);
  stepper1.runToPosition();
  digitalWrite(disconv,LOW);
  stepper1.moveTo(0);
  stepper1.runToPosition();
    delay(1000); 
    flagmet[0]=0;
      digitalWrite(lcdconnect,LOW);
      }
    }
     if(flagmet[1]==1&&((millis()-productsmetal[1])>=20000)){ // check if the timer of the second product of same type is finished
  digitalWrite(disconv,HIGH);   // same code as previous
  stepper1.moveTo(10000);
  stepper1.runToPosition();
  delay(1000);
  digitalWrite(disconv,LOW);
  stepper1.moveTo(0);
  stepper1.runToPosition();
    delay(1000); 
    flagmet[1]=0;
    digitalWrite(lcdconnect,LOW);
    }
     if(flagnon[0]==1&&((millis()-productsnon[0])>=20000)){ // check time for non metal product is finished
      if(flagnon[1]==1){  //same code as previous
  digitalWrite(disconv,HIGH);
  stepper2.moveTo(9000);
  stepper2.runToPosition();
  delay(1000);
  digitalWrite(disconv,LOW);
  stepper2.moveTo(0);
  stepper2.runToPosition();
    delay(1000);
    flagnon[0]=0;
      digitalWrite(lcdconnect,LOW);
      }else
      {
  digitalWrite(disconv,HIGH);
  stepper2.moveTo(10000);
  stepper2.runToPosition();
  delay(1000);
  digitalWrite(disconv,LOW);
  stepper2.moveTo(0);
  stepper2.runToPosition();
    delay(1000);
    flagnon[0]=0;
        digitalWrite(lcdconnect,LOW);
        }
      }
       if(flagnon[1]==1&&((millis()-productsnon[1])>=20000))
       {
  digitalWrite(disconv,HIGH);
  stepper2.moveTo(10000);
  stepper2.runToPosition();
  delay(1000);
  digitalWrite(disconv,LOW);
  stepper2.moveTo(0);
  stepper2.runToPosition();
    delay(1000);
    flagnon[1]=0;
    digitalWrite(lcdconnect,LOW);
    }
}
