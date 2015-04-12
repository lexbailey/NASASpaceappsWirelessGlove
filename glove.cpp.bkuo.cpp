#include "rs232.h"
#include "gears.h"

#ifndef GLOVETOLERANCE
#define GLOVETOLERANCE 6
#endif


typedef struct gloveData
{
	int16_t fing1;
	int16_t fing2;
	int16_t fing3;
	int16_t fing4;
	int16_t fing5;
	int16_t handGyX;
	int16_t handGyY;
	int16_t handGyZ;
	int16_t handTemp;
	int16_t handAccX;
	int16_t handAccY;
	int16_t handAccZ;
	int16_t armGyX;
	int16_t armGyY;
	int16_t armGyZ;
	int16_t armTemp;
	int16_t armAccX;
	int16_t armAccY;
	int16_t armAccZ;
} gloveData_t;

typedef enum 
{
	rest, zoom, move, rotate
} gloveMode_t;


gloveData_t grelax;
gloveData_t gtense;
gloveData_t gdiff;
gloveData_t gd;
gloveData_t gdnorm;
gloveData_t gstartstate;

gloveMode_t cMode;

gloveData_t operator - (gloveData_t a, gloveData_t b){
	gloveData_t result;
	result.fing1 = a.fing1 - b.fing1;
	result.fing2 = a.fing2 - b.fing2;
	result.fing3 = a.fing3 - b.fing3;
	result.fing4 = a.fing4 - b.fing4;
	result.fing5 = a.fing5 - b.fing5;
	result.handGyX = a.handGyX - b.handGyX;
	result.handGyY = a.handGyY - b.handGyY;
	result.handGyZ = a.handGyZ - b.handGyZ;
	result.handTemp = a.handTemp - b.handTemp;
	result.handAccX = a.handAccX - b.handAccX;
	result.handAccY = a.handAccY - b.handAccY;
	result.handAccZ = a.handAccZ - b.handAccZ;
	result.armGyX = a.armGyX - b.armGyX;
	result.armGyY = a.armGyY - b.armGyY;
	result.armGyZ = a.armGyZ - b.armGyZ;
	result.armTemp = a.armTemp - b.armTemp;
	result.armAccX = a.armAccX - b.armAccX;
	result.armAccY = a.armAccY - b.armAccY;
	result.armAccZ = a.armAccZ - b.armAccZ;
	return result;
}

float lx,ly,lz;
float sx,sy,sz;
float x,y,z;


void detectGesture(gloveData_t gnorm, gloveData_t gdiff, gloveData_t *gstartstate){
	gloveData_t gdiff1 = gdiff - gnorm;
	int f = 0;
	int finger1Tense = (gdiff1.fing1 <= 0 + GLOVETOLERANCE)? 1 <<4 : 0;
	int finger2Tense = (gdiff1.fing2 <= 0 + GLOVETOLERANCE)? 1 <<3 : 0;
	int finger3Tense = (gdiff1.fing3 <= 0 + GLOVETOLERANCE)? 1 <<2 : 0;
	int finger4Tense = (gdiff1.fing4 <= 0 + GLOVETOLERANCE)? 1 <<1 : 0;
	int finger5Tense = (gdiff1.fing5 <= 0 + GLOVETOLERANCE)? 1 : 0;

	f = finger1Tense + finger2Tense + finger3Tense + finger4Tense + finger5Tense;

	if(f == 0b01000){
		printf("FISTO!\n");
		if (cMode != rotate){
			(*gstartstate) = gnorm;
			sx = gnorm.handGyX/32768.0f;
			sy = gnorm.handGyY/32768.0f;
			sz = gnorm.handGyZ/32768.0f;
		}
		cMode = rotate;
		
	}
	else if(f == 0b00111){
		printf("ZOOOoooom!\n");
		cMode = zoom;
	}
	else if(f == 0b10011 ){
		printf("MOVE!\n");
		cMode = move;
	}
	else
	{
		cMode = rest;
	}
}



void doTheThing(){
	if (cMode == rotate){
		printf("%.3f, %.3f, %.3f\n", gstartstate.handGyX/32768.0f, gstartstate.handGyY/32768.0f, gstartstate.handGyZ/32768.0f);
		x = lx+((gstartstate.handGyX/32768.0f)-sx);
		y = ly+((gstartstate.handGyY/32768.0f)-sy);
		z = lz+((gstartstate.handGyZ/32768.0f)-sz);
		setRotation(x,y,z);

	}
	else{
		lx = x;
		ly = y;
		lz = z;
	}
}


int16_t blank;

#define COMPORT 7


void clearBuffer(int port){
	unsigned char buf[999];
	while (RS232_PollComport(port, buf, 999) == 999){}
}


bool parseData(gloveData_t *gd, char* str)
{
	int n = sscanf(str, "[%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd]", &(gd->fing1),&(gd->fing2),&(gd->fing3),&(gd->fing4),&(gd->fing5),&(gd->handGyX),&(gd->handGyY),&(gd->handGyZ),&(gd->handTemp),&(gd->handAccX),&(gd->handAccY),&(gd->handAccZ),&(gd->armGyX),&(gd->armGyY),&(gd->armGyZ),&(gd->armTemp),&(gd->armAccX),&(gd->armAccY),&(gd->armAccZ));
	return n > 0;
}

void printData(gloveData_t gd){

	printf("%hd, %hd, %hd, %hd, %hd, - , %hd, %hd, %hd\n", gd.fing1, gd.fing2, gd.fing3, gd.fing4, gd.fing5, gd.handGyX, gd.handGyY, gd.handGyZ);

}

void waitForEnter(){
	char c; scanf("%c", &c);
}

void acquireData(gloveData_t *gd, bool clear = false){
	unsigned char *buf;
	char *bigBuf;
	buf = (unsigned char*)malloc(sizeof(unsigned char) * 2);
	bigBuf = (char *)malloc(sizeof(char) * 100);
	int bbP = 0;
	buf[1] = '\0';
	int n;
	bool done = false;
	if(clear){
		clearBuffer(COMPORT);
	}
	while (!done){
		n = RS232_PollComport(COMPORT, buf, 1);
		if (n>0) {
			if (bbP > 98){bbP = 0;}
			bigBuf[bbP] = buf[0];
			bbP++; 
			if (buf[0] == ']'){
				bigBuf[bbP] = '\0';
				bbP = 0;
				//printf("%s\n", bigBuf);
				if (parseData(gd, bigBuf)){
					done = true;
				}
			}
		}
	}
}



void mainloop(){
	acquireData(&gd);
	gdnorm = gd - grelax;
	//printData(gdnorm);
	detectGesture(gdnorm, gdiff, &gstartstate);
	doTheThing();
}

int main(int argc, char **argv)
{
	lx = 0;
	ly = 0;
	lz = 0;
	if (RS232_OpenComport(COMPORT, 9600) == 0){
		cMode = rest;

		printf("connected\n");
		printf("Hold your hand in a relaxed position and press enter\n");
		waitForEnter();
		acquireData(&grelax,true);
		printData(grelax);

		printf("Hold your hand in a fist (or as best you can) and press enter\n");
		waitForEnter();
		acquireData(&gtense,true);
		printData(gtense);

		gdiff = gtense - grelax;

		printf("Difference is: \n");
		printData(gdiff);

		printf("press enter to continue\n");
		waitForEnter();
		acquireData(&gd,true);
		setOtherLoop(&mainloop);
		glMain(argc, argv);
	}
	else{
		printf("fail\n");
	}
	

	return 0;

}
