// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "openrgb.h"
#include "eeconfig.h"
#include "ws2812.h"
#include "helper.h"


/*******************************************
                   Macros
*******************************************/

// LED Indices
#define LEDINDEX_NUMLOCK    2
#define LEDINDEX_CAPSLOCK   1
#define LEDINDEX_SCROLLLOCK 0


/*******************************************
           Function declarations
*******************************************/

static bool keylayer_erase_eeprom(bool activated, void *context);
static bool keylayer_rgb_val_down(bool activated, void *context);
static bool keylayer_rgb_val_up(bool activated, void *context);
static bool keylayer_save_directmode(bool activated, void *context);


/*******************************************
               EEPROM Globals
*******************************************/

uint8_t saved_directrgb[RGB_MATRIX_LED_COUNT][3];


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

// CTRL + AltGR + ESC -> Save Direct Mode to EEPROM
// Implemented via a custom action because of complex logic
const key_override_t ctrlaltgr_esc = {
    .trigger_mods    = MOD_BIT(KC_RCTL) | MOD_BIT(KC_RALT),
    .layers          = ~0,
    .suppressed_mods = MOD_BIT(KC_RCTL) | MOD_BIT(KC_RALT),
    .trigger         = KC_ESC,
    .replacement     = KC_NO,
    .custom_action   = keylayer_save_directmode,
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
    &ctrlaltgr_esc,
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
          Keyboard Initialization
*******************************************/

/*==============================
    keyboard_post_init_user

    Called when the keyboard initializes
==============================*/

void keyboard_post_init_user(void)
{
    // If the last mode we were using was OpenRGB's direct mode, load the per-key data from EEPROM
    if (rgb_matrix_get_mode() == RGB_MATRIX_CUSTOM_OPENRGB_SAVE_DIRECT)
        eeconfig_read_user_datablock(saved_directrgb, 0, sizeof(saved_directrgb));
}


/*******************************************
             Key Layers Actions
*******************************************/

/*==============================
    keylayer_erase_eeprom

    Callback function for AltGR + Esc

    Erases the EEPROM

    @param Whether the key was activated or deactivated
    @param Context passed into the custom action (unused)
    @return If you return false, the replacement key is not registered/unregistered as it would normally. 
            Return true to register and unregister the override normally.
==============================*/

static bool keylayer_erase_eeprom(bool activated, void *context)
{
    if (activated)
        eeconfig_init();
    return false;
}


/*==============================
    keylayer_rgb_val_down

    Callback function for AltGR + F11

    Lowers the backlight brightness

    @param Whether the key was activated or deactivated
    @param Context passed into the custom action (unused)
    @return If you return false, the replacement key is not registered/unregistered as it would normally. 
            Return true to register and unregister the override normally.
==============================*/

static bool keylayer_rgb_val_down(bool activated, void *context)
{
    if (activated)
        rgb_matrix_decrease_val();
    return false;
}


/*==============================
    keylayer_rgb_val_up

    Callback function for AltGR + F12

    Increases the backlight brightness

    @param Whether the key was activated or deactivated
    @param Context passed into the custom action (unused)
    @return If you return false, the replacement key is not registered/unregistered as it would normally. 
            Return true to register and unregister the override normally.
==============================*/

static bool keylayer_rgb_val_up(bool activated, void *context)
{
    if (activated)
        rgb_matrix_increase_val();
    return false;
}


/*==============================
    keylayer_save_directmode

    Callback function for Ctrl + AltGR + ESC

    Saves the direct mode per-key RGB data to EEPROM

    @param Whether the key was activated or deactivated
    @param Context passed into the custom action (unused)
    @return If you return false, the replacement key is not registered/unregistered as it would normally. 
            Return true to register and unregister the override normally.
==============================*/

static bool keylayer_save_directmode(bool activated, void *context)
{
    if (activated)
    {
        uint8_t mode = rgb_matrix_get_mode();

        // Only save if the keyboard was in Direct Mode in OpenRGB
        if (mode == RGB_MATRIX_COMMUNITY_MODULE_OPENRGB_DIRECT)
        {
            // Fetch per key RGB data
            for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
                saved_directrgb[i][0] = g_openrgb_direct_mode_colors[i].r;
                saved_directrgb[i][1] = g_openrgb_direct_mode_colors[i].g;
                saved_directrgb[i][2] = g_openrgb_direct_mode_colors[i].b;
            }

            // Save it to a user data block in EEPROM
            eeconfig_update_user_datablock(
                saved_directrgb,
                0,
                sizeof(saved_directrgb)
            );

            // Set the effect mode to the custom direct mode effect
            rgb_matrix_mode(RGB_MATRIX_CUSTOM_OPENRGB_SAVE_DIRECT);
        }
    }
    return false;
}


/*******************************************
              Status LED Logic
*******************************************/

extern ws2812_led_t ws2812_leds[];


/*==============================
    enable_ledindicator

    Enables a specific status LED.

    @param The index of the LED to modify
==============================*/

static void enable_ledindicator(uint32_t index)
{
    rgb_t rgb;
    hsv_t hsv;
    uint8_t mode = rgb_matrix_get_mode();

    // Get the HSV data depending on the effect that's active
    if (mode == RGB_MATRIX_COMMUNITY_MODULE_OPENRGB_DIRECT) // Using OpenRGB's direct mode LED data
    {
        rgb.r = g_openrgb_direct_mode_colors[index].r;
        rgb.g = g_openrgb_direct_mode_colors[index].g;
        rgb.b = g_openrgb_direct_mode_colors[index].b;
        hsv = rgb_to_hsv(rgb);
    }
    else if (mode == RGB_MATRIX_CUSTOM_OPENRGB_SAVE_DIRECT) // Using the saved direct mode LED data
    {
        rgb.r = saved_directrgb[index][0];
        rgb.g = saved_directrgb[index][1];
        rgb.b = saved_directrgb[index][2];
        hsv = rgb_to_hsv(rgb);
    }
    else // Using the current effect's LED data
    {
        rgb.r = ws2812_leds[index].r;
        rgb.g = ws2812_leds[index].g;
        rgb.b = ws2812_leds[index].b;
        if (rgb.r < 1 && rgb.g < 1 && rgb.b < 1) // At low brightness, RGB values are useless, so fallback to the full keyboard color
            hsv = rgb_matrix_get_hsv();
        else
            hsv = rgb_to_hsv(rgb);
    }

    // Force the key to always be at max brightness
    hsv.v = RGB_MATRIX_MAXIMUM_BRIGHTNESS;

    // Set the indicator color
    rgb = hsv_to_rgb(hsv);
    rgb_matrix_set_color(index, rgb.r, rgb.g, rgb.b);
}


/*==============================
    rgb_matrix_indicators_user

    Handles status indicator LEDS (Caps lock, Scroll lock, Num lock)

    @return Whether to continue running the keyboard-level callback
==============================*/

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


/*==============================
    rgb_matrix_indicators_advanced_kb

    Keyboard-level callback, invoked after current animation frame is 
    rendered but before it is flushed to the LEDs.

    Currently used to limit the LED brightness in direct mode, which
    bypasses the safety limits that were explicitly set by me to
    prevent the LEDs from pulling too much current.

    @param  The index of the first LED in this batch.
    @param  The index of the last LED in this batch.
    @return Currently unused
==============================*/

bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max)
{
    if (!rgb_matrix_indicators_advanced_user(led_min, led_max))
        return false;

    uint8_t mode = rgb_matrix_get_mode();
    if (mode == RGB_MATRIX_COMMUNITY_MODULE_OPENRGB_DIRECT)
    {
        for (uint8_t i=led_min; i<led_max; i++)
        {
            rgb_t rgb = safe_rgb_brightness((rgb_t){
                g_openrgb_direct_mode_colors[i].r,
                g_openrgb_direct_mode_colors[i].g,
                g_openrgb_direct_mode_colors[i].b
            });
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
    }

    return true;
}