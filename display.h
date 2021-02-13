#ifndef display_h
#define display_h

#include <StateMachine.h>

void displayInit();

void displayOn(ActionContext *);
void displayOff(ActionContext *);
void updateDisplay(ActionContext *);

void displayPrintStr(const char*);

#endif
