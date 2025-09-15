/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include QMK_KEYBOARD_H

enum layers {
    _BASE = 0,
    _NUMBERS,
    _SYMBOLS,
    _MODIFIERS,
    _ARROWS,
    _MOUSE,
    _FUNCTION
};

enum custom_keycodes {
    NTILDE = SAFE_RANGE,
    A_ACUTE,
    E_ACUTE,
    I_ACUTE,
    O_ACUTE,
    U_ACUTE,
    DEGREE,
    INV_EXCL,
    INV_QUES,
};

// Spanish/unicode combos
const uint16_t PROGMEM caps_combo[] = {KC_J, KC_F, COMBO_END};
const uint16_t PROGMEM esc_combo[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM tab_combo[] = {KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM arrows_combo[] = {OSL(_NUMBERS), OSL(_SYMBOLS), COMBO_END};
const uint16_t PROGMEM mouse_combo[] = {KC_ENT, KC_SPC, COMBO_END};
const uint16_t PROGMEM locknum_combo[] = {OSL(_NUMBERS), KC_SPC, COMBO_END};
const uint16_t PROGMEM locksym_combo[] = {OSL(_SYMBOLS), KC_ENT, COMBO_END};
const uint16_t PROGMEM function_combo[] = {KC_TAB, KC_BSPC, COMBO_END};
const uint16_t PROGMEM ntilde_combo[] = {KC_N, KC_F, COMBO_END};
const uint16_t PROGMEM acute_a_combo[] = {KC_A, KC_J, COMBO_END};
const uint16_t PROGMEM acute_e_combo[] = {KC_E, KC_J, COMBO_END};
const uint16_t PROGMEM acute_i_combo[] = {KC_I, KC_F, COMBO_END};
const uint16_t PROGMEM acute_o_combo[] = {KC_O, KC_F, COMBO_END};
const uint16_t PROGMEM acute_u_combo[] = {KC_U, KC_F, COMBO_END};

combo_t key_combos[] = {
    COMBO(caps_combo, KC_CAPS),
    COMBO(esc_combo, KC_ESC),
    COMBO(tab_combo, KC_TAB),
    COMBO(arrows_combo, MO(_ARROWS)),
    COMBO(mouse_combo, MO(_MOUSE)),
    COMBO(locknum_combo, TO(_NUMBERS)),
    COMBO(locksym_combo, TO(_SYMBOLS)),
    COMBO(function_combo, OSL(_FUNCTION)),
    COMBO(ntilde_combo, NTILDE),
    COMBO(acute_a_combo, A_ACUTE),
    COMBO(acute_e_combo, E_ACUTE),
    COMBO(acute_i_combo, I_ACUTE),
    COMBO(acute_o_combo, O_ACUTE),
    COMBO(acute_u_combo, U_ACUTE),
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    // --- Spanish/unicode keys ---
    if (record->event.pressed) {
        bool is_shift = keyboard_report->mods & MOD_MASK_SHIFT;
        bool is_caps  = host_keyboard_led_state().caps_lock;
        bool uppercase = is_shift ^ is_caps;

        switch (keycode) {
            case NTILDE: send_unicode_string(uppercase ? "Ñ" : "ñ"); return false;
            case A_ACUTE: send_unicode_string(uppercase ? "Á" : "á"); return false;
            case E_ACUTE: send_unicode_string(uppercase ? "É" : "é"); return false;
            case I_ACUTE: send_unicode_string(uppercase ? "Í" : "í"); return false;
            case O_ACUTE: send_unicode_string(uppercase ? "Ó" : "ó"); return false;
            case U_ACUTE: send_unicode_string(uppercase ? "Ú" : "ú"); return false;
            case DEGREE: send_unicode_string("º"); return false;
            case INV_EXCL: send_unicode_string("¡"); return false;
            case INV_QUES: send_unicode_string("¿"); return false;
        }
    }

    return true;
}

void keyboard_post_init_user(void) {
    unicode_config.input_mode = UNICODE_MODE_LINUX;
    #ifdef RGB_MATRIX_ENABLE
    rgb_matrix_enable();
    rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
    #endif
}

// --- RGB layer indicator ---
layer_state_t layer_state_set_user(layer_state_t state) {
    #ifdef RGB_MATRIX_ENABLE
    switch (get_highest_layer(state)) {
        case _BASE: rgb_matrix_sethsv_noeeprom(HSV_BLUE); break;
        case _NUMBERS: rgb_matrix_sethsv_noeeprom(HSV_PURPLE); break;
        case _SYMBOLS: rgb_matrix_sethsv_noeeprom(HSV_YELLOW); break;
        case _MODIFIERS: rgb_matrix_sethsv_noeeprom(HSV_GREEN); break;
        case _ARROWS: rgb_matrix_sethsv_noeeprom(HSV_ORANGE); break;
        case _MOUSE: rgb_matrix_sethsv_noeeprom(HSV_CYAN); break;
        case _FUNCTION: rgb_matrix_sethsv_noeeprom(HSV_RED); break;
        default: rgb_matrix_sethsv_noeeprom(HSV_MAGENTA); break;
    }
    #endif
    return state;
}

// --- Keymaps ---
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_split_3x6_3(
        KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T,                                KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSPC,
        OSM(MOD_LALT), KC_A, KC_S, KC_D, KC_F, KC_G,                         KC_H, KC_J, KC_K, KC_L, KC_SCLN, OSM(MOD_LCTL),
        OSM(MOD_LSFT), KC_Z, KC_X, KC_C, KC_V, KC_B,                         KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_QUOT,
                              MO(_MODIFIERS), OSL(_NUMBERS), KC_ENT,   KC_SPC, OSL(_SYMBOLS), MO(_MODIFIERS)
    ),
    [_NUMBERS] = LAYOUT_split_3x6_3(
        XXXXXXX, XXXXXXX, XXXXXXX, KC_LCBR, KC_RCBR, KC_EXLM,                KC_EQUAL, KC_7, KC_8, KC_9, KC_0, XXXXXXX,
        _______, XXXXXXX, XXXXXXX, KC_LPRN, KC_RPRN, OSM(MOD_LSFT),          KC_MINS, KC_4, KC_5, KC_6, KC_DOT, KC_COMM,
        _______, XXXXXXX, XXXXXXX, KC_LBRC, KC_RBRC, XXXXXXX,                KC_PLUS,  KC_1, KC_2, KC_3, KC_SLSH, KC_ASTR,
                                         _______, TO(_BASE), KC_ENT,   KC_SPC, TO(_SYMBOLS), _______
    ),
    [_SYMBOLS] = LAYOUT_split_3x6_3(
        KC_TAB, INV_EXCL, DEGREE, KC_PIPE, KC_AMPR, KC_EXLM,                 KC_EQUAL, KC_LCBR, KC_RCBR, KC_DQUO, KC_ASTR, KC_BSPC,
        _______, INV_QUES, KC_AT, KC_TILD, KC_SLSH, KC_QUES,                 KC_UNDS, KC_LPRN, KC_RPRN, KC_DLR, KC_COLN, _______,
        _______, KC_CIRC, KC_LT, KC_GT, KC_HASH, KC_MINS,                    KC_PLUS, KC_LBRC, KC_RBRC, KC_PERC, KC_BSLS, KC_GRV,
                                         _______, TO(_BASE), KC_ENT,   KC_SPC, TO(_NUMBERS), _______
    ),
    [_MODIFIERS] = LAYOUT_split_3x6_3(
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        _______, KC_LSFT, KC_LCTL, KC_LGUI, KC_LALT, XXXXXXX,                XXXXXXX, KC_LALT, KC_LGUI, KC_LCTL, KC_LSFT, _______,
        _______, XXXXXXX, LCS(KC_X), LCS(KC_C), LCS(KC_V), XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                         XXXXXXX, TO(_BASE), KC_ENT,   KC_SPC, XXXXXXX, XXXXXXX
    ),
    [_ARROWS] = LAYOUT_split_3x6_3(
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                         XXXXXXX, TO(_BASE), KC_ENT,   KC_SPC, XXXXXXX, XXXXXXX
    ),
    [_MOUSE] = LAYOUT_split_3x6_3(
        XXXXXXX, XXXXXXX, XXXXXXX, MS_WHLU, XXXXXXX, MS_BTN2,                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, MS_ACL0, MS_WHLL, MS_WHLD, MS_WHLR, MS_BTN1,                MS_LEFT, MS_DOWN, MS_UP,   MS_RGHT, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, MS_BTN3,                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                         XXXXXXX, TO(_BASE), KC_ENT,   KC_SPC, XXXXXXX, XXXXXXX
    ),
    [_FUNCTION] = LAYOUT_split_3x6_3(
        QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6,
        RM_TOGG, RM_HUEU, RM_SATU, RM_VALU, XXXXXXX, XXXXXXX,                KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
        RM_NEXT, RM_HUED, RM_SATD, RM_VALD, XXXXXXX, XXXXXXX,                KC_PSCR, KC_SCRL, KC_PAUS, KC_INS, KC_DEL, XXXXXXX,
                                         XXXXXXX, TO(_BASE), KC_ENT,   KC_SPC, TO(_SYMBOLS), XXXXXXX
    )
};

// Encoder map for Corne with 4 encoders (your original configuration)
#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
  [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
  [1] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
  [2] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
  [3] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT), ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_RGHT, KC_LEFT), },
};
#endif

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (is_keyboard_master()) {
        return OLED_ROTATION_270; // Rotate master 90 degrees for vertical display
    } else {
        return OLED_ROTATION_0;  // Rotate slave 90 degrees (opposite direction)
    }
}

static void oled_render_vertical_layers(void) {
    uint8_t current_layer = get_highest_layer(layer_state);
    oled_write_P(PSTR("LAYER"), false);
    oled_advance_page(true);
    // Layer 0 - Base
    if (current_layer == 0) {
        oled_write_P(PSTR(">base"), true);  // White background, black text
    } else {
        oled_write_P(PSTR(" base"), false);
    }
    // Layer 1 - Numbers
    if (current_layer == 1) {
        oled_write_P(PSTR(">nums"), true);  // White background, black text
    } else {
        oled_write_P(PSTR(" nums"), false);
    }
    // Layer 2 - Symbols
    if (current_layer == 2) {
        oled_write_P(PSTR(">syms"), true);  // White background, black text
    } else {
        oled_write_P(PSTR(" syms"), false);
    }
    if (current_layer == 3) {
        oled_write_P(PSTR(">mods"), true);  // White background, black text
    } else {
        oled_write_P(PSTR(" mods"), false);
    }
    // Layer 1 - Numbers
    if (current_layer == 4) {
        oled_write_P(PSTR(">arws"), true);  // White background, black text
    } else {
        oled_write_P(PSTR(" arws"), false);
    }
    // Layer 1 - Numbers
    if (current_layer == 5) {
        oled_write_P(PSTR(">mous"), true);  // White background, black text
    } else {
        oled_write_P(PSTR(" mous"), false);
    }
    // Layer 3 - Function
    if (current_layer == 6) {
        oled_write_P(PSTR(">func"), true);  // White background, black text
    } else {
        oled_write_P(PSTR(" func"), false);
    }
    oled_advance_page(true);
}

static void oled_render_mods_status(void) {
    uint8_t mods = get_mods() | get_oneshot_mods();
    oled_write_P(PSTR("MODS:"), false);
    // Display SCAG format (Shift, Ctrl, Gui, Alt)
    char mods_str[5] = "----";
    mods_str[4] = '\0';  // Null terminate the string
    if (mods & MOD_MASK_SHIFT) {
        mods_str[0] = 'S';
    }
    if (mods & MOD_MASK_CTRL) {
        mods_str[1] = 'C';
    }
    if (mods & MOD_MASK_GUI) {
        mods_str[2] = 'G';
    }
    if (mods & MOD_MASK_ALT) {
        mods_str[3] = 'A';
    }
    oled_write(mods_str, false);
    oled_advance_page(true);
    oled_advance_page(true);
}

static void oled_render_caps_status(void) {
    bool caps = host_keyboard_led_state().caps_lock;
    if (caps) {
        oled_write_P(PSTR("CAPS "), true);  // White background when active
    } else {
        oled_write_P(PSTR("caps "), false);
    }
    oled_advance_page(true);
    oled_advance_page(true);
}

static void oled_render_master_info(void) {
    oled_render_vertical_layers();
    oled_render_mods_status();
    oled_render_caps_status();
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        oled_render_master_info();
        return false;  // We handled the master display
    } else {
        return true;   // Let QMK handle the slave display (default implementation)
    }
}
#endif // OLED_ENABLE

