#include "Alarm.h"
#include <Arduino.h>

// Constructeur
Alarm::Alarm(int rPin, int gPin, int bPin, int buzzerPin, float* distancePtr)
  : _rPin(rPin), _gPin(gPin), _bPin(bPin), _buzzerPin(buzzerPin), _distance(distancePtr) {
  pinMode(_rPin, OUTPUT);
  pinMode(_gPin, OUTPUT);
  pinMode(_bPin, OUTPUT);
  pinMode(_buzzerPin, OUTPUT);

  _turnOff();
}

// Couleurs du gyrophare
void Alarm::setColourA(int r, int g, int b) {
  _colA[0] = r;
  _colA[1] = g;
  _colA[2] = b;
}

void Alarm::setColourB(int r, int g, int b) {
  _colB[0] = r;
  _colB[1] = g;
  _colB[2] = b;
}

void Alarm::setVariationTiming(unsigned long ms) {
  _variationRate = ms;
}

void Alarm::setDistance(float d) {
  _distanceTrigger = d;
}

void Alarm::setTimeout(unsigned long ms) {
  _timeoutDelay = ms;
}

void Alarm::turnOff() {
  _turnOffFlag = true;
}

void Alarm::turnOn() {
  _turnOnFlag = true;
}

void Alarm::test() {
  _state = TESTING;
  _testStartTime = millis();
}

AlarmState Alarm::getState() const {
  return _state;
}

void Alarm::_setRGB(int r, int g, int b) {
  analogWrite(_rPin, r);
  analogWrite(_gPin, g);
  analogWrite(_bPin, b);
}

void Alarm::_turnOff() {
  _setRGB(0, 0, 0);
  digitalWrite(_buzzerPin, LOW);
}

void Alarm::update() {
  _currentTime = millis();

  switch (_state) {
    case OFF:
      _offState();
      break;
    case WATCHING:
      _watchState();
      break;
    case ON:
      _onState();
      break;
    case TESTING:
      _testingState();
      break;
  }
}

void Alarm::_offState() {
  _turnOff();
  if (_turnOnFlag) {
    _turnOnFlag = false;
    _state = WATCHING;
  }
}

void Alarm::_watchState() {
  if (*_distance <= _distanceTrigger) {
    _state = ON;
    _lastDetectedTime = millis();
    _lastUpdate = millis();
    _currentColor = false;
  } else {
    _turnOff();
  }
}

void Alarm::_onState() {
  if (_currentTime - _lastUpdate >= _variationRate) {
    _lastUpdate = _currentTime;

    if (_currentColor) {
      _setRGB(_colA[0], _colA[1], _colA[2]);
    } else {
      _setRGB(_colB[0], _colB[1], _colB[2]);
    }

    _currentColor = !_currentColor;

    digitalWrite(_buzzerPin, _currentColor);
  }

  if (*_distance > _distanceTrigger) {
    if (_currentTime - _lastDetectedTime > _timeoutDelay) {
      _state = WATCHING;
      _turnOff();
    }
  } else {
    _lastDetectedTime = _currentTime;
  }
}

void Alarm::_testingState() {
  if (_currentTime - _testStartTime < 3000) {
    _onState();
  } else {
    _state = OFF;
    _turnOff();
  }
}
