#include "swr_power.h"
#include "swr_constants.h"
#include "swr_eeprom.h"

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float adcToFwdVoltage(uint16_t adcValue) {
  return mapFloat(adcValue, calibrationDataDummy(5.0).fwd, calibrationDataDummy(200.0).fwd, powerToVoltage(LOW_POWER), powerToVoltage(HIGH_POWER));
}

uint16_t fwdVoltageToAdc(float voltage) {
  return mapFloat(voltage, powerToVoltage(LOW_POWER), powerToVoltage(HIGH_POWER), calibrationDataDummy(5.0).fwd, calibrationDataDummy(200.0).fwd);
}

float adcToRvrVoltage(uint16_t adcValue) {
  return mapFloat(adcValue, calibrationDataDummy(5.0).rvr, calibrationDataOpen(), powerToVoltage(0.0), powerToVoltage(5.0));
}

uint16_t RvrVoltageToAdc(float voltage) {
  return mapFloat(voltage, powerToVoltage(0.0), powerToVoltage(5.0), calibrationDataDummy(5.0).rvr, calibrationDataOpen());
}

float voltageToPower(float voltage) {
  return (voltage*voltage) / 50.0;
}

float powerToVoltage(float power) {
  return sqrt(power * 50.0);
}

void updateComplex(float *magnitudeDb, float *phase) {
  float adcMax = analogRead(COMPLEX_VREF_PIN);

  float adcMagnitude = analogRead(COMPLEX_MAGNITUDE_PIN);
  float magnitudeNormalized = (adcMagnitude / adcMax) * 2.0 - 1.0;
  *magnitudeDb = magnitudeNormalized * 30.0;

  float adcPhase = analogRead(COMPLEX_PHASE_PIN);
  float phaseNormalized = 1.0 - (adcPhase / adcMax);
  *phase = phaseNormalized * 180.0;
}

void updatePower(float *power_fwd, float *power_rvr) {
  uint16_t adcFwdValue = analogRead(POWER_FWD_PIN);
  float voltageFwd = adcToFwdVoltage(adcFwdValue);
  *power_fwd = voltageToPower(voltageFwd);

  uint16_t adcRvrValue = analogRead(POWER_RVR_PIN);
  float voltageRvr = adcToRvrVoltage(adcRvrValue);
  *power_rvr = voltageToPower(voltageRvr);
}
