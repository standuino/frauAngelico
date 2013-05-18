
#include "HWDefines.h"

/*-----------------------------------------------------------------------*/
/*------------------------------ HW LAYER -------------------------------*/
/*-----------------------------------------------------------------------*/

enum ButtonMode { 
  DISABLED, BUTTON, SWITCH, RADIOBUTTON, TWO_STATES_RELEASE_BUTTON};

/* Definition of HW components positions ****************/
/* Its stored in flash memory because its never changed */
/* andtherefore it saves SRAM memory ********************/

prog_uchar buttonPins[NUMBER_OF_BUTTONS] PROGMEM = {  
  BUTTON_EDIT_MODE_PIN, 
  BUTTON_DISTORT_PIN, 
  BUTTON_RECORD_PIN, 
  BUTTON_INSTRUMENT_1_PIN, 
  BUTTON_INSTRUMENT_2_PIN,
  BUTTON_INSTRUMENT_3_PIN,
  BUTTON_INSTRUMENT_4_PIN,
  BUTTON_SHIFT_PIN
};

prog_uchar knobPins[NUMBER_OF_KNOBS] PROGMEM = {  
  KNOB_1_PIN, 
  KNOB_2_PIN, 
  KNOB_3_PIN, 
  KNOB_4_PIN, 
  KNOB_5_PIN
};



prog_uchar DISTORTKnobToEEPromValue[NUMBER_OF_KNOBS] PROGMEM = { 
  KNOB_DISTORT_RATE_INDEX, 
  KNOB_DISTORT_AMT_INDEX,
  KNOB_DISTORT_PITCH_INDEX,
  KNOB_DISTORT_BITCRUSH_INDEX,
  KNOB_DISTORT_DEST_INDEX
};

prog_uchar RECORDKnobToEEPromValue[NUMBER_OF_KNOBS] PROGMEM = {
  KNOB_RECORD_TEMPO_INDEX,
  KNOB_RECORD_LENGTH_INDEX,
  KNOB_RECORD_PATTERN_INDEX,
  KNOB_RECORD_VOL2_INDEX,
  KNOB_RECORD_VOL1_INDEX
};



/* Diod pins are the same as button pins *******/
/* therefor it doesnt have to be defined twice */
/* in order to save some space in SRAM *********/
/*byte diodPins[NUMBER_OF_DIODS] = {  DIOD_EDIT_MODE_PIN, 
 DIOD_DISTORT_PIN, 
 DIOD_PATTERN_SELECT_PIN, 
 DIOD_INSTRUMENT_1_PIN, 
 DIOD_INSTRUMENT_2_PIN,
 DIOD_INSTRUMENT_3_PIN,
 DIOD_INSTRUMENT_4_PIN
 };*/

ButtonMode buttonModes[NUMBER_OF_BUTTONS] = { 
  SWITCH, 
  SWITCH, 
  SWITCH, 
  BUTTON, 
  BUTTON,
  BUTTON,
  BUTTON
};

unsigned char knobMaxValues[NUMBER_OF_KNOBS*3] = { 
  127,PCMtotal,16,255,1023,63,1023,255,63,1023,16,6,255,7,255
};

short knobValues[NUMBER_OF_KNOBS] = { 
  0, 
  0, 
  0, 
  0, 
  0,
};

/* Gets or sets pin state for button on specific index ***/
/* Its implemented as a set of bits in bytes to save the */
/* SRAM memory *******************************************/

unsigned char pinStates = 0;
unsigned char buttonStates = 0;
unsigned char diodStates = 0;
unsigned char buttonStateChanges = 0;
unsigned char knobFreezed = 0;
unsigned char knobChanges = 0;
unsigned char knobMode=0;
boolean moonRead;
#define SHOULD_READ_BUTTON 49

unsigned char buttonShouldRead;

unsigned char radioButtonConnections[NUMBER_OF_BUTTONS];

long releaseButtonStartTimes[NUMBER_OF_BUTTONS];

unsigned int GetKnobValue(unsigned int index) {
  return map(knobValues[index], 0, 1024, 0, knobMaxValues[index+NUMBER_OF_KNOBS*knobMode]);
}

unsigned int GetKnobValue(unsigned int index, unsigned int freezedValue) {
  if (GetHashState(knobFreezed, index) == TRUE) {
    return freezedValue;
  } 
  else {
    return map(knobValues[index], 0, 1024, 0, knobMaxValues[index+NUMBER_OF_KNOBS*knobMode]);
  }
}

unsigned char GetHashState(unsigned char hash, unsigned char index) {
  return ((hash & (1 << index)) >> index);
  //bitRead(hash, index);
}

void SetHashState(unsigned char * hash, unsigned char index, unsigned char value) {
  if (value == TRUE) {
    *hash |= (1 << index);
  } 
  else {
    *hash &= ~(1 << index);
  }
}

void SetButtonMode(unsigned char index, unsigned char value) {
  ButtonMode newMode = (ButtonMode)value;
  if (newMode == BUTTON) {
    digitalWrite(pgm_read_word_near(buttonPins + index), LOW);
    pinMode(pgm_read_word_near(buttonPins + index), INPUT);
  } 
  else if (buttonModes[index] == BUTTON) {
    pinMode(pgm_read_word_near(buttonPins + index), OUTPUT);
  }
  buttonModes[index] = newMode;
}

void InitializeUIValues() {
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
    radioButtonConnections[i] = RADIO_NO_CONNECTION;
  }
  digitalWrite(pgm_read_word_near(buttonPins + BUTTON_INSTRUMENT_1_INDEX), LOW);
  pinMode(pgm_read_word_near(buttonPins + BUTTON_INSTRUMENT_1_INDEX), INPUT);
  digitalWrite(pgm_read_word_near(buttonPins + BUTTON_INSTRUMENT_2_INDEX), LOW);
  pinMode(pgm_read_word_near(buttonPins + BUTTON_INSTRUMENT_2_INDEX), INPUT);
  digitalWrite(pgm_read_word_near(buttonPins + BUTTON_INSTRUMENT_3_INDEX), LOW);
  pinMode(pgm_read_word_near(buttonPins + BUTTON_INSTRUMENT_3_INDEX), INPUT);
  digitalWrite(pgm_read_word_near(buttonPins + BUTTON_INSTRUMENT_4_INDEX), LOW);
  pinMode(pgm_read_word_near(buttonPins + BUTTON_INSTRUMENT_4_INDEX), INPUT);
}

void RemoveRadioButtons(byte buttonIndex/*, ButtonMode newMode*/) {
  while (radioButtonConnections[buttonIndex] != 255) {
    byte thisButtonIndex = buttonIndex;
    buttonIndex = radioButtonConnections[buttonIndex];
    radioButtonConnections[thisButtonIndex] = RADIO_NO_CONNECTION;
    buttonModes[thisButtonIndex] = BUTTON;
  } 
}

void SetRadioButtons(byte * connectedButtons, byte size) {
  if (size > 1) { 
    RemoveRadioButtons(connectedButtons[0]);
    radioButtonConnections[connectedButtons[0]] = connectedButtons[size - 1];
    buttonModes[connectedButtons[0]] = RADIOBUTTON;
    //Serial.print("Set radio mode for button: ");
    //Serial.print((int)connectedButtons[0]);
    while (size > 1) {
      RemoveRadioButtons(connectedButtons[size - 1]);
      radioButtonConnections[connectedButtons[size - 1]] = connectedButtons[size - 2];
      buttonModes[connectedButtons[size - 1]] = RADIOBUTTON;
      //Serial.print("Set radio mode for button: ");
      //Serial.print((int)connectedButtons[size - 1]);
      size--;
    }
  }
}
boolean monomeButton, muteButton, lastMuteButton, metronome;

void UpdateButtonUIInputs() {

  pinMode(12,INPUT_PULLUP); //HACK BUTTON HERE
  muteButton=!digitalRead(12);  //HACK BUTTON HERE

  pinMode(6,INPUT_PULLUP); //HACK BUTTON HERE
  monomeButton=!digitalRead(6);  //HACK BUTTON HERE

  buttonStateChanges = 0;

  for (int i = 3; i < 7; i++) {
    digitalWrite(pgm_read_word_near(buttonPins + i), LOW);
    pinMode(pgm_read_word_near(buttonPins + i), INPUT); 
    unsigned char newPinState = (digitalRead(pgm_read_word_near(buttonPins + i)) == 1)? TRUE : FALSE;
    SetHashState(&buttonStateChanges, i, (newPinState != GetHashState(pinStates, i))? TRUE : FALSE);
    SetHashState(&buttonStates, i, newPinState);
    SetHashState(&pinStates, i, newPinState);
  }
  //Initialize changes to false = not changed

  if(buttonShouldRead>=SHOULD_READ_BUTTON){
    buttonShouldRead=0;

  }
  else{
    buttonShouldRead++;
  }

  if(buttonShouldRead==0){
    for (int i = 0; i < 3; i++) {

      //Diaods of all buttons are controlled by it natural behaviour so there is no need to switch the
      //mode from input to output etc. It fixes the problem with anoying background sound in output
      if (buttonModes[i] != BUTTON) {

        digitalWrite(pgm_read_word_near(buttonPins + i), LOW);
        pinMode(pgm_read_word_near(buttonPins + i), INPUT); 

      }
      if(currentMode==RECORD_MODE){

        digitalWrite(pgm_read_word_near(buttonPins + BUTTON_RECORD_INDEX), LOW);
        pinMode(pgm_read_word_near(buttonPins + BUTTON_RECORD_INDEX), INPUT); 

      }

      unsigned char newPinState = (digitalRead(pgm_read_word_near(buttonPins + i)) == 1)? TRUE : FALSE;

      switch (buttonModes[i]) {
      case TWO_STATES_RELEASE_BUTTON:
        if ((newPinState == TRUE) && (GetHashState(pinStates, i) == FALSE)) {
          releaseButtonStartTimes[i] = millis();
        } 
        else if ((newPinState == FALSE) && (GetHashState(pinStates, i) == TRUE)) {
          SetHashState(&buttonStateChanges, i, TRUE); //justPressed justReleased
          SetHashState(&buttonStates, i, ((millis() - releaseButtonStartTimes[i]) > 500)? TRUE : FALSE);
        }
        break;
      case BUTTON:
        SetHashState(&buttonStateChanges, i, (newPinState != GetHashState(pinStates, i))? TRUE : FALSE);
        SetHashState(&buttonStates, i, newPinState);

        break;
      case SWITCH:
        SetHashState(&buttonStateChanges, i, ((newPinState == TRUE) && (GetHashState(pinStates, i) == FALSE))? TRUE : FALSE);
        if (GetHashState(buttonStateChanges, i) == TRUE) {
          SetHashState(&buttonStates, i, (GetHashState(buttonStates, i) + 1) % 2);
        }        
        break;
      case RADIOBUTTON:
        if ((newPinState == TRUE) && (GetHashState(buttonStates, i) == FALSE)) {
          unsigned char currentButtonIndex = radioButtonConnections[i];
          SetHashState(&buttonStates, i, TRUE);
          SetHashState(&buttonStateChanges, i, TRUE);
          while ((currentButtonIndex != RADIO_NO_CONNECTION) && (currentButtonIndex != i)) {
            if (GetHashState(buttonStates, currentButtonIndex) == TRUE) {
              SetHashState(&buttonStates, currentButtonIndex, FALSE);
              SetHashState(&buttonStateChanges, currentButtonIndex, TRUE);
            } 
            currentButtonIndex = radioButtonConnections[currentButtonIndex];            
          }
        }
        break;
      }
      SetHashState(&pinStates, i, newPinState);

      //Diaods of all buttons are controlled by it natural behaviour so there is no need to switch the
      //mode from input to output etc. It fixes the problem with anoying background sound in output
      if (buttonModes[i] != BUTTON) {
        pinMode(pgm_read_word_near(buttonPins + i), OUTPUT);    
      }
    }
  }

  /*
  if(moonRead){
   }
   moonRead=!moonRead;
   */
  pinMode(pgm_read_word_near(buttonPins + 7), INPUT_PULLUP); 
  unsigned char newPinState = (digitalRead(pgm_read_word_near(buttonPins + 7)) == 0)? TRUE : FALSE;
  SetHashState(&buttonStateChanges, 7, (newPinState != GetHashState(pinStates, 7))? TRUE : FALSE);
  SetHashState(&buttonStates, 7, newPinState);
  SetHashState(&pinStates, 7, newPinState);




}

void UpdateKnobUIInputs() {
  knobChanges = 0;
  for (int i = 0; i < NUMBER_OF_KNOBS; i++) {
    short newValue = analogRead(pgm_read_word_near(knobPins + i));
    short distance = abs(newValue - knobValues[i]); 
    if (GetHashState(knobFreezed, i) == TRUE) {
      if (distance > KNOB_FREEZE_DISTANCE) {
        SetHashState(&knobFreezed, i, FALSE);
        SetHashState(&knobChanges, i, TRUE);
        knobValues[i] = newValue;
      }
    } 
    else if (abs(newValue - knobValues[i]) > KNOB_TOLERANCE) {
      knobValues[i] = newValue;
      SetHashState(&knobChanges, i, TRUE);
    }
  }
}



void UpdateUIInputs() {

  UpdateButtonUIInputs();
  UpdateKnobUIInputs();
}

void FlashDiodBlocking(unsigned char diodIndex, unsigned char flashCount, int delayTime) {
  pinMode(/*diodPins*/pgm_read_word_near(buttonPins + diodIndex), OUTPUT);
  for (int i = 0;  i < flashCount; i++) {
    digitalWrite(/*diodPins*/pgm_read_word_near(buttonPins + diodIndex), HIGH);
    delay(delayTime);
    digitalWrite(/*diodPins*/pgm_read_word_near(buttonPins + diodIndex), LOW);
    delay(delayTime);
  }
  pinMode(/*diodPins*/pgm_read_word_near(buttonPins + diodIndex), INPUT);
}
unsigned char showNow;

void ReflectUI() {
  
  if(showNow==3){
    showNow=0;
    for (int i = 0; i < NUMBER_OF_DIODS; i++) {


      if (buttonModes[i] != BUTTON){
        pinMode(/*diodPins*/pgm_read_word_near(buttonPins + i), OUTPUT);
        if (GetHashState(diodStates, i) == TRUE) {
          digitalWrite(/*diodPins*/pgm_read_word_near(buttonPins + i), HIGH);
        } 
        else {
          digitalWrite(/*diodPins*/pgm_read_word_near(buttonPins + i), LOW);
        }
      }

    }
    if(currentMode==RECORD_MODE){
      pinMode(/*diodPins*/pgm_read_word_near(buttonPins + BUTTON_RECORD_INDEX), OUTPUT);
      digitalWrite(/*diodPins*/pgm_read_word_near(buttonPins + BUTTON_RECORD_INDEX), HIGH);
    }
    //if(currentMode==RECORD_MODE | currentMode==SEQUENCER_MODE | currentMode==SWITCH_PRESET_MODE) {
    if (GetHashState(diodStates, 7) == TRUE) {

      pinMode(13, OUTPUT);
      digitalWrite(13, HIGH);

    }
    else{
      pinMode(13, OUTPUT);
      digitalWrite(13, LOW);
    }
 if(currentMode != RECORD_MODE && currentMode != EDIT_MODE && currentMode != EDIT_DISTORT_MODE && currentMode != SAVE_PRESET_MODE){
    if(muteButton){ 
      for(int i=3;i<7;i++){

        pinMode(/*diodPins*/pgm_read_word_near(buttonPins + i), OUTPUT);
        if( GetHashState(buttonStates,7) ){
          digitalWrite(/*diodPins*/pgm_read_word_near(buttonPins + i), bitRead(muteHash,i-3+4) );
        }
        else{
          digitalWrite(/*diodPins*/pgm_read_word_near(buttonPins + i), bitRead(muteHash,i-3) );
        }
      }
    }
 }
    // }
  }
  showNow++;
}





