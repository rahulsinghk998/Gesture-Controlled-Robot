#include<Wire.h>
#include<avr/io.h>
#include<avr/interrupt.h>

#define LEDPIN         13
#define BAUDRATE       9600
#define THRESHOLD_X1   6000
#define THRESHOLD_X2   8500
#define THRESHOLD_X3   1100
#define THRESHOLD_X4   1350
#define THRESHOLD_X5   1500
#define THRESHOLD_Y1   6000
#define THRESHOLD_Y2   8500
#define THRESHOLD_Y3   1100
#define THRESHOLD_Y4   1350
#define THRESHOLD_Y5   1500

#define SAMPLE_FREQ    25

//#define address 0x1E //0011110b, I2C 7bit address of HMC5883
const int MPU_1=0x68;  // I2C address of the MPU-6050
const int MPU_2=0x69;  // I2C address of the MPU-6050

int tobe_sent=0;
int16_t x1,x2;
int16_t y1,y2;
int ocra_val=15625/SAMPLE_FREQ;


void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_1);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  
  Wire.beginTransmission(MPU_2);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(BAUDRATE);
  
  //Added while loop// Loop will run untill the communication is started
  while(!Serial){ }
  
    pinMode(LEDPIN, OUTPUT);
    // initialize Timer1
    cli();          // disable global interrupts
    TCCR1A = 0;     // set entire TCCR1A register to 0
    TCCR1B = 0;     // same for TCCR1B
 
    // set compare match register to desired timer count:
    OCR1A = ocra_val;        //25 samples per second
    // turn on CTC mode:
    TCCR1B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler:
    TCCR1B |= (1 << CS10);
    TCCR1B |= (1 << CS12);
    // enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);
    // enable global interrupts:
    sei();
}

//Function Declarations
ISR(Timer1_COMPA_vect) // timer compare interrupt service routine
{
  Serial.write(tobe_sent);
  //digitalWrite(ledPin, digitalRead(ledPin) ^ 1); // toggle LED pin
}



void error_reduction(int state, int *count)
{
  if(state==1){
    *count = *count-(*count%10);
    }
  else if(state==2){
    *count = *count-((*count/10)*10);
  }
}


int MakeDecision(int16_t AcX_1,int16_t AcX_2,int16_t AcY_1,int16_t AcY_2)
{
   if(AcX_1>7000){
   error_reduction(1, &tobe_sent);
   tobe_sent= tobe_sent+1;
 }
 if(AcX_2>7000){
   error_reduction(2, &tobe_sent);
   tobe_sent= tobe_sent+10;
 }
 if(AcX_1<-7000){
    error_reduction(1, &tobe_sent);
   tobe_sent= tobe_sent+2;
 }
 if(AcX_2<-7000){
    error_reduction(2, &tobe_sent);
   tobe_sent= tobe_sent+20;
 }
 if(AcY_1>7000){
    error_reduction(1, &tobe_sent);
   tobe_sent= tobe_sent+3;
 }
 if(AcY_2>7000){
    error_reduction(2, &tobe_sent);
   tobe_sent= tobe_sent+30;
 }
 if(AcY_1<-7000){
    error_reduction(1, &tobe_sent);
   tobe_sent= tobe_sent+4;
}
 if(AcY_2<-7000){
    error_reduction(2, &tobe_sent);
   tobe_sent= tobe_sent+40;
}
if((AcX_2<7000)&&(AcX_2>-7000)&&(AcY_2<7000)&&(AcY_2>-7000)){
    error_reduction(2, &tobe_sent);
   tobe_sent= tobe_sent+0;
}
if((AcX_1<7000)&&(AcX_1>-7000)&&(AcY_1<7000)&&(AcY_1>-7000)){
    error_reduction(1, &tobe_sent);
   tobe_sent= tobe_sent+0;
  }
}

  
void loop()
{
  Wire.beginTransmission(MPU_1);
  Wire.write(0x3B);                   // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_1,4,true);     // request a total of 14 registers
  
  x1=Wire.read()<<8|Wire.read();      // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  x2=Wire.read()<<8|Wire.read();      // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  
  Wire.beginTransmission(MPU_2);
  Wire.write(0x3B);                   // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_2,4,true);     //request a total of 14 registers
  
  y1=Wire.read()<<8|Wire.read();      // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  y2=Wire.read()<<8|Wire.read();      // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  
  MakeDecision(x1,x2,y1,y2);
}



