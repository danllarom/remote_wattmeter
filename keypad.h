  int a=0;
  int *punt_a=&a;
  int long_a=4;
  int *punt_long_a=&long_a;
  int b=0;
  int *punt_b=&b;
  int long_b=4;
  int *punt_long_b=&long_b;
  int c=0;
  int *punt_c=&c;
  int button1;
  int button2;

void evento1(){
  if (digitalRead(button2)==HIGH){
    if(*punt_c==1){
      *punt_c=0;
    }
    else if(*punt_a < (*punt_long_a-1)){
      *punt_a = *punt_a + 1;
    }
    else{
      *punt_a = 0;
    }
  }
  else{
    if(*punt_b < (*punt_long_b-1)){
      *punt_b = *punt_b + 1;
    }
    else{
      *punt_b = 0;
    }
    *punt_c=1; 
  }
}

void evento2(){
  if (digitalRead(button1)==HIGH){
    if(*punt_c==1){
      *punt_c=0;
    }
    else if(*punt_a > 0){
      *punt_a = *punt_a - 1;
    }
    else{
      *punt_a = (*punt_long_a-1);
    }
  }
  else{
    if(*punt_b > 0){
      *punt_b = *punt_b - 1;
    }
    else{
      *punt_b = (*punt_long_b-1);
    } 
    *punt_c=1;
  }
}

void keypad(int buton1, int buton2){
  button1=buton1;
  button2=buton2;
  attachInterrupt(digitalPinToInterrupt(buton1), evento1, RISING); //5
  attachInterrupt(digitalPinToInterrupt(buton2), evento2, RISING);
}
