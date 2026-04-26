int L_sen, R_sen;

void setup()
{
  pinMode(10, INPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);

  while (digitalRead(10) != 1);
}

void loop()
{
  L_sen = analogRead(A1);
  R_sen = analogRead(A0);

  if (L_sen < 25 && R_sen < 25)
  {
    forward();
  }
  else if (L_sen > 25  && R_sen > 25)
  {
    left();                                   // Kindly change the ir values acording to your surface they can change //
  }                                                     
  else if (L_sen < 25 && R_sen > 25)
  {
    right();
  }
  else if (L_sen < 25 && R_sen < 25)
  {
    right();
  }
}

void forward(){
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  digitalWrite(14, HIGH);
  digitalWrite(15, LOW);

}
void left(){
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
  digitalWrite(14, HIGH);
  digitalWrite(15, LOW);
}
void right(){
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  digitalWrite(14, LOW);
  digitalWrite(15, HIGH);

}
void pause(){
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  digitalWrite(14, LOW);
  digitalWrite(15, LOW);
}


