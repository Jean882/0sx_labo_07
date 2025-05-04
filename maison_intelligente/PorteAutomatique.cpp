#include "PorteAutomatique.h"
#include <Arduino.h>

// Constructeur
PorteAutomatique::PorteAutomatique(int p1, int p2, int p3, int p4, float& distancePtr)
  : _stepper(AccelStepper::HALF4WIRE, p1, p3, p2, p4), _distance(distancePtr) {
  _stepper.setMaxSpeed(1000); // Ajustable selon le moteur
  _stepper.setAcceleration(500);
  _stepper.setSpeed(200);
}

// Doit être appelée dans loop()
void PorteAutomatique::update() {
  _currentTime = millis();
  _stepper.run();

  _mettreAJourEtat();
  
  switch (_etat) {
    case OUVERTE:
      _ouvertState();
      break;
    case FERMEE:
      _fermeState();
      break;
    case EN_OUVERTURE:
      _ouvertureState();
      break;
    case EN_FERMETURE:
      _fermetureState();
      break;
  }
}

// Réglages
void PorteAutomatique::setAngleOuvert(float angle) {
  _angleOuvert = angle;
}

void PorteAutomatique::setAngleFerme(float angle) {
  _angleFerme = angle;
}

void PorteAutomatique::setPasParTour(int steps) {
  _stepsPerRev = steps;
}

void PorteAutomatique::setDistanceOuverture(float distance) {
  _distanceOuverture = distance;
}

void PorteAutomatique::setDistanceFermeture(float distance) {
  _distanceFermeture = distance;
}

// États
void PorteAutomatique::_mettreAJourEtat() {
  if (_etat == OUVERTE && _distance > _distanceFermeture) {
    _etat = EN_FERMETURE;
    _fermer();
  } else if (_etat == FERMEE && _distance < _distanceOuverture) {
    _etat = EN_OUVERTURE;
    _ouvrir();
  }
}

void PorteAutomatique::_ouvertState() {
  // Ne fait rien en état ouvert
}

void PorteAutomatique::_fermeState() {
  // Ne fait rien en état fermé
}

void PorteAutomatique::_ouvertureState() {
  if (_stepper.distanceToGo() == 0) {
    _etat = OUVERTE;
  }
}

void PorteAutomatique::_fermetureState() {
  if (_stepper.distanceToGo() == 0) {
    _etat = FERMEE;
  }
}

// Actions
void PorteAutomatique::_ouvrir() {
  long steps = _angleEnSteps(_angleOuvert);
  _stepper.moveTo(steps);
}

void PorteAutomatique::_fermer() {
  long steps = _angleEnSteps(_angleFerme);
  _stepper.moveTo(steps);
}

// Aide
long PorteAutomatique::_angleEnSteps(float angle) const {
  return static_cast<long>((angle / 360.0) * _stepsPerRev);
}

// Infos
const char* PorteAutomatique::getEtatTexte() const {
  switch (_etat) {
    case FERMEE: return "FERMEE";
    case OUVERTE: return "OUVERTE";
    case EN_OUVERTURE: return "OUVERTURE...";
    case EN_FERMETURE: return "FERMETURE...";
    default: return "INCONNU";
  }
}

float PorteAutomatique::getAngle() const {
  return (_stepper.currentPosition() * 360.0) / _stepsPerRev;
}
