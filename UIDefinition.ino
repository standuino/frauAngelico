


unsigned char currentSound = 0;

unsigned char customSoundValues[NUMBER_OF_KNOBS];


long lastStepTime;

int currentSequencerStep;
unsigned char patternLength=31, mysteryValue;
boolean record;


void SetDISTORTKnobValues() {
/*
  knobMaxValues[KNOB_DISTORT_RATE_INDEX] = 100;
  knobMaxValues[KNOB_DISTORT_DEST_INDEX] = 6;
  knobMaxValues[KNOB_DISTORT_SMOOTH_INDEX] = 8;
  knobMaxValues[KNOB_DISTORT_SHAPE_INDEX] = 6;
  knobMaxValues[KNOB_DISTORT_AMOUT_INDEX] = 255; 
*/
}

void SetSynthKnobValues() {
  knobMaxValues[0] = knobMaxValues[1] = knobMaxValues[2] = knobMaxValues[3] = knobMaxValues[4] = 255;
}

void SetRECORDKnobValues() {
  knobMaxValues[0] = knobMaxValues[1] = knobMaxValues[2] = knobMaxValues[3] = knobMaxValues[4] = 255;
}


void ResetInstrumentButtons() {
  SetHashState(&buttonStates, BUTTON_INSTRUMENT_1_INDEX, FALSE);
  SetHashState(&buttonStates, BUTTON_INSTRUMENT_2_INDEX, FALSE);
  SetHashState(&buttonStates, BUTTON_INSTRUMENT_3_INDEX, FALSE);
  SetHashState(&buttonStates, BUTTON_INSTRUMENT_4_INDEX, FALSE);
}

void SetInstrumentButtonsToSwitches() {
  SetButtonMode(BUTTON_INSTRUMENT_1_INDEX, SWITCH);
  SetButtonMode(BUTTON_INSTRUMENT_2_INDEX, SWITCH);
  SetButtonMode(BUTTON_INSTRUMENT_3_INDEX, SWITCH);
  SetButtonMode(BUTTON_INSTRUMENT_4_INDEX, SWITCH);
}

void SetInstrumentButtonsToButtons() {
  SetButtonMode(BUTTON_INSTRUMENT_1_INDEX, BUTTON);
  SetButtonMode(BUTTON_INSTRUMENT_2_INDEX, BUTTON);
  SetButtonMode(BUTTON_INSTRUMENT_3_INDEX, BUTTON);
  SetButtonMode(BUTTON_INSTRUMENT_4_INDEX, BUTTON);
}

void SetInstrumentButtonsToTwoStatesButtons() {
  SetButtonMode(BUTTON_INSTRUMENT_1_INDEX, TWO_STATES_RELEASE_BUTTON);
  SetButtonMode(BUTTON_INSTRUMENT_2_INDEX, TWO_STATES_RELEASE_BUTTON);
  SetButtonMode(BUTTON_INSTRUMENT_3_INDEX, TWO_STATES_RELEASE_BUTTON);
  SetButtonMode(BUTTON_INSTRUMENT_4_INDEX, TWO_STATES_RELEASE_BUTTON);
}

void SetEditModeUI() {
  SetHashState(&buttonStates, BUTTON_DISTORT_INDEX, FALSE); 
  SetHashState(&buttonStates, BUTTON_RECORD_INDEX, FALSE);
  SetSynthKnobValues();  
}

void SetEditRECORDModeUI() {
  SetHashState(&buttonStates, BUTTON_DISTORT_INDEX, FALSE); 
  SetHashState(&buttonStates, BUTTON_RECORD_INDEX, FALSE);
  SetRECORDKnobValues();  
}
/*
boolean UseRECORD() {
  switch (currentMode) {
    case PLAY_MODE:
    case PLAY_RECORD_MODE:
    case PLAY_DISTORT_MODE:
    case EDIT_RECORD_MODE:
      return true;
      break;
  }
  return false;
}
*/

boolean UseDISTORT() {
  /*
  switch (currentMode) {
    case PLAY_MODE:
    case PLAY_RECORD_MODE:
    case PLAY_DISTORT_MODE:
    case EDIT_DISTORT_MODE:
    case EDIT_RECORD_MODE:
      return true;
      break;
  }
  return false;
  */
}


void UpdateApplicationLayer() {
  
  //Define transitions between states
  if(currentMode != RECORD_MODE && currentMode != EDIT_MODE && currentMode != EDIT_DISTORT_MODE && currentMode != SAVE_PRESET_MODE){
    
  if(muteButton){
    for(int i=0;i<4;i++){
     if( GetHashState(buttonStateChanges, i+3) &&  GetHashState(buttonStates, i+3) ){
      if( GetHashState(buttonStates,7) ){
        bitWrite(muteHash, i+4, !bitRead(muteHash,i+4));
      }
      else{
         bitWrite(muteHash, i, !bitRead(muteHash,i));
      }
     }
    }
  }
  
  }
  if(currentMode == RECORD_MODE){
    if(muteButton && !lastMuteButton){
     metronome=!metronome; 
    }
  }
  lastMuteButton=muteButton;
  
  switch (currentMode) {
    case NORMAL_MODE:
      if (GetHashState(buttonStateChanges, BUTTON_EDIT_MODE_INDEX) == TRUE) {
        currentMode = EDIT_MODE; 
        
        ResetInstrumentButtons();
        FreezeKnobs();
        StopSound();
      } else if (GetHashState(buttonStateChanges, BUTTON_RECORD_INDEX) == TRUE) {
        currentMode = SEQUENCER_MODE;
        FreezeKnobs();
        mysteryValue=0;
        ResetInstrumentButtons();
        ResetSequencer();
        StopSound();
      } else if (GetHashState(buttonStateChanges, BUTTON_DISTORT_INDEX) == TRUE) {
        currentMode = DISTORT_MODE;
        ResetInstrumentButtons();
        UnFreezeKnobs();
        StopSound();
        
      } else {
        ProceedNormalMode();
      }
      break;
    case SEQUENCER_MODE:
     if (GetHashState(buttonStateChanges, BUTTON_RECORD_INDEX) == TRUE) { 
       currentMode = NORMAL_MODE;
       mysteryValue=0;
        ResetInstrumentButtons();
        FreezeKnobs();
        StopSound();
     } else if (GetHashState(buttonStateChanges, BUTTON_DISTORT_INDEX) == TRUE) {
      //mysteryValue=0;
         currentMode = SWITCH_PRESET_MODE;
      ResetInstrumentButtons();
        FreezeKnobs();
        StopSound();
     }
    else if (GetHashState(buttonStateChanges, BUTTON_EDIT_MODE_INDEX) == TRUE) {
      mysteryValue=0;
      currentMode = RECORD_MODE;
     ResetInstrumentButtons();
        FreezeKnobs();
        SetButtonMode(BUTTON_RECORD_INDEX, BUTTON);
      // SetInstrumentButtonsToTwoStatesButtons();
     }
     else {
       ProceedSequencerMode();
     }
     break; 
    case DISTORT_MODE:
     if (GetHashState(buttonStateChanges, BUTTON_RECORD_INDEX) == TRUE) { 
       currentMode = SWITCH_PRESET_MODE;
       mysteryValue=0;
       
       ResetInstrumentButtons();
        FreezeKnobs();
        
     } else if (GetHashState(buttonStateChanges, BUTTON_DISTORT_INDEX) == TRUE) {
       currentMode = NORMAL_MODE;
     StopSound();
      ResetInstrumentButtons();
        FreezeKnobs();
     }
    else if (GetHashState(buttonStateChanges, BUTTON_EDIT_MODE_INDEX) == TRUE) {
      StopSound();
       currentMode = EDIT_DISTORT_MODE;
       ResetInstrumentButtons();
        FreezeKnobs();
     }
     else {
       ProceedNormalMode();
     }
     break;
    case EDIT_MODE:
      if (GetHashState(buttonStateChanges, BUTTON_EDIT_MODE_INDEX) == TRUE) {
        //StorePresets();
        currentMode = NORMAL_MODE;
        StopSound();
       ResetInstrumentButtons();
        FreezeKnobs();
      } else if(GetHashState(buttonStateChanges, BUTTON_RECORD_INDEX) == TRUE) {
        currentMode = RECORD_MODE;
        mysteryValue=0;
        StopSound();
        SetButtonMode(BUTTON_DISTORT_INDEX, BUTTON); //tap
        ResetInstrumentButtons();
        FreezeKnobs();
      } else if(GetHashState(buttonStateChanges, BUTTON_DISTORT_INDEX) == TRUE) {
        currentMode = EDIT_DISTORT_MODE;
       StopSound();
        ResetInstrumentButtons();
        FreezeKnobs();
      } else {
        ProceedEditMode();
      }
      break;
    case SWITCH_PRESET_MODE:
      if ((GetHashState(buttonStateChanges, BUTTON_EDIT_MODE_INDEX) == TRUE) && (GetHashState(buttonStates, BUTTON_EDIT_MODE_INDEX) == TRUE)) {
       // SavetPresets();
       //meneni presetu?
      }
      else if (GetHashState(buttonStateChanges, BUTTON_RECORD_INDEX) == TRUE) {
        currentMode = DISTORT_MODE;
        //SetDISTORTKnobValues();
         ResetInstrumentButtons();
        FreezeKnobs();
      } 
      else if (GetHashState(buttonStateChanges, BUTTON_DISTORT_INDEX) == TRUE) {
        currentMode = SEQUENCER_MODE;
        mysteryValue=0;
         ResetInstrumentButtons();
        FreezeKnobs();
      } else {
        
       
        ProceedSequencerMode();
        if (ProceedSwitchPatternMode()) {
          currentMode = SWITCH_PRESET_MODE;
          ResetInstrumentButtons();
          SetInstrumentButtonsToButtons();
        }
      }
      
     
        
      break;
    case RECORD_MODE:
      if (GetHashState(buttonStateChanges, BUTTON_EDIT_MODE_INDEX) == TRUE) {
        mysteryValue=0;
         SetButtonMode(BUTTON_DISTORT_INDEX, SWITCH);
        currentMode = SEQUENCER_MODE;
        SetInstrumentButtonsToButtons();
        SetButtonMode(BUTTON_RECORD_INDEX, SWITCH);
        
      ResetInstrumentButtons();
      
        FreezeKnobs();
      } else if (GetHashState(buttonStateChanges, BUTTON_RECORD_INDEX) == TRUE) {
       
        
      } else {
        ProceedSequencerMode();
      }     
      break;
    case EDIT_DISTORT_MODE:
      if (GetHashState(buttonStateChanges, BUTTON_EDIT_MODE_INDEX) == TRUE) {
        currentMode = DISTORT_MODE;
      
         ResetInstrumentButtons();
        FreezeKnobs();
     
      } else if (GetHashState(buttonStateChanges, BUTTON_RECORD_INDEX) == TRUE) {
       currentMode = SAVE_PRESET_MODE;
       
       
      } else if (GetHashState(buttonStateChanges, BUTTON_DISTORT_INDEX) == TRUE) {
        currentMode = EDIT_MODE;  
        StopSound();
         ResetInstrumentButtons();
      
        FreezeKnobs();
      }else {
        ProceedEditMode();
      }     
      break;
      
      case SAVE_PRESET_MODE:
      
      if (GetHashState(buttonStateChanges, BUTTON_RECORD_INDEX) == TRUE) {
       currentMode = EDIT_DISTORT_MODE;
      }
      
      else{
      for(int i=0;i<4;i++){
          if (GetHashState(buttonStateChanges, i+3) && GetHashState(buttonStates, i+3)){
         FlashDiodBlocking(BUTTON_RECORD_INDEX, 3, 50) ;
         StorePreset(currentPreset);
         LoadPreset(i);
         currentPreset=i;
          FlashDiodBlocking(BUTTON_RECORD_INDEX, 3, 50) ;
          currentMode = NORMAL_MODE;
          }
          
        }
        
        if(GetHashState(buttonStates, 7)){
        // ExportPresets();
        }
        
      }
      break;
  }
  
  
  //UpdateDISTORTApplicationLayer();
  
}  


void ReflectApplicationLayer() {
  
  //Turn off all diaods
  diodStates = 0;
  switch (currentMode) {
    case NORMAL_MODE:
      SetHashState(&diodStates, DIOD_EDIT_MODE_INDEX, FALSE);
      SetHashState(&diodStates, DIOD_DISTORT_INDEX, FALSE);
      SetHashState(&diodStates, DIOD_RECORD_INDEX, FALSE);
     if(!midiNoteOn){ SetHashState(&diodStates, DIOD_MOON_INDEX, GetHashState(buttonStates,7));} /////////
    else{ SetHashState(&diodStates, DIOD_MOON_INDEX, TRUE); }
      break;
    case DISTORT_MODE:
      SetHashState(&diodStates, DIOD_EDIT_MODE_INDEX, FALSE);
      SetHashState(&diodStates, DIOD_DISTORT_INDEX, TRUE);
      SetHashState(&diodStates, DIOD_RECORD_INDEX, FALSE);
      if(!midiNoteOn) {SetHashState(&diodStates, DIOD_MOON_INDEX, GetHashState(buttonStates,7));}
      else{ SetHashState(&diodStates, DIOD_MOON_INDEX, TRUE); }
      break;
    case SEQUENCER_MODE:
      SetHashState(&diodStates, DIOD_EDIT_MODE_INDEX, FALSE);
      SetHashState(&diodStates, DIOD_DISTORT_INDEX, FALSE);
      SetHashState(&diodStates, DIOD_RECORD_INDEX, TRUE);
       if(currentSequencerStep % 4 ==0){
       SetHashState(&diodStates, DIOD_MOON_INDEX, TRUE); 
      }
      else{
      SetHashState(&diodStates, DIOD_MOON_INDEX, FALSE); 
      }
      if(currentSequencerStep==0){
      SetHashState(&diodStates, DIOD_EDIT_MODE_INDEX, TRUE);
      SetHashState(&diodStates, DIOD_DISTORT_INDEX, TRUE);
      SetHashState(&diodStates, DIOD_RECORD_INDEX, TRUE);
      }
      else{
      SetHashState(&diodStates, DIOD_EDIT_MODE_INDEX, FALSE);
      SetHashState(&diodStates, DIOD_DISTORT_INDEX, FALSE);
      SetHashState(&diodStates, DIOD_RECORD_INDEX, TRUE);
      }
      break;
    case EDIT_MODE:
      SetHashState(&diodStates, DIOD_EDIT_MODE_INDEX, TRUE);
      SetHashState(&diodStates, DIOD_DISTORT_INDEX, FALSE);
      SetHashState(&diodStates, DIOD_RECORD_INDEX, FALSE);
     if(!midiNoteOn) {SetHashState(&diodStates, DIOD_MOON_INDEX, GetHashState(buttonStates,7));}
      else{ SetHashState(&diodStates, DIOD_MOON_INDEX, TRUE); }
      break;
    case EDIT_DISTORT_MODE:
      SetHashState(&diodStates, DIOD_EDIT_MODE_INDEX, TRUE);
      SetHashState(&diodStates, DIOD_DISTORT_INDEX, TRUE);
      SetHashState(&diodStates, DIOD_RECORD_INDEX, FALSE);
     if(!midiNoteOn) {SetHashState(&diodStates, DIOD_MOON_INDEX, GetHashState(buttonStates,7));}
      else{ SetHashState(&diodStates, DIOD_MOON_INDEX, TRUE); }
      break;
      case SAVE_PRESET_MODE:
      SetHashState(&diodStates, DIOD_EDIT_MODE_INDEX, TRUE);
      SetHashState(&diodStates, DIOD_DISTORT_INDEX, TRUE);
      SetHashState(&diodStates, DIOD_RECORD_INDEX, TRUE);
       if(!midiNoteOn) {SetHashState(&diodStates, DIOD_MOON_INDEX, GetHashState(buttonStates,7));}
      else{ SetHashState(&diodStates, DIOD_MOON_INDEX, TRUE); }
      break;
    case RECORD_MODE: {
      SetHashState(&diodStates, DIOD_EDIT_MODE_INDEX, TRUE);
      SetHashState(&diodStates, DIOD_DISTORT_INDEX, FALSE);
      SetHashState(&diodStates, DIOD_RECORD_INDEX, TRUE);
       if(currentSequencerStep % 4 ==0){
       SetHashState(&diodStates, DIOD_MOON_INDEX, TRUE); 
      }
      else{
        SetHashState(&diodStates, DIOD_MOON_INDEX, FALSE); 
      }
      if(currentSequencerStep==0){
      SetHashState(&diodStates, DIOD_EDIT_MODE_INDEX, TRUE);
      SetHashState(&diodStates, DIOD_DISTORT_INDEX, TRUE);
      SetHashState(&diodStates, DIOD_RECORD_INDEX, TRUE);

      }
      else{
      SetHashState(&diodStates, DIOD_EDIT_MODE_INDEX, TRUE);
      SetHashState(&diodStates, DIOD_DISTORT_INDEX, FALSE);
      SetHashState(&diodStates, DIOD_RECORD_INDEX, TRUE);
      }
    }
      break;
    case SWITCH_PRESET_MODE:
      
       if(currentSequencerStep % 4 ==0){
       SetHashState(&diodStates, DIOD_MOON_INDEX, TRUE); 
      }
      else{
        SetHashState(&diodStates, DIOD_MOON_INDEX, FALSE); 
      }
      if(currentSequencerStep==0){
      SetHashState(&diodStates, DIOD_EDIT_MODE_INDEX, TRUE);
      SetHashState(&diodStates, DIOD_DISTORT_INDEX, TRUE);
      SetHashState(&diodStates, DIOD_RECORD_INDEX, TRUE);

      }
      else{
      SetHashState(&diodStates, DIOD_EDIT_MODE_INDEX, FALSE);
      SetHashState(&diodStates, DIOD_DISTORT_INDEX, TRUE);
      SetHashState(&diodStates, DIOD_RECORD_INDEX, TRUE);
      }
      
      break;
  }

  SetHashState(&diodStates, DIOD_INSTRUMENT_1_INDEX, GetHashState(buttonStates, BUTTON_INSTRUMENT_1_INDEX));
  SetHashState(&diodStates, DIOD_INSTRUMENT_2_INDEX, GetHashState(buttonStates, BUTTON_INSTRUMENT_2_INDEX));
  SetHashState(&diodStates, DIOD_INSTRUMENT_3_INDEX, GetHashState(buttonStates, BUTTON_INSTRUMENT_3_INDEX));
  SetHashState(&diodStates, DIOD_INSTRUMENT_4_INDEX, GetHashState(buttonStates, BUTTON_INSTRUMENT_4_INDEX));
 
}     


void FreezeKnobs () {
  for (int i = 0; i < NUMBER_OF_KNOBS; i++) {
    SetHashState(&knobFreezed, i, TRUE);
  }
}

void UnFreezeKnobs () {
  for (int i = 0; i < NUMBER_OF_KNOBS; i++) {
    SetHashState(&knobFreezed, i, FALSE);
  }
}




void LoadSound(unsigned char index) {
  /*
  // No sound is selected mute the sound
  if (index == 0) {
    if (UseRECORD()) {
      ReleaseRECORD();
    } else {
      SetNoSound();
      isSound = false;
    }
  } else {
    // Sound number 1 is stored in the index 0 because sound number 0 is MUTE
    unsigned char soundIndex = index - 1; 
    LoadCustomSound(auduinoZvuk[soundIndex][0], 
                    auduinoZvuk[soundIndex][1], 
                    auduinoZvuk[soundIndex][2], 
                    auduinoZvuk[soundIndex][3], 
                    map(auduinoZvuk[soundIndex][4], 0, 255, 0, 128));
    
  }
*/
}


void LoadCustomSound(unsigned char grainPhaseInc, unsigned char grainDecay, unsigned char grain2PhaseInc, unsigned char grain2Decay, unsigned char syncPhaseInc) {
    /*Serial.print((int)grainPhaseInc);Serial.print("\t");
    Serial.print((int)grainDecay);Serial.print("\t");
    Serial.print((int)grain2PhaseInc);Serial.print("\t");
    Serial.print((int)grain2Decay);Serial.print("\t");
    Serial.print((int)syncPhaseInc);Serial.print("\t");
    Serial.println("-----------------------------------");*/  
    /*
  grainPhaseInc_NoDISTORT = (mapPhaseInc(grainPhaseInc << 1) / 2);
    grainDecay_NoDISTORT = ((grainDecay << 1 ) / 8);
    grain2PhaseInc_NoDISTORT = (mapPhaseInc(grain2PhaseInc << 1) / 2);
    grain2Decay_NoDISTORT = ((grain2Decay << 1) / 4);
    /*uint16_t newSyncPhaseInc = midiTable[syncPhaseInc];
    if (pitchBand != 127) {
      char 
    }*/
    /*
    syncPhaseInc_NoDISTORT= midiTable[syncPhaseInc];
    isSound = true; 
    */
}

void LoadDISTORT(unsigned char index) {
  if (index == 0) {
    /*DISTORTAmt = 0;
    DISTORTType = 0;
    DISTORTSteps = 0;
    DISTORTShape = (DISTORTShape)0;
    DISTORTRate = 0; 
    ResetDISTORT();*/
  } else {
    // Sound number 1 is stored in the index 0 because sound number 0 is MUTE
    unsigned char soundIndex = index - 1; 
    //Get DISTORT calues for current sound
  //  LoadCustomDISTORT(auduinoEnvelope[soundIndex][0], auduinoEnvelope[soundIndex][1], auduinoEnvelope[soundIndex][2], auduinoEnvelope[soundIndex][3], auduinoEnvelope[soundIndex][4]);
  }
}



void ProceedEditMode() {
  
  if(currentMode==EDIT_DISTORT_MODE){
  knobMode=1;
if (!GetHashState(knobFreezed, 0))  SetValue(CUT,lastSound,GetKnobValue(0));
 if (!GetHashState(knobFreezed, 1)) SetValue(LOOP_LENGTH,lastSound,GetKnobValue(1));
 if (!GetHashState(knobFreezed, 2)) SetValue(SHIFT_SPEED,lastSound,GetKnobValue(2));
 if (!GetHashState(knobFreezed, 3)) SetValue(START,lastSound,GetKnobValue(3));
 if (!GetHashState(knobFreezed, 4)) SetValue(LOOP_DIRECTION,lastSound,GetKnobValue(4));
}
else{
  knobMode=0;
if (!GetHashState(knobFreezed, 0))  SetValue(VOLUME,lastSound,GetKnobValue(0));
if (!GetHashState(knobFreezed, 1))  SetValue(SAMPLE,lastSound,GetKnobValue(1));
if (!GetHashState(knobFreezed, 2))  SetValue(PITCH,lastSound,GetKnobValue(2));
if (!GetHashState(knobFreezed, 3))  SetValue(BIT_CRUSH,lastSound,GetKnobValue(3));
  OCR1A = F_CPU / map(analogRead(4),0,1024,1000,15000); 
}
  ProceedPlayback();
}


unsigned int StepLength(){
  unsigned int _stepLength= 15000/tempo;
 return _stepLength;
}

void ResetSequencer(){
  lastStepTime=millis()-StepLength();
    currentSequencerStep=-1;
}

void ProceedSequencerMode() {
  SetButtonMode(7,BUTTON);
  //unsigned char velocity=map(GetKnobValue(2),0,255,0,127);
  if(!monomeButton){
  ProceedPlayback();
  }
  

  
  if(millis()-lastStepTime>=StepLength()){
    
    lastStepTime=millis();
    currentSequencerStep++;
     if(currentSequencerStep>patternLength){
      currentSequencerStep=0;
    SetHashState(&knobFreezed, 2, TRUE);  
    
   } 
   
   
   
   if(monomeButton){ // HACK HERE
   for(int i=0;i<4;i++){
     if(GetHashState(buttonStates, 7)){
       if( GetHashState(buttonStates, 3+i) ){
         currentSequencerStep=4*i+16;
       }
     }
     
     else{
      if( GetHashState(buttonStates, 3+i) ){
         currentSequencerStep=4*i;
       }
     }
    
   }
   //currentSequencerStep=0;
   } // HACK HERE
   if(record){
   if(metronome){
     
    if(currentSequencerStep%16==0){
      playNote(17  ,127);
    }
   
   else if(currentSequencerStep % 4 == 0){
   playNote(16,127);
   } 
   
   }
   }
   //clodwigplatz  
   
   
   for(int i=0;i<8;i++){
    
     if (currentMode==RECORD_MODE && GetHashState(buttonStates, BUTTON_RECORD_INDEX) == TRUE){
       SetStep(currentSequencerStep,i,false);
     }
    if(GetStep(currentSequencerStep,i)){
     DealMystery();
     
     if(record){
       playNote(i,127);
     }
     else{
       
     if(ShouldPlayNote(i)) playNote(i,127);
     
     }
     
    } 
   }
   
  }
  if(currentMode==SWITCH_PRESET_MODE){
  knobMode=1;
  }

  if(currentMode==RECORD_MODE){
   record=true; 
     knobMode=2;
  }
  else{
    record=false;
  }
  if(currentMode!=SWITCH_PRESET_MODE){
    knobMode=2;
   if (!GetHashState(knobFreezed, 1)) currentPattern=GetKnobValue(1);
   if (!GetHashState(knobFreezed, 3)) patternLength= pow(2,GetKnobValue(3)-1);
   if (!GetHashState(knobFreezed, 3) && (GetKnobValue(3)-1)<0) patternLength=0;
   if (!GetHashState(knobFreezed, 2)) tempo=20+GetKnobValue(2);
   if (!GetHashState(knobFreezed, 0)) mysteryValue=GetKnobValue(0);
    if(record){
      if(!monomeButton){
    RecordStep();
      }
     if (GetHashState(buttonStateChanges, BUTTON_DISTORT_INDEX) == TRUE && GetHashState(buttonStates, BUTTON_DISTORT_INDEX)==TRUE){
      tap(); 
     }
    }
  }
  
  OCR1A = F_CPU /map(analogRead(4),0,1024,1000,15000);
 
}

boolean ShouldPlayNote(unsigned char _NOTE){
  
  boolean _returnValue;
 _returnValue = (bitRead(muteHash,_NOTE))? true : false ;
 return _returnValue;
 
}

void DealMystery(){
  if(mysteryValue==0){
    mystery=false;
  }
  else{
    mystery=false;
  
 unsigned char _MYSTERY_MAIN = mysteryValue/4;
 unsigned char _MYSTERY_SIDE = 3-mysteryValue % 4;
  
 switch(_MYSTERY_MAIN){
   
   case 0:
   if(currentSequencerStep%2>0) mystery = (random(_MYSTERY_SIDE)==0)? true : false; 
    hardMystery=false;
   
  break;
  case 1:
   if(currentSequencerStep%4>0) mystery = (random(_MYSTERY_SIDE)==0)? true : false; 
    hardMystery=false;
  break;
  case 2:
   if(currentSequencerStep%8>0) mystery = (random(_MYSTERY_SIDE)==0)? true : false; 
   hardMystery=true;
   
  break;
  case 3:
   if(currentSequencerStep%16>0) mystery = (random(_MYSTERY_SIDE)==0)? true : false; 
    hardMystery=true;
  break;
  
 } 
  }
 
}

void RecordStep(){
  
   for(int i=0;i<4;i++){
    if( GetHashState(buttonStateChanges,i+3) &&  GetHashState(buttonStates,i+3)) {
      if(millis()-lastStepTime > StepLength()/2){
        if(currentSequencerStep==patternLength){
          if(GetHashState(buttonStates,7)){
          SetStep(0,i+4,true);
          }
          else{
            SetStep(0,i,true);
          }
        }
        else{
          if(GetHashState(buttonStates,7)){
        SetStep(currentSequencerStep+1,i+4,true);
          }
          else{
            SetStep(currentSequencerStep+1,i,true);
          }
        }
      }
      else{
         if(GetHashState(buttonStates,7)){
        SetStep(currentSequencerStep,i+4,true);
        
         }
         
         else{
           SetStep(currentSequencerStep,i,true);
         }
      }
    }
  }
  
}
unsigned char numberOfTap;
long lastTapTime;
int tapMemory[8];

void tap(){
  if(millis()-lastTapTime>3000){
   numberOfTap=0; 
  }
 if(numberOfTap==0){
  lastTapTime=millis();
  numberOfTap++;
 } 
 else{
   if(numberOfTap>8){
    for(int i=0;i<8;i++) {
      tapMemory[i]=tapMemory[i+1];
    }
    numberOfTap=8;
   }
  tapMemory[numberOfTap-1]= millis()-lastTapTime;
  lastTapTime=millis();
  int tapTogether;
  for(int i=0;i<numberOfTap;i++){
   tapTogether+=tapMemory[i]; 
  }
  tempo = 120000/(tapTogether/numberOfTap); // normaly 60000
 }
}

boolean ProceedSwitchPatternButton(unsigned char buttonIndex , unsigned char patternIndex) {
  /*
  if (GetHashState(buttonStateChanges, buttonIndex) == TRUE) {
    if (GetHashState(buttonStates, buttonIndex) == TRUE) {
      currentPreset=patternIndex + 4;
      LoadPreset(patternIndex + 4);
    } else {
      currentPreset=patternIndex;
      LoadPreset(patternIndex);
    }
    return true;
  }
  return false;
  */
}

boolean ProceedSwitchPatternMode() {
  /*
  if (ProceedSwitchPatternButton(BUTTON_INSTRUMENT_1_INDEX, 0)) {
    return true;
  }
  if (ProceedSwitchPatternButton(BUTTON_INSTRUMENT_2_INDEX, 1)) {
    return true;
  }
  if (ProceedSwitchPatternButton(BUTTON_INSTRUMENT_3_INDEX, 2)) {
    return true;
  }
  if (ProceedSwitchPatternButton(BUTTON_INSTRUMENT_4_INDEX, 3)) {
    return true;
  }
  return false;
  */
}

void ProceedPlayback(){
  unsigned char velocity=127;//map(GetKnobValue(4),0,knobMaxValues[4+3*knobMode],0,127);
mystery=false;
  for(int i=0;i<4;i++){
    if( GetHashState(buttonStateChanges,i+3) &&  GetHashState(buttonStates,i+3)) {
      if(GetHashState(buttonStates,7)){
        playNote(i+4,velocity); 
      }
      else{
        playNote(i,velocity); 
      }
    }
  }
  
}


void ProceedNormalMode() {
  SetButtonMode(7,BUTTON);
if(currentMode==DISTORT_MODE){
  knobMode=1;
}
else{
  knobMode=0;
}

ProceedPlayback();

OCR1A = F_CPU /map(analogRead(4),0,1024,1000,15000); ; 

 
}
 
  



