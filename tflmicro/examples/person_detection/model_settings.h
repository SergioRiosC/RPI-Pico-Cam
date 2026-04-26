#ifndef TENSORFLOW_LITE_MICRO_EXAMPLES_PERSON_DETECTION_MODEL_SETTINGS_H_
#define TENSORFLOW_LITE_MICRO_EXAMPLES_PERSON_DETECTION_MODEL_SETTINGS_H_

// Dimensiones del modelo primate vs felino
constexpr int kNumCols = 88;
constexpr int kNumRows = 88;
constexpr int kNumChannels = 3;  // RGB (grayscale triplicado)

constexpr int kMaxImageSize = kNumCols * kNumRows * kNumChannels;  // 23,232 bytes

// Clases
constexpr int kCategoryCount = 2;
constexpr int kFelinoIndex = 0;
constexpr int kPrimateIndex = 1;

extern const char* kCategoryLabels[kCategoryCount];

#endif  // TENSORFLOW_LITE_MICRO_EXAMPLES_PERSON_DETECTION_MODEL_SETTINGS_H_
