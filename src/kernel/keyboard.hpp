//  ===== Date Created: 25 September, 2020 ===== 

#if !defined(KEYBOARD_HPP)
#define KEYBOARD_HPP

#include "system.hpp"

enum KeyboardRegion
{
	US_ENGLISH,
};

// NOTE(fkp): This is a namespace/enum instead of an enum class
// because I want to be able to `using namespace` this in the source
// file.
namespace Key
{

enum KeyCode
{
	// Not really keys, but ok...
	Error, None,
	
	Escape, Backspace, Delete, Space, Tab, Insert, Enter,
	CapsLock, ScrollLock, NumLock,

	_1, _2, _3, _4, _5, _6, _7, _8, _9, _0,

	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,

	LeftControl, RightControl, LeftShift, RightShift, LeftAlt, RightAlt, Menu, Application,

	PageUp, PageDown, Home, End,
	LeftArrow, RightArrow, UpArrow, DownArrow,

	Comma, Period, ForwardSlash, Backslash,
	Plus, Minus, Equal, Underscore, Question, Bang,
	Ampersand, Percent, Dollar, Hash, At, Asterisk, Caret,
	Colon, SemiColon, SingleQuote, DoubleQuote, Pipe, Backtick,
	
	LeftParen, RightParen, LeftBrace, RightBrace,
	LeftSquareBracket, RightSquareBracket, LeftAngleBracket, RightAngleBracket,
};

}

// TODO(fkp): Modifiers like shift
Key::KeyCode convert_scancode_to_keycode(u8 scancode, KeyboardRegion region);
u8 convert_keycode_to_character(Key::KeyCode keycode);

#endif
