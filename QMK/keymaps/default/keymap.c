// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H


/*******************************************
           Function declarations
*******************************************/

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

// CTRL + Shift + ESC -> Clear EEPROM
// Implemented via a custom action because EE_CLR wasn't triggering in this setup
const key_override_t ctrl_shift_esc = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_ESC,
    .replacement     = KC_NO,
    .custom_action   = keylayer_erase_eeprom,
};

// CTRL + Shift + F1 -> Screen brightness down
const key_override_t ctrl_shift_f1 = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_F1,
    .replacement     = KC_BRID,
};

// CTRL + Shift + F2 -> Screen brightness up
const key_override_t ctrl_shift_f2 = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_F2,
    .replacement     = KC_BRIU,
};

// CTRL + Shift + F3 -> Windows+P (Projection mode)
const key_override_t ctrl_shift_f3 = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_F3,
    .replacement     = LGUI(KC_P),
};

// CTRL + Shift + F4 -> Search bar
const key_override_t ctrl_shift_f4 = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_F4,
    .replacement     = KC_WSCH,
};

// CTRL + Shift + F5 -> Previous track
const key_override_t ctrl_shift_f5 = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_F5,
    .replacement     = KC_MPRV,
};

// CTRL + Shift + F6 -> Pause/play
const key_override_t ctrl_shift_f6 = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_F6,
    .replacement     = KC_MPLY,
};

// CTRL + Shift + F7 -> Next track
const key_override_t ctrl_shift_f7 = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_F7,
    .replacement     = KC_MNXT,
};

// CTRL + Shift + F8 -> Mute audio
const key_override_t ctrl_shift_f8 = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_F8,
    .replacement     = KC_MUTE,
};

// CTRL + Shift + F9 -> Volume down
const key_override_t ctrl_shift_f9 = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_F9,
    .replacement     = KC_VOLD,
};

// CTRL + Shift + F10 -> Volume up
const key_override_t ctrl_shift_f10 = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_F10,
    .replacement     = KC_VOLU,
};

// CTRL + Shift + F11 -> Backlight brightness down
// Implemented via a custom action because RM_VALD wasn't triggering in this setup
const key_override_t ctrl_shift_f11 = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_F11,
    .replacement     = KC_NO,
    .custom_action   = keylayer_rgb_val_down,
};

// CTRL + Shift + F12 -> Backlight brightness up
// Implemented via a custom action because RM_VALU wasn't triggering in this setup
const key_override_t ctrl_shift_f12 = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_F12,
    .replacement     = KC_NO,
    .custom_action   = keylayer_rgb_val_up,
};

// CTRL + Shift + Print Screen -> F20 (Mute microphone)
const key_override_t ctrl_shift_prtscr = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_PSCR,
    .replacement     = KC_F20,
};

// CTRL + Shift + Scroll lock -> Eject disk
const key_override_t ctrl_shift_scrlck = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_SCRL,
    .replacement     = KC_EJCT,
};

// CTRL + Shift + Pause -> Sleep
const key_override_t ctrl_shift_pause = {
    .trigger_mods    = MOD_MASK_CS,
    .layers          = ~0,
    .suppressed_mods = MOD_MASK_CS,
    .trigger         = KC_PAUS,
    .replacement     = KC_SLEP,
};

const key_override_t *key_overrides[] = {
    &ctrl_shift_esc,
    &ctrl_shift_f1,
    &ctrl_shift_f2,
    &ctrl_shift_f3,
    &ctrl_shift_f4,
    &ctrl_shift_f5,
    &ctrl_shift_f6,
    &ctrl_shift_f7,
    &ctrl_shift_f8,
    &ctrl_shift_f9,
    &ctrl_shift_f10,
    &ctrl_shift_f11,
    &ctrl_shift_f12,
    &ctrl_shift_prtscr,
    &ctrl_shift_scrlck,
    &ctrl_shift_pause,
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

static void set_led(uint32_t index, hsv_t hsv)
{
    hsv.v = RGB_MATRIX_MAXIMUM_BRIGHTNESS;
    rgb_t rgb = hsv_to_rgb(hsv);
    rgb_matrix_set_color(index, rgb.r, rgb.g, rgb.b);
}

bool rgb_matrix_indicators_user(void)
{
    hsv_t hsv = rgb_matrix_get_hsv();

    // Scroll Lock
    if (host_keyboard_led_state().scroll_lock)
        set_led(0, hsv);
    else
        rgb_matrix_set_color(0, 0, 0, 0);

    // Caps Lock
    if (host_keyboard_led_state().caps_lock)
        set_led(1, hsv);
    else
        rgb_matrix_set_color(1, 0, 0, 0);

    // Num Lock
    if (host_keyboard_led_state().num_lock)
        set_led(2, hsv);
    else
        rgb_matrix_set_color(2, 0, 0, 0);

    // Return true to continue running the keyboard-level callback
    return true;
}