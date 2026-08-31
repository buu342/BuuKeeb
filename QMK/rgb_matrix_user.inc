RGB_MATRIX_EFFECT(OPENRGB_SAVE_DIRECT)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#include "helper.h"

extern uint8_t saved_directrgb[RGB_MATRIX_LED_COUNT][3];

static bool OPENRGB_SAVE_DIRECT(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    for (uint8_t i = led_min; i < led_max; i++) {
        rgb_t rgb = safe_rgb_brightness(
            saved_directrgb[i][0],
            saved_directrgb[i][1],
            saved_directrgb[i][2]
        );
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    return rgb_matrix_check_finished_leds(led_max);
}

#endif