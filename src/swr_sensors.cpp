#include "swr_sensors.h"
#include "swr_constants.h"
#include "swr_power.h"
#include "swr_eeprom.h"

SensorData readSensors(SensorData lastData) {
  RawSensorData rawData = readSensorsRaw();

  SensorData data;

  float voltageFwd = adcToVoltage(rawData.fwdVoltage, true);
  data.fwdPower = voltageToPower(voltageFwd);
  data.fwdPower = data.fwdPower * SENSOR_AVERAGING_ALPHA + lastData.fwdPower * (1.0 - SENSOR_AVERAGING_ALPHA);

  float voltageRvr = adcToVoltage(rawData.reflVoltage, false);
  data.reflPower = voltageToPower(voltageRvr);
  data.reflPower = data.reflPower * SENSOR_AVERAGING_ALPHA + lastData.reflPower * (1.0 - SENSOR_AVERAGING_ALPHA);

  if( data.reflPower > data.fwdPower )
    data.reflPower = data.fwdPower;

  if( data.fwdPower >= TRANSMIT_THREASHOLD_POWER ) {
    if( envelopeDetectorForSwr() )
      data.swr = powerToSwr(data.fwdPower, data.reflPower);
    else if( differentialForSwr() )
      data.swr = dbToSwr(data.differentialMagnitudeDb);
    data.active = true;
  }
  else {
    data.swr = 1.0;
    data.differentialMagnitudeDb = 0.0;
    data.differentialPhaseDeg = 0.0;
    data.differentialMagnitudeDbShifted = 0.0;
    data.differentialPhaseDegShifted = 0.0;
    data.active = false;
    return data;
  }

  float magnitudeNormalized = (((float)rawData.differentialMagnitude) / ((float)rawData.differentialVref)) * 2.0 - 1.0;
  data.differentialMagnitudeDb = magnitudeNormalized * 30.0;
  if( lastData.active )
    data.differentialMagnitudeDb = data.differentialMagnitudeDb * SENSOR_AVERAGING_ALPHA + lastData.differentialMagnitudeDb * (1.0 - SENSOR_AVERAGING_ALPHA);

  float phaseNormalized = 1.0 - (((float)rawData.differentialPhase) / ((float)rawData.differentialVref));
  data.differentialPhaseDeg = phaseNormalized * 180.0;
  if( lastData.active )
    data.differentialPhaseDeg = data.differentialPhaseDeg * SENSOR_AVERAGING_ALPHA + lastData.differentialPhaseDeg * (1.0 - SENSOR_AVERAGING_ALPHA);

  float magnitudeNormalizedShifted = (((float)rawData.differentialMagnitudeShifted) / ((float)rawData.differentialVrefShifted)) * 2.0 - 1.0;
  data.differentialMagnitudeDbShifted = magnitudeNormalizedShifted * 30.0;
  if( lastData.active )
    data.differentialMagnitudeDbShifted = data.differentialMagnitudeDbShifted * SENSOR_AVERAGING_ALPHA + lastData.differentialMagnitudeDbShifted * (1.0 - SENSOR_AVERAGING_ALPHA);

  float phaseNormalizedShifted = 1.0 - (((float)rawData.differentialPhaseShifted) / ((float)rawData.differentialVrefShifted));
  data.differentialPhaseDegShifted = phaseNormalizedShifted * 180.0;
  if( lastData.active )
    data.differentialPhaseDegShifted = data.differentialPhaseDegShifted * SENSOR_AVERAGING_ALPHA + lastData.differentialPhaseDegShifted * (1.0 - SENSOR_AVERAGING_ALPHA);

  return data;
}

RawSensorData readSensorsRaw() {
  RawSensorData data;

  data.fwdVoltage = analogRead(POWER_FWD_PIN);
  data.reflVoltage = analogRead(POWER_REFL_PIN);
  data.differentialVref = analogRead(DIFFERENTIAL_VREF_PIN);
  data.differentialPhase = analogRead(DIFFERENTIAL_PHASE_PIN);
  data.differentialMagnitude = analogRead(DIFFERENTIAL_MAGNITUDE_PIN);
  data.differentialVrefShifted = analogRead(SHIFTED_DIFFERENTIAL_VREF_PIN);
  data.differentialPhaseShifted = analogRead(SHIFTED_DIFFERENTIAL_PHASE_PIN);
  data.differentialMagnitudeShifted = analogRead(SHIFTED_DIFFERENTIAL_MAGNITUDE_PIN);

  return data;
}
