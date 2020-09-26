//  ===== Date Created: 25 September, 2020 =====

#include "keyboard.hpp"

constexpr u8 usEnglishKeymap[] = {
	/* error */ 0,

	/* escape */ 0,

	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', /* backspace */ 0,
	/* tab */ 0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	/* enter */ 0, /* left control */ 0,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
	'`', /* left shift */ 0, '\\',
	'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', /* right shift */ 0,
	'*', /* left alt */ 0, ' ', /* caps lock */ 0,
	/* f1 - f10 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* num lock */ 0, /* scroll lock */ 0, /* home */ 0, 
};

using namespace Key;

// TODO(fkp): Shift-ed version
constexpr KeyCode usEnglishKeymapRegular[] = {
	Error, Escape,
	_1, _2, _3, _4, _5, _6, _7, _8, _9, _0,
	Minus, Equal, Backspace, Tab,
	Q, W, E, R, T, Y, U, I, O, P, LeftSquareBracket, RightSquareBracket,
	Enter, LeftControl,
	A, S, D, F, G, H, J, K, L, SemiColon, SingleQuote, Backtick, LeftShift, Backslash,
	Z, X, C, V, B, N, M, Comma, Period, ForwardSlash, RightShift, Asterisk,
	LeftAlt, Space, CapsLock,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
	NumLock, ScrollLock,
	Home, UpArrow, PageUp, None, LeftArrow, None,
	RightArrow, None, End, DownArrow, PageDown, Insert, Delete, None
};

Key::KeyCode convert_scancode_to_keycode(u8 scancode, KeyboardRegion region)
{
	switch (region)
	{
	case US_ENGLISH:
	{
	} return usEnglishKeymapRegular[scancode];
	}
}

u8 convert_keycode_to_character(Key::KeyCode keycode)
{
	switch (keycode)
	{
	case _1: return '1';
	case _2: return '2';
	case _3: return '3';
	case _4: return '4';
	case _5: return '5';
	case _6: return '6';
	case _7: return '7';
	case _8: return '8';
	case _9: return '9';
	case _0: return '0';

	case A: return 'a';
	case B: return 'b';
	case C: return 'c';
	case D: return 'd';
	case E: return 'e';
	case F: return 'f';
	case G: return 'g';
	case H: return 'h';
	case I: return 'i';
	case J: return 'j';
	case K: return 'k';
	case L: return 'l';
	case M: return 'm';
	case N: return 'n';
	case O: return 'o';
	case P: return 'p';
	case Q: return 'q';
	case R: return 'r';
	case S: return 's';
	case T: return 't';
	case U: return 'u';
	case V: return 'v';
	case W: return 'w';
	case X: return 'x';
	case Y: return 'y';
	case Z: return 'z';

	case Space:					return ' ';
	case Comma:					return ',';
	case Period:				return '.';
	case ForwardSlash:			return '/';
	case Backslash:				return '\\';
	case Plus:					return '+';
	case Minus:					return '-';
	case Equal:					return '=';
	case Underscore:			return '_';
	case Question:				return '?';
	case Bang:					return '!';
	case Ampersand:				return '&';
	case Percent:				return '%';
	case Dollar:				return '$';
	case Hash:					return '#';
	case At:					return '@';
	case Asterisk:				return '*';
	case Caret:					return '^';
	case Colon:					return ':';
	case SemiColon:				return ';';
	case SingleQuote:			return '\'';
	case DoubleQuote:			return '"';
	case Pipe:					return '|';
	case Backtick:				return '`';
	case LeftParen:				return '(';
	case RightParen:			return ')';
	case LeftBrace:				return '{';
	case RightBrace:			return '}';
	case LeftSquareBracket:		return '[';
	case RightSquareBracket:	return ']';
	case LeftAngleBracket:		return '<';
	case RightAngleBracket:		return '>';

	default: return 0;
	}
}
