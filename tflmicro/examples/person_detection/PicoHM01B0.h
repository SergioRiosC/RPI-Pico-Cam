#ifndef PicoHM01B0_H
#define PicoHM01B0_H

#include <stdint.h>
#include <string.h>
#include "hardware/pio.h"

#ifdef __ARM_ARCH

struct sensor_reg;

class PicoHM01B0_config {
public:
    uint i2c_dat_gpio;
    uint i2c_clk_gpio;
    uint vsync_gpio;
    uint d0_gpio;
    uint pclk_gpio;
    int mclk_gpio;
    uint mclk_freq;
    bool bus_4bit;
    bool flip_horizontal, flip_vertical;

    PicoHM01B0_config() {
        memset(this, 0, sizeof(*this));
    }
};


class PicoHM01B0 {
public:
    int begin(const PicoHM01B0_config &config);
    void start_streaming(float frame_rate, bool binning_2x2, bool qvga_mode);
    void start_capture(uint8_t *dest);
    bool is_frame_ready(void);
    void wait_for_frame(void);
    void stop_streaming(void);
    void set_fixed_exposure(float exposure_ms, int d_gain, int a_gain);
    void set_auto_exposure(void);

    float get_actual_frame_rate_fps(void) const { return actual_frame_rate; }

    int get_cols(void) const { return binning_2x2 ? 164 : 324; }
    int get_rows(void) const {
        int ret = qvga_mode ? 244 : 324;
        return binning_2x2 ? ret / 2 : ret;
    }

    PicoHM01B0() { state = STATE_RESET; }

private:
    enum state_t {
        STATE_RESET = 0,
        STATE_BEGIN = 1,
        STATE_STREAMING = 2,
        STATE_CAPTURING = 3,
    };

    state_t state;
    PicoHM01B0_config config;
    float frame_rate;
    float actual_frame_rate;
    int clock_div;
    bool qvga_mode;
    bool binning_2x2;
    bool exp_auto;
    int exp_lines, exp_analog_gain, exp_digital_gain;

    PIO data_pio;
    uint data_pio_sm;
    uint data_pio_offset;
    uint dma_channel;

    PIO clock_pio;
    uint clock_pio_sm;
    uint clock_pio_offset;

    uint line_length, line_count;

    void calc_optimal_length(void);
    void set_clock_vars(void);
    void i2c_bus_start(void);
    void i2c_bus_stop(void);
    void i2c_bus_send_ack(void);
    int i2c_bus_write_byte(int data);
    int i2c_write_reg(int regID, int regDat);
    void arducam_regs_write(const sensor_reg *camera_regs, int count);
};

#else
#error PicoHM01B0 only works on RP2040 architecture
#endif

#endif