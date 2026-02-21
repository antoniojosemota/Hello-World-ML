#include <EloquentTinyML.h>
#include "luxia.h" // O arquivo que você gerou
#include <Wire.h>
#include <BH1750.h>

const int PINO_LED = 15; // PINO D15
// Configurações do modelo
#define NUMBER_OF_INPUTS 1  // Ajuste para o número de entradas do seu modelo
#define NUMBER_OF_OUTPUTS 3 // Ajuste para o número de saídas do seu modelo
#define TENSOR_ARENA_SIZE 8*1024 // 8KB de RAM (aumente se o modelo for grande)

// Cria a instância do modelo
Eloquent::TinyML::TfLite<NUMBER_OF_INPUTS, NUMBER_OF_OUTPUTS, TENSOR_ARENA_SIZE> ml;
BH1750 lightMeter;

void setup() {
    Serial.begin(115200);
    
    // Carrega o modelo usando o array que está no modelo_data.h
    // O nome 'lux_model_int8_tflite' deve ser igual ao nome da variável dentro do .h
    if (!ml.begin(luxia_tflite)) {
        Serial.println("Falha ao carregar o modelo!");
        while (1);
    }
    Serial.println("Modelo carregado com sucesso!");

      Serial.begin(115200);

    // Inicializa I2C
    Wire.begin(); // Para ESP32: SDA 21, SCL 22 por padrão

    // Inicializa sensor
    if (lightMeter.begin()) {
      Serial.println("BH1750 iniciado com sucesso!");
    } else {
      Serial.println("Erro ao iniciar BH1750");
    }

    pinMode(PINO_LED, OUTPUT);
  }

void loop() {
    float lux = lightMeter.readLightLevel();
    float lux_normalizado = lux / 1000.0f; // Mantenha a normalização que você usou

    float input_data[1] = { lux_normalizado };
    float output[3];
    float prediction = ml.predict(input_data, output);

    int classe = 0;
    float major = output[0];
    
    for(int i = 1; i < 3; i++){
      if(output[i] > major){
        major = output[i];
        classe = i;
      }
    }

    Serial.print("Lux: "); Serial.print(lux);
    Serial.print(" | Probabilidades: ");
    Serial.print(output[0]); Serial.print(" ");
    Serial.print(output[1]); Serial.print(" ");
    Serial.print(output[2]);
    // NOVA LÓGICA BASEADA NOS SEUS RESULTADOS:
    // Se estiver escuro (IA alta, ex: 0.9), ACENDE o LED
    switch (classe) {
        case 0:
            digitalWrite(PINO_LED, HIGH);
            Serial.println(" -> ESTADO: ESCURO (LED ACESO)");
            break;

        case 1:
            Serial.println(" -> ESTADO: NORMAL (MEIA LUZ)");
            break;

        case 2:
            digitalWrite(PINO_LED, LOW);
            Serial.println(" -> ESTADO: MUITO CLARO (LED APAGADO)");
            break;
    }

    
    delay(2000);
}
