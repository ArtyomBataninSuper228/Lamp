void setup() {
  pinMode(PB4, OUTPUT); //Первый контакт реле
  pinMode(PB5, OUTPUT); //Второй контакт реле
  pinMode(PB6, INPUT); //Измерение тока
  pinMode(PB7, INPUT); //Измерение яркости
  Serial.begin(2000000); //Старт COM порта

}
int a = 0;
int f = 100000;
float T = 0.25;
short data_L[25000];
short data_I[25000];


void loop() {
  digitalWrite(PB4, HIGH);
  digitalWrite(PB5, HIGH);
  a = Serial.parseInt();
  switch (a){
  case 6824:// Начало измерений на первой спирали
    {
      digitalWrite(PB4, LOW);
      for(int i = 0; i < 25000; i++){
        dara_L[i] = analogRead(PB6);
        dara_I[i] = analogRead(PB7);
      }
      for(int i = 0; i < 25000; i++){
        Serial.print(data_L[i]);
        Serial.print(" ");
        Serial.println(data_L[i]);
      }

      break;
    }
    case 6825:// Начало измерений на второй спирали
    {
      digitalWrite(PB5, LOW);
      for(int i = 0; i < 25000; i++){
        dara_L[i] = analogRead(PB6);
        dara_I[i] = analogRead(PB7);
      }
      for(int i = 0; i < 25000; i++){
        Serial.print(data_L[i]);
        Serial.print(" ");
        Serial.println(data_L[i]);
      }

      break;
    }
  }

}
