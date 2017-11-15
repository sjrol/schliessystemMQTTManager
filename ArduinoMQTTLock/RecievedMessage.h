#ifndef __RECIEVEDMESSAGE_H__
#define __RECIEVEDMESSAGE_H__

//void uebernimmt rx vom mqtt server
//switch setzt zeiten/error
double relaisState;
bool messagerecieved = 0;

void ReceivedMessage(char* topic, byte* payload, unsigned int length) {//Setzt neue Nachrichten des rx Topics in Relaiszeiten um
  messagerecieved=1;
  switch ((char)payload[0]) {
    case 's':
      relaisState = 3000 + millis(); //Kurz 3 sec
      break;
    case 'm':
      relaisState = 6000 + millis(); //Mittel 6 sec
      break;
    case 'l':
      relaisState = 12000 + millis(); //Lang 12 sec
      break;
    case 'o':
      relaisState = 86400000 + millis();//Dauer offen (1d)
      break;
    case 'c':
      relaisState = 0 + millis(); //close
      break;
    case 'e':
      LEDControl(1);
      delay(2000);
    default:
      break;
  }
}




#endif
