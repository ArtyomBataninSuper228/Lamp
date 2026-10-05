void setup() {
  pinMode(PB4, OUTPUT); //Первый контакт реле
  pinMode(PB5, OUTPUT); //Второй контакт реле
  pinMode(PB0, INPUT); //Измерение тока
  pinMode(PB1, INPUT); //Измерение яркости
  Serial.begin(2000000); //Старт COM порта

}
int a = 0;
int f = 100000;
float T = 0.25;
short data_L[10000];
short data_I[10000];


void loop() {
  digitalWrite(PB4, HIGH);
  digitalWrite(PB5, HIGH);
  a = Serial.parseInt();
  switch (a){
  case 6824:// Начало измерений на первой спирали
    {
      digitalWrite(PB4, LOW);
      for(int i = 0; i < 10000; i++){
        data_L[i] = analogRead(PB0);
        data_I[i] = analogRead(PB1);
      }
      for(int i = 0; i < 10000; i++){
        Serial.print(data_L[i]);
        Serial.print(" ");
        Serial.println(data_I[i]);
      }

      break;
    }
    case 6825:// Начало измерений на второй спирали
    {
      digitalWrite(PB5, LOW);
      for(int i = 0; i < 10000; i++){
        data_L[i] = analogRead(PB0);
        data_I[i] = analogRead(PB1);
      }
      for(int i = 0; i < 10000; i++){
        Serial.print(data_L[i]);
        Serial.print(" ");
        Serial.println(data_I[i]);
      }

      break;
    }
    case 6826:
    {
      Serial.print(analogRead(PB0));
      Serial.print(" ");
      Serial.println(analogRead(PB1));
    }
  }

}
