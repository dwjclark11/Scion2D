#pragma once
#include <SDL.h>
/*
 *  The key numbers are defined based on SDLK key code values
 */

constexpr int KEY_UNKNOWN = -1;

constexpr int SCION_KEY_BACKSPACE = SDLK_BACKSPACE;
constexpr int SCION_KEY_TAB = SDLK_TAB;
constexpr int SCION_KEY_CLEAR = SDLK_CLEAR;
constexpr int SCION_KEY_RETURN = SDLK_RETURN;
constexpr int SCION_KEY_PAUSE = SDLK_PAUSE;
constexpr int SCION_KEY_ESCAPE = SDLK_ESCAPE;
constexpr int SCION_KEY_SPACE = SDLK_SPACE;
constexpr int SCION_KEY_EXCLAIM = SDLK_EXCLAIM;
constexpr int SCION_KEY_QUOTEDBL = SDLK_QUOTEDBL;
constexpr int SCION_KEY_HASH = SDLK_HASH;
constexpr int SCION_KEY_DOLLAR = SDLK_DOLLAR;
constexpr int SCION_KEY_AMPERSAND = SDLK_AMPERSAND;
constexpr int SCION_KEY_QUOTE = SDLK_QUOTE;
constexpr int SCION_KEY_LEFTPAREN = SDLK_LEFTPAREN;
constexpr int SCION_KEY_RIGHTPAREN = SDLK_RIGHTPAREN;
constexpr int SCION_KEY_ASTERISK = SDLK_ASTERISK;
constexpr int SCION_KEY_PLUS = SDLK_PLUS;
constexpr int SCION_KEY_COMMA = SDLK_COMMA;
constexpr int SCION_KEY_MINUS = SDLK_MINUS;
constexpr int SCION_KEY_PERIOD = SDLK_PERIOD;
constexpr int SCION_KEY_SLASH = SDLK_SLASH;

constexpr int SCION_KEY_0 = SDLK_0;
constexpr int SCION_KEY_1 = SDLK_1;
constexpr int SCION_KEY_2 = SDLK_2;
constexpr int SCION_KEY_3 = SDLK_3;
constexpr int SCION_KEY_4 = SDLK_4;
constexpr int SCION_KEY_5 = SDLK_5;
constexpr int SCION_KEY_6 = SDLK_6;
constexpr int SCION_KEY_7 = SDLK_7;
constexpr int SCION_KEY_8 = SDLK_8;
constexpr int SCION_KEY_9 = SDLK_9;

constexpr int SCION_KEY_COLON = SDLK_COLON;
constexpr int SCION_KEY_SEMICOLON = SDLK_SEMICOLON;
constexpr int SCION_KEY_LESS = SDLK_LESS;
constexpr int SCION_KEY_EQUALS = SDLK_EQUALS;
constexpr int SCION_KEY_GREATER = SDLK_GREATER;
constexpr int SCION_KEY_QUESTION = SDLK_QUESTION;
constexpr int SCION_KEY_AT = SDLK_AT;

constexpr int SCION_KEY_LEFTBRACKET = SDLK_LEFTBRACKET;
constexpr int SCION_KEY_BACKSLASH = SDLK_BACKSLASH;
constexpr int SCION_KEY_RIGHTBRACKET = SDLK_RIGHTBRACKET;
constexpr int SCION_KEY_CARET = SDLK_CARET;
constexpr int SCION_KEY_UNDERSCORE = SDLK_UNDERSCORE;
constexpr int SCION_KEY_BACKQUOTE = SDLK_BACKQUOTE;
constexpr int SCION_KEY_A = SDLK_a;
constexpr int SCION_KEY_B = SDLK_b;
constexpr int SCION_KEY_C = SDLK_c;
constexpr int SCION_KEY_D = SDLK_d;
constexpr int SCION_KEY_E = SDLK_e;
constexpr int SCION_KEY_F = SDLK_f;
constexpr int SCION_KEY_G = SDLK_g;
constexpr int SCION_KEY_H = SDLK_h;
constexpr int SCION_KEY_I = SDLK_i;
constexpr int SCION_KEY_J = SDLK_j;
constexpr int SCION_KEY_K = SDLK_k;
constexpr int SCION_KEY_L = SDLK_l;
constexpr int SCION_KEY_M = SDLK_m;
constexpr int SCION_KEY_N = SDLK_n;
constexpr int SCION_KEY_O = SDLK_o;
constexpr int SCION_KEY_P = SDLK_p;
constexpr int SCION_KEY_Q = SDLK_q;
constexpr int SCION_KEY_R = SDLK_r;
constexpr int SCION_KEY_S = SDLK_s;
constexpr int SCION_KEY_T = SDLK_t;
constexpr int SCION_KEY_U = SDLK_u;
constexpr int SCION_KEY_V = SDLK_v;
constexpr int SCION_KEY_W = SDLK_w;
constexpr int SCION_KEY_X = SDLK_x;
constexpr int SCION_KEY_Y = SDLK_y;
constexpr int SCION_KEY_Z = SDLK_z;

constexpr int SCION_KEY_DELETE = SDLK_DELETE;
constexpr int SCION_KEY_CAPSLOCK = SDLK_CAPSLOCK;

constexpr int SCION_KEY_F1 = SDLK_F1;
constexpr int SCION_KEY_F2 = SDLK_F2;
constexpr int SCION_KEY_F3 = SDLK_F3;
constexpr int SCION_KEY_F4 = SDLK_F4;
constexpr int SCION_KEY_F5 = SDLK_F5;
constexpr int SCION_KEY_F6 = SDLK_F6;
constexpr int SCION_KEY_F7 = SDLK_F7;
constexpr int SCION_KEY_F8 = SDLK_F8;
constexpr int SCION_KEY_F9 = SDLK_F9;
constexpr int SCION_KEY_F10 = SDLK_F10;
constexpr int SCION_KEY_F11 = SDLK_F11;
constexpr int SCION_KEY_F12 = SDLK_F12;

constexpr int SCION_KEY_PRINTSCREEN = SDLK_PRINTSCREEN;
constexpr int SCION_KEY_SCROLLOCK = SDLK_SCROLLLOCK;

constexpr int SCION_KEY_INSERT = SDLK_INSERT;
constexpr int SCION_KEY_HOME = SDLK_HOME;
constexpr int SCION_KEY_PAGEUP = SDLK_PAGEUP;
constexpr int SCION_KEY_END = SDLK_END;
constexpr int SCION_KEY_PAGEDOWN = SDLK_PAGEDOWN;

constexpr int SCION_KEY_RIGHT = SDLK_RIGHT;
constexpr int SCION_KEY_LEFT = SDLK_LEFT;
constexpr int SCION_KEY_DOWN = SDLK_DOWN;
constexpr int SCION_KEY_UP = SDLK_UP;

constexpr int SCION_KEY_NUMLOCK = SDLK_NUMLOCKCLEAR;
constexpr int SCION_KEY_KP_DIVIDE = SDLK_KP_DIVIDE;
constexpr int SCION_KEY_KP_MULTIPLY = SDLK_KP_MULTIPLY;
constexpr int SCION_KEY_KP_MINUS = SDLK_KP_MINUS;
constexpr int SCION_KEY_KP_PLUS = SDLK_KP_PLUS;
constexpr int SCION_KEY_KP_ENTER = SDLK_KP_ENTER;

constexpr int SCION_KEY_KP1 = SDLK_KP_1;
constexpr int SCION_KEY_KP2 = SDLK_KP_2;
constexpr int SCION_KEY_KP3 = SDLK_KP_3;
constexpr int SCION_KEY_KP4 = SDLK_KP_4;
constexpr int SCION_KEY_KP5 = SDLK_KP_5;
constexpr int SCION_KEY_KP6 = SDLK_KP_6;
constexpr int SCION_KEY_KP7 = SDLK_KP_7;
constexpr int SCION_KEY_KP8 = SDLK_KP_8;
constexpr int SCION_KEY_KP9 = SDLK_KP_9;
constexpr int SCION_KEY_KP0 = SDLK_KP_0;

constexpr int SCION_KEY_KP_PERIOD = SDLK_KP_PERIOD;

constexpr int SCION_KEY_LCTRL = SDLK_LCTRL;
constexpr int SCION_KEY_LSHIFT = SDLK_LSHIFT;
constexpr int SCION_KEY_LALT = SDLK_LALT;
constexpr int SCION_KEY_RCTRL = SDLK_RCTRL;
constexpr int SCION_KEY_RSHIFT = SDLK_RSHIFT;
constexpr int SCION_KEY_RALT = SDLK_RALT;

// Put windows naming specific keys here
#ifdef _WIN32
constexpr int SCION_KEY_LWIN = SDLK_LGUI;
constexpr int SCION_KEY_RWIN = SDLK_RGUI;
#else
constexpr int SCION_KEY_LGUI = SDLK_LGUI;
constexpr int SCION_KEY_RGUI = SDLK_RGUI;
#endif
