void setup() {
  pinMode(PB4, OUTPUT); //Первый контакт реле
  pinMode(PB5, OUTPUT); //Второй контакт реле
  pinMode(PB0, INPUT); //Измерение тока
  pinMode(PB1, INPUT); //Измерение яркости
  Serial.begin(2000000); //Старт COM порта
  ADC->CCR &= ~ADC_CCR_ADCPRE; 
  ADC1->SMPR2 &= ~(ADC_SMPR2_SMP8 | ADC_SMPR2_SMP9);
  delay(500);

}
const int NUM = 20000;
int a = 0;
int f = 100000;
float T = 0.25;
short data_L[NUM];
short data_I[NUM];


void loop() {
  digitalWrite(PB4, HIGH);
  digitalWrite(PB5, HIGH); 
  a = Serial.parseInt();
  switch (a){
  case 6824:// Начало измерений на первой спирали
    {
      unsigned long startTime = micros();
      digitalWrite(PB4, LOW);
      for(int i = 0; i < NUM; i++){
        data_L[i] = analogRead(PB0);
        data_I[i] = analogRead(PB1);
      }
      unsigned long endTime = micros();
      Serial.println(endTime-startTime);
      for(int i = 0; i < NUM; i++){
        Serial.print(data_L[i]);
        Serial.print(" ");
        Serial.println(data_I[i]);
      }

      break;
    }
    case 6825:// Начало измерений на второй спирали
    {
      digitalWrite(PB5, LOW);
      for(int i = 0; i < NUM; i++){
        data_L[i] = analogRead(PB0);
        data_I[i] = analogRead(PB1);
      }
      for(int i = 0; i < NUM; i++){
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
