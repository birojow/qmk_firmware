// Copyright 2024 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "keymap_brazilian_abnt2.h"
#include "os_detection.h"

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP
} td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

// Tap dances
enum {
    ONE_EXCL,
    COMM_LTEQ,
    DOT_GTEQ,
    EQUAL_EQEQEQ,
    BR_SLSH_QMARK,
    QUOT_DQUOT_NUBS
};

td_state_t cur_dance(tap_dance_state_t *state);

// tap = 1! double tap = != hold = !==
void one_excl_finished(tap_dance_state_t *state, void *user_data);
void one_excl_reset(tap_dance_state_t *state, void *user_data);

// tap = ,< hold = <=
void comm_lteq_finished(tap_dance_state_t *state, void *user_data);
void comm_lteq_reset(tap_dance_state_t *state, void *user_data);

// tap = .> hold = >=
void dot_gteq_finished(tap_dance_state_t *state, void *user_data);
void dot_gteq_reset(tap_dance_state_t *state, void *user_data);

// tap = =+ hold = ===
void equal_eqeqeq_finished(tap_dance_state_t *state, void *user_data);
void equal_eqeqeq_reset(tap_dance_state_t *state, void *user_data);

// tap = /? hold = ?.let {
void br_slsh_qmark_finished(tap_dance_state_t *state, void *user_data);
void br_slsh_qmark_reset(tap_dance_state_t *state, void *user_data);

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        TD(ONE_EXCL), KC_2, KC_3, KC_4, KC_5, KC_6,             KC_7, KC_8, KC_9, KC_0, KC_MINUS, TD(EQUAL_EQEQEQ),
        KC_ESC, KC_Q, KC_W, KC_E, KC_R, KC_T,                   KC_Y, KC_U, KC_I, KC_O, KC_P, BR_LBRC,
        BR_CCED, LCTL_T(KC_A), LALT_T(KC_S), LGUI_T(KC_D), LSFT_T(KC_F), KC_G,         KC_H, RSFT_T(KC_J), RGUI_T(KC_K), RALT_T(KC_L), RCTL_T(KC_SLSH), KC_NUBS,
        BR_QUOT, KC_Z, KC_X, KC_C, KC_V, KC_B,                  KC_N, KC_M, TD(COMM_LTEQ), TD(DOT_GTEQ), TD(BR_SLSH_QMARK), KC_BSLS, 
            TG(2), LT(2, KC_TAB), ALL_T(KC_SPC),                            ALL_T(KC_ENT), LT(3, KC_BSPC), TG(3)),
    [1] = LAYOUT(
        TD(ONE_EXCL), KC_2, KC_3, KC_4, KC_5, KC_6,             KC_7, KC_8, KC_9, KC_0, KC_MINUS, TD(EQUAL_EQEQEQ),
        KC_ESC, KC_Q, KC_W, KC_E, KC_R, KC_T,                   KC_Y, KC_U, KC_I, KC_O, KC_P, BR_LBRC,
        BR_CCED, LGUI_T(KC_A), LALT_T(KC_S), LCTL_T(KC_D), LSFT_T(KC_F), KC_G,         KC_H, RSFT_T(KC_J), RCTL_T(KC_K), RALT_T(KC_L), RGUI_T(KC_SLSH), BR_QUOT,
        KC_NUBS, KC_Z, KC_X, KC_C, KC_V, KC_B,                  KC_N, KC_M, TD(COMM_LTEQ), TD(DOT_GTEQ), TD(BR_SLSH_QMARK), KC_BSLS, 
            TG(2), LT(2, KC_TAB), ALL_T(KC_SPC),                            ALL_T(KC_ENT), LT(3, KC_BSPC), TG(3)),
    [2] = LAYOUT(
        KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6,               KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
        KC_TRNS, DF(0), DF(1), KC_TRNS, KC_TRNS, KC_BRIU,       KC_HOME, KC_PGDN, KC_PGUP, KC_END, BR_ACUT, KC_VOLU,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_BRID,   KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, BR_TILD, KC_VOLD,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_PSCR,   KC_DEL, KC_MPRV, KC_MPLY, KC_MNXT, KC_INS, KC_MUTE,
            KC_TRNS, KC_TRNS, KC_TRNS,                              KC_TRNS, KC_TRNS, KC_TRNS),
    [3] = LAYOUT(
        KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6,               KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
        KC_TRNS, DM_REC1, DM_REC2, DM_PLY1, DM_PLY2, KC_TRNS,   MS_WHLL, MS_WHLD, MS_WHLU, MS_WHLR, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,   MS_LEFT, MS_DOWN, MS_UP, MS_RGHT, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS, MS_BTN1, MS_BTN2, KC_TRNS, KC_TRNS, KC_TRNS,
            KC_TRNS, KC_TRNS, KC_TRNS,                              KC_TRNS, KC_TRNS, KC_TRNS)
};

td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (!state->pressed) return TD_SINGLE_TAP;
        else return TD_SINGLE_HOLD;
    } else if (state->count == 2) {
        return TD_DOUBLE_TAP;
    } else {
        return TD_UNKNOWN;
    }
}

// 1! / != / !==
static td_tap_t one_excl_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void one_excl_finished(tap_dance_state_t *state, void *user_data) {
    one_excl_state.state = cur_dance(state);
    switch (one_excl_state.state) {
        case TD_SINGLE_TAP:
            if (get_mods() & MOD_MASK_SHIFT) {
                tap_code16(KC_EXLM);
            } else {
                tap_code(KC_1);
            }
            break;
        case TD_SINGLE_HOLD:
            send_string("!==");
            break;
        case TD_DOUBLE_TAP:
            send_string("!=");
            break;
        default:
            break;
    }
}

void one_excl_reset(tap_dance_state_t *state, void *user_data) {
    one_excl_state.state = TD_NONE;
}

// ,< / <=
static td_tap_t comm_lteq_state = {
    .is_press_action = true,
    .state = TD_NONE
};

// Tap: KC_COMM, Hold: <=
void comm_lteq_finished(tap_dance_state_t *state, void *user_data) {
    comm_lteq_state.state = cur_dance(state);
    switch (comm_lteq_state.state) {
        case TD_SINGLE_TAP:
            tap_code(KC_COMM);
            break;
        case TD_SINGLE_HOLD:
            send_string("<=");
            break;
        default:
            break;
    }
}

void comm_lteq_reset(tap_dance_state_t *state, void *user_data) {
    comm_lteq_state.state = TD_NONE;
}

// .> / >=
static td_tap_t dot_gteq_state = {
    .is_press_action = true,
    .state = TD_NONE
};

// Tap: KC_DOT, Hold: >=
void dot_gteq_finished(tap_dance_state_t *state, void *user_data) {
    dot_gteq_state.state = cur_dance(state);
    switch (dot_gteq_state.state) {
        case TD_SINGLE_TAP:
            tap_code(KC_DOT);
            break;
        case TD_SINGLE_HOLD:
            send_string(">=");
            break;
        default:
            break;
    }
}

void dot_gteq_reset(tap_dance_state_t *state, void *user_data) {
    dot_gteq_state.state = TD_NONE;
}

// = / ===
static td_tap_t equal_eqeqeq_state = {
    .is_press_action = true,
    .state = TD_NONE
};

// Tap: KC_EQUAL, Hold: ===
void equal_eqeqeq_finished(tap_dance_state_t *state, void *user_data) {
    equal_eqeqeq_state.state = cur_dance(state);
    switch (equal_eqeqeq_state.state) {
        case TD_SINGLE_TAP:
            tap_code(KC_EQUAL);
            break;
        case TD_SINGLE_HOLD:
            send_string("===");
            break;
        default:
            break;
    }
}

void equal_eqeqeq_reset(tap_dance_state_t *state, void *user_data) {
    equal_eqeqeq_state.state = TD_NONE;
}

// /? / ?.let {
    static td_tap_t br_slsh_qmark_state = {
        .is_press_action = true,
        .state = TD_NONE
    };

// Tap: BR_SLSH, Hold: ?.let {
void br_slsh_qmark_finished(tap_dance_state_t *state, void *user_data) {
    br_slsh_qmark_state.state = cur_dance(state);
    switch (br_slsh_qmark_state.state) {
        case TD_SINGLE_TAP:
            tap_code(BR_SLSH);
            break;
        case TD_DOUBLE_TAP:
            register_code(KC_LSFT);
            tap_code(BR_SLSH);
            tap_code(KC_SLSH);
            unregister_code(KC_LSFT);
        case TD_SINGLE_HOLD:
            register_code(KC_LSFT);
            tap_code(BR_SLSH);
            unregister_code(KC_LSFT);
            send_string(".let ");
            register_code(KC_LSFT);
            tap_code(BR_LBRC);
            unregister_code(KC_LSFT);
            break;
        default:
            break;
    }
}

void br_slsh_qmark_reset(tap_dance_state_t *state, void *user_data) {
    br_slsh_qmark_state.state = TD_NONE;
}

tap_dance_action_t tap_dance_actions[] = {
    [ONE_EXCL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, one_excl_finished, one_excl_reset),
    [COMM_LTEQ] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, comm_lteq_finished, comm_lteq_reset),
    [DOT_GTEQ] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dot_gteq_finished, dot_gteq_reset),
    [EQUAL_EQEQEQ] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, equal_eqeqeq_finished, equal_eqeqeq_reset),
    [BR_SLSH_QMARK] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, br_slsh_qmark_finished, br_slsh_qmark_reset)
};
