#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

#define CHANNELS       7

#define NOTE_C0        24
#define NOTE_C1        36
#define NOTE_C2        48

#define NOTEON_DEBOUNCE_TIME 0
#define NOTEOFF_DEBOUNCE_TIME 0

#define SECONDARYNOTE_TRIGGER_TIME 1000

volatile int g_switchPins[CHANNELS]                       = { 2,3,4,5,6,7,0 };
volatile int g_ledPins[CHANNELS]                          = { 9,10,11,12,13,A5,A4 };

volatile int g_noteStats[CHANNELS]                        = { LOW,LOW,LOW,LOW,LOW,LOW,LOW }; 
volatile unsigned long g_last_risingedge_timer[CHANNELS]  = { 0,0,0,0,0,0,0 };
volatile unsigned long g_last_fallingedge_timer[CHANNELS]  = { 0,0,0,0,0,0, };

void pciSetup(byte pin) // Pin change interrupt registrieren
{
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

void setup()
{
  MIDI.begin(MIDI_CHANNEL_OMNI);
  
  for (int thisSwitch = 0; thisSwitch < CHANNELS; thisSwitch++)
  {
    pinMode(g_switchPins[thisSwitch], INPUT_PULLUP);
    pciSetup(g_switchPins[thisSwitch]);
  }
  for (int thisLed = 0; thisLed < CHANNELS; thisLed++)
  {
    pinMode(g_ledPins[thisLed], OUTPUT); 
  }
  
  // zweimal im kreis blinken
  volatile int ringblinkLEDPins[CHANNELS] = { 13,11,9,10,12,A5,A4, };
  for( int i=0; i<(3*CHANNELS); i++ ) {
    digitalWrite(ringblinkLEDPins[i%CHANNELS],HIGH);
    delay(30);
    digitalWrite(ringblinkLEDPins[i%CHANNELS],LOW);
    delay(30);
  }
  
  // zum spass abschliessend blinken
  delay(75);
  for( int i=0; i<CHANNELS; i++ ) {
    digitalWrite(g_ledPins[i], HIGH); }
  delay(75);
  for( int i=0; i<CHANNELS; i++ ) {
    digitalWrite(g_ledPins[i], LOW); }
  delay(75);
  for( int i=0; i<CHANNELS; i++ ) {
    digitalWrite(g_ledPins[i], HIGH); }
  delay(75);
  for( int i=0; i<CHANNELS; i++ ) {
    digitalWrite(g_ledPins[i], LOW); }
  delay(75);
  for( int i=0; i<CHANNELS; i++ ) {
    digitalWrite(g_ledPins[i], HIGH); }
  delay(75);
  for( int i=0; i<CHANNELS; i++ ) {
    digitalWrite(g_ledPins[i], LOW); }
    
  // initialen Zustand pruefen
  processInputs();

}

void loop()
{
  bool somethingtodo = false;
  bool play_secondarynote[CHANNELS];
  // pruefen ob ein pin laenger als SECONDARYNOTE_TRIGGER_TIME auf LOW (aktiv) ist
  for (int i = 0; i < CHANNELS; i++)
  {
    if( digitalRead(g_switchPins[i]) == LOW
     && (millis()-g_last_risingedge_timer[i]) > SECONDARYNOTE_TRIGGER_TIME )
    {
      play_secondarynote[i] = true;
      somethingtodo = true;
    }
    else
    {
      play_secondarynote[i] = false;
    }
  }
  
  if( somethingtodo )
  {
    for (int j = 0; j < CHANNELS; j++) {
      if( play_secondarynote[j] ) {
        MIDI.sendNoteOn(j+NOTE_C1,127,1); } }
        
    for( int i = 0; i < CHANNELS; i++ ) {
      if( play_secondarynote[i] ) {
        digitalWrite(g_ledPins[i], LOW); } }
    delay(75);
    for( int i = 0; i < CHANNELS; i++ ) {
      if( play_secondarynote[i] ) {
        digitalWrite(g_ledPins[i], HIGH); } }
    delay(75);
    for( int i = 0; i < CHANNELS; i++ ) {
      if( play_secondarynote[i] ) {
        digitalWrite(g_ledPins[i], LOW); } }
    delay(75);
    for( int i = 0; i < CHANNELS; i++ ) {
      if( play_secondarynote[i] ) {
        digitalWrite(g_ledPins[i], HIGH); } }
    delay(75);
    for( int i = 0; i < CHANNELS; i++ ) {
      if( play_secondarynote[i] ) {
        digitalWrite(g_ledPins[i], LOW); } }
    delay(75);
    for( int i = 0; i < CHANNELS; i++ ) {
      if( play_secondarynote[i] ) {
        digitalWrite(g_ledPins[i], g_noteStats[i]); } }
        
    for (int j = 0; j < CHANNELS; j++) {
      if( play_secondarynote[j] ) {
        MIDI.sendNoteOff(j+NOTE_C1,127,1);
        g_last_risingedge_timer[j] = millis(); } }
  }
  else
  {
    delay(75);
  }
}

void processInputs()
{
  int switchStats[CHANNELS]                 = { HIGH, HIGH, HIGH, HIGH, HIGH };
  unsigned long risingedge_timer[CHANNELS]  = { 0,0,0,0,0 };
  unsigned long fallingedge_timer[CHANNELS] = { 0,0,0,0,0 };
  bool action[CHANNELS]                     = { false, false, false, false, false };
  
  for (int i = 0; i < CHANNELS; i++)
  {
    switchStats[i] = digitalRead(g_switchPins[i]);
    
    if( switchStats[i] == g_noteStats[i] ) // wenn gleich gab es aenderung - switchstat ist wegen pullup negiertes notestat
    {
      if( switchStats[i] == LOW )
      {
        risingedge_timer[i] = millis();
      }
      else
      {
        fallingedge_timer[i] = millis();
      }
      action[i] = true;
    }
  }
  for (int j = 0; j < CHANNELS; j++)
  {
    if( !action[j] )
      continue;
    
    if( switchStats[j]==LOW )
    //&&  risingedge_timer[j]-g_last_risingedge_timer[j] > NOTEON_DEBOUNCE_TIME )
    {
      MIDI.sendNoteOn(j+NOTE_C0,127,1);
      g_noteStats[j] = HIGH;
      digitalWrite(g_ledPins[j], HIGH);
      g_last_risingedge_timer[j] = millis();
    }
    else //if( switchStats[j]==HIGH
         //&&  fallingedge_timer[j]-g_last_fallingedge_timer[j] > NOTEOFF_DEBOUNCE_TIME )
    {
      MIDI.sendNoteOff(j+NOTE_C0,127,1);
      g_noteStats[j] = LOW;
      digitalWrite(g_ledPins[j], LOW);
      g_last_fallingedge_timer[j] = millis();
    }
  }
}

ISR(PCINT2_vect) // Interrupt Service Routine fuer D0 - D7
{
  processInputs();
}

