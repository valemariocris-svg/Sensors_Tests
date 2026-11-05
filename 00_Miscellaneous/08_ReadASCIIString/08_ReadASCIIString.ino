/* ------------------------------------------------------------------------- */
char Request[2][3] = {
    {'c', 'a', 'l'},
    {'o', 'f', 'f'},
};

char buffRead[8];
char bufRequest[8];

/* ------------------------------------------------------------------------- */
void setup() {
  Serial.begin(115200);
}

/* ------------------------------------------------------------------------- */
void loop() {
  int  Index      = 0;
  int  ReqIndex   = 0;
  char Character  = ' ';
  int  ReqLen   = 0;

  while (Serial.available() > 0) {
    char Character = Serial.read();
    if(Index < 3) {
      buffRead[Index] = Character;
      Index++;
    }    
  }
  if(Index > 0) {
    for(int i=0; i<Index; i++) { 
      Character = buffRead[i];
      // if((uint8_t) Character < 0x20) Serial.println("LESS");
      // if(Character != '\n' && Character != '\0') {
      if( (Character > 0x2F) && (Character <= 0x7C) ) {
        Serial.print(buffRead[i]);
        bufRequest[ReqIndex++] = Character;
      }
    }
  }

  ReqLen = 0;
  if(ReqIndex > 0) {
    for(int ir=0; ir<2; ir++) {
      for(int ic=0; ic<3; ic++) {
        if(bufRequest[ic] == Request[ir][ic]) { ReqLen++; }
      }
      if(ReqLen == 3) {
        if(ir==0) Serial.println("CALIBRA");
        else if(ir==1) Serial.println("OFFSET");
        else           Serial.println("NONE");
        break;
      }
    }
    // Serial.println("Fine");
  }
}
/* ------------------------------------------------------------------------- */

//function to convert ascii char[] to hex-string (char[])
// void string2hexString(char* input, char* output)
// {
//     int loop;
//     int i;

//     i = 0;
//     loop = 0;

//     while (input[loop] != '\0') {
//         sprintf((char*)(output + i), "%02X", input[loop]);
//         loop += 1;
//         i += 2;
//     }
//     //insert NULL at the end of the output string
//     output[i++] = '\0';
// }
