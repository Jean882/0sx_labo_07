#include <AccelStepper.h>
#include <Wire.h>
#include <LCD_I2C.h>
#include <HCSR04.h>
#include <Buzzer.h>
#include <Button.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Alarm.h"
#include "PorteAutomatique.h"

// Define Pins
#define TRIGGER_PIN 2
#define ECHO_PIN 3
#define IN_1 8
#define IN_2 9
#define IN_3 10
#define IN_4 11
#define MOTOR_INTERFACE_TYPE 4
#define BTN_PIN 12

// RGB LED PINS
const int PIN_RED   = 7;
const int PIN_GREEN = 6;
const int PIN_BLUE  = 5;

// OLED configuration
#define OLED_RESET     -1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// LCD I2C
LCD_I2C lcd(0x27, 16, 2);

// Capteur ultrason
HCSR04 distanceSensor(TRIGGER_PIN, ECHO_PIN);
float distance = 0;

// Composants encapsulés
Alarm alarm(PIN_RED, PIN_GREEN, PIN_BLUE, BTN_PIN, &distance);
PorteAutomatique porte(IN_1, IN_2, IN_3, IN_4, distance);

// Timing pour affichage non-bloquant
unsigned long previousMillis = 0;
const unsigned long interval = 100;

void setup() {
  Serial.begin(115200);

  // Initialisation des affichages
  lcd.begin();
  lcd.backlight();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Erreur OLED"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(F("Systeme domotique"));
  display.display();

  // Paramètres du système d'alarme
  alarm.setDistance(15); // seuil déclenchement
  alarm.setTimeout(3000); // ms après éloignement
  alarm.setVariationTiming(200);
  alarm.setColourA(255, 0, 0); // rouge
  alarm.setColourB(255, 255, 0); // jaune

  // Paramètres de la porte
  porte.setAngleFerme(0);
  porte.setAngleOuvert(90);
  porte.setPasParTour(2048);
  porte.setDistanceOuverture(30);
  porte.setDistanceFermeture(60);
}

void loop() {
  unsigned long currentMillis = millis();

  // Mesure de la distance partagée
  distance = distanceSensor.dist();

  // Mise à jour des objets
  alarm.update();
  porte.update();

  // Mise à jour de l'affichage toutes les 100 ms
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    lcd.setCursor(0, 0);
    lcd.print("Dist: ");
    lcd.print(distance);
    lcd.print(" cm   ");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Distance: ");
    display.print(distance);
    display.println(" cm");
    display.display();
  }
}
