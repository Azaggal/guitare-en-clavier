/*
  SD card datalogger

  This example shows how to log data from three analog sensors
  to an SD card using the SD library.

  The circuit:
   analog sensors on analog ins 0, 1, and 2
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created  24 Nov 2010
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

/*const int guiitarPIN = A0;
const int seuil = 512;
int periode = 0;
int nombrePassage = 0;
int dernierPassage = 0;
float frequence = 0;

void setup() {
  // Open serial communications and wait for port to open:
  pinMode(guiitarPIN, INPUT);
  Serial.begin(9600);
  
}

void loop() {
  // make a string for assembling the data to log:
  int guiitarValue = analogRead(guiitarPIN);
  unsigned long maintenant = micros();

  if ((guiitarValue > seuil && dernierPassage < seuil) || (guiitarValue < seuil && dernierPassage > seuil)) {
    periode += maintenant - dernierPassage;
    nombrePassage ++;
    dernierPassage = maintenant;
    
  }

  if (maintenant - dernierPassage > 200*/

const int guitarePIN = A0;

const int seuilZero = 5;
const int amplitudeMin = 30;
const unsigned long timeout = 30000;
int nbaffile = 0;
char*  note_precedente = "Do";
int octave_precedent = 0;

int prevSignal = 0;
unsigned long lastCrossing = 0;
unsigned long periode = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int adc = analogRead(guitarePIN);
  int signal = adc - 512;

  static int maxAmp = 0;
  static unsigned long tAmp = 0;

  if (abs(signal) > maxAmp) {
    maxAmp = abs(signal);
  }

  if (millis() - tAmp > 20) {
    if (maxAmp < amplitudeMin) {
      maxAmp = 0;
      return;
    }

    maxAmp = 0;
    tAmp = millis();
  }

  if (prevSignal < -seuilZero && signal > seuilZero) {
    unsigned long now = micros();
    periode = now - lastCrossing;
    lastCrossing = now;

    if (periode < timeout && periode > 0) {
      float frequence = 1e6 / periode;
      afficherNote(frequence);
    }
  }

  prevSignal = signal;
}

void afficherNote (float f) {
  const char* notes[] = {
    "Do", "Do#", "Re", "Re#", "Mi", "Fa", "Fa#", "Sol", "Sol#", "La", "La#", "Si"
  };

  if (f < 50 || f > 1000) return;

  int noteMidi = round(12 * log(f/440.0) / log(2) + 69);
  int note = noteMidi % 12;
  int octave = (noteMidi / 12) - 1;

  if (note_precedente == notes[note] && octave_precedent == octave) {
    nbaffile ++;
  }
  else {
    nbaffile = 0;
  }

  if (nbaffile >= 2) {
    if (note_precedente == "Mi" && octave == 2) {
      Serial.print("T");
    }
    else if (note_precedente == "Mi" && octave == 4) {
      Serial.print("I");
    }
    else if (note_precedente == "Sol" && octave == 3) {
      Serial.print("M");
    }
    else if (note_precedente == "Re" && octave == 3) {
      Serial.print("O");
    }
    else if (note_precedente == "La" && octave == 2) {
      Serial.print("E");
    }
    else if (note_precedente == "Si" && octave == 3) {
      Serial.print(" ");
    }
    
    delay(1000);
  }

  note_precedente = notes[note];
  octave_precedent = octave;
  
  
}
