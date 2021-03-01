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

NodeConnector nodeConnector("pm");

// sensor for ambient temperature, humidity and pressure
Bme280Plugin bme250Plugin = Bme280Plugin("bme", true, D2, D1);

// button to turn on display when needed
InterruptPlugin interruptPlugin = InterruptPlugin("d5", D5);

// SCT-013 current sensor used for consumed power measurement
PowerSensorPlugin powerSensorPlugin = PowerSensorPlugin("pow", A0);

void setup() {
  Serial.begin(1000000);

  delay(100);
  pinMode(D5, INPUT); // digital input for control button
 
  displayInit();
  displayPrintStr("Loading...");

  if (nodeConnector.setup(D5, false, 3000)) {
    Serial.println(F("State Machine Definition loaded"));
  } else {
    Serial.println(F("No SMD. Halt."));
    while(true) delay(100000);
  }
  nodeConnector.sm.registerAction("display_off", displayOff);
  nodeConnector.sm.registerAction("display_on", displayOn);
  nodeConnector.sm.registerAction("display_update", updateDisplay);

  nodeConnector.sm.registerPlugin(&bme250Plugin);
  nodeConnector.sm.registerPlugin(&interruptPlugin);
  nodeConnector.sm.registerPlugin(&powerSensorPlugin);

  Serial.println(F("Actions and plugins registered"));

  nodeConnector.start();
  Serial.println(F("State machine initialized"));

  displayPrintStr("Loaded");

  delay(2000);
}

void loop() {
  nodeConnector.loop();
}
