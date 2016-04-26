#include <Keypad.h>
#include <EEPROM.h>
// standard keypad set-up code taken from Alexander Brevig
// keypad example "HelloKeypad" in IDE File->Examples->Keypad
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
 {'1','2','3'},
 {'4','5','6'},
 {'7','8','9'},
 {'*','0','#'}
};
// this may be different on different keypads - please check!
byte rowPins[ROWS] = {11, 10, 9, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5}; //connect to the column pinouts of the keypad
// attach keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int PinRelay = 3; // pin for Relay
int Green = 12; // pin for Green Led
int Red = 13; // pin for Red Led
boolean canChange=false; // flag determining if we can change the code
boolean isOpen=false; // flag determining the state of the lock
boolean isValid=false; // flag determining the validity of an input
char entryCode[4]={'1','4','6','4'}; // The code you need (overwritten from EEPROM)
char inputB[4]={'*','*','*','*'}; // the keypad input buffer
int i;
long t=-1;
int b;

// set up the device
void setup(){
 Serial.begin(9600); // this is added for debugging - allows you to echo the keys to the computer

 //pinMode(5,OUTPUT); //LED 1
 pinMode(Green,OUTPUT); //LED 2
 pinMode(PinRelay,OUTPUT); //SOLENOID

 digitalWrite(PinRelay,LOW); // SOLENOID OFF
 digitalWrite(Green,LOW); //
// digitalWrite(5,HIGH); // 4 LOW, 5 HIGH BI-COLOUR LED IS RED

/* 
 read code from EEPROM
 IMPORTANT - initialise first otherwise you'll never get in!
*/
 for (i=0;i<4;i++)
 {
    entryCode[i]=char(EEPROM.read(i));
 }

}

void loop(){

 // t is the count-down timer. It is set when the lock is
 // open and counts down to zero, at which point the lock is closed
 if (t>0) t--;
 //Serial.println(t);  
 if (t<=0)
 {
    canChange=false;
    digitalWrite(PinRelay,LOW); // SOLENOID OFF
    digitalWrite(Green,LOW);
 }
if (t<=990000)
 {
    isOpen=false;
    digitalWrite(PinRelay,LOW); // SOLENOID OFF
 //   digitalWrite(Green,LOW);
 }

 char key = keypad.getKey(); // get a key (if pressed)

 if (key){
 Serial.println(key); // echo to computer fo debugging

 if (key=='*') // if '*' check the last four digits are the entry code
 {
    Serial.println(inputB[0]);
    Serial.println(inputB[1]);
    Serial.println(inputB[2]);
    Serial.println(inputB[3]);

 // could do in a loop, but didn't!
    if(inputB[0]==entryCode[0] &&
    inputB[1]==entryCode[1] &&
    inputB[2]==entryCode[2] &&
    inputB[3]==entryCode[3] )
    {
       Serial.println("good code");
       isOpen=true; // code ok - set open flag true
       canChange=true;
       inputB[0]='*'; // reset input buffer
       inputB[1]='*';
       inputB[2]='*';
       inputB[3]='*';
       b=0;
       t=1000000; // reset countdown timer
     }
     else
     {
       isOpen=false; // code wrong - set open flag false
       b++;
       t=0;
       inputB[0]='*'; // reset input buffer
       inputB[1]='*';
       inputB[2]='*';
       inputB[3]='*';
       if(b==3)
       {
        Serial.println("BLOQUE");
        digitalWrite(Red,HIGH);
        delay(10000);  
        digitalWrite(Red,LOW);
        Serial.println("DEBLOQUE");
        b=0;
        }
     }
     
 //isOpen=!isOpen;
     if(isOpen)
     {
       digitalWrite(PinRelay,HIGH); // SOLENOID ON
       digitalWrite(Green,HIGH);
     }
     else
     {
       digitalWrite(PinRelay,LOW); // SOLENOID OFF
       digitalWrite(Green,LOW);
     }

//canChange=!canChange;
     if(canChange)
     {
       //digitalWrite(2,HIGH); // SOLENOID ON
       digitalWrite(4,HIGH);
     }
     else
     {
       //digitalWrite(2,LOW); // SOLENOID OFF
       digitalWrite(4,LOW);
     }
 
}
 else
 { // a key other than '*' has been pressed
 /*
 lock is open and '#' pressed - store last 4 keys in eeprom as
 new lock, but only if last 4 entries are digits
 */
     if (canChange && key=='#')
     {
 // check a valid 4 digit code is in the input buffer
       Serial.println("CHANGEMENT");
       isValid=true;
       for (i=0;i<4;i++) if(inputB[i]=='*' || inputB[i]=='#') isValid=false;

       if (isValid)
       {
         for (i=0;i<4;i++)
         {
           entryCode[i]=inputB[i]; // copy contents of input buffer to code buffer
           EEPROM.write(i,entryCode[i]); // write the values in the EEPROM so they stay!
         }

 // flash LED to signal change
         for (i=0;i<4;i++)
         {
           digitalWrite(Green,LOW);
           delay(500);
           digitalWrite(Green,HIGH);
           delay(500);
         }
         t=0;
     }
 }
 else
 { // a digit has been pressed. First shift the contents of the key buffer along...
     for (i=0;i<3;i++)
     {
       inputB[i]=inputB[i+1]; // you lose the first key in the buffer doing this
     }
     inputB[3]=key; // ...then add the new key at the end of the list
 }
 }
 }
}
