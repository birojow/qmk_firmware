// Copyright 2024 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "keymap_brazilian_abnt2.h"
#include "os_detection.h"

char text[]= "";

enum custom_keycodes {
    TESTE = SAFE_RANGE,
};

// Define o tipo de evento do teclado: pressionar, soltar ou tocar
typedef enum {
    QMK_TAP,  // Pressionar e soltar a tecla (toque rápido)
    QMK_DOWN, // Apenas pressionar a tecla
    QMK_UP    // Apenas soltar a tecla
} qmk_event_type_t;

// Estrutura para representar um evento de teclado
typedef struct {
    uint16_t keycode;        // Keycode do QMK (ex: KC_A, KC_LSFT)
    qmk_event_type_t type;   // Tipo de evento (TAP, DOWN, UP)
} qmk_event_t;

void add_event(qmk_event_t** events, int* num_events, int* capacity, uint16_t keycode, qmk_event_type_t type) {
    if (*num_events >= *capacity) {
        *capacity *= 2;
        qmk_event_t* temp_events = (qmk_event_t*) realloc(*events, (*capacity) * sizeof(qmk_event_t));
        if (temp_events == NULL) {
            return;
        }
        *events = temp_events;
    }
    (*events)[*num_events].keycode = keycode;
    (*events)[*num_events].type = type;
    (*num_events)++;
}

qmk_event_t* string_to_qmk_events(const char* str, int* num_events) {
    qmk_event_t* events = NULL;
    *num_events = 0;
    int capacity = 10;
    events = (qmk_event_t*) malloc(capacity * sizeof(qmk_event_t));
    if (events == NULL) return NULL;

    for (int i = 0; str[i] != '\0'; ++i) {
        char c = str[i];
        bool needs_shift = false;
        uint16_t keycode = KC_NO;

        // Variável para controlar quantos caracteres avançar após processar uma sequência de escape
        int chars_to_skip = 0;

        // Verifica se é uma sequência de escape
        if (c == '\\' && str[i+1] != '\0') {
            char escape_prefix = str[i+1];
            
            // Lógica para teclas de função (F1-F12)
            if (escape_prefix == 'f' && str[i+2] != '\0') {
                int f_num = -1;
                if (str[i+2] >= '1' && str[i+2] <= '9') {
                    f_num = str[i+2] - '0';
                    chars_to_skip = 2; // e.g., \f1
                } else if (str[i+2] == '1' && str[i+3] >= '0' && str[i+3] <= '2') { // F10, F11, F12
                    f_num = 10 + (str[i+3] - '0');
                    chars_to_skip = 3; // e.g., \f10
                }

                if (f_num >= 1 && f_num <= 12) {
                    keycode = KC_F1 + (f_num - 1); // KC_F1 é o base, F2 = KC_F1 + 1, etc.
                    add_event(&events, num_events, &capacity, keycode, QMK_TAP);
                    i += chars_to_skip;
                    continue; // Pula para a próxima iteração
                }
            }

            // Lógica para outras sequências de escape (s, c, a, e, x)
            if (str[i+2] != '\0') { // Garante que há o caractere de ação (D, U, T)
                char action_char = str[i+2];

                switch (escape_prefix) {
                    case 's': // Shift
                        if (action_char == 'D') add_event(&events, num_events, &capacity, KC_LSFT, QMK_DOWN);
                        else if (action_char == 'U') add_event(&events, num_events, &capacity, KC_LSFT, QMK_UP);
                        else if (action_char == 'T') add_event(&events, num_events, &capacity, KC_LSFT, QMK_TAP);
                        chars_to_skip = 2; break;
                    case 'c': // Control
                        if (action_char == 'D') add_event(&events, num_events, &capacity, KC_LCTL, QMK_DOWN);
                        else if (action_char == 'U') add_event(&events, num_events, &capacity, KC_LCTL, QMK_UP);
                        else if (action_char == 'T') add_event(&events, num_events, &capacity, KC_LCTL, QMK_TAP);
                        chars_to_skip = 2; break;
                    case 'a': // Alt
                        if (action_char == 'D') add_event(&events, num_events, &capacity, KC_LALT, QMK_DOWN);
                        else if (action_char == 'U') add_event(&events, num_events, &capacity, KC_LALT, QMK_UP);
                        else if (action_char == 'T') add_event(&events, num_events, &capacity, KC_LALT, QMK_TAP);
                        chars_to_skip = 2; break;
                    case 'e': // Enter
                        if (action_char == 'D') add_event(&events, num_events, &capacity, KC_ENT, QMK_DOWN);
                        else if (action_char == 'U') add_event(&events, num_events, &capacity, KC_ENT, QMK_UP);
                        else if (action_char == 'T') add_event(&events, num_events, &capacity, KC_ENT, QMK_TAP);
                        chars_to_skip = 2; break;
                    case 'x': // Escape
                        if (action_char == 'D') add_event(&events, num_events, &capacity, KC_ESC, QMK_DOWN);
                        else if (action_char == 'U') add_event(&events, num_events, &capacity, KC_ESC, QMK_UP);
                        else if (action_char == 'T') add_event(&events, num_events, &capacity, KC_ESC, QMK_TAP);
                        chars_to_skip = 2; break;
                    case '\\': // Backslash literal (e.g., \\\\ na string de entrada)
                        add_event(&events, num_events, &capacity, KC_NUBS, QMK_TAP);
                        chars_to_skip = 1; // Pula apenas o segundo '\' que foi interpretado como literal
                        break;
                    default:
                        // Não é uma sequência de escape reconhecida
                        break;
                }
                
                if (chars_to_skip > 0) { // Se uma sequência de escape foi processada
                    i += chars_to_skip;
                    continue; // Pula para a próxima iteração
                }
            }
        }
        
        // Lógica para caracteres normais (ou backslash não parte de uma sequência de escape válida/completa)
        if (c >= 'A' && c <= 'Z') {
            needs_shift = true;
            keycode = c - 'A' + KC_A;
        } else {
            switch (c) {
                case 'a': keycode = KC_A; break; case 'b': keycode = KC_B; break;
                case 'c': keycode = KC_C; break; case 'd': keycode = KC_D; break;
                case 'e': keycode = KC_E; break; case 'f': keycode = KC_F; break;
                case 'g': keycode = KC_G; break; case 'h': keycode = KC_H; break;
                case 'i': keycode = KC_I; break; case 'j': keycode = KC_J; break;
                case 'k': keycode = KC_K; break; case 'l': keycode = KC_L; break;
                case 'm': keycode = KC_M; break; case 'n': keycode = KC_N; break;
                case 'o': keycode = KC_O; break; case 'p': keycode = KC_P; break;
                case 'q': keycode = KC_Q; break; case 'r': keycode = KC_R; break;
                case 's': keycode = KC_S; break; case 't': keycode = KC_T; break;
                case 'u': keycode = KC_U; break; case 'v': keycode = KC_V; break;
                case 'w': keycode = KC_W; break; case 'x': keycode = KC_X; break;
                case 'y': keycode = KC_Y; break; case 'z': keycode = KC_Z; break;

                case '0': keycode = KC_0; break; case '1': keycode = KC_1; break;
                case '2': keycode = KC_2; break; case '3': keycode = KC_3; break;
                case '4': keycode = KC_4; break; case '5': keycode = KC_5; break;
                case '6': keycode = KC_6; break; case '7': keycode = KC_7; break;
                case '8': keycode = KC_8; break; case '9': keycode = KC_9; break;

                case ' ': keycode = KC_SPC; break;
                case '\n': keycode = KC_ENT; break;
                case '\t': keycode = KC_TAB; break;

                case '[': keycode = BR_LBRC; break;
                case '{': needs_shift = true; keycode = BR_LBRC; break;
                
                case ']': keycode = KC_BSLS; break;
                case '}': needs_shift = true; keycode = KC_BSLS; break;

                case '\'': keycode = BR_QUOT; break;
                case '"': needs_shift = true; keycode = BR_QUOT; break;

                case '/': keycode = BR_SLSH; break;
                case '?': needs_shift = true; keycode = BR_SLSH; break;

                case '\\': keycode = KC_NUBS; break;
                case '|': needs_shift = true; keycode = KC_NUBS; break;

                case ';': keycode = KC_SLSH; break;
                case ':': needs_shift = true; keycode = KC_SLSH; break;

                case '-': keycode = KC_MINS; break;
                case '_': needs_shift = true; keycode = KC_MINS; break;

                case '=': keycode = KC_EQL; break;
                case '+': needs_shift = true; keycode = KC_EQL; break;

                case ',': keycode = KC_COMM; break;
                case '<': needs_shift = true; keycode = KC_COMM; break;

                case '.': keycode = KC_DOT; break;
                case '>': needs_shift = true; keycode = KC_DOT; break;

                case '`': keycode = KC_GRV; break;
                case '~': needs_shift = true; keycode = KC_GRV; break;

                case '!': needs_shift = true; keycode = KC_1; break;
                case '@': needs_shift = true; keycode = KC_2; break;
                case '#': needs_shift = true; keycode = KC_3; break;
                case '$': needs_shift = true; keycode = KC_4; break;
                case '%': needs_shift = true; keycode = KC_5; break;
                case '^': needs_shift = true; keycode = KC_6; break;
                case '&': needs_shift = true; keycode = KC_7; break;
                case '*': needs_shift = true; keycode = KC_8; break;
                case '(': needs_shift = true; keycode = KC_9; break;
                case ')': needs_shift = true; keycode = KC_0; break;

                default: keycode = KC_NO; break;
            }
        }

        if (needs_shift) {
            add_event(&events, num_events, &capacity, KC_LSFT, QMK_DOWN);
        }

        if (keycode != KC_NO) {
            add_event(&events, num_events, &capacity, keycode, QMK_TAP);
        }

        if (needs_shift) {
            add_event(&events, num_events, &capacity, KC_LSFT, QMK_UP);
        }
    }

    qmk_event_t* final_events = (qmk_event_t*) realloc(events, (*num_events) * sizeof(qmk_event_t));
    if (final_events == NULL && *num_events > 0) {
        return events;
    }
    return final_events;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case TESTE:
        if (record->event.pressed) {
            const char* test_string = "iclass BottomlessPit<T> {\\eTvar items: List<T> = emptyList()\\eTprivate set\\eT\\eT\\eTfun add(element: T) {\\eTitems += element\\xTjo\\eTfun addAll(elements: Collection<T>) {\\eTitems += elements\\xT?items\\eTnn\\sD\\f6\\sUthings\\eT\\xT\\xT";
            int num_events = 0;
            qmk_event_t* events = string_to_qmk_events(test_string, &num_events);

            if (events == NULL) {
                SEND_STRING("Erro ao converter a string.\n");
                return false;
            }

            for(int i = 0; i < num_events; i++) {
                qmk_event_type_t type = events[i].type;
                uint16_t kc = events[i].keycode;
                switch (type) {
                case QMK_TAP:
                    tap_code_delay(kc, 75);
                    break;
                case QMK_DOWN:
                    register_code(kc);
                    break;
                case QMK_UP:
                    unregister_code(kc);
                    break;
                default:
                    break;
                }
            }

            free(events);
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

