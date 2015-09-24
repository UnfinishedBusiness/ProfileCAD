#ifndef Machine_
#define Machine_
#include <application.h>

void machineInit();
void machineConnect();
void machineDisconnect();
void machineWrite(std::string);
std::string machineRead();

void machineTinyGCommand(std::string c);

#endif
