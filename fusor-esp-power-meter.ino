/*
 *  Power metering node
 *  See readme.md for more details
 */

// Uncomment and rebuild libraries for debug messages
// #define SM_DEBUGGER

#include <Arduino.h>

#include <StateMachine.h>
#include <StateMachineDebug.h>
#include <NodeConnector.h>
#include <Bme280Plugin.h>
#include <InterruptPlugin.h>
#include <PowerSensorPlugin.h>
#include <DSCAlarmSystemPlugin.h>

#include "display.h"

void sleepFunction(unsigned long ms)
{
  delay(ms);
}

unsigned long getTime()
{
  return millis();
}

void debugPrinter(const char *message)
{
  Serial.print(message);
}

NodeConnector nodeConnector;

StateMachineController sm = StateMachineController("pm", sleepFunction, getTime);

// sensor for ambient temperature, humidity and pressure
Bme280Plugin bme250Plugin = Bme280Plugin("bme", true, D2, D1);

// button to turn on display when needed
InterruptPlugin interruptPlugin = InterruptPlugin("d5", D5);

// SCT-013 current sensor used for consumed power measurement
PowerSensorPlugin powerSensorPlugin = PowerSensorPlugin("pow", A0);

void setup() {
#ifdef SM_DEBUGGER
  __debugPrinter = debugPrinter;
#endif
 
  Serial.begin(1000000);

  delay(100);
  pinMode(D5, INPUT); // digital input for control button
 
  displayInit();
  displayPrintStr("Loading...");

  nodeConnector.setup(D5, false, 3000);
  if (nodeConnector.isSmdLoaded) Serial.println(F("State Machine Definition loaded"));
  else {
    Serial.println(F("No SMD. Halt."));
    while(true) delay(100000);
  }

  nodeConnector.initSM(&sm);
  Serial.println(F("State Machine created"));

  sm.registerAction("display_off", displayOff);
  sm.registerAction("display_on", displayOn);
  sm.registerAction("display_update", updateDisplay);

  sm.registerPlugin(&bme250Plugin);
  sm.registerPlugin(&interruptPlugin);
  sm.registerPlugin(&powerSensorPlugin);

  Serial.println(F("Actions and plugins registered"));

  sm.init();
  Serial.println(F("State machine initialized"));

  displayPrintStr("Loaded");

  delay(2000);
}

void loop() {
  sm.cycle();
  nodeConnector.loop();
}
