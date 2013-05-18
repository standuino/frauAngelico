#define PRESET_SIZE 255
#define NUMBER_OF_PRESETS 4

#define NUMBER_OF_SAMPLE_BYTE 0
#define NUMBER_OF_SAMPLE_BITS 4
#define NUMBER_OF_SAMPLE_OFFSET 0

#define NUMBER_OF_VOLUME_BYTE 0
#define NUMBER_OF_VOLUME_BITS 4
#define NUMBER_OF_VOLUME_OFFSET 4

#define NUMBER_OF_PITCH_BYTE 1

#define NUMBER_OF_LOOP_LENGTH_BYTE_1 2
#define NUMBER_OF_LOOP_LENGTH_BYTE_2 4
#define NUMBER_OF_LOOP_LENGTH_BITS_1 8
#define NUMBER_OF_LOOP_LENGTH_BITS_2 2
#define NUMBER_OF_LOOP_LENGTH_OFFSET_1 0
#define NUMBER_OF_LOOP_LENGTH_OFFSET_2 0


#define NUMBER_OF_CUT_BYTE 3
#define NUMBER_OF_CUT_BITS 6
#define NUMBER_OF_CUT_OFFSET 0

#define NUMBER_OF_START_BYTE_1 3
#define NUMBER_OF_START_BYTE_2 4
#define NUMBER_OF_START_BITS_1 2
#define NUMBER_OF_START_BITS_2 4
#define NUMBER_OF_START_OFFSET_1 6
#define NUMBER_OF_START_OFFSET_2 0

#define NUMBER_OF_LOOP_DIRECTION_BYTE 4
#define NUMBER_OF_LOOP_DIRECTION_BITS 2
#define NUMBER_OF_LOOP_DIRECTION_OFFSET 4



#define NUMBER_OF_BIT_CRUSH_BYTE 5


#define NUMBER_OF_SHIFT_SPEED_BYTE 6


enum valueType {
  VOLUME,SAMPLE,LOOP_LENGTH,CUT,LOOP_DIRECTION,SHIFT_SPEED,BIT_CRUSH,PITCH,START};

unsigned char currentPreset = 0;
unsigned char currentPattern = 0;

unsigned int GetValue(unsigned char _VALUE_TYPE,unsigned char _SOUND){
  unsigned int _returnValue=0; 

  switch(_VALUE_TYPE){
  case SAMPLE:
    for(int i=0;i<NUMBER_OF_SAMPLE_BITS;i++){
      bitWrite(_returnValue,i,bitRead(drumSound[_SOUND][NUMBER_OF_SAMPLE_BYTE],i+NUMBER_OF_SAMPLE_OFFSET)); 
    }
    break;

  case VOLUME:
    for(int i=0;i<NUMBER_OF_VOLUME_BITS;i++){
      bitWrite(_returnValue,i,bitRead(drumSound[_SOUND][NUMBER_OF_VOLUME_BYTE],i+NUMBER_OF_VOLUME_OFFSET)); 
    }
    _returnValue=127-(_returnValue<<4);
    break;
  case PITCH:
    _returnValue = drumSound[_SOUND][NUMBER_OF_PITCH_BYTE]; 
    break;
  case LOOP_LENGTH:
    for(int i=0;i<NUMBER_OF_LOOP_LENGTH_BITS_1;i++){
      bitWrite(_returnValue,i,bitRead(drumSound[_SOUND][NUMBER_OF_LOOP_LENGTH_BYTE_1],i+NUMBER_OF_LOOP_LENGTH_OFFSET_1)); 
    }
    for(int i=0;i<NUMBER_OF_LOOP_LENGTH_BITS_2;i++){
      bitWrite(_returnValue,i+NUMBER_OF_LOOP_LENGTH_BITS_1,bitRead(drumSound[_SOUND][NUMBER_OF_LOOP_LENGTH_BYTE_2],i+NUMBER_OF_LOOP_LENGTH_OFFSET_2)); 
    }

    break;
  case CUT:
    for(int i=0;i<NUMBER_OF_CUT_BITS;i++){
      bitWrite(_returnValue,i,bitRead(drumSound[_SOUND][NUMBER_OF_CUT_BYTE],i+NUMBER_OF_CUT_OFFSET)); 
    }
    break;
  case START:
    for(int i=0;i<NUMBER_OF_START_BITS_1;i++){
      bitWrite(_returnValue,i,bitRead(drumSound[_SOUND][NUMBER_OF_START_BYTE_1],i+NUMBER_OF_START_OFFSET_1)); 
    }
    for(int i=0;i<NUMBER_OF_START_BITS_2;i++){
      bitWrite(_returnValue,i+NUMBER_OF_START_BITS_1,bitRead(drumSound[_SOUND][NUMBER_OF_START_BYTE_2],i+NUMBER_OF_START_OFFSET_2)); 
    }
    break;
  case LOOP_DIRECTION:
    for(int i=0;i<NUMBER_OF_LOOP_DIRECTION_BITS;i++){
      bitWrite(_returnValue,i,bitRead(drumSound[_SOUND][NUMBER_OF_LOOP_DIRECTION_BYTE],i+NUMBER_OF_LOOP_DIRECTION_OFFSET)); 
    }
    break;

  case SHIFT_SPEED:
    _returnValue = drumSound[_SOUND][NUMBER_OF_SHIFT_SPEED_BYTE]; 
    break;

  case BIT_CRUSH:
    _returnValue = drumSound[_SOUND][NUMBER_OF_BIT_CRUSH_BYTE]; 
    break;
  }
  return _returnValue; 

}


void SetValue(unsigned char _VALUE_TYPE,unsigned char _SOUND, unsigned int _setValue){


  switch(_VALUE_TYPE){
  case SAMPLE:
    for(int i=0;i<NUMBER_OF_SAMPLE_BITS;i++){
      bitWrite(drumSound[_SOUND][NUMBER_OF_SAMPLE_BYTE],i+NUMBER_OF_SAMPLE_OFFSET,bitRead(_setValue,i));  
    }
    break;
  case VOLUME:
    _setValue=7-(_setValue>>4);
    for(int i=0;i<NUMBER_OF_VOLUME_BITS;i++){
      bitWrite(drumSound[_SOUND][NUMBER_OF_VOLUME_BYTE],i+NUMBER_OF_VOLUME_OFFSET,bitRead(_setValue,i));  
    }
    break;
  case PITCH:
    drumSound[_SOUND][NUMBER_OF_PITCH_BYTE]=_setValue;  
    break;
  case LOOP_LENGTH:
    for(int i=0;i<NUMBER_OF_LOOP_LENGTH_BITS_1;i++){
      bitWrite(drumSound[_SOUND][NUMBER_OF_LOOP_LENGTH_BYTE_1],i+NUMBER_OF_LOOP_LENGTH_OFFSET_1,bitRead(_setValue,i));  
    }
    for(int i=0;i<NUMBER_OF_LOOP_LENGTH_BITS_2;i++){
      bitWrite(drumSound[_SOUND][NUMBER_OF_LOOP_LENGTH_BYTE_2],i+NUMBER_OF_LOOP_LENGTH_OFFSET_2,bitRead(_setValue,i+NUMBER_OF_LOOP_LENGTH_BITS_1));  
    }
    break;
  case CUT:
    for(int i=0;i<NUMBER_OF_CUT_BITS;i++){
      bitWrite(drumSound[_SOUND][NUMBER_OF_CUT_BYTE],i+NUMBER_OF_CUT_OFFSET,bitRead(_setValue,i));  
    }
    break;
  case START:
    for(int i=0;i<NUMBER_OF_START_BITS_1;i++){
      bitWrite(drumSound[_SOUND][NUMBER_OF_START_BYTE_1],i+NUMBER_OF_START_OFFSET_1,bitRead(_setValue,i));  
    }
    for(int i=0;i<NUMBER_OF_START_BITS_2;i++){
      bitWrite(drumSound[_SOUND][NUMBER_OF_START_BYTE_2],i+NUMBER_OF_START_OFFSET_2,bitRead(_setValue,i+NUMBER_OF_START_BITS_1));  
    }
    break;
  case LOOP_DIRECTION:
    for(int i=0;i<NUMBER_OF_LOOP_DIRECTION_BITS;i++){
      bitWrite(drumSound[_SOUND][NUMBER_OF_LOOP_DIRECTION_BYTE],i+NUMBER_OF_LOOP_DIRECTION_OFFSET,bitRead(_setValue,i));  
    }
    break;

  case SHIFT_SPEED:
    drumSound[_SOUND][NUMBER_OF_SHIFT_SPEED_BYTE]=_setValue;  
    break;
  case BIT_CRUSH:
    drumSound[_SOUND][NUMBER_OF_BIT_CRUSH_BYTE]=_setValue;

    break;
  }

}



boolean GetStep(unsigned char _STEP,unsigned char _SOUND){
  boolean isStep;

  isStep=bitRead(pattern[currentPattern][_STEP],_SOUND);

  return isStep;
}

void SetStep(unsigned char _STEP,unsigned char _SOUND, boolean _ISSTEP){

  bitWrite(pattern[currentPattern][_STEP],_SOUND,_ISSTEP);

}


void ExportPresets() {
  Serial.end(); 
  Serial.begin(9600);
 // Serial.print("prog_uchar flashPreset[1024] PROGMEM = {");

  
  for(int i=0;i<1024;i++){
    Serial.print(EEPROM.read(i));
    
    if(i%32==0) Serial.println();
    
    
  }
 // Serial.println("} ");
Serial.end(); 
Serial.begin(31250);
  //MIDI.begin(GetMIDIChannel());
}


void StorePreset(unsigned char index) {
  if (index < 4) {
    int offset = index * 255;
    for (int j = 0; j < 8; j++) {
      for (int k = 0; k < 7; k++) {
        EEPROM.write(offset + ((7 * j) + k), drumSound[j][k]);
      }
    }

    for (int j = 0; j < 6; j++) {
      for (int k = 0; k < 32; k++) {

        EEPROM.write(offset + 63 + ((32 * j) + k), pattern[j][k]);
      }
    }
    EEPROM.write(offset + 62,tempo);
  }
}


void LoadPreset(unsigned char index) {  

  int offset = index * 255;
  for (int j = 0; j < 8; j++) {
    for (int k = 0; k < 7; k++) {


      drumSound[j][k]=EEPROM.read(offset + ((7 * j) + k));
    }
  }

  for (int j = 0; j < 6; j++) {
    for (int k = 0; k < 32; k++) {

      pattern[j][k]= EEPROM.read(offset + 63 + ((32 * j) + k));
    }
  }
  tempo=EEPROM.read(offset + 62);


}
void ClearMemmory(){
  for(int j=0;j<4;j++){
    LoadPreset(j);
    currentPreset=j;
    for(int i=0;i<8;i++){

      SetValue(CUT,i,63);
      SetValue(LOOP_LENGTH,i,0);
      SetValue(SHIFT_SPEED,i,0);
      SetValue(START,i,0);
      SetValue(LOOP_DIRECTION,i,0);
      SetValue(VOLUME,i,127);
      SetValue(SAMPLE,i,i);
      SetValue(PITCH,i,16);
      SetValue(BIT_CRUSH,i,0);

    }
    for(int i=0;i<6;i++){
      currentPattern=i;
      for(int k=0;k<32;k++){
        for(int l=0;l<8;l++){

          SetStep(k,l,false);

        }
      }
    }
    tempo=120;
    StorePreset(j);
  }
}


unsigned char GetMIDIChannel() {
  return EEPROM.read(1023);
}

void SetMIDIChannel(unsigned char value) {
  EEPROM.write(1023, value);
}
void IsThereChangeInMidiChannel(){
  for(int i=0;i<4;i++){
    if(GetHashState(buttonStates,i+3))
    { 
      channel=i;
      
    }
  }

}



void ShouldIClearMemory(){ 
  if(digitalRead(BUTTON_INSTRUMENT_1_PIN) && digitalRead(BUTTON_INSTRUMENT_1_PIN) && digitalRead(BUTTON_INSTRUMENT_3_PIN) && digitalRead(BUTTON_INSTRUMENT_4_PIN)){
    FlashDiodBlocking(BUTTON_RECORD_INDEX, 10, 50) ;
    pinMode(BUTTON_INSTRUMENT_1_PIN,INPUT);
    pinMode(BUTTON_INSTRUMENT_2_PIN,INPUT);
    pinMode(BUTTON_INSTRUMENT_3_PIN,INPUT);
    pinMode(BUTTON_INSTRUMENT_4_PIN,INPUT);

    ClearMemmory();
    FlashDiodBlocking(BUTTON_RECORD_INDEX, 10, 50) ;
  }

}



