#include <Arduino.h>

// Размер массивов (вы просили по 10к на каждый канал)
const int CH_SIZE = 10000;
const int TOTAL_SAMPLES = CH_SIZE * 2; // Общий буфер для DMA (каналы идут вперемешку)

// Основные массивы, куда мы разложим данные
uint16_t bufferCh1[CH_SIZE];
uint16_t bufferCh2[CH_SIZE];

// Общий буфер, куда DMA будет складывать данные "на лету"
alignas(4) uint16_t dmaBuffer[TOTAL_SAMPLES]; 

// Используем пины PA0 (ADC1_IN0) и PA1 (ADC1_IN1)
const int pin1 = PA0;
const int pin2 = PA1;

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
volatile bool isCaptureDone = false;

// Обработчик завершения работы DMA
extern "C" void DMA2_Stream0_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_adc1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  if (hadc->Instance == ADC1) {
    HAL_ADC_Stop_DMA(hadc); // Останавливаем АЦП
    isCaptureDone = true;   // Флаг готовности данных
  }
}

void configureHardwareADC() {
  // 1. Включаем тактирование АЦП и DMA
  __HAL_RCC_ADC1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  // 2. Настраиваем DMA2 Stream 0 Channel 0 (привязано к ADC1 на F411)
  hdma_adc1.Instance = DMA2_Stream0;
  hdma_adc1.Init.Channel = DMA_CHANNEL_0;
  hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_adc1.Init.MemInc = DMA_MINC_ENABLE; // Сдвигаем указатель памяти
  hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // 16 бит
  hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma_adc1.Init.Mode = DMA_NORMAL; // Сбор до заполнения буфера
  hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  HAL_DMA_Init(&hdma_adc1);

  __HAL_LINKDMA(&hadc1, hdma_adc1, hdma_adc1);

  // Настраиваем прерывания DMA в системе
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

  // 3. Конфигурируем сам АЦП1 на максимальную скорость
  hadc1.Instance = ADC1;
  // Делитель тактовой частоты АЦП. DIV2 дает максимум скорости на F411!
  hadc1.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2; 
  hadc1.Init.Resolution = ADC_RESOLUTION_12B; // Честные 12 бит
  hadc1.Init.ScanConvMode = ENABLE;           // Включаем сканирование группы каналов
  hadc1.Init.ContinuousConvMode = ENABLE;     // Непрерывный режим "в упор"
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; // Без таймера, на макс. частоте
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;             // Опрашиваем ровно 2 канала по очереди
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  HAL_ADC_Init(&hadc1);

  // 4. Задаем очередность (Ранги) и время выборки
  ADC_ChannelConfTypeDef sConfig = {0};
  
  // Канал 1 (PA0) -> Ранг 1
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  // 3 цикла - минимально возможное время для F411 (Максимум скорости)
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES; 
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  // Канал 2 (PA1) -> Ранг 2
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

void setup() {
  Serial.begin(2000000);
  pinMode(PB4, OUTPUT); //Первый контакт реле
  pinMode(PB5, OUTPUT); //Второй контакт реле
  // Инициализируем пины как аналоговые входы
  pinMode(pin1, INPUT_ANALOG);
  pinMode(pin2, INPUT_ANALOG);


  // Инициализируем железные регистры АЦП
  configureHardwareADC();
}

void loop() {
  digitalWrite(PB4, HIGH);//Реле имеет инвертированный вход
  digitalWrite(PB5, HIGH);
  a = Serial.parseInt();
  switch (a){
  case 6824:// Начало измерений на первой спирали
    {
      digitalWrite(PB4, LOW);
      isCaptureDone = false;

      unsigned long startTime = micros();

      // Запускаем АЦП в связке с DMA
      HAL_ADC_Start_DMA(&hadc1, (uint32_t*)dmaBuffer, TOTAL_SAMPLES);

      // Процессор полностью свободен, пока железо пишет массивы!
      while (!isCaptureDone) {
       // Ждем окончания записи буфера
      }

      unsigned long endTime = micros();
      float duration = (endTime - startTime) / 1000.0; // Время в мс

      // Раскладываем общий буфер DMA на два независимых массива каналов
      // Так как режим был Scan, данные лежат так: [Ch1_0, Ch2_0, Ch1_1, Ch2_1, ...]
      for (int i = 0; i < CH_SIZE; i++) {
        bufferCh1[i] = dmaBuffer[i * 2];
        bufferCh2[i] = dmaBuffer[i * 2 + 1];
      }


      // Выведем первые 5 измерений для проверки
      for(int i = 0; i < CH_SIZE; i++){
        Serial.print(bufferCh1[i]); Serial.print(" "); Serial.println(bufferCh2[i]);
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
