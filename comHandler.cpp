#include "comHandler.h"

bool serialRunning = false;
bool threadRunning = true;
const char* mport;
int mbaud;

comHandler * comHandler_ME;

void comHandler::connectionFunc(const char* port, int baud){
	if (RS232_OpenComport(MAIN_COM, baud, port))
	{
		pConnected = false;
		serialRunning = false;

		if (openComFail!=NULL){
			openComFail();
		}

	}
	else{
		pConnected = true;
		serialRunning = true;

		if (openComSuccess!=NULL){
			openComSuccess();
		}

		while(serialRunning){
			//poll rx
			int n;
			unsigned char buf[4096];
			if (n=RS232_PollComport(MAIN_COM, buf, 4095)){
				buf[n] = '\0';

				if (message!=NULL){
					message((char*)buf);
				}

			}
		}
	}
	RS232_CloseComport(MAIN_COM);
	pConnected = false;

	if (gracefulDisconnect!=NULL){
		gracefulDisconnect();
	}

}

void comHandler::connect(const char* port, int baud){
	mport = port;
	mbaud = baud;
	serialRunning = true;
}

void comHandler::disconnect(){
	serialRunning = false;
}

void* comHandler::serialThread(void *args){
	while (threadRunning){
		if (serialRunning) connectionFunc(mport, mbaud);
	}
}

int comHandler::sendCommand(const char* command){
	if (!pConnected) {
		return 1; //not connected
	}
	else
	{
		RS232_SendBuf(MAIN_COM, (unsigned char *) command, strlen(command));
		RS232_SendByte(MAIN_COM, '\n');
		return 0;
	}
}

void * bounceSerialThread(void * args){
	comHandler_ME->serialThread(args);
}

void comHandler::setCallbacks (
			void (*mfopenComFail)(),
			void (*mfopenComSuccess)(),
			void (*mfgracefulDisconnect)(),
			void (*mfmessage)(char *),
			void (*mfreturnMessage)(int , float, float, float, float, float, float, float)
){
 openComFail=mfopenComFail;
 openComSuccess=mfopenComSuccess;
 gracefulDisconnect=mfgracefulDisconnect;
 message=mfmessage;
 returnMessage=mfreturnMessage;
}

comHandler::comHandler(){
	comHandler_ME = this;
	pConnected = false;
	serialRunning = false;
	pthread_create (&serial_tid, NULL, bounceSerialThread, NULL);
}

comHandler::~comHandler(){
	threadRunning = false;
	pthread_join(serial_tid, NULL);
}
