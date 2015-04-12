/*
	This is the header file for comHandler.cpp
	This is where you will find the class for talking to the printer
*/

#ifndef COMHANDLER_INCLUDE
#define COMHANDLER_INCLUDE

#include "rs232.h"

#define MAIN_COM 0

class comHandler{
	private:
		bool pConnected;	//true if connected
		bool serialRunning;	//true if a connection is desired (was desired recently)
		const char* mport;	//port name
		int mbaud;		//baud rate
		pthread_t serial_tid;	//pointer to thread that handles serial
		
		//these are callbacks for various parts of the thread that handles serial.
		void (*openComFail)();
		void (*openComSuccess)();
		void (*gracefulDisconnect)();
		void (*message)(char *);
		void (*returnMessage)(int , float, float, float, float, float, float, float);

		//this is the function that does all the clever
		void connectionFunc(const char* port, int baud);
		
	public:
		void* serialThread(void *args);
		void connect(const char* port, int baud);
		void disconnect();
		int sendCommand(const char* command);
		void setCallbacks (
					void (*mfopenComFail)(),
					void (*mfopenComSuccess)(),
					void (*mfgracefulDisconnect)(),
					void (*mfmessage)(char *),
					void (*mfreturnMessage)(int , float, float, float, float, float, float, float)
		);

		comHandler();
		~comHandler();
};

#endif
