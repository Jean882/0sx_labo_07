#include "Alarm.h"
#include <Arduino.h>

// Constructeur
Alarm::Alarm(int rPin, int gPin, int bPin, int buzzerPin, float* distancePtr)
  : _rPin(rPin), _gPin(gPin), _bPin(bPin), _buzzerPin(buzzerPin), _distance(distancePtr) {
  pinMode(_rPin, OUTPUT);
  pinMode(_gPin, OUTPUT);
  pinMode(_bPin, OUTPUT);
  pinMode(_buzzerPin, OUTPUT);

  _setRGB(0, 0, 0);
  digitalWrite(_buzzerPin, LOW);
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
  analogWrite(_rPin, LOW);
  analogWrite(_gPin, HIGH);
  analogWrite(_bPin, LOW);
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

// État OFF : tout est éteint
void Alarm::_offState() {
  _turnOff();
  if (_turnOnFlag) {
    _turnOnFlag = false;
    _state = WATCHING;
  }
}

// État de surveillance : déclenche si présence détectée
void Alarm::_watchState() {
  if (_turnOffFlag) {
    _turnOffFlag = false;
    _state = OFF;
  } else if (*_distance < _distanceTrigger) {
    _state = ON;
    _lastDetectedTime = _currentTime;
  }
}

// État ON : clignotement des couleurs et buzzer
void Alarm::_onState() {
  if (_currentTime - _lastUpdate >= _variationRate) {
    _lastUpdate = _currentTime;

    if (_currentColor) {
      _setRGB(_colA[0], _colA[1], _colA[2]);
    } else {
      _setRGB(_colB[0], _colB[1], _colB[2]);
    }

    _currentColor = !_currentColor;

    // Active le buzzer par impulsions
    digitalWrite(_buzzerPin, _currentColor);
  }

  // Gère la désactivation après éloignement
  if (*_distance >= _distanceTrigger) {
    if (_currentTime - _lastDetectedTime > _timeoutDelay) {
      _state = WATCHING;
      _turnOff();
    }
  } else {
    _lastDetectedTime = _currentTime;
  }
}

// État de test (simule ON pendant 3 sec)
void Alarm::_testingState() {
  if (_currentTime - _testStartTime < 3000) {
    _onState(); // Simule l'état ON
  } else {
    _state = OFF;
    _turnOff();
  }
}
