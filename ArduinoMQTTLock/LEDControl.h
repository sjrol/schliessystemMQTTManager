#ifndef __LEDCONTROL_H__
#define __LEDCONTROL_H__

//void setzt status led r g rg=y
void LEDControl(int color) {
  switch (color) {
    case 0:
      digitalWrite(D2, LOW);
      digitalWrite(D3, LOW);
      break;
    case 1:
      digitalWrite(D2, HIGH);
      digitalWrite(D3, LOW);
      break;
    case 2:
      digitalWrite(D2, LOW);
      digitalWrite(D3, HIGH);
      break;
    case 3:
      digitalWrite(D2, HIGH);
      digitalWrite(D3, HIGH);
      break;
    default:
      digitalWrite(D2, LOW);
      digitalWrite(D3, LOW);
      break;
  }
}

#endif
