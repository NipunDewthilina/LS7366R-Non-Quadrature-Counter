//=========================HEADER=============================================================
/* 
   Hardware: Arduino Uno R3
   Powered 
   
   LS7366 Breakout    -------------   Arduino
   -----------------                    -------
            MOSI   -------------------   SDO (D11)
            MISO   -------------------   SDI (D12)
            SCK    -------------------   SCK (D13)
            SS     -------------------   SS (D7)
            GND    -------------------   GND
            VDD    -------------------   VCC (5.0V)
            CNT_EN -------------------   VCC(5.0V)
            DFLAG  -------------------   
//============================================================================================
*/

// Inclde the standard Arduino SPI Library, please ensure the SPI pins are
// connected properly for your Arduino version
#include <SPI.h>
#include "LS7366R.h"
// Slave Select pin
const int slaveSelect = 7;
const int dflagPin = 8;
signed long countOTR = 0;
signed long countCNTR = 0;
unsigned int confi0 = 0;
unsigned int confi1 = 0;

/*Check Overflow*/
int isOverflowed(){
  if (digitalRead(dflagPin) == LOW){
    return 1;
  }
  else{
    return 0;
  }
}
/*Init Counter*/
void initCounter() {
  
  // Set slave selects as outputs
  pinMode(slaveSelect, OUTPUT);
  
  // Raise select pin
  // Communication begins when you drop the individual select signsl
  digitalWrite(slaveSelect,HIGH);
  
  SPI.begin();
  
  // Initialize LS7366R
  //  Non Quadrature Mode
  //    Clock division factor: 0
  //    Index Reset Mode
  //    single cycle count mode
  //    x2 quatrature count mode (two counts per non quadrature cycle)
  //    Count Overflow
  //    Enable Counter
  // NOTE: For more information on commands, see datasheet
  digitalWrite(slaveSelect,LOW);        // Begin SPI conversation
  SPI.transfer((byte)(WR|MDR0));                       // Write to MDR0
  SPI.transfer((byte)MDR0_CONF); //Config settings in MDR0
  digitalWrite(slaveSelect,HIGH); //Terminate SPI conversation
  delayMicroseconds(100);// provides some breathing room between SPI conversations
  digitalWrite(slaveSelect,LOW);//Begin SPI  
  SPI.transfer((byte)(WR|MDR1)); //write to MDR1
  SPI.transfer((byte)MDR1_CONF);// Configure settings in MDR1
  digitalWrite(slaveSelect,HIGH);       // Terminate SPI conversation 


}
/*Read from CNTR*/
long readCNTR(){
    unsigned int count_1_CNTR,count_2_CNTR;
    long count_value;
    digitalWrite(slaveSelect,LOW);      // Begin SPI conversation
    SPI.transfer((byte)(RD|CNTR));
    count_1_CNTR = SPI.transfer(0x00);           // Read highest order byte
    count_2_CNTR = SPI.transfer(0x00);           // Read lowest order byte
    digitalWrite(slaveSelect,HIGH); //Terminate SPI conversation
  
  // Calculate encoder count
  count_value = (count_1_CNTR << 8) + count_2_CNTR;
  return count_value;
}
/*Read From OTR*/
long readOTR() {
  
  // Initialize temporary variables for SPI read
    unsigned int count_1, count_2;
    long count_value;  
    digitalWrite(slaveSelect,LOW);      // Begin SPI conversation
    SPI.transfer((byte)(LOAD | OTR));
    digitalWrite(slaveSelect, HIGH);// Load OTR (CNTR load to OTR)
    delayMicroseconds(100);// provides some breathing room between SPI conversations
    digitalWrite(slaveSelect,LOW);
    SPI.transfer((byte)(RD|OTR));                     // Request count from OTR
    count_1 = SPI.transfer(0x00);           // Read highest order byte
    count_2 = SPI.transfer(0x00);           // Read lowest order byte
    digitalWrite(slaveSelect,HIGH);     // Terminate SPI conversation 
  
  // Calculate encoder count
  count_value = (count_1 << 8) | count_2;
  return count_value;
}

void clearCount() {  
  // Set encoder1's data register to 0
  digitalWrite(slaveSelect,LOW);      // Begin SPI conversation  
  SPI.transfer((byte)(CLR | CNTR));   
  digitalWrite(slaveSelect,HIGH);     // Terminate SPI conversation   
}

unsigned int readMDR(int number){
  unsigned int confi = 0;
  if (number == 0){
    digitalWrite(slaveSelect,LOW);      // Begin SPI conversation  
    SPI.transfer((byte)(RD|MDR0));
    confi = SPI.transfer(0x00);
    digitalWrite(slaveSelect,HIGH);     // Terminate SPI conversation
    return confi;
  }
  else if(number == 1){
    digitalWrite(slaveSelect,LOW);      // Begin SPI conversation  
    SPI.transfer((byte)(RD|MDR1));
    confi = SPI.transfer(0x00);
    digitalWrite(slaveSelect,HIGH);     // Terminate SPI conversation
    return confi;
  }
}
void setup() {
 Serial.begin(9600);      // Serial com for data output
 pinMode(dflagPin,INPUT);
 initCounter();       Serial.println("Counter Initialized...");  
 confi0 = readMDR(0);
 confi1 = readMDR(1);
 Serial.print("Configuration of MDR0: ");Serial.print(confi0,BIN);
 Serial.print("Configuration of MDR1: ");Serial.print(confi1,BIN);
 clearCount();  Serial.println("Counter Cleared...");
}

void loop() {
 delay(500);
 
 // Retrieve current encoder counters
 countOTR = readOTR(); 
 //countCNTR = readCNTR();
 
 Serial.print("Count from OTR: "); Serial.print(countOTR);  
// Serial.print("Count from CNTR: "); Serial.print(countCNTR);
 
 //check Carry then reset the counter
 if (isOverflowed()){
  clearCount();
 }
}
