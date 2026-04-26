#include "image_provider.h"
#include "model_settings.h"
#include "arducam.h"
#include "pico/stdlib.h"
#include "tensorflow/lite/micro/micro_time.h"
#include <climits>

// Buffer temporal para frame completo de la cámara (96×96 es lo que captura arducam por defecto)
static uint8_t raw_buffer[96 * 96];

// Redimensiona 96×96 → 88×88 (nearest neighbor)
static void resize_96_to_88(const uint8_t* src, uint8_t* dst) {
  const float scale = 96.0f / 88.0f;
  for (int y = 0; y < 88; y++) {
    for (int x = 0; x < 88; x++) {
      int src_x = (int)(x * scale);
      int src_y = (int)(y * scale);
      dst[y * 88 + x] = src[src_y * 96 + src_x];
    }
  }
}

TfLiteStatus GetImage(tflite::ErrorReporter* error_reporter, int image_width,
                      int image_height, int channels, int8_t* image_data) {

  static bool first = true;
  if (first) {
    arducam.systemInit();
    if (arducam.busDetect()) {
      TF_LITE_REPORT_ERROR(error_reporter, "Bus detect failed.");
      return kTfLiteError;
    }
    if (arducam.cameraProbe()) {
      TF_LITE_REPORT_ERROR(error_reporter, "Camera probe failed.");
      return kTfLiteError;
    }
    arducam.cameraInit(YUV);
    first = false;
  }

  // Captura al buffer raw
  capture(raw_buffer);

  // Buffer temporal 88×88 grayscale
  uint8_t gray88[88 * 88];
  resize_96_to_88(raw_buffer, gray88);

  // Triplicar canal grayscale → RGB + cuantizar a int8
  for (int i = 0; i < 88 * 88; i++) {
    int8_t val = (int8_t)((int)gray88[i] - 128);
    image_data[i * 3 + 0] = val;  // R
    image_data[i * 3 + 1] = val;  // G
    image_data[i * 3 + 2] = val;  // B
  }

  return kTfLiteOk;
}
