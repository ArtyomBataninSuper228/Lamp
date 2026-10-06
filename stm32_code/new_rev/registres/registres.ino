#include <Arduino.h>

// Размер массивов
const int NUM = 20000;
int a = 0;
int f = 100000;
float T = 0.25;
short data_L[NUM];
short data_I[NUM];

// Переопределяем функцию настройки АЦП ядра STM32duino.
// Ядро само вызовет этот код при первом запуске analogRead.
#ifdef __cplusplus
extern "C" {
#endif
void adc_sampling_time(ADC_HandleTypeDef *hadc, uint32_t sampling_time) {
  // 1. Разгоняем тактовую частоту АЦП до максимума (DIV2)
  hadc->Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
  HAL_ADC_Init(hadc);

  // 2. Жестко прописываем 3 цикла выборки для ВСЕХ каналов (максимум скорости)
  ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  
  // Применяем настройки для каналов PB0 (Channel 8) и PB1 (Channel 9)
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 1;
  HAL_ADC_ConfigChannel(hadc, &sConfig);
  
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = 1;
  HAL_ADC_ConfigChannel(hadc, &sConfig);
}
#ifdef __cplusplus
}
#endif

void setup() {
  pinMode(PB4, OUTPUT); // Первый контакт реле
  pinMode(PB5, OUTPUT); // Второй контакт реле
  
  // Настраиваем как аналоговые входы (INPUT_ANALOG убирает лишние шумы)
  pinMode(PB0, INPUT_ANALOG); 
  pinMode(PB1, INPUT_ANALOG); 
  
  Serial.begin(2000000); // Старт COM порта
  delay(500);
}

void loop() {
  digitalWrite(PB4, HIGH);
  digitalWrite(PB5, HIGH); 
  
  if (Serial.available() > 0) {
    a = Serial.parseInt();
    switch (a){
    case 6824: // Начало измерений на первой спирали
      {
        digitalWrite(PB4, LOW);
        delay(12); // Даем реле время физически замкнуться
        
        unsigned long startTime = micros();
        
        // Обычный аналогрид теперь работает на максимальной скорости железа!
        for(int i = 0; i < NUM; i++){
          data_L[i] = analogRead(PB0);
          data_I[i] = analogRead(PB1);
        }
        
        unsigned long endTime = micros();
        digitalWrite(PB4, HIGH); // Выключаем реле
        
        // Выводим точное время в микросекундах
        Serial.print("Time taken (us): ");
        Serial.println(endTime - startTime);
        
        // Выводим данные в порт
        for(int i = 0; i < NUM; i++){
          Serial.print(data_L[i]);
          Serial.print(" ");
          Serial.println(data_I[i]);
        }
        break;
      }
      
    case 6825: // Начало измерений на второй спирали
      {
        digitalWrite(PB5, LOW);
        delay(12);
        
        unsigned long startTime = micros();
        for(int i = 0; i < NUM; i++){
          data_L[i] = analogRead(PB0);
          data_I[i] = analogRead(PB1);
        }
        unsigned long endTime = micros();
        digitalWrite(PB5, HIGH);
        
        Serial.print("Time taken (us): ");
        Serial.println(endTime - startTime);
        
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
        break;
      }
    }
  }
}
