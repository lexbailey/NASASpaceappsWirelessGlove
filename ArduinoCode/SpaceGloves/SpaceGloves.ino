#include <LiquidCrystal.h>
#include<Wire.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int MPU1=0x68;  // I2C address of the MPU-6050
const int MPU2=0x69;
int16_t AcX1,AcY1,AcZ1,Tmp1,GyX1,GyY1,GyZ1;
int16_t AcX2,AcY2,AcZ2,Tmp2,GyX2,GyY2,GyZ2;
//Setting the Analog Point
 int f1=A0;
 int f2=A1;
 int f3=A2;
 int f4=A3;
 int f5=A4;
 //Initial data
 int flex1_int=0;
 int flex2_int=0;
 int flex3_int=0;
 int flex4_int=0;
 int flex5_int=0;
 // int data
 int flex1=0;
 int flex2=0;
 int flex3=0;
 int flex4=0;
 int flex5=0;
 
void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU2);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Wire.beginTransmission(MPU1);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial2.begin(9600);
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("2015 Space Challenge");
  lcd.setCursor(0, 1);
  lcd.print("  Space Gloves");
  delay(1000);
  selfadjust();
}

void loop() 
{
  screen1();
  screen2();
}

int Zoom()
{
  lcd.clear();
  int flex1_new=0;
  int flex1_pre=0;
  int flex2_new=0;
  int flex2_pre=0;
  float zoomrate=0;
  lcd.setCursor(0, 0);
  lcd.print("   Zoom   ");
  lcd.setCursor(0, 1);
  lcd.print("Rate:");
  lcd.setCursor(14, 1);
  lcd.print("%");
  while(analogRead(f4)-flex4_int<30)
 {
  flex1_new=analogRead(f1)-flex1_int;
  flex2_new=analogRead(f2)-flex2_int;
  if(flex1_new-flex1_pre>1)
  {
    zoomrate+=(flex1_new-flex1_pre);
  }
  else if(flex2_new-flex2_pre<-1)
  {
    zoomrate-=(flex2_pre-flex2_new);
  }
  flex1_pre=flex1_new;
  flex2_pre=flex2_new;
  lcd.setCursor(8, 1);
  lcd.print(100+zoomrate/5);
  lcd.setCursor(0, 2);
  lcd.print("+:F1    -:F2");
  lcd.setCursor(0, 3);
  lcd.print("Exit: Finger4");
  }
  lcd.clear();
  return screen1();
};

void selfadjust()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Before we Start:");
  lcd.setCursor(0, 1);
  lcd.print("Relax your hand");
  lcd.setCursor(0, 2);
  lcd.print("Once finished,move");
  lcd.setCursor(0, 3);
  lcd.print("the wrist");
  flex1_int=analogRead(f1);
  flex2_int=analogRead(f2);
  flex3_int=analogRead(f3);
  flex4_int=analogRead(f4);
  flex5_int=analogRead(f5);
  wristcontrol();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Finished!!");
  delay(1000);
  lcd.clear();
}

void Maininterface()
{
  lcd.setCursor(0, 0);
  lcd.print("  Space Gloves App");
  lcd.setCursor(0, 1);
  lcd.print("F1:");
  lcd.setCursor(0, 2);
  lcd.print("F2:");
  lcd.setCursor(0, 3);
  lcd.print("F3:");
};

int screen1()
{
  Maininterface();
  lcd.setCursor(8, 1);
  lcd.print("Zoom");
  lcd.setCursor(8, 2);
  lcd.print("Health");
  lcd.setCursor(8, 3);
  lcd.print("Gesture");
  getmpu();
  while (abs(AcX1)<8000&&abs(AcY1)<8000&&abs(AcZ1)<8000)
  {
    getmpu();
    if(analogRead(f1)-flex1_int>15)
    {
      Zoom();
    };
    if(analogRead(f2)-flex2_int>30)
    {
      gettemp();
    };
    if(analogRead(f3)-flex3_int>30)
    {
      gesture();
    }
  }
};

void screen2()
{
  lcd.clear();
  getmpu();
  while (abs(AcX1)<8000&&abs(AcY1)<8000&&abs(AcZ1)<8000)
  {
  getmpu();
  Maininterface();
  lcd.setCursor(8, 1);
  lcd.print("Reset");
  lcd.setCursor(8, 2);
  lcd.print("Remote");
  lcd.setCursor(8, 3);
  lcd.print("AXIS Data");
  if(analogRead(f1)-flex1_int>15)
    {
      selfadjust();
    };
    if(analogRead(f2)-flex2_int>30)
    {
      remote();
    };
    if(analogRead(f3)-flex3_int>30)
    {
      gyro();
    }
  }
}
int gettemp()
{ 
  lcd.clear();
  while(analogRead(f4)-flex4_int<30)
 {
  lcd.setCursor(0, 0);
  lcd.print(" Temperature ");
  getmpu();
  lcd.setCursor(0, 2);
  lcd.print(Tmp1);
  lcd.print(" Degree");
  lcd.setCursor(0, 3);
  lcd.print("Exit: Finger4");
 }
 lcd.clear();
 return screen1();
};

void wristcontrol()
{
  getmpu();
  while (abs(AcX1)<8000&&abs(AcY1)<8000&&abs(AcZ1)<8000)
  {getmpu();}
};

int gesture()
{
  lcd.clear();
  int g1,g2,g3,g4,g5;
  g1=analogRead(f1)-flex1_int;
  g2=analogRead(f2)-flex2_int;
  g3=analogRead(f3)-flex3_int;
  g4=analogRead(f4)-flex4_int;
  g5=analogRead(f5)-flex5_int;
  lcd.setCursor(0, 0);
  lcd.print(" Gesture ");
  lcd.setCursor(0, 2);
  lcd.print("Exit:Move Wrist");
  while(abs(AcX1)<8000&&abs(AcY1)<8000&&abs(AcZ1)<8000)
  {
    lcd.setCursor(0, 1);
    lcd.print("Action:         ");
  getmpu();
  g1=analogRead(f1)-flex1_int;
  g2=analogRead(f2)-flex2_int;
  g3=analogRead(f3)-flex3_int;
  g4=analogRead(f4)-flex4_int;
  g5=analogRead(f5)-flex5_int;
  if(g1>=20&&g2>=20)
  {
    if(g3>=20&&g4>=20&&g5>=20)
    {
      lcd.setCursor(8, 1);
      lcd.print("Grab");
      delay(500);
    }
    else
    {
      lcd.setCursor(8, 1);
      lcd.print(" Zoom ");
      delay(500);
    }
  }
  lcd.setCursor(8, 1);
      lcd.print("      ");
    if(g2<=20&&g3<=20&&g1>=20&&g4>=20)
    {
      lcd.setCursor(8, 1);
      lcd.print(" Drag ");
      delay(500);
    }
  }
 lcd.clear();
 return screen1();
};
void getmpu()
{
  Wire.beginTransmission(MPU1);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU1,14,true);  // request a total of 14 registers
  GyX1=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  GyY1=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  GyZ1=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp1=(Wire.read()<<8|Wire.read())/340.00+36.53;  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  AcY1=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  AcX1=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  AcZ1=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
};

void remote()
{
  for(;;)
  {
 flex1=analogRead(f1);
 flex2=analogRead(f2);
 flex3=analogRead(f3);
 flex4=analogRead(f4);
 flex5=analogRead(f5);
 
  Wire.beginTransmission(MPU1);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU1,14,true);  // request a total of 14 registers
  GyX1=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  GyY1=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  GyZ1=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp1=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  AcY1=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  AcX1=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  AcZ1=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  Wire.beginTransmission(MPU2);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU2,14,true);  // request a total of 14 registers
  GyX2=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  GyY2=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  GyZ2=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp2=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  AcY2=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  AcX2=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  AcZ2=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  Serial2.print("[");
  Serial2.print(flex1);Serial2.print(",");
  Serial2.print(flex2);Serial2.print(",");
  Serial2.print(flex3);Serial2.print(",");
  Serial2.print(flex4);Serial2.print(",");
  Serial2.print(flex5);Serial2.print(",");
  Serial2.print(GyX1);Serial2.print(",");
  Serial2.print(GyY1);Serial2.print(",");
  Serial2.print(GyZ1);Serial2.print(",");
  Serial2.print(Tmp1);Serial2.print(",");
  Serial2.print(AcX1);Serial2.print(",");
  Serial2.print(AcY1);Serial2.print(",");
  Serial2.print(AcZ1);Serial2.print(",");
  Serial2.print(GyX2);Serial2.print(",");
  Serial2.print(GyY2);Serial2.print(",");
  Serial2.print(GyZ2);Serial2.print(",");
  Serial2.print(Tmp2);Serial2.print(",");
  Serial2.print(AcX2);Serial2.print(",");
  Serial2.print(AcY2);Serial2.print(",");
  Serial2.print(AcZ2);
  Serial2.print("]");
  };
};

void gyro()
{
   int x,y;
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Position Data");
   lcd.setCursor(0, 1);
   lcd.print(" X     Y");
   getmpu();
   while(analogRead(f4)-flex4_int<30)
 {
   lcd.setCursor(0, 2);
   lcd.print("                     ");
   getmpu();
   GyX1=GyX1/200; GyY1=GyY1/200; 
   x=10*tan(GyX1*3.1416/180);
   y=10*tan(GyY1*3.1416/180);
   lcd.setCursor(0, 2);
   lcd.print(x); lcd.print(" | ");
   lcd.print(y);
   delay(100);
 }
 lcd.clear();
};
