#include "image_provider.h"
#include "model_settings.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

// Pines cámara HM01B0 (según tu cableado confirmado)
#define CAM_SDA   4
#define CAM_SCL   5
#define CAM_VSYNC 16
#define CAM_HREF  15
#define CAM_PCLK  14
#define CAM_D0    6

// Buffer para frame completo 324×324
static uint8_t raw_buffer[324 * 324];

// Buffer de salida 88×88×3
static uint8_t scaled_buffer[kNumRows * kNumCols * kNumChannels];

// Declaraciones externas del driver HM01B0 del repo
extern "C" {
  #include "HM01B0.h"
}

// Redimensiona 324×324 → 88×88 (nearest neighbor)
static void resize_324_to_88(const uint8_t* src, uint8_t* dst_gray) {
  const float scale = 324.0f / 88.0f;
  for (int y = 0; y < 88; y++) {
    for (int x = 0; x < 88; x++) {
      int src_x = (int)(x * scale);
      int src_y = (int)(y * scale);
      dst_gray[y * 88 + x] = src[(src_y * 324) + src_x];
    }
  }
}

// Triplica canal grayscale → RGB en buffer de salida
static void gray_to_rgb(const uint8_t* gray, uint8_t* rgb) {
  for (int i = 0; i < kNumRows * kNumCols; i++) {
    rgb[i * 3 + 0] = gray[i];  // R
    rgb[i * 3 + 1] = gray[i];  // G
    rgb[i * 3 + 2] = gray[i];  // B
  }
}

TfLiteStatus GetImage(tflite::ErrorReporter* error_reporter,
                      int image_width, int image_height, int channels,
                      int8_t* image_data) {

  // Captura frame 324×324
  hm01b0_capture_frame(raw_buffer);

  // Temp buffer 88×88 grayscale
  uint8_t gray88[88 * 88];

  // Resize
  resize_324_to_88(raw_buffer, gray88);

  // Grayscale → RGB
  gray_to_rgb(gray88, scaled_buffer);

  // Convertir uint8 [0,255] → int8 [-128,127] (cuantización INT8)
  for (int i = 0; i < kNumRows * kNumCols * kNumChannels; i++) {
    image_data[i] = (int8_t)((int)scaled_buffer[i] - 128);
  }

  return kTfLiteOk;
}
