//#include <MIDI.h>
#include <EEPROM.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "PCM_Sound.h"

enum mode { 
  NORMAL_MODE, EDIT_MODE, DISTORT_MODE, EDIT_DISTORT_MODE, SEQUENCER_MODE, RECORD_MODE, SWITCH_PRESET_MODE, SAVE_PRESET_MODE};
enum SelectedPattern {
  PATTERN_1, PATTERN_2, PATTERN_3, PATTERN_4};

mode currentMode = NORMAL_MODE;

// ======================================================================================= //


unsigned char drumSound[8][7]; // 56 bytů ... 4x =224 do eeprom
unsigned char pattern[6][32]; // 255 bytů  ... 4x =760 do eeprom

unsigned char tempo=120;



boolean bitReverse[6], mystery, hardMystery;
boolean midiNoteOn;
byte bitAnd, bitOr;
unsigned char unisono;
unsigned char unisonoSpeed;
unsigned char sound = 0,lastSound = 0, sample, currentVelocity;
unsigned char inputChannel=1;
unsigned char channel=inputChannel-1;
unsigned char muteHash=255;


void HandleNoteOn(byte channel, byte note, byte velocity) {   
  //Velocity 0 acts like note Off
  if (velocity == 0) {
    // SetMIDINoteOff(note);
  } 
  else {
    // SetMIDIMode(true);
    //SetMIDINote(note);
  }
}

void HandleNoteOff(byte channel, byte note, byte velocity) { 
  // SetMIDINoteOff(note);
}

void HandleProgramChange(byte channel, byte program) { 
  // SetMIDIProgram(program);
}


// ======================================================================================= //
void setup()
{   
  ShouldIClearMemory();
  beginBeatVox();
  LoadPreset(0);
  //Serial.begin(9600);

  Serial.begin(31250);
  //Serial.begin(38400);

  //Serial.print(freeMemory());
  randomSeed(5);
 channel = GetMIDIChannel();
  UpdateUIInputs();
  IsThereChangeInMidiChannel();
  FlashDiodBlocking(channel+3, 3, 50) ;
  SetMIDIChannel(channel);


  /*
  MIDI.begin();
   //MIDI.begin(inputChannel + 1);
   MIDI.turnThruOn();
   MIDI.setHandleNoteOn(HandleNoteOn); 
   MIDI.setHandleNoteOff(HandleNoteOff);
   MIDI.setHandleProgramChange(HandleProgramChange);
   */
  // Serial.begin(9600);
  //Serial.print("s");
 
}

// ======================================================================================= //
boolean randomizeNow, higherRandom;
void loop()
{
  /*
if(higherRandom){
   if(random(5)==0) randomizeNow=true; 
   else randomizeNow=false;
   }
   else{
   if(random(10)==0) randomizeNow=true; 
   else randomizeNow=false;
   }
   */

  // MIDI.read();
  DoMidi2();
  UpdateUIInputs();
  UpdateApplicationLayer();
  //RenderSound();
  ReflectApplicationLayer();
  ReflectUI();


  //fakeRoutine();
}
unsigned char state, note;
boolean noteOn;


