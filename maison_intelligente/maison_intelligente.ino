#include <HCSR04.h>
#include "Alarm.h"
#include "PorteAutomatique.h"

// --- Capteur ultrason ---
const int trigPin = 2;
const int echoPin = 3;
float distance = 0;
HCSR04 distanceSensor(trigPin, echoPin);

// --- LED RGB + buzzer ---
const int rPin = 7;
const int gPin = 6;
const int bPin = 5;
const int buzzerPin = 12;
Alarm alarme(rPin, gPin, bPin, buzzerPin, &distance);

// --- Moteur pas-à-pas ---
const int in1 = 8;
const int in2 = 9;
const int in3 = 10;
const int in4 = 11;
PorteAutomatique porte(in1, in2, in3, in4, distance);

// --- Timer de mesure ---
unsigned long lastMeasure = 0;
const unsigned long measureInterval = 100; // ms

void setup() {
  Serial.begin(115200);

  // Configuration alarme
  alarme.setColourA(255, 0, 0);     // Rouge
  alarme.setColourB(0, 0, 255);     // Bleu
  alarme.setVariationTiming(500);  // Clignote toutes les 500ms
  alarme.setDistance(15);          // Seuil alarme (cm)
  alarme.setTimeout(3000);         // Délai arrêt alarme
  alarme.turnOn();                 // Démarre alarme

  // Configuration porte
  porte.setAngleFerme(0);
  porte.setAngleOuvert(90);
  porte.setPasParTour(2048);       // Pour 28BYJ-48
  porte.setDistanceOuverture(30);  // Ouvre si ≤ 30 cm
  porte.setDistanceFermeture(60);  // Ferme si > 60 cm
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastMeasure >= measureInterval) {
    lastMeasure = currentMillis;
    distance = distanceSensor.dist();

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  alarme.update();  // Gestion clignotement et buzzer
  porte.update();   // Gestion ouverture/fermeture

  // Debug
  
}
