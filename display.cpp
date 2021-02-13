#include <Wire.h>
#include <limits.h>
#include <U8g2lib.h>
//https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
//https://github.com/olikraus/u8g2/wiki/u8g2reference
//https://github.com/olikraus/u8g2/wiki/setup_tutorial

#include <StateMachine.h>

U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R2, D1, D2);

const int sclPin = D1;
const int sdaPin = D2;

char dspBuffer[20];

bool displayEnabled = true;

long int dspPower = 0;
float dspTemp = 0.0;
int dspHumidity = 0;
int dspPressure = 0;

const char *tplPower = "%dW";
const char *tplMetter = "%dkWh";
const char *tplTemp = "%0.1fC";
const char *tplHumidity = "%d%%";
const char *tplPressure = "%dhPa";
const char *tplTime = "%02d:%02d";

void displayOff(ActionContext *ctx)
{
  displayEnabled = false;
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}

void displayOn(ActionContext *ctx)
{
  displayEnabled = true;
}

void displayInit(){
  u8g2.begin();
}

void displayPrintStr(const char* str){
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(0, 20, str);
  u8g2.sendBuffer();
}

void updateDisplay(ActionContext *ctx){
  u8g2.clearBuffer();

  if (!displayEnabled) {
    u8g2.sendBuffer();
    return;
  }

  long int power = ctx->compute->getVarInt("pow.w");
  long int peakMetter = ctx->compute->getVarInt("pow.peak");
  long int offPeakMetter = ctx->compute->getVarInt("pow.off_peak");
  long int totalMetter = ctx->compute->getVarInt("pow.total");
  bool isPeakTime = ctx->compute->getVarInt("pow.is_peak") > 0;
 
  float temp = ctx->compute->getVarInt("bme.temp_c");
  long int humidity = ctx->compute->getVarInt("bme.hum");
  long int pressure = ctx->compute->getVarInt("bme.pres_hpa");

  u8g2.setFont(u8g2_font_6x10_tf);

  // 1st line

  // indicate active day/night time counter
  u8g2.drawStr(isPeakTime ? 0 : 56, 10, ">");

  sprintf(dspBuffer, tplMetter, peakMetter);
  u8g2.drawStr(8, 10, dspBuffer);

  sprintf(dspBuffer, tplMetter, offPeakMetter);
  u8g2.drawStr(64, 10, dspBuffer);

  // 2nd line
  
  sprintf(dspBuffer, tplPower, power);
  u8g2.drawStr(0, 21, dspBuffer);
  
//  sprintf(TimeBuff, tplTime, hour(), minute());
//  u8g2.drawStr(70, 21, TimeBuff);
//
//  if (dspStatus == STATUS_UPLOADING) {
//    u8g2.drawStr(120, 21, "*");
//  }

  // 3rd line
  sprintf(dspBuffer, tplTemp, temp);
  u8g2.drawStr(0,32, dspBuffer);

  sprintf(dspBuffer, tplHumidity, humidity);
  u8g2.drawStr(38,32, dspBuffer);
  
  sprintf(dspBuffer, tplPressure, pressure);
  u8g2.drawStr(70,32, dspBuffer);

  u8g2.sendBuffer();
}
