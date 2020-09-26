//  ===== Date Created: 25 September, 2020 =====

#include "keyboard.hpp"

using namespace Key;

constexpr KeyCode usEnglishKeymapRegular[] = {
	Error, Escape,
	_1, _2, _3, _4, _5, _6, _7, _8, _9, _0,
	Minus, Equal, Backspace, Tab,
	q, w, e, r, t, y, u, i, o, p, LeftSquareBracket, RightSquareBracket,
	Enter, LeftControl,
	a, s, d, f, g, h, j, k, l, SemiColon, SingleQuote, Backtick, LeftShift, Backslash,
	z, x, c, v, b, n, m, Comma, Period, ForwardSlash, RightShift, Asterisk,
	LeftAlt, Space, CapsLock,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
	NumLock, ScrollLock,
	Home, UpArrow, PageUp, None, LeftArrow, None,
	RightArrow, None, End, DownArrow, PageDown, Insert, Delete, None
};

constexpr KeyCode usEnglishKeymapShifted[] = {
	Error, Escape,
	Bang, At, Hash, Dollar, Percent, Caret, Ampersand, Asterisk, LeftParen, RightParen,
	Underscore, Plus, Backspace, Tab,
	Q, W, E, R, T, Y, U, I, O, P, LeftBrace, RightBrace,
	Enter, LeftControl,
	A, S, D, F, G, H, J, K, L, Colon, DoubleQuote, Tilde, LeftShift, Pipe,
	Z, X, C, V, B, N, M, LeftAngleBracket, RightAngleBracket, Question, RightShift, Asterisk,
	LeftAlt, Space, CapsLock,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
	NumLock, ScrollLock,
	Home, UpArrow, PageUp, None, LeftArrow, None,
	RightArrow, None, End, DownArrow, PageDown, Insert, Delete, None
};

Key::KeyCode convert_scancode_to_keycode(u8 scancode, bool shiftPressed, KeyboardRegion region)
{
	switch (region)
	{
	case KeyboardRegion::EnglishUS:
	{
		if (shiftPressed)
		{
			return usEnglishKeymapShifted[scancode & 0x7f];
		}
		else
		{
			return usEnglishKeymapRegular[scancode];
		}
	}
	}
}

u8 convert_keycode_to_character(Key::KeyCode keycode)
{
	switch (keycode)
	{
	case Backspace: return '\b';
	case Enter: return '\n';
	case Tab: return '\t';
		
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

	case a: return 'a';
	case b: return 'b';
	case c: return 'c';
	case d: return 'd';
	case e: return 'e';
	case f: return 'f';
	case g: return 'g';
	case h: return 'h';
	case i: return 'i';
	case j: return 'j';
	case k: return 'k';
	case l: return 'l';
	case m: return 'm';
	case n: return 'n';
	case o: return 'o';
	case p: return 'p';
	case q: return 'q';
	case r: return 'r';
	case s: return 's';
	case t: return 't';
	case u: return 'u';
	case v: return 'v';
	case w: return 'w';
	case x: return 'x';
	case y: return 'y';
	case z: return 'z';

	case A: return 'A';
	case B: return 'B';
	case C: return 'C';
	case D: return 'D';
	case E: return 'E';
	case F: return 'F';
	case G: return 'G';
	case H: return 'H';
	case I: return 'I';
	case J: return 'J';
	case K: return 'K';
	case L: return 'L';
	case M: return 'M';
	case N: return 'N';
	case O: return 'O';
	case P: return 'P';
	case Q: return 'Q';
	case R: return 'R';
	case S: return 'S';
	case T: return 'T';
	case U: return 'U';
	case V: return 'V';
	case W: return 'W';
	case X: return 'X';
	case Y: return 'Y';
	case Z: return 'Z';

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
	case Tilde:					return '~';
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
