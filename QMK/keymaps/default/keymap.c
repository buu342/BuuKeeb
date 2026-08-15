// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "openrgb.h"

// Comment this define to "overclock" the direct mode LEDs
// I will not be responsible for wrecked boards!
#define LIMIT_DIRECTMODE_BRIGHTNESS

// LED Indices
#define LEDINDEX_NUMLOCK    2
#define LEDINDEX_CAPSLOCK   1
#define LEDINDEX_SCROLLLOCK 0


/*******************************************
           Function declarations
*******************************************/

static hsv_t rgb_to_hsv(rgb_t rgb);
static bool keylayer_erase_eeprom(bool activated, void *context);
static bool keylayer_rgb_val_down(bool activated, void *context);
static bool keylayer_rgb_val_up(bool activated, void *context);


/*******************************************
             Keymaps and Layers
*******************************************/

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * ┌───┐   ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┐
     * │Esc│   │F1 │F2 │F3 │F4 │ │F5 │F6 │F7 │F8 │ │F9 │F10│F11│F12│ │PSc│Scr│Pse│
     * └───┘   └───┴───┴───┴───┘ └───┴───┴───┴───┘ └───┴───┴───┴───┘ └───┴───┴───┘
     * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───────┐ ┌───┬───┬───┐ ┌───┬───┬───┬───┐
     * │ ` │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │ Backsp│ │Ins│Hom│PgU│ │Num│ / │ * │ - │
     * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤ ├───┼───┼───┤ ├───┼───┼───┼───┤
     * │ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ [ │ ] │     │ │Del│End│PgD│ │ 7 │ 8 │ 9 │   │
     * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┐ Ent│ └───┴───┴───┘ ├───┼───┼───┤ + │
     * │ Caps │ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ ' │ # │    │               │ 4 │ 5 │ 6 │   │
     * ├────┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┴────┤     ┌───┐     ├───┼───┼───┼───┤
     * │Shft│ \ │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │    Shift │     │ ↑ │     │ 1 │ 2 │ 3 │   │
     * ├────┼───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬────┬────┤ ┌───┼───┼───┐ ├───┴───┼───┤Ent│
     * │Ctrl│GUI │Alt │                        │ Alt│ GUI│Menu│Ctrl│ │ ← │ ↓ │ → │ │   0   │ . │   │
     * └────┴────┴────┴────────────────────────┴────┴────┴────┴────┘ └───┴───┴───┘ └───────┴───┴───┘
     */
    [0] = LAYOUT_fullsize_iso(
        KC_ESC,           KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,     KC_PSCR, KC_SCRL, KC_PAUS,

        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,    KC_INS,  KC_HOME, KC_PGUP,    KC_NUM,  KC_PSLS, KC_PAST, KC_PMNS,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_ENT,     KC_DEL,  KC_END,  KC_PGDN,    KC_P7,   KC_P8,   KC_P9,   KC_PPLS,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_NUHS,                                           KC_P4,   KC_P5,   KC_P6,
        KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT,             KC_UP,               KC_P1,   KC_P2,   KC_P3,   KC_PENT,
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, KC_RGUI, KC_APP,  KC_RCTL,    KC_LEFT, KC_DOWN, KC_RGHT,    KC_P0,            KC_PDOT
    ),
};

// AltGR + ESC -> Clear EEPROM
// Implemented via a custom action because EE_CLR wasn't triggering in this setup
const key_override_t altgr_esc = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_ESC,
    .replacement     = KC_NO,
    .custom_action   = keylayer_erase_eeprom,
};

// AltGR + F1 -> Screen brightness down
const key_override_t altgr_f1 = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_F1,
    .replacement     = KC_BRID,
};

// AltGR + F2 -> Screen brightness up
const key_override_t altgr_f2 = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_F2,
    .replacement     = KC_BRIU,
};

// AltGR + F3 -> Windows+P (Projection mode)
const key_override_t altgr_f3 = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_F3,
    .replacement     = LGUI(KC_P),
};

// AltGR + F4 -> Search bar
const key_override_t altgr_f4 = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_F4,
    .replacement     = KC_WSCH,
};

// AltGR + F5 -> Previous track
const key_override_t altgr_f5 = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_F5,
    .replacement     = KC_MPRV,
};

// AltGR + F6 -> Pause/play
const key_override_t altgr_f6 = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_F6,
    .replacement     = KC_MPLY,
};

// AltGR + F7 -> Next track
const key_override_t altgr_f7 = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_F7,
    .replacement     = KC_MNXT,
};

// AltGR + F8 -> Mute audio
const key_override_t altgr_f8 = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_F8,
    .replacement     = KC_MUTE,
};

// AltGR + F9 -> Volume down
const key_override_t altgr_f9 = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_F9,
    .replacement     = KC_VOLD,
};

// AltGR + F10 -> Volume up
const key_override_t altgr_f10 = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_F10,
    .replacement     = KC_VOLU,
};

// AltGR + F11 -> Backlight brightness down
// Implemented via a custom action because RM_VALD wasn't triggering in this setup
const key_override_t altgr_f11 = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_F11,
    .replacement     = KC_NO,
    .custom_action   = keylayer_rgb_val_down,
};

// AltGR + F12 -> Backlight brightness up
// Implemented via a custom action because RM_VALU wasn't triggering in this setup
const key_override_t altgr_f12 = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_F12,
    .replacement     = KC_NO,
    .custom_action   = keylayer_rgb_val_up,
};

// AltGR + Print Screen -> F20 (Mute microphone)
const key_override_t altgr_prtscr = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_PSCR,
    .replacement     = KC_F20,
};

// AltGR + Scroll lock -> Eject disk
const key_override_t altgr_scrlck = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_SCRL,
    .replacement     = KC_EJCT,
};

// AltGR + Pause -> Sleep
const key_override_t altgr_pause = {
    .trigger_mods    = MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RALT),
    .trigger         = KC_PAUS,
    .replacement     = KC_SLEP,
};

const key_override_t *key_overrides[] = {
    &altgr_esc,
    &altgr_f1,
    &altgr_f2,
    &altgr_f3,
    &altgr_f4,
    &altgr_f5,
    &altgr_f6,
    &altgr_f7,
    &altgr_f8,
    &altgr_f9,
    &altgr_f10,
    &altgr_f11,
    &altgr_f12,
    &altgr_prtscr,
    &altgr_scrlck,
    &altgr_pause,
};


/*******************************************
             Key Layers Actions
*******************************************/

static bool keylayer_erase_eeprom(bool activated, void *context)
{
    if (activated)
        eeconfig_init();
    return false;
}

static bool keylayer_rgb_val_down(bool activated, void *context)
{
    if (activated)
        rgb_matrix_decrease_val();
    return false;
}

static bool keylayer_rgb_val_up(bool activated, void *context)
{
    if (activated)
        rgb_matrix_increase_val();
    return false;
}


/*******************************************
              Status LED Logic
*******************************************/

static void enable_ledindicator(uint32_t index)
{
    rgb_t rgb;
    hsv_t hsv;
    uint8_t mode = rgb_matrix_get_mode();
    if (mode == RGB_MATRIX_COMMUNITY_MODULE_OPENRGB_DIRECT)
    {
        rgb.r = g_openrgb_direct_mode_colors[index].r;
        rgb.g = g_openrgb_direct_mode_colors[index].g;
        rgb.b = g_openrgb_direct_mode_colors[index].b;
        hsv = rgb_to_hsv(rgb);
    }
    else
        hsv = rgb_matrix_get_hsv();
    hsv.v = RGB_MATRIX_MAXIMUM_BRIGHTNESS;
    rgb = hsv_to_rgb(hsv);
    rgb_matrix_set_color(index, rgb.r, rgb.g, rgb.b);
}

bool rgb_matrix_indicators_user(void)
{
    // Scroll Lock
    if (host_keyboard_led_state().scroll_lock)
        enable_ledindicator(LEDINDEX_SCROLLLOCK);
    else
        rgb_matrix_set_color(LEDINDEX_SCROLLLOCK, 0, 0, 0);

    // Caps Lock
    if (host_keyboard_led_state().caps_lock)
        enable_ledindicator(LEDINDEX_CAPSLOCK);
    else
        rgb_matrix_set_color(LEDINDEX_CAPSLOCK, 0, 0, 0);

    // Num Lock
    if (host_keyboard_led_state().num_lock)
        enable_ledindicator(LEDINDEX_NUMLOCK);
    else
        rgb_matrix_set_color(LEDINDEX_NUMLOCK, 0, 0, 0);

    // Return true to continue running the keyboard-level callback
    return true;
}

bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max)
{
    if (!rgb_matrix_indicators_advanced_user(led_min, led_max))
        return false;

    #ifdef LIMIT_DIRECTMODE_BRIGHTNESS
        uint8_t mode = rgb_matrix_get_mode();
        if (mode == RGB_MATRIX_COMMUNITY_MODULE_OPENRGB_DIRECT)
        {
            for (uint8_t i=led_min; i<led_max; i++)
            {
                rgb_t rgb = {
                    g_openrgb_direct_mode_colors[i].r,
                    g_openrgb_direct_mode_colors[i].g,
                    g_openrgb_direct_mode_colors[i].b,
                };
                hsv_t hsv = rgb_to_hsv(rgb);
                hsv.v = (((uint16_t)hsv.v) * RGB_MATRIX_MAXIMUM_BRIGHTNESS) / 255;
                rgb = hsv_to_rgb(hsv);
                rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
            }
        }
    #endif

    return true;
}


/*******************************************
              Helper Functions
*******************************************/

/*
    QMK uses the CIE1931 curve to calculate the HSV values as per our perception.
    The CIE1931 implementation uses a table that was generated via Python code,
    which is available from this (now dead) website (use Web Archive):
    http://jared.geek.nz/2013/feb/linear-led-pwm

    I took that code and inverted the logic. This is the code used to generate
    the inverted table:

    INPUT_SIZE = 255
    OUTPUT_SIZE = 255
    INT_TYPE = 'const unsigned char'

    def cie1931(L):
        L = L*100.0
        if L <= 8:
            return (L/902.3)
        else:
            return ((L+16.0)/116.0)**3


    x = range(INPUT_SIZE + 1)
    cie = [
        round(cie1931(float(L) / INPUT_SIZE) * OUTPUT_SIZE)
        for L in x
    ]

    inverse = []
    for Y in range(OUTPUT_SIZE + 1):
        best_x = 0
        best_error = abs(cie[0] - Y)

        for X in range(1, INPUT_SIZE + 1):
            error = abs(cie[X] - Y)

            if error < best_error:
                best_x = X
                best_error = error

        inverse.append(best_x)

    f = open('cie1931_inverse.h', 'w')
    f.write('// Inverse CIE1931 correction table\n')
    f.write('// Automatically generated\n\n')

    f.write('%s cie_inverse[%d] = {\n' % (INT_TYPE, OUTPUT_SIZE + 1))
    f.write('\t')
    for i, value in enumerate(inverse):
        f.write('%d, ' % value)
        if i % 10 == 9:
            f.write('\n\t')

    f.write('\n};\n\n')
    f.close()
*/
const unsigned char CIE1931_CURVE_INVERSE[256] PROGMEM = {
    0,   5,   14,  23,  31,  37,  42,  47,  51,  55, 
    58,  62,  65,  68,  71,  73,  76,  78,  81,  83, 
    85,  87,  89,  91,  93,  95,  97,  99,  100, 102, 
    104, 105, 107, 109, 110, 112, 113, 114, 116, 117, 
    119, 120, 121, 122, 124, 125, 126, 127, 129, 130, 
    131, 132, 133, 134, 135, 137, 138, 139, 140, 141, 
    142, 143, 144, 145, 146, 147, 148, 149, 150, 150, 
    151, 152, 153, 154, 155, 156, 157, 158, 158, 159, 
    160, 161, 162, 163, 163, 164, 165, 166, 167, 167, 
    168, 169, 170, 170, 171, 172, 173, 173, 174, 175, 
    176, 176, 177, 178, 178, 179, 180, 180, 181, 182, 
    183, 183, 184, 185, 185, 186, 187, 187, 188, 188, 
    189, 190, 190, 191, 192, 192, 193, 193, 194, 195, 
    195, 196, 197, 197, 198, 198, 199, 199, 200, 201, 
    201, 202, 202, 203, 203, 204, 205, 205, 206, 206, 
    207, 207, 208, 208, 209, 210, 210, 211, 211, 212, 
    212, 213, 213, 214, 214, 215, 215, 216, 216, 217, 
    217, 218, 218, 219, 219, 220, 220, 221, 221, 222, 
    222, 223, 223, 224, 224, 225, 225, 226, 226, 227, 
    227, 228, 228, 229, 229, 230, 230, 230, 231, 231, 
    232, 232, 233, 233, 234, 234, 235, 235, 235, 236, 
    236, 237, 237, 238, 238, 238, 239, 239, 240, 240, 
    241, 241, 242, 242, 242, 243, 243, 244, 244, 244, 
    245, 245, 246, 246, 247, 247, 247, 248, 248, 249, 
    249, 249, 250, 250, 251, 251, 251, 252, 252, 253, 
    253, 253, 254, 254, 255, 255, 
};

static hsv_t rgb_to_hsv(rgb_t rgb)
{
    hsv_t   hsv;
    uint8_t max, min;
    uint16_t delta;

    max   = MAX(rgb.r, MAX(rgb.g, rgb.b));
    min   = MIN(rgb.r, MIN(rgb.g, rgb.b));
    delta = max - min;

    // Undo the CIE1931 brightness curve.
    hsv.v = pgm_read_byte(&CIE1931_CURVE_INVERSE[max]);

    // Grayscale
    if (delta == 0)
    {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    // Saturation
    hsv.s = ((uint16_t)delta*255)/max;

    // Hue
    if (max == rgb.r)
    {
        int16_t h = (((int16_t)rgb.g - rgb.b)*85)/(delta*2);
        if (h < 0)
            h += 256;
        hsv.h = h;
    } 
    else if (max == rgb.g)
        hsv.h = 85 + (((int16_t)rgb.b - rgb.r)*85)/(delta*2);
    else
        hsv.h = 171 + (((int16_t)rgb.r - rgb.g)*85)/(delta*2);

    return hsv;
}