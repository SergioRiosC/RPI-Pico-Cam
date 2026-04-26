#include "image_provider.h"
#include "model_settings.h"
#include "PicoHM01B0.h"
#include "pico/stdlib.h"
#include "tensorflow/lite/micro/micro_time.h"
#include <climits>

// Pines según tu cableado confirmado
#define CAM_SDA   4
#define CAM_SCL   5
#define CAM_VSYNC 16
#define CAM_HREF  15
#define CAM_PCLK  14
#define CAM_D0    6

static PicoHM01B0 Camera;
static uint8_t raw_buffer[324 * 324];
static bool initialized = false;

// Resize 324×324 → 88×88 nearest neighbor
static void resize_to_88(const uint8_t* src, uint8_t* dst) {
  const float scale = 324.0f / 88.0f;
  for (int y = 0; y < 88; y++) {
    for (int x = 0; x < 88; x++) {
      int sx = (int)(x * scale);
      int sy = (int)(y * scale);
      dst[y * 88 + x] = src[sy * 324 + sx];
    }
  }
}

TfLiteStatus GetImage(tflite::ErrorReporter* error_reporter,
                      int image_width, int image_height, int channels,
                      int8_t* image_data) {

  if (!initialized) {
    PicoHM01B0_config config;
    config.i2c_dat_gpio    = CAM_SDA;
    config.i2c_clk_gpio    = CAM_SCL;
    config.vsync_gpio      = CAM_VSYNC;
    config.d0_gpio         = CAM_D0;
    config.pclk_gpio       = CAM_PCLK;
    config.mclk_gpio       = -1;
    config.bus_4bit        = false;
    config.flip_vertical   = false;
    config.flip_horizontal = false;

    if (!Camera.begin(config)) {
      TF_LITE_REPORT_ERROR(error_reporter, "Camera init failed.");
      return kTfLiteError;
    }
    Camera.start_streaming(25, false, false);
    initialized = true;
  }

  // Captura frame
  Camera.start_capture(raw_buffer);
  Camera.wait_for_frame();
  // Captura segunda vez para estabilizar exposición
  Camera.start_capture(raw_buffer);
  Camera.wait_for_frame();

  // Resize 324×324 → 88×88
  uint8_t gray88[88 * 88];
  resize_to_88(raw_buffer, gray88);

  // Grayscale → RGB + cuantizar a int8
  for (int i = 0; i < 88 * 88; i++) {
    int8_t val = (int8_t)((int)gray88[i] - 128);
    image_data[i * 3 + 0] = val;  // R
    image_data[i * 3 + 1] = val;  // G
    image_data[i * 3 + 2] = val;  // B
  }

  return kTfLiteOk;
}