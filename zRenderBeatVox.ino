/*
***************************************************
-----STANDUINO-------FRAU ANGELICO-----------------
***************************************************

based on the fraAngelico hardware and software
about fraAngelico 
http://www.standuino.eu/devices/instruments/fra-angelico/

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
drum machine:
    -sample based polyphonic plaback (6 voices)
    -8 fully adjustable sounds for each preset that can be triggered by midi or by built in sequencer
    -microsampling features to adjust the sound ( loop length, shift speed, start, cut)
    -bitCrusher, pitchDown, softVolume
    -sample rate knob
    -step sequencer with record feature, tap tempo, pattern length and shuffle
    -it can store 6 double-presets(32 steps) in each preset
    -4 presets
    
    
software by:  Vaclav Pelousek and Martin Baar
developed by Standuino: www.standuino.eu
more information: http://www.standuino.eu/devices/instruments/frauangelico/
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
*/

#define SHOWFREEMEM 1        // Debug Only - Prints on the Serial output the current RAM left
#define MAXVOICES 6
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#  define MIDI_EN 57     // PF3 = 57
#else
#  define MIDI_EN 17
#endif

unsigned int sampleLen[MAXVOICES];
unsigned char bitCrush[MAXVOICES];
unsigned char volume[MAXVOICES];
/*
unsigned char param[MAXVOICES];
unsigned char lfoRate[MAXVOICES];
unsigned char lfoAmt[MAXVOICES];
unsigned char lfoDest[MAXVOICES];
*/
unsigned char pitch[MAXVOICES];

int samplePos[MAXVOICES];  // tune/fine
char rate[MAXVOICES];      // tune/fine
//unsigned char noteVelMatrix[6][242];  // This is used to add Velocity without doing any heavy processing divisions (but also uses most of the RAM)
unsigned char velComp[7]={0,60,90,105,113,118,121};//{121,60,30,15,8,4,2};
unsigned char* samplePCM[MAXVOICES];
unsigned char rateCounter[MAXVOICES];
int loopStart[MAXVOICES];
int loopLength[MAXVOICES];
unsigned char loopSpeed[MAXVOICES];
unsigned char loopDirection[MAXVOICES];
boolean currentDirection[MAXVOICES];

unsigned int mixer[2] = {
  363,363};
unsigned char voice = 0;
unsigned char notevel[MAXVOICES];
//byte incomingByte;
//byte note;
//byte noteOn = 0;
//byte state = 0;

boolean flipFlop = false;
boolean isReverse = false;
int freeMemory();


void beginBeatVox(){

  memset(sampleLen,0,sizeof(sampleLen));
  memset(samplePos,0,sizeof(samplePos));
  memset(samplePCM,0,sizeof(samplePCM));
  memset(rate,0,sizeof(rate));
  memset(notevel,3,sizeof(notevel));

  // Create the Velocity Matrix Values //
  float val = 0.0f;
  float valr = 1.0f;
  for (int x=0; x<6; x++)
  {
    if (x == 1)        valr = 0.80f;
    else if (x == 2) valr = 0.60f;
    else if (x == 3) valr = 0.35f;
    else if (x == 4) valr = 0.20f;
    else if (x == 5) valr = 0.08f;
/*
    for (int v=0; v<242; v++)
    {
      if (x == 0) noteVelMatrix[x][v] = v;
      else
      {
        val = (((2.0f/242.0f)*float(v))-1.0f)*valr;
        noteVelMatrix[x][v] = (unsigned int)((val+1.0f)*(242.0f/2.0f));
      }
    }
    */
  }

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
#else
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
#endif


  pinMode(3, OUTPUT);



  TCCR2A = _BV(COM2B1) | _BV(WGM20);
  TCCR2B = _BV(CS20);


  cli();


  TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
  TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));

  // No prescaler (p.134)
  TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

  // Set the compare register (OCR1A).
  // OCR1A is a 16-bit register, so we have to do this with
  // interrupts disabled to be safe.
  OCR1A = F_CPU / 22000;    // 16e6 / 8000 = 2000

    // Enable interrupt when TCNT1 == OCR1A (p.136)
  TIMSK1 |= _BV(OCIE1A);

  // lastSample = pgm_read_byte(&slavoj[slavoj_length-1]);

  sei(); 
  
  rate[0] = rate[1] = rate[2] = rate[3] = rate[4] = rate[5] = 1;//(analogValue/120) * ((isReverse) ? -1 : 1);

}

boolean pingVoice = true;
unsigned char getFreeVoice(void)
{
  if (pingVoice)
  {
    pingVoice = false;
    if (samplePCM[0] == 0) return 0;
    if (samplePCM[1] == 0) return 1;
    if (samplePCM[2] == 0) return 2;
    if (samplePCM[3] == 0) return 3;
    if (samplePCM[4] == 0) return 4;
    if (samplePCM[5] == 0) return 5;
  }
  else
  {
    pingVoice = true;
    if (samplePCM[5] == 0) return 5;
    if (samplePCM[4] == 0) return 4;
    if (samplePCM[3] == 0) return 3;
    if (samplePCM[2] == 0) return 2;
    if (samplePCM[1] == 0) return 1;
    if (samplePCM[0] == 0) return 0;
  }

  return 5;
}

// ================================ //

void playNote(unsigned char note, unsigned char velocity)
{

   //Serial.println(rateCounter[voice]);
  
  currentVelocity=velocity;
  

lastSound=sound;
sound=note;


//sample=note;  
  
if(currentMode==EDIT_MODE  | currentMode==EDIT_DISTORT_MODE){
  if(sound!=lastSound){
 FreezeKnobs(); 
  }
}
  voice = getFreeVoice();
  samplePCM[voice] = 0; // First stop any sound reference //

  
 renderSound();
  //goodOldTrash(velocity);
  /*
  Serial.print(sample);
  Serial.print("   ");
  Serial.print(bitCrush[voice],DEC);
  Serial.print("   ");
  Serial.print(rate[voice],DEC);
  Serial.print("   ");
  Serial.print(notevel[voice],DEC);
  Serial.print("   ");
  Serial.print(loopStart[voice],DEC);
  Serial.print("   ");
  Serial.print(loopSpeed[voice],DEC);
  Serial.print("   ");
  Serial.print(sampleLen[voice],DEC);
  Serial.print("   ");
  Serial.println(loopLength[voice],DEC);
  
  Serial.println(GetValue(PITCH,sound),DEC);


  */
}

void GoodOldTrash(unsigned char velocity){
  
     /*
 if( sound > PCMtotal){
   
 }
 
 else{
  sampleLen[voice]=PCMlen[sound]/64*GetValue(CUT,sound); //tady cut
  samplePCM[voice] = PCMdata[sound];    
 }
 
 
  notevel[voice] =7-(GetValue(VOLUME,sound)>>4);
  param[voice]=GetValue(PARAM,sound);
    lfoRate[voice]=GetValue(LFO_RATE,sound);
    lfoDest[voice]=GetValue(LFO_DEST,sound);
    lfoAmt[voice]=GetValue(LFO_AMT,sound);
    bitCrush[voice]=GetValue(BIT_CRUSH,sound);
    pitch[voice]=GetValue(PITCH,sound);
    */
    sample=GetValue(SAMPLE,sound);
    samplePos[voice] = PCMlen[sample]/64*map(analogRead(0),0,1024,0,64);
   loopStart[voice]=samplePos[voice];
     notevel[voice] =7-(velocity>>4);
 sampleLen[voice] = samplePos[voice] + (PCMlen[sample]-samplePos[voice])/64*map(analogRead(3),0,1024,0,64);
  samplePCM[voice] = PCMdata[sample];

  rate[voice] =16;//map(analogRead(3),0,1024,0,32);
 
 loopSpeed[voice]=map(analogRead(2),0,1024,1,255);
 loopLength[voice]=analogRead(1);
//loopDirection[voice]=map(analogRead(4),0,1024,0,4);
/*
bitCrush[voice]=0;
for(int i=0;i<map(analogRead(4),0,1024,0,8);i++){
  bitWrite(bitCrush[voice],i,1);
}
*/
bitCrush[voice]=0;//map(analogRead(4),0,1024,0,255);


 // bitCrush[voice]=getValue(BIT_CRUSH,sound);
 
}

void renderSound(){
  switch(currentMode){
 case NORMAL_MODE:
SetAllValuesFromMemory();
//GoodOldTrash(127);

 break;
 
 case EDIT_MODE:
 sample=GetValue(SAMPLE,sound);
loopStart[voice]= 0;
 samplePos[voice]=loopStart[voice];
 
 samplePCM[voice] = PCMdata[sample];
 
 rate[voice] = GetValue(PITCH,sound)+1;
 bitCrush[voice]=GetValue(BIT_CRUSH,sound); 
 notevel[voice] =map(GetValue(VOLUME,sound),127,0,0,80);
 
  if(noteOn) notevel[voice]=map(currentVelocity,127,0,0,80);
 sampleLen[voice] =PCMlen[sample];
 loopSpeed[voice]=1;
 //loopDirection[voice]=0;
 loopLength[voice]=0;

 
 break;
 
 case EDIT_DISTORT_MODE:
 SetAllValuesFromMemory();
 break;
 
 case DISTORT_MODE:
 //GoodOldTrash(127);
 
 sample=GetValue(SAMPLE,sound);
  rate[voice] =GetValue(PITCH,sound)+1;
 bitCrush[voice]=GetValue(BIT_CRUSH,sound); 
notevel[voice] =map(GetValue(VOLUME,sound),127,0,0,80);
 
  loopStart[voice]= PCMlen[sample]/64*GetKnobValue(3);
 samplePos[voice]=loopStart[voice];
 
 sampleLen[voice] = samplePos[voice] + (PCMlen[sample]-samplePos[voice])/64*GetKnobValue(0);
 
 loopSpeed[voice]=GetKnobValue(2);
 //loopDirection[voice]=GetValue(LOOP_DIRECTION,sound);
 loopLength[voice]=GetKnobValue(1);
  if(noteOn) notevel[voice]=map(currentVelocity,127,0,0,80);
 samplePCM[voice] = PCMdata[sample];
 break;
 
 case SEQUENCER_MODE:
 SetAllValuesFromMemory();
 
 break;
 
 case SWITCH_PRESET_MODE:
 sample=GetValue(SAMPLE,sound);
 rate[voice] =GetValue(PITCH,sound)+1;
 bitCrush[voice]=GetValue(BIT_CRUSH,sound); 
 notevel[voice] =map(GetValue(VOLUME,sound),127,0,0,80);
 
  loopStart[voice]= PCMlen[sample]/64*GetKnobValue(3);
 samplePos[voice]=loopStart[voice];
 
 sampleLen[voice] = samplePos[voice] + (PCMlen[sample]-samplePos[voice])/64*GetKnobValue(0);
 
 loopSpeed[voice]=GetKnobValue(2);
 //loopDirection[voice]=GetValue(LOOP_DIRECTION,sound);
 loopLength[voice]=GetKnobValue(1);
  if(noteOn) notevel[voice]=map(currentVelocity,127,0,0,80);
 samplePCM[voice] = PCMdata[sample];
 
 break;
 
 case RECORD_MODE:
 if(sound>15){
   
   sample=12;
     loopStart[voice]= 0;
 samplePos[voice]=0;
 sampleLen[voice] =PCMlen[sample];
  loopSpeed[voice]=sound-15;
    rate[voice] =16;
 bitCrush[voice]=0; 
 notevel[voice] =15;  
 loopLength[voice]=0;
  samplePCM[voice] = PCMdata[sample];
  
 }
 else{
   
 SetAllValuesFromMemory();
 
 }
 
 break;
    
  }
  


 
}


  void SetAllValuesFromMemory(){
    //GoodOldTrash(127);
    if(mystery){
      RenderMystery();
  samplePCM[voice] = PCMdata[sample];
    }
    else{
      sample=GetValue(SAMPLE,sound);
     loopStart[voice]= PCMlen[sample]/64*GetValue(START,sound);
 samplePos[voice]=loopStart[voice];
 sampleLen[voice] = samplePos[voice] + (PCMlen[sample]-samplePos[voice])/64*GetValue(CUT,sound);
  loopSpeed[voice]=GetValue(SHIFT_SPEED,sound)+1;
    
     
    rate[voice] =GetValue(PITCH,sound)+1;
 bitCrush[voice]=GetValue(BIT_CRUSH,sound); 
 notevel[voice] =map(GetValue(VOLUME,sound),127,0,0,80);
  if(noteOn) notevel[voice]=map(currentVelocity,127,0,0,80);
  
 loopLength[voice]=GetValue(LOOP_LENGTH,sound);
  samplePCM[voice] = PCMdata[sample];
    }
 
 //  sampleLen[voice] =PCMlen[sample];//debug
  }
// ==== ==== ==== ==== ==== ==== ==== ==== ======================================================= //

void RenderMystery(){
  if(hardMystery){
    if(mysteryValue==15){
      rate[voice] =random(knobMaxValues[2]);
    }
    else{
     rate[voice] =GetValue(PITCH,sound)+1;
    }
   sample=random(knobMaxValues[1]); 
    
  }
  else{
    rate[voice] =GetValue(PITCH,sound)+1;
   sample=GetValue(SAMPLE,sound); 
  }
  
      loopStart[voice]= PCMlen[sample]/64*random(knobMaxValues[8]);
 samplePos[voice]=loopStart[voice];
 sampleLen[voice] = samplePos[voice] + (PCMlen[sample]-samplePos[voice])/64*random(knobMaxValues[5]);
  loopSpeed[voice]=random(knobMaxValues[7])+1;
    
     
    
 bitCrush[voice]=random(knobMaxValues[3]); 
  loopLength[voice]=random(knobMaxValues[6]); 
 notevel[voice] =map(GetValue(VOLUME,sound),127,0,0,80);
 

  
}


void nextSample(unsigned char nv,unsigned char nm){
  
if (samplePos[nv] >= sampleLen[nv] || samplePos[nv] < 0) samplePCM[nv] = 0;
  if (samplePCM[nv] != 0)
  { 
   
  int output = (unsigned int)pgm_read_byte(&samplePCM[nv][samplePos[nv]]);
      if(output>121){
       output-=notevel[nv];
       if(output<121) output=121;
      }
      
      else{
        output+=notevel[nv];
        if(output>=121) output=121;
      }
      
      mixer[nm] += output|bitCrush[nv];
     // mixer[nm] += velComp[notevel[nv]];
     // mixer[nm] += (((noteVelMatrix[notevel[nv]][(unsigned int)pgm_read_byte(&samplePCM[nv][samplePos[nv][0]])])>>bitCrush)<<bitCrush);
      
    //+i*unisonoSpeed

if(rate[nv]<16){

  rateCounter[nv]++;
 
  if(rateCounter[nv]>(16-rate[nv])){
    rateCounter[nv]=0;
     samplePos[nv]++;
  }
  
}
else{
    samplePos[nv] += rate[nv]-15;
}

if(loopSpeed[nv]!=0){
  /*
switch(loopDirection[nv]){
  case 0:
currentDirection[nv]=true;

break;
case 3:
currentDirection[nv]=false;
break;
case 2:
higherRandom=true;
if(randomizeNow) currentDirection[nv]=!currentDirection[nv];
break;
case 1:
higherRandom=false;
if(randomizeNow) currentDirection[nv]=!currentDirection[nv];
break;


}
*/
//if(currentDirection[nv]){
if(samplePos[nv]-loopStart[nv]>loopLength[nv]){
  loopStart[nv]+=loopSpeed[nv];
 samplePos[nv]=loopStart[nv];
}
/*
}

else{
if(samplePos[nv]-loopStart[nv]>loopLength[nv]){
  loopStart[nv]-=loopSpeed[nv];
 samplePos[nv]=loopStart[nv];
}
}
*/
}    
  }
   
else mixer[nm] += 121;

}

// ======================================================================================= //
/*
#define nextSample(nv,nm)\
if (samplePos[nv][0] >= sampleLen[nv] || samplePos[nv][0] < 0) samplePCM[nv] = 0;\
  if (samplePCM[nv] != 0)\
  { \
    if(bitReverse) mixer[nm] += (noteVelMatrix[notevel[nv]][(unsigned int)pgm_read_byte(&samplePCM[nv][samplePos[nv][0]])]<<bitCrush)/2;\ 
  else  mixer[nm] += ((noteVelMatrix[notevel[nv]][(unsigned int)pgm_read_byte(&samplePCM[nv][samplePos[nv][0]])])>>bitCrush)<<bitCrush;\
    samplePos[nv][0] += rate[nv][0];\
    samplePos[nv][1] += rate[nv][1];\
    if (samplePos[nv][1] > 120) { \
    samplePos[nv][1] = 0; \
    if (isReverse) samplePos[nv][0]--; \
    else samplePos[nv][0]++; \
  }\
  } \
else mixer[nm] += 121;\
*/
// ======================================================================================= //
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
ISR(TIMER4_OVF_vect)
#else
ISR(TIMER1_COMPA_vect)
#endif
{   
  // Do this first so the PWM is updated faster //
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  OCR4A = mixer[0];
  OCR4B = mixer[1];
#else
  OCR2B = mixer[0]/7;

#endif

  mixer[0] = mixer[1] = 0;
  nextSample(0,0);
  nextSample(1,0);
  nextSample(2,0);
  nextSample(3,0);
  nextSample(4,0);
  nextSample(5,0);
}

// ======================================================================================= //
// Checks the RAM left - the ATmega328 has only 2K of RAM //
#if SHOWFREEMEM
extern unsigned int __data_start;
extern unsigned int __data_end;
extern unsigned int __bss_start;
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

int freeMemory()
{
  int free_memory;

  if((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);

  return free_memory;
}
#endif
// ======================================================================================= //

void StopSound() {
  for(int i=0;i<MAXVOICES;i++){
    samplePCM[i] = 0; 
  }
}

void DoMidi2(){
 if (Serial.available() > 0) {
    // read the incoming byte:
   byte incomingByte = Serial.read();
    
   switch (state){
      case 0:
    // look for as status-byte, our channel, note on
    if (incomingByte== (144 | channel)){ 
        
         state=1;
         noteOn=true;
        }
    // look for as status-byte, our channel, note off
   else if (incomingByte== (128 | channel)){ 
       
         state=1;
         midiNoteOn=false;
         //noteOff=true;
        }
        
       case 1:
       // get the note to play or stop
       if(incomingByte < 128) {
          note=incomingByte;
          state=2;
       }
       else{
       state = 0;  // reset state machine as this should be a note number
       }
       break;
       
       case 2:
       // get the velocity
       if(incomingByte < 128) {
         if(noteOn){
          if(incomingByte!=0){ 
          playNote(note%12, incomingByte); 
          
         midiNoteOn=true;
       }
          else{ 
          midiNoteOn=false;
        }
         }// fire off the solenoid
         noteOn=false;
       }
        if(Serial.available()>0){
       state = 1;
        noteOn=true;
      }
      else{
      state = 0;  // reset //
      }
        
     }
  } 
  
}


