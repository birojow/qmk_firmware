// Copyright 2024 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "keymap_brazilian_abnt2.h"
#include "os_detection.h"
#include <time.h>

enum custom_keycodes {
    TESTE = SAFE_RANGE,
};

void waitFor (unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case TESTE:
        if (record->event.pressed) {
            uint16_t codes[] = {KC_C, KC_L, KC_A, KC_S,KC_S, KC_SPACE, KC_P, KC_I, KC_T}; 
            for (int i = 0; sizeof(codes); i++) {
                tap_code(codes[i]);
                waitFor(1);
            }
        }
        break;
    }
    return true;
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_1, KC_2, KC_3, KC_4, KC_5, KC_6,                     KC_7, KC_8, KC_9, KC_0, KC_MINUS, KC_EQUAL,
        KC_ESC, KC_Q, KC_W, KC_E, KC_R, KC_T,                   KC_Y, KC_U, KC_I, KC_O, KC_P, BR_LBRC,
        BR_CCED, LGUI_T(KC_A), LALT_T(KC_S), LCTL_T(KC_D), LSFT_T(KC_F), KC_G,         KC_H, RSFT_T(KC_J), RCTL_T(KC_K), RALT_T(KC_L), RGUI_T(KC_SLSH), BR_QUOT,
        KC_NUBS, KC_Z, KC_X, KC_C, KC_V, KC_B,                  KC_N, KC_M, KC_COMM, KC_DOT, BR_SLSH, KC_BSLS, 
            TG(1), LT(1, KC_TAB), ALL_T(KC_SPC),                    ALL_T(KC_ENT), LT(2, KC_BSPC), TG(2)),
    [1] = LAYOUT(
        KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6,               KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_BRIU,   KC_HOME, KC_PGDN, KC_PGUP, KC_END, BR_ACUT, KC_VOLU,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_BRID,   KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, BR_TILD, KC_VOLD,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_PSCR,   KC_DEL, KC_MPRV, KC_MPLY, KC_MNXT, KC_INS, KC_MUTE,
            KC_TRNS, KC_TRNS, KC_TRNS,                              KC_TRNS, KC_TRNS, KC_TRNS),
    [2] = LAYOUT(
        TESTE, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
            KC_TRNS, KC_TRNS, KC_TRNS,                              KC_TRNS, KC_TRNS, KC_TRNS)
};

