#include "main_functions.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

#define LED_PIN 22

int main(int argc, char* argv[]) {
  // Inicializar LED
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  // 5 parpadeos rápidos al arrancar = programa inició OK
  for (int i = 0; i < 5; i++) {
    gpio_put(LED_PIN, 1); sleep_ms(100);
    gpio_put(LED_PIN, 0); sleep_ms(100);
  }

  setup();

  // 2 parpadeos lentos = setup() completado OK
  for (int i = 0; i < 2; i++) {
    gpio_put(LED_PIN, 1); sleep_ms(500);
    gpio_put(LED_PIN, 0); sleep_ms(500);
  }

  while (true) {
    loop();
  }
}