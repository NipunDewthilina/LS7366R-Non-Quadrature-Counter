

#ifndef LS7366R_h
#define LS7366R_h

#define CLR B00000000
#define RD B01000000
#define WR B10000000
#define LOAD B11000000

#define MDR0 B00001000
#define MDR1 B00010000
#define DTR B00011000
#define CNTR B00100000
#define OTR B00101000
#define STR B00110000

// filter factor 1
// async index
// no index
// free-running
// 4x quadrature
#define MDR0_CONF B00110100

// no flag
// enabled
// 32 bits
#define MDR1_CONF B10100010

//functions
int isOverflowed();
void initCounter();
long readCNTR();
long readOTR() ;
void clearCount();
unsigned int readMDR(int number);

#endif
