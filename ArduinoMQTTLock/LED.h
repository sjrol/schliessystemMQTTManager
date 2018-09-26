#ifndef __LED_H__
#define __LED_H__

unsigned long led_blink_delay = 400;
char led_lastColor = '0';

// Set LED red, green or yellow (r+g) 
void led_set(char color) {
  if (led_lastColor == color)
    return;
  led_lastColor = color;
  switch (color) {
    case 'r':
      Serial.println("LED set red.");
      digitalWrite(D2, HIGH);
      digitalWrite(D3, LOW);
      break;
    case 'g':
      Serial.println("LED set green.");
      digitalWrite(D2, LOW);
      digitalWrite(D3, HIGH);
      break;
    case 'y':
      Serial.println("LED set yellow.");
      digitalWrite(D2, HIGH);
      digitalWrite(D3, HIGH);
      break;
    case '0':
    default:
      led_lastColor = '0';
      Serial.println("LED set off.");
      digitalWrite(D2, LOW);
      digitalWrite(D3, LOW);
      break;
  }
}

void led_blink(char color, unsigned int count) {
  Serial.println("LED blinking ...");
  for (int i=0; i<count; i++) {
    led_set('y');
    delay(led_blink_delay);
    led_set('0');
    delay(led_blink_delay);
  }
}

void led_party() {
  Serial.println("LED party ...");
  for(int i=0; i<3; i++) {
    led_set('r');
    delay(100);
    led_set('g');
    delay(100);
    led_set('y');
    delay(100);
  }
  led_set('0');
}

void led_error() {
  Serial.println("LED error ...");
  led_set('r');
  delay(2000);
  led_set('0');
}

void led_ready() {
  Serial.println("LED ready ...");
  for(int i=0; i<3; i++) {
    led_set('g');
    delay(100);
    led_set('r');
    delay(100);
    led_set('g');
    delay(100);
  }
  led_set('0');
}

#endif
