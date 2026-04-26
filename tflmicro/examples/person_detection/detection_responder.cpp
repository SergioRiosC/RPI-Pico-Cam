#include "detection_responder.h"
#include "model_settings.h"
#include "pico/stdlib.h"
#include <cstdio>

// LED integrado del Pico W
#define LED_PIN 25

void RespondToDetection(tflite::ErrorReporter* error_reporter,
                        int8_t felino_score,
                        int8_t primate_score) {

  // Convertir scores INT8 → float [0,1]
  // La cuantización INT8 del modelo: real_value = (int8_val - zero_point) * scale
  // Para salida típica de MobileNetV1 INT8: zero_point=-128, scale=1/256
  float primate_prob = (primate_score + 128) / 255.0f;
  float felino_prob  = (felino_score  + 128) / 255.0f;

  const float UMBRAL = 0.25f;

  const char* resultado;
  if (primate_prob > UMBRAL) {
    resultado = "PRIMATE";
  } else {
    resultado = "FELINO";
  }

  // Output por serial
  printf("--- Inferencia ---\n");
  printf("Primate : %.3f\n", primate_prob);
  printf("Felino  : %.3f\n", felino_prob);
  printf("Resultado: %s\n\n", resultado);

  // LED parpadea diferente según clase
  // PRIMATE: 3 parpadeos rápidos
  // FELINO:  1 parpadeo lento
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  if (primate_prob > UMBRAL) {
    for (int i = 0; i < 3; i++) {
      gpio_put(LED_PIN, 1); sleep_ms(100);
      gpio_put(LED_PIN, 0); sleep_ms(100);
    }
  } else {
    gpio_put(LED_PIN, 1); sleep_ms(500);
    gpio_put(LED_PIN, 0);
  }
}
