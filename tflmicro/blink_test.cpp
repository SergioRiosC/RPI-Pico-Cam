#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LED_PIN 25
#define LED_EXT 0

int main() {
  gpio_init(LED_PIN);
  gpio_init(LED_EXT);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  gpio_set_dir(LED_EXT, GPIO_OUT);
  while (true) {
    gpio_put(LED_PIN, 1);
    gpio_put(LED_EXT, 1);
    sleep_ms(500);
    gpio_put(LED_PIN, 0);
    gpio_put(LED_EXT, 0);
    sleep_ms(500);
  }
}