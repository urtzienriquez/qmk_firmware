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

// Spanish characters are sent as X11 Compose sequences, with KC_APP as Multi_key.
// Host setup: setxkbmap us -option lv3:ralt_switch,compose:menu (and QT_IM_MODULE=compose for Qt apps)
enum custom_keycodes {
    DEGREE = SAFE_RANGE,
    INV_EXCL,
    INV_QUES,
    ACCENT,
};

// ACCENT arms an accent for the next key:
// ACCENT + a/e/i/o/u = á é í ó ú, ACCENT + n = ñ, ACCENT + c = ç, ACCENT ACCENT + u = ü, ACCENT + ! / ? = ¡ ¿
// Shift+ACCENT + a/o = ã õ, Shift+ACCENT Shift+ACCENT + a/e/o = â ê ô
enum accent_state { ACC_NONE, ACC_ACUTE, ACC_DIAERESIS, ACC_TILDE, ACC_CIRCUMFLEX };
static enum accent_state accent = ACC_NONE;

const uint16_t PROGMEM caps_combo[] = {KC_J, KC_F, COMBO_END};
const uint16_t PROGMEM esc_combo[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM tab_combo[] = {KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM arrows_combo[] = {KC_ENT, MT(MOD_LSFT, KC_SPC), COMBO_END};
const uint16_t PROGMEM mouse_combo[] = {OSL(_NUMBERS), OSL(_SYMBOLS), COMBO_END};
const uint16_t PROGMEM locknum_combo[] = {OSL(_NUMBERS), MT(MOD_LSFT, KC_SPC), COMBO_END};
const uint16_t PROGMEM locksym_combo[] = {KC_ENT, OSL(_SYMBOLS), COMBO_END};
const uint16_t PROGMEM function_combo[] = {MO(_MODIFIERS), MT(MOD_LSFT, KC_SPC), COMBO_END};

combo_t key_combos[] = {
    COMBO(caps_combo, KC_CAPS),
    COMBO(esc_combo, KC_ESC),
    COMBO(tab_combo, KC_TAB),
    COMBO(arrows_combo, MO(_ARROWS)),
    COMBO(mouse_combo, MO(_MOUSE)),
    COMBO(locknum_combo, TO(_NUMBERS)),
    COMBO(locksym_combo, TO(_SYMBOLS)),
    COMBO(function_combo, MO(_FUNCTION)),
};

// Send <Multi_key> a b [c]. Mods are cleared so a held shift can't alter the sequence;
// shift is re-applied only to the last key. Caps Lock is handled by X itself.
static void compose_tap(uint16_t a, uint16_t b, uint16_t c, bool shift_last) {
    const uint8_t mods = get_mods();
    const uint8_t osm  = get_oneshot_mods();
    clear_mods();
    clear_oneshot_mods();
    send_keyboard_report();

    tap_code(KC_APP);
    tap_code16(a);
    if (c == KC_NO) {
        tap_code16(shift_last ? LSFT(b) : b);
    } else {
        tap_code16(b);
        tap_code16(shift_last ? LSFT(c) : c);
    }

    set_mods(mods);
    set_oneshot_mods(osm);
    send_keyboard_report();
}

// Keys that shouldn't consume a pending accent (shift held for a capital, layer switch for ! ?).
static bool is_mod_or_layer_key(uint16_t keycode, keyrecord_t *record) {
    if (IS_QK_MOD_TAP(keycode)) return record->tap.count == 0;
    return IS_MODIFIER_KEYCODE(keycode) || IS_QK_ONE_SHOT_LAYER(keycode) || IS_QK_MOMENTARY(keycode) || IS_QK_TO(keycode);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) return true;

    switch (keycode) {
        case ACCENT:
            if ((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT) {
                accent = (accent == ACC_TILDE) ? ACC_CIRCUMFLEX : ACC_TILDE;
            } else {
                accent = (accent == ACC_ACUTE) ? ACC_DIAERESIS : ACC_ACUTE;
            }
            return false;
        case DEGREE: compose_tap(KC_CIRC, KC_UNDS, KC_O, false); return false;
        case INV_EXCL: compose_tap(KC_EXLM, KC_EXLM, KC_NO, false); return false;
        case INV_QUES: compose_tap(KC_QUES, KC_QUES, KC_NO, false); return false;
    }

    if (accent == ACC_NONE || is_mod_or_layer_key(keycode, record)) return true;

    const enum accent_state state = accent;
    const bool shift = (get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
    uint16_t mark;
    switch (state) {
        case ACC_DIAERESIS: mark = KC_DQUO; break;
        case ACC_TILDE: mark = KC_TILD; break;
        case ACC_CIRCUMFLEX: mark = KC_CIRC; break;
        default: mark = KC_QUOT; break;
    }
    accent = ACC_NONE;

    switch (keycode) {
        case KC_A: case KC_E: case KC_I: case KC_O: case KC_U:
            compose_tap(mark, keycode, KC_NO, shift);
            return false;
        case KC_N: compose_tap(KC_TILD, KC_N, KC_NO, shift); return false;
        case KC_C: compose_tap(KC_COMM, KC_C, KC_NO, shift); return false;
        case KC_EXLM: compose_tap(KC_EXLM, KC_EXLM, KC_NO, false); return false;
        case KC_QUES: compose_tap(KC_QUES, KC_QUES, KC_NO, false); return false;
    }
    return true;  // any other key cancels the accent and types normally
}

void keyboard_post_init_user(void) {
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
        KC_LALT, KC_A, KC_S, KC_D, KC_F, KC_G,                         KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT,
        ACCENT, KC_Z, KC_X, KC_C, KC_V, KC_B,                          KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, ACCENT,
                              MO(_MODIFIERS), OSL(_NUMBERS), KC_ENT,    MT(MOD_LSFT, KC_SPC), OSL(_SYMBOLS), MO(_MODIFIERS)
    ),
    [_NUMBERS] = LAYOUT_split_3x6_3(
        XXXXXXX, XXXXXXX, XXXXXXX, KC_LCBR, KC_RCBR, KC_EXLM,                KC_EQUAL, KC_7, KC_8, KC_9, KC_0, _______,
        XXXXXXX, XXXXXXX, XXXXXXX, KC_LPRN, KC_RPRN, XXXXXXX,                KC_MINS, KC_4, KC_5, KC_6, KC_DOT, KC_COMM,
        XXXXXXX, XXXXXXX, XXXXXXX, KC_LBRC, KC_RBRC, XXXXXXX,                KC_PLUS,  KC_1, KC_2, KC_3, KC_SLSH, KC_ASTR,
                                         _______, TO(_BASE), KC_ENT,    MT(MOD_LSFT, KC_SPC), TO(_SYMBOLS), _______
    ),
    [_SYMBOLS] = LAYOUT_split_3x6_3(
        KC_TAB, INV_EXCL, DEGREE, KC_PIPE, KC_AMPR, KC_EXLM,                 KC_EQUAL, KC_LCBR, KC_RCBR, KC_DQUO, KC_ASTR, _______,
        XXXXXXX, INV_QUES, KC_AT, KC_TILD, KC_SLSH, KC_QUES,                 KC_UNDS, KC_LPRN, KC_RPRN, KC_DLR, KC_COLN, KC_GRV,
        XXXXXXX, KC_CIRC, KC_LT, KC_MINS, KC_GT, KC_HASH,                    KC_PLUS, KC_LBRC, KC_RBRC, KC_PERC, KC_BSLS, _______,
                                         _______, TO(_BASE), KC_ENT,    MT(MOD_LSFT, KC_SPC), TO(_NUMBERS), _______
    ),
    [_MODIFIERS] = LAYOUT_split_3x6_3(
        XXXXXXX, XXXXXXX, XXXXXXX, KC_PAGE_UP, XXXXXXX, XXXXXXX,             KC_PAGE_DOWN, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, KC_LSFT, KC_LCTL, KC_LGUI, KC_LALT, XXXXXXX,                XXXXXXX, KC_LALT, KC_LGUI, KC_LCTL, KC_LSFT, XXXXXXX,
        XXXXXXX, XXXXXXX, LCS(KC_X), LCS(KC_C), LCS(KC_V), XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                         XXXXXXX, TO(_BASE), KC_ENT,    MT(MOD_LSFT, KC_SPC), XXXXXXX, XXXXXXX
    ),
    [_ARROWS] = LAYOUT_split_3x6_3(
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, KC_LSFT, KC_LCTL, KC_LGUI, KC_LALT, XXXXXXX,                KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                         XXXXXXX, TO(_BASE), KC_ENT,    MT(MOD_LSFT, KC_SPC), XXXXXXX, XXXXXXX
    ),
    [_MOUSE] = LAYOUT_split_3x6_3(
        XXXXXXX, XXXXXXX, XXXXXXX, MS_WHLU, XXXXXXX, MS_BTN2,                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, MS_ACL0, MS_WHLL, MS_WHLD, MS_WHLR, MS_BTN1,                MS_LEFT, MS_DOWN, MS_UP,   MS_RGHT, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, MS_BTN3,                XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                         XXXXXXX, TO(_BASE), KC_ENT,    MT(MOD_LSFT, KC_SPC), XXXXXXX, XXXXXXX
    ),
    [_FUNCTION] = LAYOUT_split_3x6_3(
        QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                XXXXXXX, KC_F7, KC_F8, KC_F9, KC_F10, XXXXXXX,
        RM_TOGG, RM_HUEU, RM_SATU, RM_VALU, XXXXXXX, XXXXXXX,                XXXXXXX, KC_F4, KC_F5, KC_F6, KC_F11, XXXXXXX,
        RM_NEXT, RM_HUED, RM_SATD, RM_VALD, XXXXXXX, XXXXXXX,                XXXXXXX, KC_F1, KC_F2, KC_F3, KC_F12, XXXXXXX,
                                         XXXXXXX, TO(_BASE), KC_ENT,    MT(MOD_LSFT, KC_SPC), TO(_SYMBOLS), XXXXXXX
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
