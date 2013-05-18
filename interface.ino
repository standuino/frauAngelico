/*

const byte pot[5]={
  0,1,2,3,4};

int potVal[5], lastPotVal[5];

const int butPins[4]={
  2,4,8,7};
const byte switchPin[3]={
  9,10,11};
boolean switchState[3], lastSwitchState[3], butSwitchState[3], lastButSwitchState[3];

boolean butState[4],lastButState[4];

byte lastSound;

void fakeRoutine(){
  unsigned char velocity=map(analogRead(2),0,1024,0,127);
  interface();
  for(int i=0;i<4;i++){
    if(butState[i] && !lastButState[i]){
      if(switchState[0]){
        playNote(60+i+4,velocity); 
      }
      else{
        playNote(60+i,velocity); 
      }
    }
  }
  OCR1A = F_CPU / map(analogRead(4),0,1024,0,30000); 
  bitCrush=map(analogRead(1),0,1024,0,16);
 unisono=map(analogRead(3),0,1024,1,16);
  unisono=map(analogRead(0),0,1024,1,16);

  if(bitCrush>7){
    bitReverse=true;
    bitCrush-=8;
  }
  else{
    bitReverse=false;
  }

}




int interface(){
  lastSound=sound;

  for(int i=0;i<4;i++){

    digitalWrite(butPins[i],LOW);
    pinMode(butPins[i],INPUT); 

    lastButState[i]=butState[i];
    butState[i]=digitalRead(butPins[i]);
    bitWrite(sound,i,butState[i]);

  }



  for(int i=0;i<5;i++){
    lastPotVal[i]=potVal[i]; 
    //lastSwitchState[i]=switchState[i];
    potVal[i]=analogRead(pot[i]); 
    // switchState[i]=digitalRead(switchPin[i]); 
  }

  /// switch buttons

  for(int i=0;i<3;i++){  
    digitalWrite(switchPin[i],LOW);
    pinMode(switchPin[i],INPUT); 
    lastSwitchState[i]=switchState[i];
    lastButSwitchState[i]=butSwitchState[i];
    butSwitchState[i]=digitalRead(switchPin[i]);
    if(butSwitchState[i] && butSwitchState[i]!=lastButSwitchState[i]){
      switchState[i]=!switchState[i]; 
    }
    pinMode(switchPin[i],OUTPUT); 
    digitalWrite(switchPin[i],switchState[i]);     



  }

  uint16_t analogValue = analogRead(1);
  isReverse = switchState[2];  
  rate[0][1] = rate[1][1] = rate[2][1] = rate[3][1] = rate[4][1] = rate[5][1] = 0;//(((analogValue/120)==0) ? analogValue : 0); //  > 120 counter
  rate[0][0] = rate[1][0] = rate[2][0] = rate[3][0] = rate[4][0] = rate[5][0] = 1;//(analogValue/120) * ((isReverse) ? -1 : 1);
  //end
}



*/


