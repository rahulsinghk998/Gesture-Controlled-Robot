#include <Servo.h>

Servo myservo1;
Servo myservo2;
Servo myservo3;

int flag=0;                
///////////////////////////////////////////////////////////////////
void pins_setup(void);
void MetalDetector(int *left, int *right);
void Front(int front_speed);
void Back(int back_speed);
void Right_turn(int right_speed);
void Left_turn(int left_speed);
void Stall(void);
int Distance(void);
void gripper_control(void);
void locomotion(void);
void Robot_Motion(void);
void debug(void);
//////////////////////////////////////////////////////////////////

//Main Code
void setup()
{
  pins_setup();
  Serial.begin(9600);
}

void loop()
{
  //debug();
//  locomotion();
// gripper_control();
Robot_Motion();
}
                
/////////////////////////////////////////////////////////////////
void pins_setup(void)
{
 pinMode(2, OUTPUT);//Trigger Pin
 pinMode(4, INPUT);//Echo Pin
   
 pinMode(7, OUTPUT);//Right Wheel 1
 pinMode(8, OUTPUT);//Right Wheel 2
 pinMode(12, OUTPUT);//Left Wheel 1
 pinMode(13, OUTPUT);//Left Wheel 2
 
 pinMode(3, OUTPUT);//Enable of Motor Driver
 
myservo1.attach(9);  // attaches the servo on pin 9 to the servo object 
  myservo2.attach(10);  // attaches the servo on pin 9 to the servo object 
  myservo3.attach(11);  // attaches the servo on pin 9 to the servo object 
 
}
/////////////////////////////////////////////////////////////////////////
void MetalDetector(int *left, int *right)
{
  *left=analogRead(A0);
  *right=analogRead(A1);
}
/////////////////////////////////////////////////////////////////////
void Front(int front_speed)
{
  analogWrite(3, front_speed);
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
  
}


void Back(int back_speed)
{
  analogWrite(3, back_speed);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  
}


void Right_turn(int right_speed)
{
  analogWrite(3, right_speed);
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  
}


void Left_turn(int left_speed)
{
  analogWrite(3, left_speed);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
  
}

void Stall(void)
{
  analogWrite(3, 0);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
  
}

/////////////////////////////////////////////////////////////
int Distance(void)
{
  /*
    The circuit:
	* VCC connection of the sensor attached to +5V
	* GND connection of the sensor attached to ground
	* TRIG connection of the sensor attached to digital pin 2
	* ECHO connection of the sensor attached to digital pin 4
  */
  int duration, distance;
  digitalWrite(2, LOW);
  delayMicroseconds(2);
  digitalWrite(2, HIGH);
  delayMicroseconds(10);
  digitalWrite(2, LOW);

  duration = pulseIn(4, HIGH);
  distance = duration / 29 / 2;
  if(distance==0)distance=300;
  return distance;
}
/////////////////////////////////////////////////////////

void gripper_control(void)
{
 int data_servo=0;
 if (Serial.available() > 0) 
{ 
 data_servo = Serial.read();
 data_servo = (data_servo/10);
 
 if(data_servo==1)
 {
   myservo1.write(0);
   //delay(15);
   Serial.println("Initial Hinge Position");
 }
 else if(data_servo==2)
 {
   myservo1.write(90);
   //delay(15);
   Serial.println("Moved Hinge Position");
 }
 else if(data_servo==3)
 {
   myservo2.write(0);
   //delay(15);
   myservo3.write(0);
   //delay(15); 
   Serial.println("Grabbed");
 }
 else if(data_servo==4)
 {
     myservo2.write(90);
     //delay(15);
     myservo3.write(90);
     //delay(15);
     Serial.println("Open");
 }
 //Serial.println(data_servo);
}
}
/////////////////////////////////////////////////////////

void locomotion(void)
{
  int locomotion_data=0;
  int Left, Right;
  int distance_obs = 0;
  
 if (Serial.available() > 0) 
{ 
 locomotion_data = Serial.read();
 
  MetalDetector(&Left,&Right);
  locomotion_data = (locomotion_data%10);
 // distance_obs = Distance();
  
  if(locomotion_data==0)
  {
    flag=1;
    Stall();
    Serial.println("Stopped");
  }
  else if(locomotion_data==1)
  {
    flag=0;
    if((Left<50)&&(Right<50))
     {Back(100);
   Serial.println("Moving Back");}
     else if((Left>50)&&(Right>50))
     {Front(100);
   Serial.println("Moving Forward");}
  }
  else if(locomotion_data==2)
  {
    flag=0;
     if((Left<50)&&(Right<50))
     {Front(100);
   Serial.println("Moving Forward");}
     else if((Left>50)&&(Right>50))
     {Back(100);
   Serial.println("Moving Back");}
   }
  else if(locomotion_data==3)
  {
    flag=0;
    if(Right<50)
    {Right_turn(100);
  Serial.println("Turning Right");}
    else if(Right>50)
    {Left_turn(100);
  Serial.println("Turning Left");}
  }
  else if(locomotion_data==4)
  {
    flag=0;
    if(Left<50)
    {Left_turn(100);
  Serial.println("Turning Left");}
    else if(Left>50)
    {Right_turn(100);
  Serial.println("Turning Right");}
  }
 // Serial.println(locomotion_data);
}
}
/////////////////////////////////////////////////////////
void Robot_Motion(void)
{
 

    locomotion();
   if(flag==1)
    gripper_control();
  
}

//////////////////////////////////////////////////////////
void debug(void)
{
  int locomotion_data;
   if (Serial.available() > 0) 
{   
      locomotion_data = Serial.read();
      locomotion_data = (locomotion_data%10);
      Serial.println(locomotion_data);
} 
}
//////////////////////////////////////////////////////////
