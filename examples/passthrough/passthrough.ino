#include <Wire.h>
#include "Adafruit_DRV2605.h"

#define PWM_PIN 5

Adafruit_DRV2605 drv;
int pwmValue = 0; 
int increments = 1;

void setup() {
  Serial.begin(9600);
  Serial.println("PWM Passthrough Test");
  drv.pwmPassthrough();
}

void loop() {
  // can find sweet spots by knowing the PWM values whilst the motor vibrates
  Serial.print("PWM Value: ");
  Serial.println(pwmValue);
  
  analogWrite(PWM_PIN, pwmValue);

  pwmValue = pwmValue + increments;

  if (pwmValue <= 0 || pwmValue >= 255) {
    increments = -increments;
  }
  
  delay(30);
}
