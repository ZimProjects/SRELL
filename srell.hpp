/*****************************************************************************
**
**  SRELL (std::regex-like library) version 4.034
**
**  Copyright (c) 2012-2023, Nozomu Katoo. All rights reserved.
**
**  Redistribution and use in source and binary forms, with or without
**  modification, are permitted provided that the following conditions are
**  met:
**
**  1. Redistributions of source code must retain the above copyright notice,
**     this list of conditions and the following disclaimer.
**
**  2. Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in the
**     documentation and/or other materials provided with the distribution.
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
**  IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
**  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
**  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
**  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
**  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
**  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
**  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
**  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
**  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
******************************************************************************
**/

#ifndef SRELL_REGEX_TEMPLATE_LIBRARY
#define SRELL_REGEX_TEMPLATE_LIBRARY

#include <stdexcept>
#include <climits>
#include <cwchar>
#include <string>
#include <locale>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <utility>
#include <vector>
#include <iterator>
#include <memory>
#include <algorithm>

#ifdef __cpp_unicode_characters
  #ifndef SRELL_CPP11_CHAR1632_ENABLED
  #define SRELL_CPP11_CHAR1632_ENABLED
  #endif
#endif
#ifdef __cpp_initializer_lists
  #ifndef SRELL_CPP11_INITIALIZER_LIST_ENABLED
  #define SRELL_CPP11_INITIALIZER_LIST_ENABLED
  #endif
#endif
#ifdef __cpp_rvalue_references
  #ifndef SRELL_CPP11_MOVE_ENABLED
  #define SRELL_CPP11_MOVE_ENABLED
  #endif
#endif
#ifdef SRELL_CPP11_MOVE_ENABLED
  #if defined(_MSC_VER) && _MSC_VER < 1900
  #define SRELL_NOEXCEPT
  #else
  #define SRELL_NOEXCEPT noexcept
  #endif
#endif
#ifdef __cpp_char8_t
  #ifndef SRELL_CPP20_CHAR8_ENABLED
	#ifdef __cpp_lib_char8_t
	#define SRELL_CPP20_CHAR8_ENABLED 2
	#else
	#define SRELL_CPP20_CHAR8_ENABLED 1
	#endif
  #endif
#endif

//  The following SRELL_NO_* macros would be useful when wanting to
//  reduce the size of a binary by turning off some feature(s).

#ifdef SRELL_NO_UNICODE_DATA

//  Prevents Unicode data used for icase (case-insensitive) matching
//  from being output into a resulting binary. In this case only the
//  ASCII characters are case-folded when icase matching is performed
//  (i.e., [A-Z] -> [a-z] only).
#define SRELL_NO_UNICODE_ICASE

//  Disables the Unicode property (\p{...} and \P{...}) and prevents
//  Unicode property data from being output into a resulting binary.
#define SRELL_NO_UNICODE_PROPERTY
#endif

//  Prevents icase matching specific functions into a resulting binary.
//  In this case the icase flag is ignored and icase matching becomes
//  unavailable.
#ifdef SRELL_NO_ICASE
#ifndef SRELL_NO_UNICODE_ICASE
#define SRELL_NO_UNICODE_ICASE
#endif
#endif

//  This macro might be removed in the future.
#ifdef SRELL_V1_COMPATIBLE
#ifndef SRELL_NO_UNICODE_PROPERTY
#define SRELL_NO_UNICODE_PROPERTY
#endif
#ifndef SRELL_NO_VMODE
#define SRELL_NO_VMODE
#endif
#define SRELL_NO_NAMEDCAPTURE
#define SRELL_NO_SINGLELINE
//#define SRELL_FIXEDWIDTHLOOKBEHIND
//  Since version 4.019, SRELL highly depends on the variable-length
//  lookbehind feature. Uncommenting this line is not recommended.
#endif

namespace srell
{
//  ["regex_constants.h" ...

	namespace regex_constants
	{
		enum syntax_option_type
		{
			icase      = 1 << 0,
			nosubs     = 1 << 1,
			optimize   = 1 << 2,
			collate    = 1 << 3,
			ECMAScript = 1 << 4,
			basic      = 1 << 5,
			extended   = 1 << 6,
			awk        = 1 << 7,
			grep       = 1 << 8,
			egrep      = 1 << 9,
			multiline  = 1 << 10,

			//  SRELL's extension.
			dotall      = 1 << 11,	//  singleline.
			unicodesets = 1 << 12
		};

		inline syntax_option_type operator&(const syntax_option_type left, const syntax_option_type right)
		{
			return static_cast<syntax_option_type>(static_cast<int>(left) & static_cast<int>(right));
		}
		inline syntax_option_type operator|(const syntax_option_type left, const syntax_option_type right)
		{
			return static_cast<syntax_option_type>(static_cast<int>(left) | static_cast<int>(right));
		}
		inline syntax_option_type operator^(const syntax_option_type left, const syntax_option_type right)
		{
			return static_cast<syntax_option_type>(static_cast<int>(left) ^ static_cast<int>(right));
		}
		inline syntax_option_type operator~(const syntax_option_type b)
		{
			return static_cast<syntax_option_type>(~static_cast<int>(b));
		}
		inline syntax_option_type &operator&=(syntax_option_type &left, const syntax_option_type right)
		{
			left = left & right;
			return left;
		}
		inline syntax_option_type &operator|=(syntax_option_type &left, const syntax_option_type right)
		{
			left = left | right;
			return left;
		}
		inline syntax_option_type &operator^=(syntax_option_type &left, const syntax_option_type right)
		{
			left = left ^ right;
			return left;
		}
	}
	//  namespace regex_constants

	namespace regex_constants
	{
		enum match_flag_type
		{
			match_default     = 0,
			match_not_bol     = 1 <<  0,
			match_not_eol     = 1 <<  1,
			match_not_bow     = 1 <<  2,
			match_not_eow     = 1 <<  3,
			match_any         = 1 <<  4,
			match_not_null    = 1 <<  5,
			match_continuous  = 1 <<  6,
			match_prev_avail  = 1 <<  7,

			format_default    = 0,
			format_sed        = 1 <<  8,
			format_no_copy    = 1 <<  9,
			format_first_only = 1 << 10,

			//  For internal use.
			match_match_      = 1 << 11
		};

		inline match_flag_type operator&(const match_flag_type left, const match_flag_type right)
		{
			return static_cast<match_flag_type>(static_cast<int>(left) & static_cast<int>(right));
		}
		inline match_flag_type operator|(const match_flag_type left, const match_flag_type right)
		{
			return static_cast<match_flag_type>(static_cast<int>(left) | static_cast<int>(right));
		}
		inline match_flag_type operator^(const match_flag_type left, const match_flag_type right)
		{
			return static_cast<match_flag_type>(static_cast<int>(left) ^ static_cast<int>(right));
		}
		inline match_flag_type operator~(const match_flag_type b)
		{
			return static_cast<match_flag_type>(~static_cast<int>(b));
		}
		inline match_flag_type &operator&=(match_flag_type &left, const match_flag_type right)
		{
			left = left & right;
			return left;
		}
		inline match_flag_type &operator|=(match_flag_type &left, const match_flag_type right)
		{
			left = left | right;
			return left;
		}
		inline match_flag_type &operator^=(match_flag_type &left, const match_flag_type right)
		{
			left = left ^ right;
			return left;
		}
	}
	//  namespace regex_constants

	//  28.5, regex constants:
	namespace regex_constants
	{
		typedef int error_type;

		static const error_type error_collate    = 100;
		static const error_type error_ctype      = 101;
		static const error_type error_escape     = 102;
		static const error_type error_backref    = 103;
		static const error_type error_brack      = 104;
		static const error_type error_paren      = 105;
		static const error_type error_brace      = 106;
		static const error_type error_badbrace   = 107;
		static const error_type error_range      = 108;
		static const error_type error_space      = 109;
		static const error_type error_badrepeat  = 110;
		static const error_type error_complexity = 111;
		static const error_type error_stack      = 112;

		//  SRELL's extensions.
		static const error_type error_utf8       = 113;
			//  The expression contained an invalid UTF-8 sequence.

		static const error_type error_property   = 114;
			//  The expression contained an invalid Unicode property name or value.

		static const error_type error_noescape   = 115;
			//  (Only in v-mode) ( ) [ ] { } / - \ | need to be escaped in a character class.

		static const error_type error_operator   = 116;
			//  (Only in v-mode) A character class contained a reserved double punctuation
			//  operator or different types of operators at the same level, such as [ab--cd].

		static const error_type error_complement = 117;
			//  (Only in v-mode) \P or a negated character class contained a property of strings.

		static const error_type error_modifier   = 118;
			//  A specific flag modifier appeared more then once, or the un-bounded form
			//  ((?ism-ism)) appeared at a position other than the beginning of the expression.

#if defined(SRELL_FIXEDWIDTHLOOKBEHIND)
		static const error_type error_lookbehind = 200;
#endif
		static const error_type error_internal   = 999;
	}
	//  namespace regex_constants

//  ... "regex_constants.h"]
//  ["regex_error.hpp" ...

//  28.6, class regex_error:
class regex_error : public std::runtime_error
{
public:

	explicit regex_error(const regex_constants::error_type ecode)
		: std::runtime_error("regex_error")	//  added for error C2512.
		, ecode_(ecode)
	{
	}

	regex_constants::error_type code() const
	{
		return ecode_;
	}

private:

	regex_constants::error_type ecode_;
};

//  ... "regex_error.hpp"]
//  ["rei_type.h" ...

	namespace re_detail
	{

#if defined(SRELL_CPP11_CHAR1632_ENABLED)

		typedef char32_t ui_l32;	//  uint_least32.

#elif defined(UINT_MAX) && UINT_MAX >= 0xFFFFFFFF

		typedef unsigned int ui_l32;

#elif defined(ULONG_MAX) && ULONG_MAX >= 0xFFFFFFFF

		typedef unsigned long ui_l32;

#else
#error could not find a suitable type for 32-bit Unicode integer values.
#endif	//  defined(SRELL_CPP11_CHAR1632_ENABLED)
	}	//  namespace re_detail

//  ... "rei_type.h"]
//  ["rei_constants.h" ...

	namespace re_detail
	{
		enum re_state_type
		{
			st_character,               //  0x00
			st_character_class,         //  0x01

			st_epsilon,                 //  0x02

			st_check_counter,           //  0x03
//			st_increment_counter,       //  0x04
			st_decrement_counter,       //  0x04
			st_save_and_reset_counter,  //  0x05
			st_restore_counter,         //  0x06

			st_roundbracket_open,       //  0x07
			st_roundbracket_pop,        //  0x08
			st_roundbracket_close,      //  0x09

			st_repeat_in_push,          //  0x0a
			st_repeat_in_pop,           //  0x0b
			st_check_0_width_repeat,    //  0x0c

			st_backreference,           //  0x0d

			st_lookaround_open,         //  0x0e

//			st_lookaround_pop,          //  0x10

			st_bol,                     //  0x0f
			st_eol,                     //  0x10
			st_boundary,                //  0x11

			st_success,                 //  0x12

#if defined(SRELLTEST_NEXTPOS_OPT)
			st_move_nextpos,            //  0x13
#endif

			st_lookaround_close        = st_success,
			st_zero_width_boundary     = st_lookaround_open,
		};
		//  re_state_type

		namespace constants
		{
			static const ui_l32 unicode_max_codepoint = 0x10ffff;
			static const ui_l32 invalid_u32value = static_cast<ui_l32>(-1);
			static const ui_l32 max_u32value = static_cast<ui_l32>(-2);
			static const ui_l32 asc_icase = 0x20;
			static const ui_l32 ccstr_empty = static_cast<ui_l32>(-1);
			static const ui_l32 infinity = static_cast<ui_l32>(~0);
		}
		//  constants

		namespace meta_char
		{
			static const ui_l32 mc_exclam = 0x21;	//  '!'
			static const ui_l32 mc_sharp  = 0x23;	//  '#'
			static const ui_l32 mc_dollar = 0x24;	//  '$'
			static const ui_l32 mc_rbraop = 0x28;	//  '('
			static const ui_l32 mc_rbracl = 0x29;	//  ')'
			static const ui_l32 mc_astrsk = 0x2a;	//  '*'
			static const ui_l32 mc_plus   = 0x2b;	//  '+'
			static const ui_l32 mc_comma  = 0x2c;	//  ','
			static const ui_l32 mc_minus  = 0x2d;	//  '-'
			static const ui_l32 mc_period = 0x2e;	//  '.'
			static const ui_l32 mc_colon  = 0x3a;	//  ':'
			static const ui_l32 mc_lt = 0x3c;		//  '<'
			static const ui_l32 mc_eq = 0x3d;		//  '='
			static const ui_l32 mc_gt = 0x3e;		//  '>'
			static const ui_l32 mc_query  = 0x3f;	//  '?'
			static const ui_l32 mc_sbraop = 0x5b;	//  '['
			static const ui_l32 mc_escape = 0x5c;	//  '\\'
			static const ui_l32 mc_sbracl = 0x5d;	//  ']'
			static const ui_l32 mc_caret  = 0x5e;	//  '^'
			static const ui_l32 mc_cbraop = 0x7b;	//  '{'
			static const ui_l32 mc_bar    = 0x7c;	//  '|'
			static const ui_l32 mc_cbracl = 0x7d;	//  '}'
		}
		//  meta_char

		namespace char_ctrl
		{
			static const ui_l32 cc_nul  = 0x00;	//  '\0'	//0x00:NUL
			static const ui_l32 cc_bs   = 0x08;	//  '\b'	//0x08:BS
			static const ui_l32 cc_htab = 0x09;	//  '\t'	//0x09:HT
			static const ui_l32 cc_nl   = 0x0a;	//  '\n'	//0x0a:LF
			static const ui_l32 cc_vtab = 0x0b;	//  '\v'	//0x0b:VT
			static const ui_l32 cc_ff   = 0x0c;	//  '\f'	//0x0c:FF
			static const ui_l32 cc_cr   = 0x0d;	//  '\r'	//0x0d:CR
		}
		//  char_ctrl

		namespace char_alnum
		{
			static const ui_l32 ch_0 = 0x30;	//  '0'
			static const ui_l32 ch_1 = 0x31;	//  '1'
			static const ui_l32 ch_7 = 0x37;	//  '7'
			static const ui_l32 ch_8 = 0x38;	//  '8'
			static const ui_l32 ch_9 = 0x39;	//  '9'
			static const ui_l32 ch_A = 0x41;	//  'A'
			static const ui_l32 ch_B = 0x42;	//  'B'
			static const ui_l32 ch_D = 0x44;	//  'D'
			static const ui_l32 ch_F = 0x46;	//  'F'
			static const ui_l32 ch_P = 0x50;	//  'P'
			static const ui_l32 ch_S = 0x53;	//  'S'
			static const ui_l32 ch_W = 0x57;	//  'W'
			static const ui_l32 ch_Z = 0x5a;	//  'Z'
			static const ui_l32 ch_a = 0x61;	//  'a'
			static const ui_l32 ch_b = 0x62;	//  'b'
			static const ui_l32 ch_c = 0x63;	//  'c'
			static const ui_l32 ch_d = 0x64;	//  'd'
			static const ui_l32 ch_f = 0x66;	//  'f'
			static const ui_l32 ch_i = 0x69;	//  'i'
			static const ui_l32 ch_k = 0x6b;	//  'k'
			static const ui_l32 ch_m = 0x6d;	//  'm'
			static const ui_l32 ch_n = 0x6e;	//  'n'
			static const ui_l32 ch_p = 0x70;	//  'p'
			static const ui_l32 ch_q = 0x71;	//  'q'
			static const ui_l32 ch_r = 0x72;	//  'r'
			static const ui_l32 ch_s = 0x73;	//  's'
			static const ui_l32 ch_t = 0x74;	//  't'
			static const ui_l32 ch_u = 0x75;	//  'u'
			static const ui_l32 ch_v = 0x76;	//  'v'
			static const ui_l32 ch_w = 0x77;	//  'w'
			static const ui_l32 ch_x = 0x78;	//  'x'
			static const ui_l32 ch_z = 0x7a;	//  'z'
		}
		//  char_alnum

		namespace char_other
		{
			static const ui_l32 co_sp    = 0x20;	//  ' '
			static const ui_l32 co_perc  = 0x25;	//  '%'
			static const ui_l32 co_amp   = 0x26;	//  '&'
			static const ui_l32 co_apos  = 0x27;	//  '\''
			static const ui_l32 co_slash = 0x2f;	//  '/'
			static const ui_l32 co_smcln = 0x3b;	//  ';'
			static const ui_l32 co_atmrk = 0x40;	//  '@'
			static const ui_l32 co_ll    = 0x5f;	//  '_'
			static const ui_l32 co_grav  = 0x60;	//  '`'
			static const ui_l32 co_tilde = 0x7e;	//  '~'
		}
		//  char_other

		namespace epsilon_type	//  Used only in the pattern compiler.
		{
			static const ui_l32 et_default  = char_ctrl::cc_nul;	//  '\0'
			static const ui_l32 et_ccastrsk = 0x2a;	//  '*'
			static const ui_l32 et_alt      = 0x7c;	//  '|'
			static const ui_l32 et_hooked   = 0x68;	//  'h'
			static const ui_l32 et_jmpinlp  = 0x2b;	//  '+'
			static const ui_l32 et_brnchend = 0x2f;	//  '/'
			static const ui_l32 et_fmrbckrf = 0x5c;	//  '\\'
			static const ui_l32 et_bo1fmrbr = 0x31;	//  '1'
			static const ui_l32 et_bo2skpd  = 0x21;	//  '!'
			static const ui_l32 et_bo2fmrbr = 0x32;	//  '2'
			static const ui_l32 et_ncgopen  = 0x3a;	//  ':'
			static const ui_l32 et_ncgclose = 0x3b;	//  ';'
		}
		//  epsilon_type
	}
	//  namespace re_detail

//  ... "rei_constants.h"]
//  ["rei_utf_traits.hpp" ...

	namespace re_detail
	{

#if defined(_MSC_VER)
#define SRELL_FORCEINLINE __forceinline
#elif defined(__GNUC__)
#define SRELL_FORCEINLINE __attribute__((always_inline))
#else
#define SRELL_FORCEINLINE
#endif

template <typename charT>
struct utf_traits_core
{
public:

	typedef charT char_type;

	static const std::size_t maxseqlen = 1;
	static const int utftype = 0;

	static const ui_l32 charbit = (sizeof (charT) * CHAR_BIT) > 21 ? 21 : (sizeof (charT) * CHAR_BIT);
	static const ui_l32 bitsetsize = 1 << (charbit > 16 ? 16 : charbit);
	static const ui_l32 bitsetmask = bitsetsize - 1;
	static const ui_l32 maxcpvalue = charbit == 21 ? 0x10ffff : ((1 << charbit) - 1);

	//  *iter
	template <typename ForwardIterator>
	static ui_l32 codepoint(ForwardIterator begin, const ForwardIterator /* end */)
	{
		return static_cast<ui_l32>(*begin);
		//  Caller is responsible for begin != end.
	}

	//  *iter++
	template <typename ForwardIterator>
	static ui_l32 codepoint_inc(ForwardIterator &begin, const ForwardIterator /* end */)
	{
		return static_cast<ui_l32>(*begin++);
		//  Caller is responsible for begin != end.
	}

	//  iter2 = iter; return *--iter2;
	template <typename BidirectionalIterator>
	static ui_l32 prevcodepoint(BidirectionalIterator cur, const BidirectionalIterator /* begin */)
	{
		return static_cast<ui_l32>(*--cur);
	}

	//  *--iter
	template <typename BidirectionalIterator>
	static ui_l32 dec_codepoint(BidirectionalIterator &cur, const BidirectionalIterator /* begin */)
	{
		return static_cast<ui_l32>(*--cur);
		//  Caller is responsible for cur != begin.
	}

#if !defined(SRELLDBG_NO_BMH)

	template <typename charT2>
	static bool is_trailing(const charT2 /* cu */)
	{
		return false;
	}

#endif	//  !defined(SRELLDBG_NO_BMH)

	static ui_l32 to_codeunits(charT out[maxseqlen], ui_l32 cp)
	{
		out[0] = static_cast<charT>(cp);
		return 1;
	}

	static ui_l32 firstcodeunit(const ui_l32 cp)
	{
		return cp;
	}
};
template <typename charT> const std::size_t utf_traits_core<charT>::maxseqlen;
template <typename charT> const int utf_traits_core<charT>::utftype;
template <typename charT> const ui_l32 utf_traits_core<charT>::charbit;
template <typename charT> const ui_l32 utf_traits_core<charT>::bitsetsize;
template <typename charT> const ui_l32 utf_traits_core<charT>::bitsetmask;
template <typename charT> const ui_l32 utf_traits_core<charT>::maxcpvalue;
//  utf_traits_core

//  common and utf-32.
template <typename charT>
struct utf_traits : public utf_traits_core<charT>
{
	static const int utftype = 32;
};
template <typename charT> const int utf_traits<charT>::utftype;
//  utf_traits

//  utf-8 specific.
template <typename charT>
struct utf8_traits : public utf_traits_core<charT>
{
public:

	//  utf-8 specific.
	static const std::size_t maxseqlen = 4;
	static const int utftype = 8;

	static const ui_l32 charbit = 8;
	static const ui_l32 bitsetsize = 0x100;
	static const ui_l32 bitsetmask = 0xff;
	static const ui_l32 maxcpvalue = 0x10ffff;

	template <typename ForwardIterator>
	static SRELL_FORCEINLINE ui_l32 codepoint(ForwardIterator begin, const ForwardIterator end)
	{
		ui_l32 codepoint = static_cast<ui_l32>(*begin & 0xff);

		if ((codepoint & 0x80) == 0)	//  1 octet.
			return codepoint;

		if (++begin != end && codepoint >= 0xc0 && (*begin & 0xc0) == 0x80)
		{
			codepoint = static_cast<ui_l32>((codepoint << 6) | (*begin & 0x3f));

			if ((codepoint & 0x800) == 0)	//  2 octets.
				return static_cast<ui_l32>(codepoint & 0x7ff);

			if (++begin != end && (*begin & 0xc0) == 0x80)
			{
				codepoint = static_cast<ui_l32>((codepoint << 6) | (*begin & 0x3f));

				if ((codepoint & 0x10000) == 0)	//  3 octets.
					return static_cast<ui_l32>(codepoint & 0xffff);

				if (++begin != end && (*begin & 0xc0) == 0x80)
				{
					codepoint = static_cast<ui_l32>((codepoint << 6) | (*begin & 0x3f));

					if (codepoint <= 0x3dfffff)	//  4 octets.
						return static_cast<ui_l32>(codepoint & 0x1fffff);
				}
			}
		}
//		else	//  80-bf, f8-ff: invalid.

		return re_detail::constants::invalid_u32value;
	}

	template <typename ForwardIterator>
	static SRELL_FORCEINLINE ui_l32 codepoint_inc(ForwardIterator &begin, const ForwardIterator end)
	{
		ui_l32 codepoint = static_cast<ui_l32>(*begin++ & 0xff);

		if ((codepoint & 0x80) == 0)	//  1 octet.
			return codepoint;

//		if (begin != end && (0x7f00 & (1 << ((codepoint >> 3) & 0xf))) && (*begin & 0xc0) == 0x80)	//  c0, c8, d0, d8, e0, e8, f0.
		if (begin != end && codepoint >= 0xc0 && (*begin & 0xc0) == 0x80)
		{
			codepoint = static_cast<ui_l32>((codepoint << 6) | (*begin++ & 0x3f));

			//  11 ?aaa aabb bbbb
			if ((codepoint & 0x800) == 0)	//  2 octets.
				return static_cast<ui_l32>(codepoint & 0x7ff);
				//  c080-c1bf: invalid. 00-7F.
				//  c280-dfbf: valid. 080-7FF.

			//  11 1aaa aabb bbbb
			if (begin != end && (*begin & 0xc0) == 0x80)
			{
				codepoint = static_cast<ui_l32>((codepoint << 6) | (*begin++ & 0x3f));

				//  111? aaaa bbbb bbcc cccc
				if ((codepoint & 0x10000) == 0)	//  3 octets.
					return static_cast<ui_l32>(codepoint & 0xffff);
					//  e08080-e09fbf: invalid. 000-7FF.
					//  e0a080-efbfbf: valid. 0800-FFFF.

				//  1111 aaaa bbbb bbcc cccc
				if (begin != end && (*begin & 0xc0) == 0x80)
				{
					codepoint = static_cast<ui_l32>((codepoint << 6) | (*begin++ & 0x3f));
					//  f0808080-f08fbfbf: invalid. 0000-FFFF.
					//  f0908080-f3bfbfbf: valid. 10000-FFFFF.
					//  f4808080-f48fbfbf: valid. 100000-10FFFF.
					//  f4908080-f4bfbfbf: invalid. 110000-13FFFF.
					//  f5808080-f7bfbfbf: invalid. 140000-1FFFFF.

					//  11 11?a aabb bbbb cccc ccdd dddd
					if (codepoint <= 0x3dfffff)	//  4 octets.
						return static_cast<ui_l32>(codepoint & 0x1fffff);
						//  11 110a aabb bbbb cccc ccdd dddd
				}
			}
		}
//		else	//  80-bf, f8-ff: invalid.

		return re_detail::constants::invalid_u32value;
	}

	template <typename BidirectionalIterator>
	static SRELL_FORCEINLINE ui_l32 prevcodepoint(BidirectionalIterator cur, const BidirectionalIterator begin)
	{
		ui_l32 codepoint = static_cast<ui_l32>(*--cur);

		if ((codepoint & 0x80) == 0)
			return static_cast<ui_l32>(codepoint & 0xff);

		if ((codepoint & 0x40) == 0 && cur != begin)
		{
			codepoint = static_cast<ui_l32>((codepoint & 0x3f) | (*--cur << 6));

			if ((codepoint & 0x3800) == 0x3000)	//  2 octets.
				return static_cast<ui_l32>(codepoint & 0x7ff);

			if ((codepoint & 0x3000) == 0x2000 && cur != begin)
			{
				codepoint = static_cast<ui_l32>((codepoint & 0xfff) | (*--cur << 12));

				if ((codepoint & 0xf0000) == 0xe0000)	//  3 octets.
					return static_cast<ui_l32>(codepoint & 0xffff);

				if ((codepoint & 0xc0000) == 0x80000 && cur != begin)
				{
					if ((*--cur & 0xf8) == 0xf0)	//  4 octets.
						return static_cast<ui_l32>((codepoint & 0x3ffff) | ((*cur & 7) << 18));
				}
			}
		}
		return re_detail::constants::invalid_u32value;
	}

	template <typename BidirectionalIterator>
	static SRELL_FORCEINLINE ui_l32 dec_codepoint(BidirectionalIterator &cur, const BidirectionalIterator begin)
	{
		ui_l32 codepoint = static_cast<ui_l32>(*--cur);

		if ((codepoint & 0x80) == 0)
			return static_cast<ui_l32>(codepoint & 0xff);

		if ((codepoint & 0x40) == 0 && cur != begin)
		{
			codepoint = static_cast<ui_l32>((codepoint & 0x3f) | (*--cur << 6));

			//  11 0bbb bbaa aaaa?
			if ((codepoint & 0x3800) == 0x3000)	//  2 octets.
				return static_cast<ui_l32>(codepoint & 0x7ff);

			//  10 bbbb bbaa aaaa?
			if ((codepoint & 0x3000) == 0x2000 && cur != begin)	//  [\x80-\xbf]{2}.
			{
				codepoint = static_cast<ui_l32>((codepoint & 0xfff) | (*--cur << 12));

				//  1110 cccc bbbb bbaa aaaa?
				if ((codepoint & 0xf0000) == 0xe0000)	//  3 octets.
					return static_cast<ui_l32>(codepoint & 0xffff);

				//  10cc cccc bbbb bbaa aaaa?
				if ((codepoint & 0xc0000) == 0x80000 && cur != begin)	//  [\x80-\xbf]{3}.
				{
					if ((*--cur & 0xf8) == 0xf0)	//  4 octets.
						return static_cast<ui_l32>((codepoint & 0x3ffff) | ((*cur & 7) << 18));
						//  d ddcc cccc bbbb bbaa aaaa
				}
			}
		}
		return re_detail::constants::invalid_u32value;
	}

#if !defined(SRELLDBG_NO_BMH)

	template <typename charT2>
	static bool is_trailing(const charT2 cu)
	{
		return (cu & 0xc0) == 0x80;
	}

#endif	//  !defined(SRELLDBG_NO_BMH)

	static ui_l32 to_codeunits(charT out[maxseqlen], ui_l32 cp)
	{
		if (cp < 0x80)
		{
			out[0] = static_cast<charT>(cp);
			return 1;
		}
		else if (cp < 0x800)
		{
			out[0] = static_cast<charT>(((cp >> 6) & 0x1f) | 0xc0);
			out[1] = static_cast<charT>((cp & 0x3f) | 0x80);
			return 2;
		}
		else if (cp < 0x10000)
		{
			out[0] = static_cast<charT>(((cp >> 12) & 0x0f) | 0xe0);
			out[1] = static_cast<charT>(((cp >> 6) & 0x3f) | 0x80);
			out[2] = static_cast<charT>((cp & 0x3f) | 0x80);
			return 3;
		}
//		else	//  if (cp < 0x110000)

		out[0] = static_cast<charT>(((cp >> 18) & 0x07) | 0xf0);
		out[1] = static_cast<charT>(((cp >> 12) & 0x3f) | 0x80);
		out[2] = static_cast<charT>(((cp >> 6) & 0x3f) | 0x80);
		out[3] = static_cast<charT>((cp & 0x3f) | 0x80);
		return 4;
	}

	static ui_l32 firstcodeunit(const ui_l32 cp)
	{
		if (cp < 0x80)
			return cp;

		if (cp < 0x800)
			return static_cast<ui_l32>(((cp >> 6) & 0x1f) | 0xc0);

		if (cp < 0x10000)
			return static_cast<ui_l32>(((cp >> 12) & 0x0f) | 0xe0);

		return static_cast<ui_l32>(((cp >> 18) & 0x07) | 0xf0);
	}
};
template <typename charT> const std::size_t utf8_traits<charT>::maxseqlen;
template <typename charT> const int utf8_traits<charT>::utftype;
template <typename charT> const ui_l32 utf8_traits<charT>::charbit;
template <typename charT> const ui_l32 utf8_traits<charT>::bitsetsize;
template <typename charT> const ui_l32 utf8_traits<charT>::bitsetmask;
template <typename charT> const ui_l32 utf8_traits<charT>::maxcpvalue;
//  utf8_traits

//  utf-16 specific.
template <typename charT>
struct utf16_traits : public utf_traits_core<charT>
{
public:

	//  utf-16 specific.
	static const std::size_t maxseqlen = 2;
	static const int utftype = 16;

	static const ui_l32 charbit = 16;
	static const ui_l32 bitsetsize = 0x10000;
	static const ui_l32 bitsetmask = 0xffff;
	static const ui_l32 maxcpvalue = 0x10ffff;

	template <typename ForwardIterator>
	static SRELL_FORCEINLINE ui_l32 codepoint(ForwardIterator begin, const ForwardIterator end)
	{
		const ui_l32 codeunit = *begin;

		if ((codeunit & 0xdc00) != 0xd800)
			return static_cast<ui_l32>(codeunit & 0xffff);

		if (++begin != end && (*begin & 0xdc00) == 0xdc00)
			return static_cast<ui_l32>((((codeunit & 0x3ff) << 10) | (*begin & 0x3ff)) + 0x10000);

		return static_cast<ui_l32>(codeunit & 0xffff);
	}

	template <typename ForwardIterator>
	static SRELL_FORCEINLINE ui_l32 codepoint_inc(ForwardIterator &begin, const ForwardIterator end)
	{
		const ui_l32 codeunit = *begin++;

		if ((codeunit & 0xdc00) != 0xd800)
			return static_cast<ui_l32>(codeunit & 0xffff);

		if (begin != end && (*begin & 0xdc00) == 0xdc00)
			return static_cast<ui_l32>((((codeunit & 0x3ff) << 10) | (*begin++ & 0x3ff)) + 0x10000);

		return static_cast<ui_l32>(codeunit & 0xffff);
	}

	template <typename BidirectionalIterator>
	static SRELL_FORCEINLINE ui_l32 prevcodepoint(BidirectionalIterator cur, const BidirectionalIterator begin)
	{
		const ui_l32 codeunit = *--cur;

		if ((codeunit & 0xdc00) != 0xdc00 || cur == begin)
			return static_cast<ui_l32>(codeunit & 0xffff);

		if ((*--cur & 0xdc00) == 0xd800)
			return static_cast<ui_l32>((((*cur & 0x3ff) << 10) | (codeunit & 0x3ff)) + 0x10000);

		return static_cast<ui_l32>(codeunit & 0xffff);
	}

	template <typename BidirectionalIterator>
	static SRELL_FORCEINLINE ui_l32 dec_codepoint(BidirectionalIterator &cur, const BidirectionalIterator begin)
	{
		const ui_l32 codeunit = *--cur;

		if ((codeunit & 0xdc00) != 0xdc00 || cur == begin)
			return static_cast<ui_l32>(codeunit & 0xffff);

		if ((*--cur & 0xdc00) == 0xd800)
			return static_cast<ui_l32>((((*cur & 0x3ff) << 10) | (codeunit & 0x3ff)) + 0x10000);
		//else	//  (codeunit & 0xdc00) == 0xdc00 && (*cur & 0xdc00) != 0xd800

		++cur;

		return static_cast<ui_l32>(codeunit & 0xffff);
	}

#if !defined(SRELLDBG_NO_BMH)

	template <typename charT2>
	static bool is_trailing(const charT2 cu)
	{
		return (cu & 0xdc00) == 0xdc00;
	}

#endif	//  !defined(SRELLDBG_NO_BMH)

	static ui_l32 to_codeunits(charT out[maxseqlen], ui_l32 cp)
	{
		if (cp < 0x10000)
		{
			out[0] = static_cast<charT>(cp);
			return 1;
		}
//		else	//  if (cp < 0x110000)

		cp -= 0x10000;
		out[0] = static_cast<charT>(((cp >> 10) & 0x3ff) | 0xd800);
		out[1] = static_cast<charT>((cp & 0x3ff) | 0xdc00);
		return 2;
	}

	static ui_l32 firstcodeunit(const ui_l32 cp)
	{
		if (cp < 0x10000)
			return cp;

		return static_cast<ui_l32>((cp >> 10) + 0xd7c0);
			//  aaaaa bbbbcccc ddddeeee -> AA AAbb bbcc/cc dddd eeee where AAAA = aaaaa - 1.
	}
};
template <typename charT> const std::size_t utf16_traits<charT>::maxseqlen;
template <typename charT> const int utf16_traits<charT>::utftype;
template <typename charT> const ui_l32 utf16_traits<charT>::charbit;
template <typename charT> const ui_l32 utf16_traits<charT>::bitsetsize;
template <typename charT> const ui_l32 utf16_traits<charT>::bitsetmask;
template <typename charT> const ui_l32 utf16_traits<charT>::maxcpvalue;
//  utf16_traits

//  specialisation for char.
template <>
struct utf_traits<char> : public utf_traits_core<char>
{
public:

	template <typename ForwardIterator>
	static ui_l32 codepoint(ForwardIterator begin, const ForwardIterator /* end */)
	{
		return static_cast<ui_l32>(static_cast<unsigned char>(*begin));
	}

	template <typename ForwardIterator>
	static ui_l32 codepoint_inc(ForwardIterator &begin, const ForwardIterator /* end */)
	{
		return static_cast<ui_l32>(static_cast<unsigned char>(*begin++));
	}

	template <typename BidirectionalIterator>
	static ui_l32 prevcodepoint(BidirectionalIterator cur, const BidirectionalIterator /* begin */)
	{
		return static_cast<ui_l32>(static_cast<unsigned char>(*--cur));
	}

	template <typename BidirectionalIterator>
	static ui_l32 dec_codepoint(BidirectionalIterator &cur, const BidirectionalIterator /* begin */)
	{
		return static_cast<ui_l32>(static_cast<unsigned char>(*--cur));
	}

#if !defined(SRELLDBG_NO_BMH)
#endif	//  !defined(SRELLDBG_NO_BMH)
};	//  utf_traits<char>

//  specialisation for signed char.
template <>
struct utf_traits<signed char> : public utf_traits<char>
{
};

//  (signed) short, (signed) int, (signed) long, (signed) long long, ...

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
template <>
struct utf_traits<char16_t> : public utf16_traits<char16_t>
{
};
#endif

#if defined(SRELL_CPP20_CHAR8_ENABLED)
template <>
struct utf_traits<char8_t> : public utf8_traits<char8_t>
{
};
#endif

	}	//  re_detail

//  ... "rei_utf_traits.hpp"]
//  ["regex_traits.hpp" ...

//  28.7, class template regex_traits:
template <class charT>
struct regex_traits
{
public:

	typedef charT char_type;
	typedef std::basic_string<char_type> string_type;
	typedef std::locale locale_type;
//	typedef bitmask_type char_class_type;
	typedef int char_class_type;

	typedef re_detail::utf_traits<charT> utf_traits;

public:

//	regex_traits();

	static std::size_t length(const char_type *p)
	{
		return std::char_traits<charT>::length(p);
	}

	charT translate(const charT c) const
	{
		return c;
	}

	charT translate_nocase(const charT c) const
	{
		return c;
	}

	template <class ForwardIterator>
	string_type transform(ForwardIterator first, ForwardIterator last) const
	{
		return string_type(first, last);
	}

	template <class ForwardIterator>
	string_type transform_primary(ForwardIterator first, ForwardIterator last) const
	{
		return string_type(first, last);
	}

	template <class ForwardIterator>
	string_type lookup_collatename(ForwardIterator first, ForwardIterator last) const
	{
		return string_type(first, last);
	}

	template <class ForwardIterator>
	char_class_type lookup_classname(ForwardIterator /* first */, ForwardIterator /* last */, bool /* icase */ = false) const
	{
		return static_cast<char_class_type>(0);
	}

	bool isctype(const charT /* c */, const char_class_type /* f */) const
	{
		return false;
	}

	int value(const charT /* ch */, const int /* radix */) const
	{
		return -1;
	}

	locale_type imbue(const locale_type /* l */)
	{
		return locale_type();
	}

	locale_type getloc() const
	{
		return locale_type();
	}
};	//  regex_traits

template <class charT>
struct u8regex_traits : public regex_traits<charT>
{
	typedef re_detail::utf8_traits<charT> utf_traits;
};

template <class charT>
struct u16regex_traits : public regex_traits<charT>
{
	typedef re_detail::utf16_traits<charT> utf_traits;
};

//  ... "regex_traits.hpp"]
//  ["rei_memory.hpp" ...

	namespace re_detail
	{
/*
 *  Similar to std::basic_string, except for:
 *    a. only allocates memory, does not initialise it.
 *    b. uses realloc() to avoid moving data as much as possible when
 *       resizing an allocated buffer.
 */
template <typename ElemT>
class simple_array
{
public:

	typedef ElemT value_type;
	typedef std::size_t size_type;
	typedef ElemT &reference;
	typedef const ElemT &const_reference;
	typedef ElemT *pointer;
	typedef const ElemT *const_pointer;

	static const size_type npos = static_cast<size_type>(-1);

public:

	simple_array()
		: buffer_(NULL)
		, size_(0)
		, capacity_(0)
	{
	}

	simple_array(const size_type initsize)
		: buffer_(NULL)
		, size_(0)
		, capacity_(0)
	{
		if (initsize)
		{
			buffer_ = static_cast<pointer>(std::malloc(initsize * sizeof (ElemT)));

			if (buffer_ != NULL)
				size_ = capacity_ = initsize;
			else
				throw std::bad_alloc();
		}
	}

	simple_array(const simple_array &right, size_type pos, size_type len = npos)
		: buffer_(NULL)
		, size_(0)
		, capacity_(0)
	{
		if (pos > right.size_)
			pos = right.size_;

		{
			const size_type len2 = right.size_ - pos;
			if (len > len2)
				len = len2;
		}

		if (len)
		{
			buffer_ = static_cast<pointer>(std::malloc(len * sizeof (ElemT)));

			if (buffer_ != NULL)
			{
				for (capacity_ = len; size_ < capacity_;)
					buffer_[size_++] = right[pos++];
			}
			else
			{
				throw std::bad_alloc();
			}
		}
	}

	simple_array(const simple_array &right)
		: buffer_(NULL)
		, size_(0)
		, capacity_(0)
	{
		operator=(right);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	simple_array(simple_array &&right) SRELL_NOEXCEPT
		: buffer_(right.buffer_)
		, size_(right.size_)
		, capacity_(right.capacity_)
	{
		right.size_ = 0;
		right.capacity_ = 0;
		right.buffer_ = NULL;
	}
#endif

	simple_array &operator=(const simple_array &right)
	{
		if (this != &right)
		{
			resize(right.size_);
			for (size_type i = 0; i < right.size_; ++i)
				buffer_[i] = right.buffer_[i];
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	simple_array &operator=(simple_array &&right) SRELL_NOEXCEPT
	{
		if (this != &right)
		{
			if (this->buffer_ != NULL)
				std::free(this->buffer_);

			this->size_ = right.size_;
			this->capacity_ = right.capacity_;
			this->buffer_ = right.buffer_;

			right.size_ = 0;
			right.capacity_ = 0;
			right.buffer_ = NULL;
		}
		return *this;
	}
#endif

	~simple_array()
	{
		if (buffer_ != NULL)
			std::free(buffer_);
	}

	size_type size() const
	{
		return size_;
	}

	void clear()
	{
		size_ = 0;
	}

	void resize(const size_type newsize)
	{
		if (newsize > capacity_)
			reserve(newsize);

		size_ = newsize;
	}

	void resize(const size_type newsize, const ElemT &type)
	{
		size_type oldsize = size_;

		resize(newsize);
		for (; oldsize < size_; ++oldsize)
			buffer_[oldsize] = type;
	}

	reference operator[](const size_type pos)
	{
		return buffer_[pos];
	}

	const_reference operator[](const size_type pos) const
	{
		return buffer_[pos];
	}

	void push_back(const_reference n)
	{
		const size_type oldsize = size_;

		if (++size_ > capacity_)
			reserve(size_);

		buffer_[oldsize] = n;
	}

	void push_backncr(const ElemT e)
	{
		push_back(e);
	}

	const_reference back() const
	{
		return buffer_[size_ - 1];
	}

	reference back()
	{
		return buffer_[size_ - 1];
	}

	void pop_back()
	{
		--size_;
	}

	simple_array &operator+=(const simple_array &right)
	{
		return append(right);
	}

	simple_array &append(const size_type size, const ElemT &type)
	{
		resize(size_ + size, type);
		return *this;
	}

	simple_array &append(const simple_array &right)
	{
		size_type oldsize = size_;

		resize(size_ + right.size_);
		for (size_type i = 0; i < right.size_; ++i, ++oldsize)
			buffer_[oldsize] = right.buffer_[i];

		return *this;
	}

	simple_array &append(const simple_array &right, size_type pos, size_type len /* = npos */)
	{
		{
			const size_type len2 = right.size_ - pos;
			if (len > len2)
				len = len2;
		}

		size_type oldsize = size_;

		resize(size_ + len);
		len += pos;	//  end.
		for (; pos < len; ++oldsize, ++pos)
			buffer_[oldsize] = right.buffer_[pos];

		return *this;
	}

	void erase(const size_type pos)
	{
		if (pos < size_)
		{
			std::memmove(buffer_ + pos, buffer_ + pos + 1, (size_ - pos - 1) * sizeof (ElemT));
			--size_;
		}
	}
	void erase(const size_type pos, const size_type len)
	{
		if (pos < size_)
		{
			size_type rmndr = size_ - pos;

			if (rmndr > len)
			{
				rmndr -= len;
				std::memmove(buffer_ + pos, buffer_ + pos + len, rmndr * sizeof (ElemT));
				size_ -= len;
			}
			else
				size_ = pos;
		}
	}

	//  For rei_compiler class.
	void insert(const size_type pos, const ElemT &type)
	{
		move_forward(pos, 1);
		buffer_[pos] = type;
	}

	void insert(size_type pos, const simple_array &right)
	{
		move_forward(pos, right.size_);
		for (size_type i = 0; i < right.size_; ++i, ++pos)
			buffer_[pos] = right.buffer_[i];
	}

	void insert(size_type destpos, const simple_array &right, size_type srcpos, size_type srclen = npos)
	{
		{
			const size_type len2 = right.size_ - srcpos;
			if (srclen > len2)
				srclen = len2;
		}

		move_forward(destpos, srclen);
		srclen += srcpos;	//  srcend.
		for (; srcpos < srclen; ++destpos, ++srcpos)
			buffer_[destpos] = right.buffer_[srcpos];
	}

	simple_array &replace(size_type pos, size_type count, const simple_array &right)
	{
		if (count < right.size_)
			move_forward(pos + count, right.size_ - count);
		else if (count > right.size_)
		{
			const pointer base = buffer_ + pos;

			std::memmove(base + right.size_, base + count, (size_ - pos - count) * sizeof (ElemT));
			size_ -= count - right.size_;
		}

		for (size_type i = 0; i < right.size_; ++pos, ++i)
			buffer_[pos] = right[i];

		return *this;
	}

	size_type find(const value_type c, size_type pos = 0) const
	{
		for (; pos <= size_; ++pos)
			if (buffer_[pos] == c)
				return pos;

		return npos;
	}

	const_pointer data() const
	{
		return buffer_;
	}

	size_type max_size() const
	{
		return maxsize_;
	}

	void swap(simple_array &right)
	{
		if (this != &right)
		{
			const pointer tmpbuffer = this->buffer_;
			const size_type tmpsize = this->size_;
			const size_type tmpcapacity = this->capacity_;

			this->buffer_ = right.buffer_;
			this->size_ = right.size_;
			this->capacity_ = right.capacity_;

			right.buffer_ = tmpbuffer;
			right.size_ = tmpsize;
			right.capacity_ = tmpcapacity;
		}
	}

private:

	void reserve(const size_type newsize)
	{
		if (newsize <= maxsize_)
		{
//			capacity_ = newsize + (newsize >> 1);	//  newsize * 1.5.
			capacity_ = ((newsize >> 8) + 1) << 8;	//  Round up to a multiple of 256.

			if (capacity_ > maxsize_)
				capacity_ = maxsize_;

			const size_type newsize_in_byte = capacity_ * sizeof (ElemT);
			const pointer oldbuffer = buffer_;

			buffer_ = static_cast<pointer>(std::realloc(buffer_, newsize_in_byte));
			if (buffer_ != NULL)
				return;

			//  Even if realloc() failed, already-existing buffer remains valid.
			std::free(oldbuffer);
//			buffer_ = NULL;
			size_ = capacity_ = 0;
		}
		throw std::bad_alloc();
	}

	void move_forward(const size_type pos, const size_type count)
	{
		const size_type oldsize = size_;

		resize(size_ + count);

		if (pos < oldsize)
		{
			const pointer base = buffer_ + pos;

			std::memmove(base + count, base, (oldsize - pos) * sizeof (ElemT));
		}
	}

private:

	pointer buffer_;
	size_type size_;
	size_type capacity_;

//	static const size_type maxsize_ = (npos - sizeof (simple_array)) / sizeof (ElemT);
	static const size_type maxsize_ = (npos - sizeof (pointer) - sizeof (size_type) * 2) / sizeof (ElemT) / 2;
};
template <typename ElemT>
const typename simple_array<ElemT>::size_type simple_array<ElemT>::npos;
//  simple_array

	}	//  namespace re_detail

//  ... "rei_memory.hpp"]
//  ["rei_bitset.hpp" ...

	namespace re_detail
	{

//  Always uses a heap instead of the stack.
template <const std::size_t Bits>
class bitset
{
private:

	typedef unsigned long array_type;

public:

	bitset()
		: buffer_(static_cast<array_type *>(std::malloc(size_in_byte_)))
	{
		if (buffer_ != NULL)
		{
			reset();
			return;
		}
		throw std::bad_alloc();
	}

	bitset(const bitset &right)
		: buffer_(static_cast<array_type *>(std::malloc(size_in_byte_)))
	{
		if (buffer_ != NULL)
		{
			operator=(right);
			return;
		}
		throw std::bad_alloc();
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	bitset(bitset &&right) SRELL_NOEXCEPT
		: buffer_(right.buffer_)
	{
		right.buffer_ = NULL;
	}
#endif

	bitset &operator=(const bitset &right)
	{
		if (this != &right)
		{
//			for (std::size_t i = 0; i < arraylength_; ++i)
//				buffer_[i] = right.buffer_[i];
			std::memcpy(buffer_, right.buffer_, size_in_byte_);
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	bitset &operator=(bitset &&right) SRELL_NOEXCEPT
	{
		if (this != &right)
		{
			if (this->buffer_ != NULL)
				std::free(this->buffer_);

			this->buffer_ = right.buffer_;
			right.buffer_ = NULL;
		}
		return *this;
	}
#endif

	~bitset()
	{
		if (buffer_ != NULL)
			std::free(buffer_);
	}

	bitset &reset()
	{
		std::memset(buffer_, 0, size_in_byte_);
		return *this;
	}

	bitset &reset(const std::size_t bit)
	{
		buffer_[bit / bits_per_elem_] &= ~(1ul << (bit & bitmask_));
		return *this;
	}

	bitset &set(const std::size_t bit)
	{
		buffer_[bit / bits_per_elem_] |= (1ul << (bit & bitmask_));
		return *this;
	}

#if 0
	void set_range(const std::size_t firstbit, const std::size_t lastbit)
	{
		const std::size_t lastelemidx = lastbit / bits_per_elem_;
		std::size_t firstelemidx = firstbit / bits_per_elem_;
		const array_type lastelemmask = ~(allbits1_ << ((lastbit & bitmask_) + 1));
		array_type ormask = allbits1_ << (firstbit & bitmask_);

		if (firstelemidx < lastelemidx)
		{
			buffer_[firstelemidx] |= ormask;
			ormask = allbits1_;

			for (++firstelemidx; firstelemidx < lastelemidx; ++firstelemidx)
				buffer_[firstelemidx] |= ormask;
		}
		ormask &= lastelemmask;
		buffer_[lastelemidx] |= ormask;

	}
#endif

	bool test(const std::size_t bit) const
	{
		return (buffer_[bit / bits_per_elem_] & (1ul << (bit & bitmask_))) != 0;
	}

	bool operator[](const std::size_t bit) const
	{
		return (buffer_[bit / bits_per_elem_] & (1ul << (bit & bitmask_))) != 0;
	}

	bitset<Bits> &flip()
	{
		for (std::size_t i = 0; i < arraylength_; ++i)
			buffer_[i] = ~buffer_[i];
		return *this;
	}

	void swap(bitset &right)
	{
		if (this != &right)
		{
			array_type *const tmpbuffer = this->buffer_;
			this->buffer_ = right.buffer_;
			right.buffer_ = tmpbuffer;
		}
	}

private:

#if defined(__cpp_constexpr)
	static constexpr std::size_t pow2leN(const std::size_t n, const std::size_t p2)
	{
		return ((p2 << 1) == 0 || (p2 << 1) > n) ? p2 : pow2leN(n, p2 << 1);
	}
	static const std::size_t bits_per_elem_ = pow2leN(CHAR_BIT * sizeof (array_type), 8);
#else
	static const std::size_t bpe_tmp_ = CHAR_BIT * sizeof (array_type);
	static const std::size_t bits_per_elem_ = bpe_tmp_ >= 64 ? 64 : (bpe_tmp_ >= 32 ? 32 : (bpe_tmp_ >= 16 ? 16 : 8));
#endif
	static const std::size_t bitmask_ = bits_per_elem_ - 1;
	static const std::size_t arraylength_ = (Bits + bitmask_) / bits_per_elem_;
	static const std::size_t size_in_byte_ = arraylength_ * sizeof (array_type);
	static const array_type allbits1_ = ~static_cast<array_type>(0);

	array_type *buffer_;
};

	}	//  namespace re_detail

//  ... "rei_bitset.hpp"]
//  ["rei_ucf.hpp" ...

	namespace re_detail
	{

#if !defined(SRELL_NO_UNICODE_ICASE)

		namespace ucf_constants
		{

#include "srell_ucfdata2.h"

		}	//  namespace ucf_constants

		namespace ucf_internal
		{

typedef ucf_constants::unicode_casefolding<ui_l32, ui_l32> ucfdata;

		}	//  namespace ucf_internal
#endif	//  !defined(SRELL_NO_UNICODE_ICASE)

		namespace ucf_constants
		{
#if !defined(SRELL_NO_UNICODE_ICASE)
			static const ui_l32 rev_maxset = ucf_internal::ucfdata::rev_maxset;
#else
			static const ui_l32 rev_maxset = 2;
#endif
		}	//  namespace ucf_constants

class unicode_case_folding
{
public:

	static ui_l32 do_casefolding(const ui_l32 cp)
	{
#if !defined(SRELL_NO_UNICODE_ICASE)
		if (cp <= ucf_internal::ucfdata::ucf_maxcodepoint)
			return cp + ucf_internal::ucfdata::ucf_deltatable[ucf_internal::ucfdata::ucf_segmenttable[cp >> 8] + (cp & 0xff)];
#else
		if (cp >= char_alnum::ch_A && cp <= char_alnum::ch_Z)	//  'A' && 'Z'
			return static_cast<ui_l32>(cp - char_alnum::ch_A + char_alnum::ch_a);	//  - 'A' + 'a'
#endif
		return cp;
	}

	static ui_l32 do_caseunfolding(ui_l32 out[ucf_constants::rev_maxset], const ui_l32 cp)
	{
#if !defined(SRELL_NO_UNICODE_ICASE)
		ui_l32 count = 0u;

		if (cp <= ucf_internal::ucfdata::rev_maxcodepoint)
		{
			const ui_l32 offset_of_charset = ucf_internal::ucfdata::rev_indextable[ucf_internal::ucfdata::rev_segmenttable[cp >> 8] + (cp & 0xff)];
			const ui_l32 *ptr = &ucf_internal::ucfdata::rev_charsettable[offset_of_charset];

			for (; *ptr != cfcharset_eos_ && count < ucf_constants::rev_maxset; ++ptr, ++count)
				out[count] = *ptr;
		}
		if (count == 0u)
			out[count++] = cp;

		return count;
#else
		const ui_l32 nocase = static_cast<ui_l32>(cp | constants::asc_icase);

		out[0] = cp;
//		if (nocase >= char_alnum::ch_A && nocase <= char_alnum::ch_Z)
		if (nocase >= char_alnum::ch_a && nocase <= char_alnum::ch_z)
		{
			out[1] = static_cast<ui_l32>(cp ^ constants::asc_icase);
			return 2u;
		}
		return 1u;
#endif
	}

	static ui_l32 count_caseunfolding(const ui_l32 cp)
	{
#if !defined(SRELL_NO_UNICODE_ICASE)
		ui_l32 count = 0u;

		if (cp <= ucf_internal::ucfdata::rev_maxcodepoint)
		{
			const ui_l32 offset_of_charset = ucf_internal::ucfdata::rev_indextable[ucf_internal::ucfdata::rev_segmenttable[cp >> 8] + (cp & 0xff)];
			const ui_l32 *ptr = &ucf_internal::ucfdata::rev_charsettable[offset_of_charset];

			for (; *ptr != cfcharset_eos_; ++ptr)
				++count;
		}
		if (count == 0u)
			++count;

		return count;
#else
		const ui_l32 nocase = static_cast<ui_l32>(cp | constants::asc_icase);

		return (nocase >= char_alnum::ch_a && nocase <= char_alnum::ch_z) ? 2u : 1u;
#endif
	}

	unicode_case_folding &operator=(const unicode_case_folding &)
	{
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	unicode_case_folding &operator=(unicode_case_folding &&) SRELL_NOEXCEPT
	{
		return *this;
	}
#endif

	void swap(unicode_case_folding & /* right */)
	{
	}

private:

#if !defined(SRELL_NO_UNICODE_ICASE)
	static const ui_l32 cfcharset_eos_ = ucf_internal::ucfdata::eos;
#endif

public:	//  For debug.

	void print_tables() const;
};
//  unicode_case_folding

	}	//  namespace re_detail

//  ... "rei_ucf.hpp"]
//  ["rei_up.hpp" ...

	namespace re_detail
	{

#if !defined(SRELL_NO_UNICODE_PROPERTY)

		namespace up_constants
		{

#include "srell_updata3.h"

			static const ui_l32 error_property = static_cast<ui_l32>(-1);
		}	//  namespace up_constants

		namespace up_internal
		{
			typedef int up_type;
			typedef const char *pname_type;

			struct pnameno_map_type
			{
				pname_type name;
				up_type pno;
			};

			struct posinfo
			{
				ui_l32 offset;
				ui_l32 numofpairs;
			};

			typedef up_constants::unicode_property_data<
				pnameno_map_type,
				posinfo,
				ui_l32
				> updata;

		}	//  namespace up_internal

//template <typename PairType>
class unicode_property
{
public:

	typedef simple_array<char> pstring;

	unicode_property()
	{
	}

	unicode_property &operator=(const unicode_property &)
	{
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	unicode_property &operator=(unicode_property &&) SRELL_NOEXCEPT
	{
		return *this;
	}
#endif

	static ui_l32 lookup_property(const pstring &name, const pstring &value)
	{
		up_type ptype = name.size() > 1 ? lookup_property_name(name) : up_constants::uptype_gc;
		const posinfo *pos = &updata::positiontable[ptype];
		ui_l32 pno = lookup_property_value(value, pos->offset, pos->numofpairs);

		if (pno == upid_error && name.size() < 2)
		{
			ptype = up_constants::uptype_bp;
			pos = &updata::positiontable[ptype];
			pno = lookup_property_value(value, pos->offset, pos->numofpairs);
		}

		return pno != upid_error ? pno : up_constants::error_property;
	}

	static ui_l32 ranges_offset(const ui_l32 property_number)
	{
		return updata::positiontable[property_number].offset;
	}

	static ui_l32 number_of_ranges(const ui_l32 property_number)
	{
		return updata::positiontable[property_number].numofpairs;
	}

	static const ui_l32 *ranges_address(const ui_l32 pno)
	{
		return &updata::rangetable[ranges_offset(pno) << 1];
	}

	static bool is_valid_pno(const ui_l32 pno)
	{
		return pno != up_constants::error_property && pno <= max_property_number;
	}

	static bool is_pos(const ui_l32 pno)
	{
		return pno > max_property_number && pno <= max_pos_number;
	}

private:

	typedef up_internal::up_type up_type;
	typedef up_internal::pname_type pname_type;
	typedef up_internal::pnameno_map_type pnameno_map_type;
	typedef up_internal::posinfo posinfo;
	typedef up_internal::updata updata;

	static up_type lookup_property_name(const pstring &name)
	{
		return lookup_property_value(name, 1, updata::propertynumbertable[0].pno);
	}

	static ui_l32 lookup_property_value(const pstring &value, const ui_l32 offset, ui_l32 count)
	{
		const pnameno_map_type *base = &updata::propertynumbertable[offset];

		while (count)
		{
			ui_l32 mid = count >> 1;
			const pnameno_map_type &map = base[mid];
			const int cmp = compare(value, map.name);

			if (cmp < 0)
			{
				count = mid;
			}
			else if (cmp > 0)
			{
				++mid;
				count -= mid;
				base += mid;
			}
			else	//if (cmp == 0)
				return static_cast<ui_l32>(map.pno);
		}
		return upid_error;
	}

	static int compare(const pstring &value, pname_type pname)
	{
		for (pstring::size_type i = 0;; ++i, ++pname)
		{
			if (value[i] == 0)
				return (*pname == 0) ? 0 : (value[i] < *pname ? -1 : 1);

			if (value[i] != *pname)
				return value[i] < *pname ? -1 : 1;
		}
	}

private:

	static const ui_l32 max_property_number = static_cast<ui_l32>(up_constants::upid_max_property_number);
	static const ui_l32 max_pos_number = static_cast<ui_l32>(up_constants::upid_max_pos_number);
#if (SRELL_UPDATA_VERSION > 300)
	static const ui_l32 upid_error = static_cast<ui_l32>(up_constants::upid_error);
#else
	static const ui_l32 upid_error = static_cast<ui_l32>(up_constants::upid_unknown);
#endif
};
//  unicode_property

#endif	//  !defined(SRELL_NO_UNICODE_PROPERTY)
	}	//  namespace re_detail

//  ... "rei_up.hpp"]
//  ["rei_range_pair.hpp" ...

	namespace re_detail
	{

struct range_pair	//  , public std::pair<charT, charT>
{
	ui_l32 second;
	ui_l32 first;

	void set(const ui_l32 min, const ui_l32 max)
	{
		this->first = min;
		this->second = max;
	}

	void set(const ui_l32 minmax)
	{
		this->first = minmax;
		this->second = minmax;
	}

	bool is_range_valid() const
	{
		return first <= second;
	}

	bool operator==(const range_pair &right) const
	{
		return this->first == right.first && this->second == right.second;
	}

	bool operator<(const range_pair &right) const
	{
		return this->second < right.first;	//  This assumes that optimise() has been called.
	}

	void swap(range_pair &right)
	{
		const range_pair tmp = *this;
		*this = right;
		right = tmp;
	}

	bool unify_range(const range_pair &right)
	{
		range_pair &left = *this;

		if (right.first <= left.second || left.second + 1 == right.first)	//  r1 <= l2 || l2+1 == r1
		{
			//  l1 l2+1 < r1 r2 excluded.

			if (left.first <= right.second || right.second + 1 == left.first)	//  l1 <= r2 || r2+1 == l1
			{
				//  r1 r2+1 < l1 l2 excluded.

				if (left.first > right.first)
					left.first = right.first;

				if (left.second < right.second)
					left.second = right.second;

				return true;
			}
		}
		return false;
	}
};
//  range_pair

struct range_pair_helper : public range_pair
{
	range_pair_helper(const ui_l32 min, const ui_l32 max)
	{
		this->first = min;
		this->second = max;
	}

	range_pair_helper(const ui_l32 minmax)
	{
		this->first = minmax;
		this->second = minmax;
	}
};
//  range_pair_helper

struct range_pairs	//  : public simple_array<range_pair>
{
public:

	typedef simple_array<range_pair> array_type;
	typedef array_type::size_type size_type;

	range_pairs()
	{
	}

	range_pairs(const range_pairs &rp) : rparray_(rp.rparray_)
	{
	}

	range_pairs &operator=(const range_pairs &rp)
	{
		rparray_.operator=(rp.rparray_);
		return *this;
	}

	range_pairs(const size_type initsize) : rparray_(initsize)
	{
	}

	range_pairs(const range_pairs &right, size_type pos, size_type size)
		: rparray_(right.rparray_, pos, size)
	{
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	range_pairs(range_pairs &&rp) SRELL_NOEXCEPT
		: rparray_(std::move(rp.rparray_))
	{
	}

	range_pairs &operator=(range_pairs &&rp) SRELL_NOEXCEPT
	{
		rparray_.operator=(std::move(rp.rparray_));
		return *this;
	}
#endif

	void clear()
	{
		rparray_.clear();
	}

	size_type size() const
	{
		return rparray_.size();
	}

	const range_pair &operator[](const size_type pos) const
	{
		return rparray_[pos];
	}
	range_pair &operator[](const size_type pos)
	{
		return rparray_[pos];
	}

	void resize(const size_type size)
	{
		rparray_.resize(size);
	}

	void swap(range_pairs &right)
	{
		rparray_.swap(right.rparray_);
	}

	void set_solerange(const range_pair &right)
	{
		rparray_.clear();
		rparray_.push_back(right);
	}

	void append_newclass(const range_pairs &right)
	{
		rparray_.append(right.rparray_);
	}

	void append_newpair(const range_pair &right)
	{
		rparray_.push_back(right);
	}

	void join(const range_pair &right)
	{
		size_type pos = 0;

		for (; pos < rparray_.size(); ++pos)
		{
			range_pair &curpair = rparray_[pos];

			if (curpair.unify_range(right))
			{
				for (++pos; pos < rparray_.size();)
				{
					if (curpair.unify_range(rparray_[pos]))
						rparray_.erase(pos);
					else
						break;
				}
				return;
			}
			if (right.second < curpair.first)
				break;
		}
		rparray_.insert(pos, right);
	}

	void merge(const range_pairs &right)
	{
		for (size_type i = 0; i < right.size(); ++i)
			join(right[i]);
	}

	bool same(ui_l32 pos, const ui_l32 count, const range_pairs &right) const
	{
		if (count == right.size())
		{
			for (ui_l32 i = 0; i < count; ++i, ++pos)
				if (!(rparray_[pos] == right[i]))
					return false;

			return true;
		}
		return false;
	}

	int relationship(const range_pairs &right) const
	{
		if (rparray_.size() == right.rparray_.size())
		{
			for (size_type i = 0; i < rparray_.size(); ++i)
			{
				if (!(this->rparray_[i] == right.rparray_[i]))
				{
					if (i == 0)
						goto check_overlap;

					return 1;	//  Overlapped.
				}
			}
			return 0;	//  Same.
		}
		check_overlap:
		return is_overlap(right) ? 1 : 2;	//  Overlapped or exclusive.
	}

	void negation()
	{
		ui_l32 begin = 0;
		range_pairs newpairs;

		for (size_type i = 0; i < rparray_.size(); ++i)
		{
			const range_pair &range = rparray_[i];

			if (begin < range.first)
				newpairs.join(range_pair_helper(begin, range.first - 1));

			begin = range.second + 1;
		}

		if (begin <= constants::unicode_max_codepoint)
			newpairs.join(range_pair_helper(begin, constants::unicode_max_codepoint));

		*this = newpairs;
	}

	bool is_overlap(const range_pairs &right) const
	{
		for (size_type i = 0; i < rparray_.size(); ++i)
		{
			const range_pair &leftrange = rparray_[i];

			for (size_type j = 0; j < right.size(); ++j)
			{
				const range_pair &rightrange = right[j];

				if (rightrange.first <= leftrange.second)	//  Excludes l1 l2 < r1 r2.
					if (leftrange.first <= rightrange.second)	//  Excludes r1 r2 < l1 l2.
						return true;
			}
		}
		return false;
	}

	void load_from_memory(const ui_l32 *array, ui_l32 number_of_pairs)
	{
		for (; number_of_pairs; --number_of_pairs, array += 2)
			join(range_pair_helper(array[0], array[1]));
	}

	void make_caseunfoldedcharset()
	{
		ui_l32 table[ucf_constants::rev_maxset] = {};
		bitset<constants::unicode_max_codepoint + 1> bs;

		for (size_type i = 0; i < rparray_.size(); ++i)
		{
			const range_pair &range = rparray_[i];

			for (ui_l32 ucp = range.first; ucp <= range.second; ++ucp)
			{
				const ui_l32 setnum = unicode_case_folding::do_caseunfolding(table, ucp);

				for (ui_l32 j = 0; j < setnum; ++j)
					bs.set(table[j]);
			}
		}
		load_from_bitset(bs);
	}

	//  For updataout.hpp.
	void remove_range(const range_pair &right)
	{
		for (size_type pos = 0; pos < rparray_.size();)
		{
			range_pair &left = rparray_[pos];

			if (right.first <= left.first && left.first <= right.second)	//  r1 <= l1 <= r2.
			{
				if (left.second > right.second)	//  r1 <= l1 <= r2 < l2.
				{
					left.first = right.second + 1;	//  carry doesn't happen.
					++pos;
				}
				else	//  r1 <= l1 <= l2 <= r2.
					rparray_.erase(pos);
			}
			else if (right.first <= left.second && left.second <= right.second)	//  r1 <= l2 <= r2.
			{
				if (left.first < right.first)	//  l1 < r1 <= l2 <= r2.
				{
					left.second = right.first - 1;
					++pos;
				}
				else	//  r1 <= l1 <= l2 <= r2.
					rparray_.erase(pos);
			}
			else if (left.first < right.first && right.second < left.second)	//  l1 < r1 && r2 < l2.
			{
				range_pair newrange(left);

				left.second = right.first - 1;
				newrange.first = right.second + 1;
				rparray_.insert(++pos, newrange);
				++pos;
			}
			else
				++pos;
		}
	}

//	template <typename ucf>
	ui_l32 consists_of_one_character(const bool icase) const
	{
		if (rparray_.size() >= 1)
		{
			ui_l32 (*const casefolding_func)(const ui_l32) = !icase ? do_nothing : unicode_case_folding::do_casefolding;
			const ui_l32 ucp1st = casefolding_func(rparray_[0].first);

			for (size_type no = 0; no < rparray_.size(); ++no)
			{
				const range_pair &cr = rparray_[no];

				for (ui_l32 ucp = cr.first;; ++ucp)
				{
					if (ucp1st != casefolding_func(ucp))
						return constants::invalid_u32value;

					if (ucp == cr.second)
						break;
				}
			}
			return ucp1st;
		}
		return constants::invalid_u32value;
	}

	void split_ranges(range_pairs &kept, range_pairs &removed, const range_pairs &rightranges) const
	{
		range_pair newpair;

		kept.rparray_ = this->rparray_;	//  Subtraction set.
		removed.clear();	//  Intersection set.

		for (size_type i = 0;; ++i)
		{
			RETRY_SAMEINDEXNO:
			if (i >= kept.rparray_.size())
				break;

			range_pair &left = kept.rparray_[i];

			for (size_type j = 0; j < rightranges.rparray_.size(); ++j)
			{
				const range_pair &right = rightranges.rparray_[j];

				if (right.first <= left.second)	//  Excludes l1 l2 < r1 r2.
				{
					if (left.first <= right.second)	//  Excludes r1 r2 < l1 l2.
					{
						if (left.first < right.first)
						{
							if (right.second < left.second)
							{
								removed.join(range_pair_helper(right.first, right.second));

								newpair.set(right.second + 1, left.second);
								left.second = right.first - 1;
								kept.rparray_.insert(i + 1, newpair);
							}
							else
							{
								removed.join(range_pair_helper(right.first, left.second));
								left.second = right.first - 1;
							}
						}
						else if (right.second < left.second)
						{
							removed.join(range_pair_helper(left.first, right.second));
							left.first = right.second + 1;
						}
						else
						{
							removed.join(range_pair_helper(left.first, left.second));
							kept.rparray_.erase(i);
							goto RETRY_SAMEINDEXNO;
						}
					}
				}
				else
					break;
			}
		}
	}

#if defined(SRELLDBG_NO_BITSET)
	bool is_included(const ui_l32 ch) const
	{
#if 01
		const range_pair *const end = rparray_.data() + rparray_.size();

		for (const range_pair *cur = rparray_.data(); cur != end; ++cur)
		{
			if (ch <= cur->second)
				return ch >= cur->first;
#else
		for (size_type i = 0; i < rparray_.size(); ++i)
		{
			if (rparray_[i].is_included(ch))
				return true;
#endif
		}
		return false;
	}
#endif	//  defined(SRELLDBG_NO_BITSET)

	//  For multiple_range_pairs functions.

	bool is_included_ls(const ui_l32 pos, ui_l32 count, const ui_l32 c) const
	{
		const range_pair *cur = &rparray_[pos];

		for (; count; ++cur, --count)
		{
			if (c <= cur->second)
				return c >= cur->first;
		}
		return false;
	}

	bool is_included(const ui_l32 pos, ui_l32 count, const ui_l32 c) const
	{
		const range_pair *base = &rparray_[pos];

		while (count)
		{
			ui_l32 mid = count >> 1;
			const range_pair &rp = base[mid];

			if (c <= rp.second)
			{
				if (c >= rp.first)
					return true;

				count = mid;
			}
			else
			{
				++mid;
				count -= mid;
				base += mid;
			}
		}
		return false;
	}

	void replace(const size_type pos, const size_type count, const range_pairs &right)
	{
		rparray_.replace(pos, count, right.rparray_);
	}

#if !defined(SRELLDBG_NO_CCPOS)

	//  For Eytzinger layout functions.

	bool is_included_el(ui_l32 pos, const ui_l32 len, const ui_l32 c) const
	{
		const range_pair *const base = &rparray_[pos];

#if defined(__GNUC__)
		__builtin_prefetch(base);
#endif
		for (pos = 0; pos < len;)
		{
			const range_pair &rp = base[pos];

			if (c <= rp.second)
			{
				if (c >= rp.first)
					return true;

				pos = (pos << 1) + 1;
			}
			else
			{
				pos = (pos << 1) + 2;
			}
		}
		return false;
	}

	ui_l32 create_el(const range_pair *srcbase, const ui_l32 srcsize)
	{
		const ui_l32 basepos = static_cast<ui_l32>(rparray_.size());

		rparray_.resize(basepos + srcsize);
		set_eytzinger_layout(0, srcbase, srcsize, &rparray_[basepos], 0);

		return srcsize;
	}

#endif	//  !defined(SRELLDBG_NO_CCPOS)

	ui_l32 total_codepoints() const
	{
		ui_l32 num = 0;

		for (size_type no = 0; no < rparray_.size(); ++no)
		{
			const range_pair &cr = rparray_[no];

			num += cr.second - cr.first + 1;
		}
		return num;
	}

private:

#if !defined(SRELLDBG_NO_CCPOS)

	ui_l32 set_eytzinger_layout(ui_l32 srcpos, const range_pair *const srcbase, const ui_l32 srclen,
		range_pair *const destbase, const ui_l32 destpos)
	{
		if (destpos < srclen)
		{
			const ui_l32 nextpos = (destpos << 1) + 1;

			srcpos = set_eytzinger_layout(srcpos, srcbase, srclen, destbase, nextpos);
			destbase[destpos] = srcbase[srcpos++];
			srcpos = set_eytzinger_layout(srcpos, srcbase, srclen, destbase, nextpos + 1);
		}
		return srcpos;
	}

#endif	//  !defined(SRELLDBG_NO_CCPOS)

	static ui_l32 do_nothing(const ui_l32 cp)
	{
		return cp;
	}

	template <typename BitSetT>
	void load_from_bitset(const BitSetT &bs)
	{
		ui_l32 begin = constants::invalid_u32value;
		range_pairs newranges;

		for (ui_l32 ucp = 0;; ++ucp)
		{
			if (ucp > constants::unicode_max_codepoint || !bs.test(ucp))
			{
				if (begin != constants::invalid_u32value)
				{
					newranges.join(range_pair_helper(begin, ucp - 1));
					begin = constants::invalid_u32value;
				}
				if (ucp > constants::unicode_max_codepoint)
					break;
			}
			else if (begin == constants::invalid_u32value && bs.test(ucp))
				begin = ucp;
		}
		rparray_.swap(newranges.rparray_);
	}

	array_type rparray_;

public:	//  For debug.

	void print_pairs(const int, const char *const = NULL, const char *const = NULL) const;
};
//  range_pairs

	}	//  namespace re_detail

//  ... "rei_range_pair.hpp"]
//  ["rei_char_class.hpp" ...

	namespace re_detail
	{

#if !defined(SRELL_NO_UNICODE_PROPERTY)

//  For RegExpIdentifierStart and RegExpIdentifierPart
struct identifier_charclass
{
public:

	void clear()
	{
		char_class_.clear();
		char_class_pos_.clear();
	}

	void setup()
	{
		if (char_class_pos_.size() == 0)
		{
			static const ui_l32 additions[] = {
				//  reg_exp_identifier_start, reg_exp_identifier_part.
				0x24, 0x24, 0x5f, 0x5f, 0x200c, 0x200d	//  '$' '_' <ZWNJ>-<ZWJ>
			};
			range_pairs ranges;

			//  For reg_exp_identifier_start.
			{
				const ui_l32 *const IDs_address = unicode_property::ranges_address(upid_bp_ID_Start);
				const ui_l32 IDs_number = unicode_property::number_of_ranges(upid_bp_ID_Start);
				ranges.load_from_memory(IDs_address, IDs_number);
			}
			ranges.load_from_memory(&additions[0], 2);
			append_charclass(ranges);

			//  For reg_exp_identifier_part.
			ranges.clear();
			{
				const ui_l32 *const IDc_address = unicode_property::ranges_address(upid_bp_ID_Continue);
				const ui_l32 IDc_number = unicode_property::number_of_ranges(upid_bp_ID_Continue);
				ranges.load_from_memory(IDc_address, IDc_number);
			}
			ranges.load_from_memory(&additions[0], 3);
			append_charclass(ranges);
		}
	}

	bool is_identifier(const ui_l32 ch, const bool part) const
	{
		const range_pair &rp = char_class_pos_[part ? 1 : 0];

		return char_class_.is_included(rp.first, rp.second, ch);
	}

private:

	void append_charclass(const range_pairs &rps)
	{
		char_class_pos_.push_back(range_pair_helper(static_cast<ui_l32>(char_class_.size()), static_cast<ui_l32>(rps.size())));
		char_class_.append_newclass(rps);
	}

	range_pairs char_class_;
	range_pairs::array_type char_class_pos_;

//  UnicodeIDStart::
//    any Unicode code point with the Unicode property "ID_Start"
//  UnicodeIDContinue::
//    any Unicode code point with the Unicode property "ID_Continue"
	static const ui_l32 upid_bp_ID_Start = static_cast<ui_l32>(up_constants::bp_ID_Start);
	static const ui_l32 upid_bp_ID_Continue = static_cast<ui_l32>(up_constants::bp_ID_Continue);
};
//  identifier_charclass
#endif	//  !defined(SRELL_NO_UNICODE_PROPERTY)

class re_character_class
{
public:

	enum
	{	//    0       1      2      3     4           5
		newline, dotall, space, digit, word, icase_word,
		//                6
		number_of_predefcls
	};

#if !defined(SRELL_NO_UNICODE_PROPERTY)
	typedef unicode_property::pstring pstring;
#endif

	re_character_class()
	{
		setup_predefinedclass();
	}

	re_character_class &operator=(const re_character_class &that)
	{
		if (this != &that)
		{
			this->char_class_ = that.char_class_;
			this->char_class_pos_ = that.char_class_pos_;
#if !defined(SRELLDBG_NO_CCPOS)
			this->char_class_el_ = that.char_class_el_;
			this->char_class_pos_el_ = that.char_class_pos_el_;
#endif
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	re_character_class &operator=(re_character_class &&that) SRELL_NOEXCEPT
	{
		if (this != &that)
		{
			this->char_class_ = std::move(that.char_class_);
			this->char_class_pos_ = std::move(that.char_class_pos_);
#if !defined(SRELLDBG_NO_CCPOS)
			this->char_class_el_ = std::move(that.char_class_el_);
			this->char_class_pos_el_ = std::move(that.char_class_pos_el_);
#endif
		}
		return *this;
	}
#endif

	bool is_included(const ui_l32 class_number, const ui_l32 c) const
	{
//		return char_class_.is_included(char_class_pos_[class_number], c);
		const range_pair &rp = char_class_pos_[class_number];

		return char_class_.is_included(rp.first, rp.second, c);
	}

#if !defined(SRELLDBG_NO_CCPOS)
	bool is_included(const ui_l32 pos, const ui_l32 len, const ui_l32 c) const
	{
			return char_class_el_.is_included_el(pos, len, c);
	}
#endif

	void setup_icase_word()
	{
		range_pair &icase_pos = char_class_pos_[icase_word];

		if (icase_pos.second == char_class_pos_[word].second)
		{
			range_pairs icasewordclass(char_class_, icase_pos.first, icase_pos.second);

			icasewordclass.make_caseunfoldedcharset();
				//  Includes 017f and 212a so that they and their case-folded
				//  characters 's' and 'k' will be excluded from the character
				//  set that /[\W]/i matches.

			char_class_.replace(icase_pos.first, icase_pos.second, icasewordclass);

			if (icase_pos.second < static_cast<ui_l32>(icasewordclass.size()))
			{
				const ui_l32 delta = static_cast<ui_l32>(icasewordclass.size() - icase_pos.second);

				for (int i = number_of_predefcls; i < static_cast<int>(char_class_pos_.size()); ++i)
					char_class_pos_[i].first += delta;
			}
			icase_pos.second = static_cast<ui_l32>(icasewordclass.size());
		}
	}

	void clear()
	{
		char_class_pos_.resize(number_of_predefcls);

		ui_l32 basesize = 0;
		for (int i = 0; i < number_of_predefcls; ++i)
			basesize += char_class_pos_[i].second;

		char_class_.resize(basesize);

#if !defined(SRELLDBG_NO_CCPOS)
		char_class_el_.clear();
		char_class_pos_el_.clear();
#endif
	}

	ui_l32 register_newclass(const range_pairs &rps)
	{
		for (range_pairs::size_type no = 0; no < char_class_pos_.size(); ++no)
		{
			const range_pair &rp = char_class_pos_[no];

			if (char_class_.same(rp.first, rp.second, rps))
				return static_cast<ui_l32>(no);
		}

		append_charclass(rps);
		return static_cast<ui_l32>(char_class_pos_.size() - 1);
	}

	range_pairs operator[](const ui_l32 no) const
	{
		const range_pair &ccpos = char_class_pos_[no];
		range_pairs rp(ccpos.second);

		for (ui_l32 i = 0; i < ccpos.second; ++i)
			rp[i] = char_class_[ccpos.first + i];

		return rp;
	}

#if !defined(SRELLDBG_NO_CCPOS)

	const range_pair &charclasspos(const ui_l32 no)	//  const
	{
		range_pair &elpos = char_class_pos_el_[no];

		if (elpos.second == 0)
		{
			const range_pair &posinfo = char_class_pos_[no];

			if (posinfo.second > 0)
			{
				elpos.first = static_cast<ui_l32>(char_class_el_.size());
				elpos.second = char_class_el_.create_el(&char_class_[posinfo.first], posinfo.second);
			}
		}
		return elpos;
	}

	void finalise()
	{
		char_class_el_.clear();
		char_class_pos_el_.resize(char_class_pos_.size());
		std::memset(&char_class_pos_el_[0], 0, char_class_pos_el_.size() * sizeof (range_pairs::array_type::value_type));
	}

#endif	//  #if !defined(SRELLDBG_NO_CCPOS)

	void optimise()
	{
	}

#if !defined(SRELL_NO_UNICODE_PROPERTY)

	ui_l32 get_propertynumber(const pstring &pname, const pstring &pvalue) const
	{
		const ui_l32 pno = static_cast<ui_l32>(unicode_property::lookup_property(pname, pvalue));

		return (pno != up_constants::error_property) ? pno : up_constants::error_property;
	}

	bool load_upranges(range_pairs &newranges, const ui_l32 property_number) const
	{
		newranges.clear();

		if (unicode_property::is_valid_pno(property_number))
		{
			if (property_number == upid_bp_Assigned)
			{
				load_updata(newranges, upid_gc_Cn);
				newranges.negation();
			}
			else
				load_updata(newranges, property_number);

			return true;
		}
		return false;
	}

	//  Properties of strings.
	bool is_pos(const ui_l32 pno) const
	{
		return unicode_property::is_pos(pno);
	}

	bool get_prawdata(simple_array<ui_l32> &seq, ui_l32 property_number)
	{
		if (property_number != up_constants::error_property)
		{
			if (property_number == upid_bp_Assigned)
				property_number = upid_gc_Cn;

			const ui_l32 *const address = unicode_property::ranges_address(property_number);
//			const ui_l32 offset = unicode_property::ranges_offset(property_number);
			const ui_l32 number = unicode_property::number_of_ranges(property_number) * 2;

			seq.resize(number);
			for (ui_l32 i = 0; i < number; ++i)
				seq[i] = address[i];

			return true;
		}
		seq.clear();
		return false;
	}

#endif	//  !defined(SRELL_NO_UNICODE_PROPERTY)

	void swap(re_character_class &right)
	{
		if (this != &right)
		{
			this->char_class_.swap(right.char_class_);
			this->char_class_pos_.swap(right.char_class_pos_);
#if !defined(SRELLDBG_NO_CCPOS)
			this->char_class_el_.swap(right.char_class_el_);
			this->char_class_pos_el_.swap(right.char_class_pos_el_);
#endif
		}
	}

private:

#if !defined(SRELL_NO_UNICODE_PROPERTY)

	void load_updata(range_pairs &newranges, const ui_l32 property_number) const
	{
		const ui_l32 *const address = unicode_property::ranges_address(property_number);
//		const ui_l32 offset = unicode_property::ranges_offset(property_number);
		const ui_l32 number = unicode_property::number_of_ranges(property_number);

		newranges.load_from_memory(address, number);
	}

#endif	//  !defined(SRELL_NO_UNICODE_PROPERTY)

	void append_charclass(const range_pairs &rps)
	{
		char_class_pos_.push_back(range_pair_helper(static_cast<ui_l32>(char_class_.size()), static_cast<ui_l32>(rps.size())));
		char_class_.append_newclass(rps);
	}

//  The production CharacterClassEscape::s  evaluates as follows:
//    Return the set of characters containing the characters that are on the right-hand side of the WhiteSpace or LineTerminator productions.
//  WhiteSpace::<TAB> <VT> <FF> <SP> <NBSP> <ZWNBSP> <USP>
//               0009 000B 000C 0020   00A0     FEFF    Zs
//  LineTerminator::<LF> <CR> <LS> <PS>
//                  000A 000D 2028 2029

	void setup_predefinedclass()
	{
#if !defined(SRELL_NO_UNICODE_PROPERTY)
		const ui_l32 *const Zs_address = unicode_property::ranges_address(upid_gc_Zs);
//		const ui_l32 Zs_offset = unicode_property::ranges_offset(upid_gc_Zs);
		const ui_l32 Zs_number = unicode_property::number_of_ranges(upid_gc_Zs);
#else
		static const ui_l32 Zs[] = {
			0x1680, 0x1680, 0x2000, 0x200a,	// 0x2028, 0x2029,
			0x202f, 0x202f, 0x205f, 0x205f, 0x3000, 0x3000
		};
#endif	//  defined(SRELL_NO_UNICODE_PROPERTY)
		static const ui_l32 allranges[] = {
			//  dotall.
			0x0000, 0x10ffff,
			//  newline.
			0x0a, 0x0a, 0x0d, 0x0d,	//  \n \r
			//  newline, space.
			0x2028, 0x2029,
			//  space.
			0x09, 0x0d,	//  \t \n \v \f \r
			0x20, 0x20,	//  ' '
			0xa0, 0xa0,	//  <NBSP>
			0xfeff, 0xfeff,	//  <BOM>
			//  digit, word.
			0x30, 0x39,	//  '0'-'9'
			0x41, 0x5a, 0x5f, 0x5f, 0x61, 0x7a	//  'A'-'Z' '_' 'a'-'z'
		};
		range_pairs ranges;

		//  newline.
		ranges.load_from_memory(&allranges[2], 3);
		append_charclass(ranges);

		//  dotall.
		ranges.clear();
		ranges.load_from_memory(&allranges[0], 1);
		append_charclass(ranges);

		//  space.
		ranges.clear();
		ranges.load_from_memory(&allranges[6], 5);
#if !defined(SRELL_NO_UNICODE_PROPERTY)
		ranges.load_from_memory(Zs_address, Zs_number);
#else
		ranges.load_from_memory(Zs, 5);
#endif
		append_charclass(ranges);

		//  digit.
		ranges.clear();
		ranges.load_from_memory(&allranges[16], 1);
		append_charclass(ranges);

		//  word.
		ranges.clear();
		ranges.load_from_memory(&allranges[16], 4);
		append_charclass(ranges);

		//  Reservation for icase_word.
		append_charclass(ranges);
	}

private:

	range_pairs char_class_;
	range_pairs::array_type char_class_pos_;

#if !defined(SRELLDBG_NO_CCPOS)
	range_pairs char_class_el_;
	range_pairs::array_type char_class_pos_el_;

#endif

#if !defined(SRELL_NO_UNICODE_PROPERTY)
	static const ui_l32 upid_gc_Zs = static_cast<ui_l32>(up_constants::gc_Space_Separator);
	static const ui_l32 upid_gc_Cn = static_cast<ui_l32>(up_constants::gc_Unassigned);
	static const ui_l32 upid_bp_Assigned = static_cast<ui_l32>(up_constants::bp_Assigned);

#endif

public:	//  For debug.

	void print_classes(const int) const;
};
//  re_character_class

	}	//  namespace re_detail

//  ... "rei_char_class.hpp"]
//  ["rei_groupname_mapper.hpp" ...

	namespace re_detail
	{

#if !defined(SRELL_NO_NAMEDCAPTURE)

template <typename charT>
class groupname_mapper
{
public:

	typedef simple_array<charT> gname_string;
	typedef typename gname_string::size_type size_type;
	static const ui_l32 notfound = 0u;

	groupname_mapper()
	{
	}

	groupname_mapper(const groupname_mapper &right)
		: names_(right.names_), keysize_classno_(right.keysize_classno_)
	{
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	groupname_mapper(groupname_mapper &&right) SRELL_NOEXCEPT
		: names_(std::move(right.names_)), keysize_classno_(std::move(right.keysize_classno_))
	{
	}
#endif

	groupname_mapper &operator=(const groupname_mapper &right)
	{
		if (this != &right)
		{
			names_ = right.names_;
			keysize_classno_ = right.keysize_classno_;
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	groupname_mapper &operator=(groupname_mapper &&right) SRELL_NOEXCEPT
	{
		if (this != &right)
		{
			names_ = std::move(right.names_);
			keysize_classno_ = std::move(right.keysize_classno_);
		}
		return *this;
	}
#endif

	void clear()
	{
		names_.clear();
		keysize_classno_.clear();
	}

	ui_l32 operator[](const gname_string &gname) const
	{
		ui_l32 pos = 0;
		for (std::size_t i = 0; i < static_cast<std::size_t>(keysize_classno_.size()); i += 2)
		{
			const ui_l32 keysize = keysize_classno_[i];

			if (keysize == static_cast<ui_l32>(gname.size()) && sameseq(pos, gname))
				return keysize_classno_[++i];

			pos += keysize;
		}
		return notfound;
	}

	gname_string operator[](const ui_l32 indexno) const
	{
		ui_l32 pos = 0;
		for (std::size_t i = 0; i < static_cast<std::size_t>(keysize_classno_.size()); ++i)
		{
			const ui_l32 keysize = keysize_classno_[i];
			const ui_l32 classno = keysize_classno_[++i];

			if (classno == indexno)
				return gname_string(names_, pos, keysize);

			pos += keysize;
		}
		return gname_string();
	}

	size_type size() const
	{
		return static_cast<size_type>(keysize_classno_.size() >> 1);
	}

	bool push_back(const gname_string &gname, const ui_l32 class_number)
	{
		const ui_l32 num = operator[](gname);

		if (num == notfound)
		{
			names_.append(gname);
			keysize_classno_.append(1, static_cast<ui_l32>(gname.size()));
			keysize_classno_.append(1, class_number);
			return true;
		}
		return false;	//  Already exists.
	}

	void swap(groupname_mapper &right)
	{
		this->names_.swap(right.names_);
		keysize_classno_.swap(right.keysize_classno_);
	}

private:

	bool sameseq(size_type pos, const gname_string &gname) const
	{
		for (size_type i = 0; i < gname.size(); ++i, ++pos)
			if (pos >= names_.size() || names_[pos] != gname[i])
				return false;

		return true;
	}

	gname_string names_;
	simple_array<ui_l32> keysize_classno_;

public:	//  For debug.

	void print_mappings(const int) const;
};
template <typename charT>
const ui_l32 groupname_mapper<charT>::notfound;
//  groupname_mapper

#endif	//  !defined(SRELL_NO_NAMEDCAPTURE)

	}	//  namespace re_detail

//  ... "rei_groupname_mapper.hpp"]
//  ["rei_state.hpp" ...

	namespace re_detail
	{

struct re_quantifier
{
	//  atleast and atmost: for check_counter.
	//  (Special case 1) in charcter_class, bol, eol, boundary, represents the offset and length
	//    of the range in the array of character classes.
	//  (Special case 2) in roundbracket_open and roundbracket_pop atleast and atmost represent
	//    the minimum and maximum bracket numbers respectively inside the brackets itself.
	//  (Special case 3) in repeat_in_push and repeat_in_pop atleast and atmost represent the
	//    minimum and maximum bracket numbers respectively inside the repetition.

	ui_l32 atleast;
		//  (Special case 4: v1) in lookaround_open represents the number of characters to be rewound.
		//  (Special case 4: v2) in lookaround_open represents: 0=lookaheads, 1=lookbehinds,
		//    2=matchpointrewinder, 3=rewinder+rerun.
		//  (Special case 5) in NFA_states[0] represents the class number of the first character class.

	ui_l32 atmost;

	ui_l32 is_greedy;

	void reset(const ui_l32 len = 1)
	{
		atleast = atmost = len;
		is_greedy = true;
	}

	void set(const ui_l32 min, const ui_l32 max)
	{
		atleast = min;
		atmost = max;
	}

	void set(const ui_l32 min, const ui_l32 max, const ui_l32 greedy)
	{
		atleast = min;
		atmost = max;
		is_greedy = greedy;
	}

	bool is_valid() const
	{
		return atleast <= atmost;
	}

	void set_infinity()
	{
		atmost = constants::infinity;
	}

	bool is_infinity() const
	{
		return atmost == constants::infinity;
	}

	bool is_same() const
	{
		return atleast == atmost;
	}

	bool is_default() const
	{
		return atleast == 1 && atmost == 1;
	}

	bool is_asterisk() const
	{
		return atleast == 0 && atmost == constants::infinity;
	}
	bool is_plus() const
	{
		return atleast == 1 && atmost == constants::infinity;
	}
	bool is_asterisk_or_plus() const
	{
		return atleast <= 1 && atmost == constants::infinity;
	}

	bool has_simple_equivalence() const
	{
		return (atleast <= 1 && atmost <= 3) || (atleast == 2 && atmost <= 4) || (atleast == atmost && atmost <= 6);
	}

	void multiply(const re_quantifier &q)
	{
		if (atleast != constants::infinity)
		{
			if (q.atleast != constants::infinity)
				atleast *= q.atleast;
			else
				atleast = constants::infinity;
		}

		if (atmost != constants::infinity)
		{
			if (q.atmost != constants::infinity)
				atmost *= q.atmost;
			else
				atmost = constants::infinity;
		}
	}

	void add(const re_quantifier &q)
	{
		if (atleast != constants::infinity)
		{
			if (q.atleast != constants::infinity && (atleast + q.atleast) >= atleast)
				atleast += q.atleast;
			else
				atleast = constants::infinity;
		}

		if (atmost != constants::infinity)
		{
			if (q.atmost != constants::infinity && (atmost + q.atmost) >= atmost)
				atmost += q.atmost;
			else
				atmost = constants::infinity;
		}
	}
};
//  re_quantifier

struct re_state
{
	ui_l32 char_num;
		//  character: for character.
		//  number: for character_class, brackets, counter, repeat, backreference.
		//  (Special case) in [0] represents a code unit for finding an entry point if
		//    the firstchar class consists of a single code unit; otherwise invalid_u32value.

	re_state_type type;

	union
	{
		std::ptrdiff_t next1;
		re_state *next_state1;
		//  (Special case 1) in lookaround_open points to the next of lookaround_close.
	};
	union
	{
		std::ptrdiff_t next2;
		re_state *next_state2;
		//  character and character_class: points to another possibility, non-backtracking.
		//  epsilon: points to another possibility, backtracking.
		//  save_and_reset_counter, roundbracket_open, and repeat_in_push: points to a
		//    restore state, backtracking.
		//  check_counter: complementary to next1 based on quantifier.is_greedy.
		//  (Special case 1) roundbracket_close, check_0_width_repeat, and backreference:
		//    points to the next state as an exit after 0 width match.
		//  (Special case 2) in NFA_states[0] holds the entry point for match_continuous/regex_match.
		//  (Special case 3) in lookaround_open points to the contents of brackets.
	};

	re_quantifier quantifier;	//  For check_counter, roundbrackets, repeasts, (?<=...) and (?<!...),
		//  and character_class.

	union
	{
		ui_l32 is_not;	//  For \B, (?!...) and (?<!...).
		ui_l32 icase;	//  For [0], backreference.
		ui_l32 multiline;	//  For bol, eol.

		ui_l32 icase_backrefno_unresolved;	//  For backreference.
			//  Bit 0: Used across compiler and algorithm.
			//  Bit 1: Used only in compiler.
	};

	void reset(const re_state_type t = st_character, const ui_l32 c = char_ctrl::cc_nul)
	{
		type = t;
		char_num = c;
		next1 = 1;
		next2 = 0;
		is_not = false;
		quantifier.reset();
	}

	bool is_character_or_class() const
	{
		return type == st_character || type == st_character_class;
	}

	bool has_quantifier() const
	{
		//  1. character:  size == 1 && type == character,
		//  2. [...]:      size == 1 && type == character_class,
		//  3. (...):      size == ? && type == roundbracket_open,
		//  4. (?:...):    size == ? && type == epsilon && character == ':',
		//  5. backref:    size == ? && type == backreference,
		//  -- assertions boundary --
		//  6. lookaround: size == ? && type == lookaround,
		//  7. assertion:  size == 0 && type == one of assertions (^, $, \b and \B).
#if !defined(SRELL_ENABLE_GT)
		return type < st_zero_width_boundary;
#else
		//  5.5. independent: size == ? && type == lookaround && char_num == '>',
		return type < st_zero_width_boundary || (type == st_lookaround_open && char_num == meta_char::mc_gt);
#endif
	}

	bool has_0widthchecker() const
	{
		return type == st_roundbracket_open || type == st_backreference;
	}

	bool is_noncapturinggroup() const
	{
		return type == st_epsilon && char_num == epsilon_type::et_ncgopen;
	}

	bool is_noncapturinggroup_begin_or_end() const
	{
		return type == st_epsilon && next2 == 0 && (char_num == epsilon_type::et_ncgopen || char_num == epsilon_type::et_ncgclose);
	}

	bool is_noncapturinggroup_containing_capturinggroup() const
	{
		return is_noncapturinggroup() && quantifier.is_valid();
	}

	bool is_branch() const
	{
		return type == st_epsilon && next2 != 0 && char_num == epsilon_type::et_alt;	//  '|'
	}

	bool is_question_or_asterisk_before_corcc() const
	{
		return type == st_epsilon && char_num == epsilon_type::et_ccastrsk;
	}

	bool is_asterisk_or_plus_for_onelen_atom() const
	{
		return type == st_epsilon && ((next1 == 1 && next2 == 2) || (next1 == 2 && next2 == 1)) && quantifier.is_asterisk_or_plus();
	}

	bool is_same_character_or_charclass(const re_state &right) const
	{
		return type == right.type && char_num == right.char_num;
	}

	std::ptrdiff_t nearnext() const
	{
		return quantifier.is_greedy ? next1 : next2;
	}

	std::ptrdiff_t farnext() const
	{
		return quantifier.is_greedy ? next2 : next1;
	}
};
//  re_state

struct re_flags
{
//	bool i;
//	bool m;
//	bool s;

#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
	bool back;
#endif

	void reset(const regex_constants::syntax_option_type /* flags */)
	{
//		i = (flags & regex_constants::icase) != 0;	//  Case-insensitive.
//		m = (flags & regex_constants::multiline) != 0;
//		s = (flags & regex_constants::dotall) != 0;

#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
		back = false;
#endif
	}

#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
	void restore_from(const re_flags &backup)
#else
	void restore_from(const re_flags &)
#endif
	{
#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
		back = backup.back;
#endif
	}
};
//  re_flags

template <typename charT>
struct re_compiler_state : public re_flags
{
	const ui_l32 *begin;

#if !defined(SRELL_NO_NAMEDCAPTURE)
	groupname_mapper<charT> unresolved_gnames;
#endif

#if !defined(SRELL_NO_UNICODE_PROPERTY)
	identifier_charclass idchecker;
#endif

	void reset(const regex_constants::syntax_option_type flags, const ui_l32 *const b)
	{
		re_flags::reset(flags);

		begin = b;

#if !defined(SRELL_NO_NAMEDCAPTURE)
		unresolved_gnames.clear();
#endif

#if !defined(SRELL_NO_UNICODE_PROPERTY)
//		idchecker.clear();	//  Keeps data once created.
#endif
	}
};
//  re_compiler_state

	}	//  namespace re_detail

//  ... "rei_state.hpp"]
//  ["rei_search_state.hpp" ...

	namespace re_detail
	{

//template <typename charT>
struct re_state;

template </* typename charT, */typename BidirectionalIterator>
struct re_search_state_core
{
	const re_state/* <charT> */ *state;
	BidirectionalIterator iter;
};

template <typename BidirectionalIterator>
struct re_submatch_core
{
	BidirectionalIterator open_at;
	BidirectionalIterator close_at;
};

template <typename BidirectionalIterator>
struct re_submatch_type
{
	re_submatch_core<BidirectionalIterator> core;
	ui_l32 counter;

	void init(const BidirectionalIterator b)
	{
		core.open_at = core.close_at = b;
		counter = 0u;
	}
};

template </*typename charT, */typename BidirectionalIterator>
struct re_search_state_types
{
	typedef re_submatch_core<BidirectionalIterator> submatch_core;
	typedef re_submatch_type<BidirectionalIterator> submatch_type;
	typedef ui_l32 counter_type;
	typedef BidirectionalIterator position_type;

	typedef std::vector<submatch_type> submatch_array;

	typedef re_search_state_core</*charT, */BidirectionalIterator> search_state_core;

	typedef std::vector<search_state_core> backtracking_array;
	typedef std::vector<submatch_core> capture_array;
	typedef simple_array<counter_type> counter_array;
	typedef std::vector<position_type> repeat_array;
};

template </*typename charT1, */typename charT2>
struct re_search_state_types</*charT1, */const charT2 *>
{
	typedef re_submatch_core<const charT2 *> submatch_core;
	typedef re_submatch_type<const charT2 *> submatch_type;
	typedef ui_l32 counter_type;
	typedef const charT2 *position_type;

	typedef simple_array<submatch_type> submatch_array;

	typedef re_search_state_core</*charT1, */const charT2 *> search_state_core;

	typedef simple_array<search_state_core> backtracking_array;
	typedef simple_array<submatch_core> capture_array;
	typedef simple_array<counter_type> counter_array;
	typedef simple_array<position_type> repeat_array;
};
//  re_search_state_types

template </*typename charT, */typename BidirectionalIterator>
class re_search_state : public re_search_state_types</*charT, */BidirectionalIterator>
{
private:

	typedef re_search_state_types</*charT, */BidirectionalIterator> base_type;

public:

	typedef typename base_type::submatch_core submatchcore_type;
	typedef typename base_type::submatch_type submatch_type;
	typedef typename base_type::counter_type counter_type;
	typedef typename base_type::position_type position_type;

	typedef typename base_type::submatch_array submatch_array;

	typedef typename base_type::search_state_core search_state_core;

	typedef typename base_type::backtracking_array backtracking_array;
	typedef typename base_type::capture_array capture_array;
	typedef typename base_type::counter_array counter_array;
	typedef typename base_type::repeat_array repeat_array;

	typedef typename backtracking_array::size_type btstack_size_type;

public:

	struct bottom_state
	{
		btstack_size_type btstack_size;
		typename capture_array::size_type capturestack_size;
		typename counter_array::size_type counterstack_size;
		typename repeat_array::size_type repeatstack_size;

		bottom_state(
			const btstack_size_type bt,
			const typename capture_array::size_type h,
			const typename counter_array::size_type c,
			const typename repeat_array::size_type r)
			: btstack_size(bt)
			, capturestack_size(h)
			, counterstack_size(c)
			, repeatstack_size(r)
		{
		}
	};

public:

	search_state_core ssc;

#if !defined(SRELL_NO_LIMIT_COUNTER)
	std::size_t failure_counter;
#endif

	BidirectionalIterator lblim;
	BidirectionalIterator srchend;

	BidirectionalIterator nextpos;

	backtracking_array bt_stack;

	capture_array capture_stack;
	counter_array counter_stack;
	repeat_array repeat_stack;

	submatch_array bracket;
	counter_array counter;
	repeat_array repeat;

	btstack_size_type btstack_size;

	BidirectionalIterator srchbegin;

	BidirectionalIterator reallblim;

	regex_constants::match_flag_type flags;
	const re_state/* <charT> */ *entry_state;

public:

	void init
	(
		const BidirectionalIterator begin,
		const BidirectionalIterator end,
		const BidirectionalIterator lookbehindlimit,
		const regex_constants::match_flag_type f
	)
	{
		reallblim = lblim = lookbehindlimit;
		nextpos = srchbegin = begin;
		srchend = end;
		flags = f;
	}

	void init_for_automaton
	(
		ui_l32 num_of_submatches,
		const ui_l32 num_of_counters,
		const ui_l32 num_of_repeats
	)
	{

		bracket.resize(num_of_submatches);
		counter.resize(num_of_counters);
		repeat.resize(num_of_repeats);

		while (num_of_submatches > 1)
			bracket[--num_of_submatches].init(this->srchend);
			//  15.10.2.9; AtomEscape:
			//  If the regular expression has n or more capturing parentheses
			//  but the nth one is undefined because it hasn't captured anything,
			//  then the backreference always succeeds.

			//  C.f., table 27 and 28 on TR1, table 142 and 143 on C++11.

		clear_stacks();
	}

#if defined(SRELL_NO_LIMIT_COUNTER)
	void reset(/* const BidirectionalIterator start */)
#else
	void reset(/* const BidirectionalIterator start, */ const std::size_t limit)
#endif
	{
		ssc.state = this->entry_state;

		bracket[0].core.open_at = ssc.iter;

#if !defined(SRELL_NO_LIMIT_COUNTER)
		failure_counter = limit;
#endif
	}

	bool is_at_lookbehindlimit() const
	{
		return ssc.iter == this->lblim;
	}

	bool is_at_srchend() const
	{
		return ssc.iter == this->srchend;
	}

	bool is_null() const
	{
		return ssc.iter == bracket[0].core.open_at;
	}

	bool match_not_bol_flag() const
	{
		if (this->flags & regex_constants::match_not_bol)
			return true;
		return false;
	}

	bool match_not_eol_flag() const
	{
		if (this->flags & regex_constants::match_not_eol)
			return true;
		return false;
	}

	bool match_not_bow_flag() const
	{
		if (this->flags & regex_constants::match_not_bow)
			return true;
		return false;
	}

	bool match_not_eow_flag() const
	{
		if (this->flags & regex_constants::match_not_eow)
			return true;
		return false;
	}

	bool match_prev_avail_flag() const
	{
		if (this->flags & regex_constants::match_prev_avail)
			return true;
		return false;
	}

	bool match_not_null_flag() const
	{
		if (this->flags & regex_constants::match_not_null)
			return true;
		return false;
	}

	bool match_continuous_flag() const
	{
		if (this->flags & regex_constants::match_continuous)
			return true;
		return false;
	}

	bool match_match_flag() const
	{
		if (this->flags & regex_constants::match_match_)
			return true;
		return false;
	}

	bool is_prev_avail() const
	{
		return reallblim != lblim || (flags & regex_constants::match_prev_avail) != 0;
	}

	bool set_bracket0(const BidirectionalIterator begin, const BidirectionalIterator end)
	{
		ssc.iter = begin;
		nextpos = end;
		return true;
	}

	void clear_stacks()
	{
		btstack_size = 0;
		bt_stack.clear();
		capture_stack.clear();
		repeat_stack.clear();
		counter_stack.clear();
	}

	btstack_size_type size() const	//  For debug.
	{
		return bt_stack.size();
	}

	bool is_empty() const	//  For debug.
	{
		if (btstack_size == 0
			&& bt_stack.size() == 0
			&& capture_stack.size() == 0
			&& repeat_stack.size() == 0
			&& counter_stack.size() == 0)
			return true;

		return false;
	}
};
//  re_search_state

	}	//  namespace re_detail

//  ... "rei_search_state.hpp"]
//  ["rei_bmh.hpp" ...

	namespace re_detail
	{

#if !defined(SRELLDBG_NO_BMH)

template <typename charT, typename utf_traits>
class re_bmh
{
public:

	re_bmh()
	{
	}

	re_bmh(const re_bmh &right)
	{
		operator=(right);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	re_bmh(re_bmh &&right) SRELL_NOEXCEPT
	{
		operator=(std::move(right));
	}
#endif

	re_bmh &operator=(const re_bmh &that)
	{
		if (this != &that)
		{
			this->u32string_ = that.u32string_;

			this->bmtable_ = that.bmtable_;
			this->repseq_ = that.repseq_;
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	re_bmh &operator=(re_bmh &&that) SRELL_NOEXCEPT
	{
		if (this != &that)
		{
			this->u32string_ = std::move(that.u32string_);

			this->bmtable_ = std::move(that.bmtable_);
			this->repseq_ = std::move(that.repseq_);
		}
		return *this;
	}
#endif

	void clear()
	{
		u32string_.clear();

		bmtable_.clear();
		repseq_.clear();
	}

	void setup(const simple_array<ui_l32> &u32s, const bool icase)
	{
		u32string_ = u32s;
		setup_();

		if (!icase)
			setup_for_casesensitive();
		else
			setup_for_icase();
	}

	template <typename RandomAccessIterator>
	bool do_casesensitivesearch(re_search_state<RandomAccessIterator> &sstate, const std::random_access_iterator_tag) const
	{
		RandomAccessIterator begin = sstate.srchbegin;
		const RandomAccessIterator end = sstate.srchend;
		std::size_t offset = static_cast<std::size_t>(repseq_.size() - 1);
		const charT *const relastchar = &repseq_[offset];

		for (; static_cast<std::size_t>(end - begin) > offset;)
		{
			begin += offset;

			if (*begin == *relastchar)
			{
				const charT *re = relastchar;
				RandomAccessIterator tail = begin;

				for (; *--re == *--tail;)
				{
					if (re == repseq_.data())
						return sstate.set_bracket0(tail, ++begin);
				}
			}
			offset = bmtable_[*begin & 0xff];
		}
		return false;
	}

	template <typename BidirectionalIterator>
	bool do_casesensitivesearch(re_search_state<BidirectionalIterator> &sstate, const std::bidirectional_iterator_tag) const
	{
		BidirectionalIterator begin = sstate.srchbegin;
		const BidirectionalIterator end = sstate.srchend;
		std::size_t offset = static_cast<std::size_t>(repseq_.size() - 1);
		const charT *const relastchar = &repseq_[offset];

		for (;;)
		{
			for (; offset; --offset, ++begin)
				if (begin == end)
					return false;

			if (*begin == *relastchar)
			{
				const charT *re = relastchar;
				BidirectionalIterator tail = begin;

				for (; *--re == *--tail;)
				{
					if (re == repseq_.data())
						return sstate.set_bracket0(tail, ++begin);
				}
			}
			offset = bmtable_[*begin & 0xff];
		}
	}

	template <typename RandomAccessIterator>
	bool do_icasesearch(re_search_state<RandomAccessIterator> &sstate, const std::random_access_iterator_tag) const
	{
		const RandomAccessIterator begin = sstate.srchbegin;
		const RandomAccessIterator end = sstate.srchend;
		std::size_t offset = bmtable_[256];
		const ui_l32 entrychar = u32string_[u32string_.size() - 1];
		const ui_l32 *const re2ndlastchar = &u32string_[u32string_.size() - 2];
		RandomAccessIterator curpos = begin;

		for (; static_cast<std::size_t>(end - curpos) > offset;)
		{
			curpos += offset;

			for (; utf_traits::is_trailing(*curpos);)
				if (++curpos == end)
					return false;

			const ui_l32 txtlastchar = utf_traits::codepoint(curpos, end);

			if (txtlastchar == entrychar || unicode_case_folding::do_casefolding(txtlastchar) == entrychar)
			{
				const ui_l32 *re = re2ndlastchar;
				RandomAccessIterator tail = curpos;

				for (; *re == unicode_case_folding::do_casefolding(utf_traits::dec_codepoint(tail, begin)); --re)
				{
					if (re == u32string_.data())
					{
						utf_traits::codepoint_inc(curpos, end);
						return sstate.set_bracket0(tail, curpos);
					}
					if (tail == begin)
						break;
				}
			}
			offset = bmtable_[txtlastchar & 0xff];
		}
		return false;
	}

	template <typename BidirectionalIterator>
	bool do_icasesearch(re_search_state<BidirectionalIterator> &sstate, const std::bidirectional_iterator_tag) const
	{
		const BidirectionalIterator begin = sstate.srchbegin;
		const BidirectionalIterator end = sstate.srchend;

		if (begin != end)
		{
			std::size_t offset = bmtable_[256];
			const ui_l32 entrychar = u32string_[offset];
			const ui_l32 *const re2ndlastchar = &u32string_[offset - 1];
			BidirectionalIterator curpos = begin;

			for (;;)
			{
				for (;;)
				{
					if (++curpos == end)
						return false;
					if (!utf_traits::is_trailing(*curpos))
						if (--offset == 0)
							break;
				}
				const ui_l32 txtlastchar = utf_traits::codepoint(curpos, end);

				if (txtlastchar == entrychar || unicode_case_folding::do_casefolding(txtlastchar) == entrychar)
				{
					const ui_l32 *re = re2ndlastchar;
					BidirectionalIterator tail = curpos;

					for (; *re == unicode_case_folding::do_casefolding(utf_traits::dec_codepoint(tail, begin)); --re)
					{
						if (re == u32string_.data())
						{
							utf_traits::codepoint_inc(curpos, end);
							return sstate.set_bracket0(tail, curpos);
						}
						if (tail == begin)
							break;
					}
				}
				offset = bmtable_[txtlastchar & 0xff];
			}
		}
		return false;
	}

private:

	void setup_()
	{
		bmtable_.resize(257);
	}

	void setup_for_casesensitive()
	{
		charT mbstr[utf_traits::maxseqlen];
		const std::size_t u32str_lastcharpos_ = static_cast<std::size_t>(u32string_.size() - 1);

		repseq_.clear();

		for (std::size_t i = 0; i <= u32str_lastcharpos_; ++i)
		{
			const ui_l32 seqlen = utf_traits::to_codeunits(mbstr, u32string_[i]);

			for (ui_l32 j = 0; j < seqlen; ++j)
				repseq_.push_back(mbstr[j]);
		}

		for (ui_l32 i = 0; i < 256; ++i)
			bmtable_[i] = static_cast<std::size_t>(repseq_.size());

		const std::size_t repseq_lastcharpos_ = static_cast<std::size_t>(repseq_.size() - 1);

		for (std::size_t i = 0; i < repseq_lastcharpos_; ++i)
			bmtable_[repseq_[i] & 0xff] = repseq_lastcharpos_ - i;
	}

	void setup_for_icase()
	{
		charT mbstr[utf_traits::maxseqlen];
		ui_l32 u32table[ucf_constants::rev_maxset];
		const std::size_t u32str_lastcharpos = static_cast<std::size_t>(u32string_.size() - 1);
		simple_array<std::size_t> minlen(u32string_.size());
		std::size_t cu_repseq_lastcharpos = 0;

		for (std::size_t i = 0; i <= u32str_lastcharpos; ++i)
		{
			const ui_l32 setnum = unicode_case_folding::do_caseunfolding(u32table, u32string_[i]);
			ui_l32 u32c = u32table[0];

			for (ui_l32 j = 1; j < setnum; ++j)
				if (u32c > u32table[j])
					u32c = u32table[j];

			if (i < u32str_lastcharpos)
				cu_repseq_lastcharpos += minlen[i] = utf_traits::to_codeunits(mbstr, u32c);
		}

		++cu_repseq_lastcharpos;

		for (std::size_t i = 0; i < 256; ++i)
			bmtable_[i] = cu_repseq_lastcharpos;

		bmtable_[256] = --cu_repseq_lastcharpos;

		for (std::size_t i = 0; i < u32str_lastcharpos; ++i)
		{
			const ui_l32 setnum = unicode_case_folding::do_caseunfolding(u32table, u32string_[i]);

			for (ui_l32 j = 0; j < setnum; ++j)
				bmtable_[u32table[j] & 0xff] = cu_repseq_lastcharpos;

			cu_repseq_lastcharpos -= minlen[i];
		}
	}

public:	//  For debug.

	void print_table() const;
	void print_seq() const;

private:

	simple_array<ui_l32> u32string_;
//	std::size_t bmtable_[256];
	simple_array<std::size_t> bmtable_;
	simple_array<charT> repseq_;
};
//  re_bmh

#endif	//  !defined(SRELLDBG_NO_BMH)
	}	//  namespace re_detail

//  ... "rei_bmh.hpp"]
//  ["rei_upos.hpp" ...

	namespace re_detail
	{

struct posdata_holder
{
	simple_array<ui_l32> indices;
	simple_array<ui_l32> seqs;
	range_pairs ranges;
	range_pair length;

	void clear()
	{
		indices.clear();
		seqs.clear();
		ranges.clear();
		length.set(1);
	}

	bool has_empty() const
	{
		return (indices.size() >= 2 && indices[0] != indices[1]) ? true : false;
	}

	bool has_data() const
	{
		return ranges.size() > 0 || indices.size() > 0;
	}

	bool may_contain_strings() const
	{
		return indices.size() > 0;	//  >= 2;
	}

	void swap(posdata_holder &right)
	{
		indices.swap(right.indices);
		seqs.swap(right.seqs);
		ranges.swap(right.ranges);
		length.swap(right.length);
	}

	void do_union(const posdata_holder &right)
	{
		simple_array<ui_l32> curseq;

		ranges.merge(right.ranges);

		if (right.has_empty() && !has_empty())
			register_emptystring();

		for (ui_l32 seqlen = 2; seqlen < static_cast<ui_l32>(right.indices.size()); ++seqlen)
		{
			const ui_l32 end = right.indices[seqlen - 1];
			ui_l32 begin = right.indices[seqlen];

			if (begin != end)
			{
				const std::size_t complen = seqlen * sizeof (ui_l32);

				ensure_length(seqlen);
				curseq.resize(seqlen);

				for (; begin < end;)
				{
					const ui_l32 inspos = find_seq(&right.seqs[begin], seqlen, complen);

					if (inspos == indices[seqlen - 1])
					{
						for (ui_l32 i = 0; i < seqlen; ++i, ++begin)
							curseq[i] = right.seqs[begin];

						seqs.insert(inspos, curseq);
						for (ui_l32 i = 0; i < seqlen; ++i)
							indices[i] += seqlen;
					}
					else
						begin += seqlen;
				}
			}
		}
		check_lengths();
	}

	void do_subtract(const posdata_holder &right)
	{
		const ui_l32 maxlen = static_cast<ui_l32>(indices.size() <= right.indices.size() ? indices.size() : right.indices.size());

		{
			range_pairs kept;
			range_pairs removed;

			ranges.split_ranges(kept, removed, right.ranges);
			ranges.swap(kept);
		}

		if (right.has_empty() && has_empty())
			unregister_emptystring();

		for (ui_l32 seqlen = 2; seqlen < maxlen; ++seqlen)
		{
			const ui_l32 end = right.indices[seqlen - 1];
			ui_l32 begin = right.indices[seqlen];

			if (begin != end)
			{
				const std::size_t complen = seqlen * sizeof (ui_l32);

				for (; begin < end;)
				{
					const ui_l32 delpos = find_seq(&right.seqs[begin], seqlen, complen);

					if (delpos < indices[seqlen - 1])
					{
						seqs.erase(delpos, seqlen);

						for (ui_l32 i = 0; i < seqlen; ++i)
							indices[i] -= seqlen;
					}
					else
						begin += seqlen;
				}
			}
		}
		check_lengths();
	}

	void do_and(const posdata_holder &right)
	{
		const ui_l32 maxlen = static_cast<ui_l32>(indices.size() <= right.indices.size() ? indices.size() : right.indices.size());
		posdata_holder newpos;
		simple_array<ui_l32> curseq;

		{
			range_pairs kept;

			ranges.split_ranges(kept, newpos.ranges, right.ranges);
			ranges.swap(newpos.ranges);
		}

		if (has_empty() && right.has_empty())
			newpos.register_emptystring();
		else if (may_contain_strings() || right.may_contain_strings())
			ensure_length(1);

		for (ui_l32 seqlen = 2; seqlen < maxlen; ++seqlen)
		{
			const ui_l32 end = right.indices[seqlen - 1];
			ui_l32 begin = right.indices[seqlen];

			if (begin != end)
			{
				const std::size_t complen = seqlen * sizeof (ui_l32);
				const ui_l32 myend = indices[seqlen - 1];

				curseq.resize(seqlen);

				for (; begin < end; begin += seqlen)
				{
					const ui_l32 srcpos = find_seq(&right.seqs[begin], seqlen, complen);

					if (srcpos < myend)
					{
						newpos.ensure_length(seqlen);

						const ui_l32 inspos = newpos.find_seq(&right.seqs[begin], seqlen, complen);

						if (inspos == newpos.indices[seqlen - 1])
						{
							for (ui_l32 i = 0; i < seqlen; ++i)
								curseq[i] = right.seqs[begin + i];

							newpos.seqs.insert(inspos, curseq);
							for (ui_l32 i = 0; i < seqlen; ++i)
								newpos.indices[i] += seqlen;
						}
					}
				}
			}
		}
		this->indices.swap(newpos.indices);
		this->seqs.swap(newpos.seqs);
		check_lengths();
	}

	void split_seqs_and_ranges(const simple_array<ui_l32> &inseqs, const bool icase, const bool back)
	{
		const ui_l32 max = static_cast<ui_l32>(inseqs.size());
		simple_array<ui_l32> curseq;

		clear();

		for (ui_l32 indx = 0; indx < max;)
		{
			const ui_l32 elen = inseqs[indx++];

			if (elen == 1)	//  Range.
			{
				ranges.join(range_pair_helper(inseqs[indx], inseqs[indx + 1]));
				indx += 2;
			}
			else if (elen == 2)
			{
				const ui_l32 ucpval = inseqs[indx++];

				if (ucpval != constants::ccstr_empty)
					ranges.join(range_pair_helper(ucpval));
				else
					register_emptystring();
			}
			else if (elen >= 3)
			{
				const ui_l32 seqlen = elen - 1;

				ensure_length(seqlen);

				const ui_l32 inspos = indices[seqlen - 1];

				curseq.resize(seqlen);
				if (!back)
				{
					for (ui_l32 j = 0; j < seqlen; ++j, ++indx)
						curseq[j] = inseqs[indx];
				}
				else
				{
					for (ui_l32 j = seqlen; j; ++indx)
						curseq[--j] = inseqs[indx];
				}

				if (icase)
				{
					for (simple_array<ui_l32>::size_type i = 0; i < curseq.size(); ++i)
						curseq[i] = unicode_case_folding::do_casefolding(curseq[i]);
				}

				const std::size_t complen = seqlen * sizeof (ui_l32);

				for (ui_l32 i = indices[seqlen];; i += seqlen)
				{
					if (i == inspos)
					{
						seqs.insert(inspos, curseq);
						for (ui_l32 j = 0; j < seqlen; ++j)
							indices[j] += seqlen;
						break;
					}

					if (std::memcmp(&seqs[i], curseq.data(), complen) == 0)
						break;
				}

			}
			//elen == 0: Padding.
		}

//		if (this->is_icase())
		if (icase)
			ranges.make_caseunfoldedcharset();

		check_lengths();

	}

private:

	void register_emptystring()
	{
		if (indices.size() < 2)
		{
			indices.resize(2);
			indices[1] = 0;
			indices[0] = 1;
		}
		else if (indices[0] == indices[1])
		{
			++indices[0];
		}
		length.first = 0;
	}

	void unregister_emptystring()
	{
		if (indices.size() >= 2 && indices[0] != indices[1])
			indices[0] = indices[1];
	}

	void ensure_length(const ui_l32 seqlen)
	{
		ui_l32 curlen = static_cast<ui_l32>(indices.size());

		if (seqlen >= curlen)
		{
			indices.resize(seqlen + 1);
			for (; curlen <= seqlen; ++curlen)
				indices[curlen] = 0;
		}
	}

	ui_l32 find_seq(const ui_l32 *const seqbegin, const ui_l32 seqlen, const std::size_t complen) const
	{
		const ui_l32 end = indices[seqlen - 1];

		for (ui_l32 begin = indices[seqlen]; begin < end; begin += seqlen)
		{
			if (std::memcmp(seqbegin, &seqs[begin], complen) == 0)
				return begin;
		}
		return end;
	}

	void check_lengths()
	{
		length.set(constants::max_u32value, 0);

		for (ui_l32 i = 2; i < static_cast<ui_l32>(indices.size()); ++i)
		{
			if (indices[i] != indices[i - 1])
			{
				if (length.first > i)
					length.first = i;
				if (length.second < i)
					length.second = i;
			}
		}

		if (ranges.size())
		{
			if (length.first > 1)
				length.first = 1;
			if (length.second < 1)
				length.second = 1;
		}

		if (has_empty())
			length.first = 0;

		if (length.second == 0)
			length.first = 0;
	}
};
//  posdata_holder

	}	//  namespace re_detail

//  ... "rei_upos.hpp"]
//  ["rei_compiler.hpp" ...

	namespace re_detail
	{

template <typename charT, typename traits>
struct re_object_core
{
protected:

	typedef re_state/*<charT>*/ state_type;
	typedef simple_array<state_type> state_array;

	state_array NFA_states;
	re_character_class character_class;

#if !defined(SRELLDBG_NO_1STCHRCLS)
	#if !defined(SRELLDBG_NO_BITSET)
	bitset<traits::utf_traits::bitsetsize> firstchar_class_bs;
	#else
	range_pairs firstchar_class;
	#endif
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
public:

	std::size_t limit_counter;

protected:
#endif

	typedef typename traits::utf_traits utf_traits;

	ui_l32 number_of_brackets;
	ui_l32 number_of_counters;
	ui_l32 number_of_repeats;
	regex_constants::syntax_option_type soflags;

#if !defined(SRELL_NO_NAMEDCAPTURE)
	groupname_mapper<charT> namedcaptures;
	typedef typename groupname_mapper<charT>::gname_string gname_string;
#endif

#if !defined(SRELLDBG_NO_BMH)
	re_bmh<charT, utf_traits> *bmdata;
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
private:

	static const std::size_t lcounter_defnum_ = 16777216;

#endif

protected:

	re_object_core() :
#if !defined(SRELL_NO_LIMIT_COUNTER)
		limit_counter(lcounter_defnum_),
#endif
		number_of_repeats(0u)
#if !defined(SRELLDBG_NO_BMH)
		, bmdata(NULL)
#endif
	{
	}

	re_object_core(const re_object_core &right)
#if !defined(SRELLDBG_NO_BMH)
		: bmdata(NULL)
#endif
	{
		operator=(right);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	re_object_core(re_object_core &&right) SRELL_NOEXCEPT
#if !defined(SRELLDBG_NO_BMH)
		: bmdata(NULL)
#endif
	{
		operator=(std::move(right));
	}
#endif

#if !defined(SRELLDBG_NO_BMH)
	~re_object_core()
	{
		if (bmdata)
			delete bmdata;
	}
#endif

	void reset(const regex_constants::syntax_option_type flags)
	{
		NFA_states.clear();
		character_class.clear();

#if !defined(SRELLDBG_NO_1STCHRCLS)
	#if !defined(SRELLDBG_NO_BITSET)
		firstchar_class_bs.reset();
	#else
		firstchar_class.clear();
	#endif
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
		limit_counter = lcounter_defnum_;
#endif

		number_of_brackets = 1;
		number_of_counters = 0;
		number_of_repeats  = 0;
		soflags = flags;	//  regex_constants::ECMAScript;

#if !defined(SRELL_NO_NAMEDCAPTURE)
		namedcaptures.clear();
#endif

#if !defined(SRELLDBG_NO_BMH)
		if (bmdata)
			delete bmdata;
		bmdata = NULL;
#endif
	}

	re_object_core &operator=(const re_object_core &that)
	{
		if (this != &that)
		{
			this->NFA_states = that.NFA_states;
			this->character_class = that.character_class;

#if !defined(SRELLDBG_NO_1STCHRCLS)
	#if !defined(SRELLDBG_NO_BITSET)
			this->firstchar_class_bs = that.firstchar_class_bs;
	#else
			this->firstchar_class = that.firstchar_class;
	#endif
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
			this->limit_counter = that.limit_counter;
#endif

			this->number_of_brackets = that.number_of_brackets;
			this->number_of_counters = that.number_of_counters;
			this->number_of_repeats = that.number_of_repeats;
			this->soflags = that.soflags;

#if !defined(SRELL_NO_NAMEDCAPTURE)
			this->namedcaptures = that.namedcaptures;
#endif

#if !defined(SRELLDBG_NO_BMH)
			if (that.bmdata)
			{
				if (this->bmdata)
					*this->bmdata = *that.bmdata;
				else
					this->bmdata = new re_bmh<charT, utf_traits>(*that.bmdata);
			}
			else if (this->bmdata)
			{
				delete this->bmdata;
				this->bmdata = NULL;
			}
#endif

			if (that.NFA_states.size())
				repair_nextstates(&that.NFA_states[0]);
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	re_object_core &operator=(re_object_core &&that) SRELL_NOEXCEPT
	{
		if (this != &that)
		{
			this->NFA_states = std::move(that.NFA_states);
			this->character_class = std::move(that.character_class);

#if !defined(SRELLDBG_NO_1STCHRCLS)
	#if !defined(SRELLDBG_NO_BITSET)
			this->firstchar_class_bs = std::move(that.firstchar_class_bs);
	#else
			this->firstchar_class = std::move(that.firstchar_class);
	#endif
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
			this->limit_counter = that.limit_counter;
#endif

			this->number_of_brackets = that.number_of_brackets;
			this->number_of_counters = that.number_of_counters;
			this->number_of_repeats = that.number_of_repeats;
			this->soflags = that.soflags;

#if !defined(SRELL_NO_NAMEDCAPTURE)
			this->namedcaptures = std::move(that.namedcaptures);
#endif

#if !defined(SRELLDBG_NO_BMH)
			if (this->bmdata)
				delete this->bmdata;
			this->bmdata = that.bmdata;
			that.bmdata = NULL;
#endif
		}
		return *this;
	}
#endif	//  defined(SRELL_CPP11_MOVE_ENABLED)

	void swap(re_object_core &right)
	{
		if (this != &right)
		{
			this->NFA_states.swap(right.NFA_states);
			this->character_class.swap(right.character_class);

#if !defined(SRELLDBG_NO_1STCHRCLS)
	#if !defined(SRELLDBG_NO_BITSET)
			this->firstchar_class_bs.swap(right.firstchar_class_bs);
	#else
			this->firstchar_class.swap(right.firstchar_class);
	#endif
#endif

#if !defined(SRELL_NO_LIMIT_COUNTER)
			{
				const std::size_t tmp_limit_counter = this->limit_counter;
				this->limit_counter = right.limit_counter;
				right.limit_counter = tmp_limit_counter;
			}
#endif

			{
				const ui_l32 tmp_numof_brackets = this->number_of_brackets;
				this->number_of_brackets = right.number_of_brackets;
				right.number_of_brackets = tmp_numof_brackets;
			}
			{
				const ui_l32 tmp_numof_counters = this->number_of_counters;
				this->number_of_counters = right.number_of_counters;
				right.number_of_counters = tmp_numof_counters;
			}
			{
				const ui_l32 tmp_numof_repeats = this->number_of_repeats;
				this->number_of_repeats = right.number_of_repeats;
				right.number_of_repeats = tmp_numof_repeats;
			}
			{
				const regex_constants::syntax_option_type tmp_soflags = this->soflags;
				this->soflags = right.soflags;
				right.soflags = tmp_soflags;
			}

#if !defined(SRELL_NO_NAMEDCAPTURE)
			this->namedcaptures.swap(right.namedcaptures);
#endif

#if !defined(SRELLDBG_NO_BMH)
			{
				re_bmh<charT, utf_traits> *const tmp_bmdata = this->bmdata;
				this->bmdata = right.bmdata;
				right.bmdata = tmp_bmdata;
			}
#endif
		}
	}

	bool set_error(const regex_constants::error_type e)
	{
//		reset();
		NFA_states.clear();
		number_of_repeats = static_cast<ui_l32>(e);
		return false;
	}

	regex_constants::error_type ecode() const
	{
		return NFA_states.size() ? 0 : static_cast<regex_constants::error_type>(number_of_repeats);
	}

private:

	void repair_nextstates(const state_type *const oldbase)
	{
		state_type *const newbase = &this->NFA_states[0];

		for (typename state_array::size_type i = 0; i < this->NFA_states.size(); ++i)
		{
			state_type &state = this->NFA_states[i];

			if (state.next_state1)
				state.next_state1 = state.next_state1 - oldbase + newbase;

			if (state.next_state2)
				state.next_state2 = state.next_state2 - oldbase + newbase;
		}
	}
};
//  re_object_core

template <typename charT, typename traits>
class re_compiler : public re_object_core<charT, traits>
{
protected:

	template <typename ForwardIterator>
	bool compile(ForwardIterator begin, const ForwardIterator end, const regex_constants::syntax_option_type flags /* = regex_constants::ECMAScript */)
	{
		simple_array<ui_l32> u32;

		while (begin != end)
		{
			const ui_l32 u32c = utf_traits::codepoint_inc(begin, end);

			if (u32c > constants::unicode_max_codepoint)
			{
				this->set_error(regex_constants::error_utf8);
				goto COMPILING_FAILURE;
			}
			u32.push_backncr(u32c);
		}

		if (!compile_core(u32.data(), u32.data() + u32.size(), flags))
		{
			COMPILING_FAILURE:
#if !defined(SRELLDBG_NO_BMH)
			if (this->bmdata)
				delete this->bmdata;
			this->bmdata = NULL;
#endif
#if !defined(SRELL_NO_THROW)
			throw regex_error(this->number_of_repeats);
#else
			return false;
#endif
		}
		return true;
	}

	bool is_icase() const
	{
#if !defined(SRELL_NO_ICASE)
		if (this->soflags & regex_constants::icase)
			return true;
#endif
		return false;
	}
	bool is_ricase() const
	{
#if !defined(SRELL_NO_ICASE)
		return /* this->NFA_states.size() && */ this->NFA_states[0].icase ? true : false;
#else
		return false;
#endif
	}

	bool is_multiline() const
	{
		if (this->soflags & regex_constants::multiline)
			return true;
		return false;
	}

	bool is_dotall() const
	{
		return (this->soflags & regex_constants::dotall) ? true : false;
	}

	bool is_vmode() const
	{
#if !defined(SRELL_NO_VMODE) && !defined(SRELL_NO_UNICODE_PROPERTY)
		return (this->soflags & regex_constants::unicodesets) ? true : false;
#else
		return false;
#endif

	}

	bool is_optimize() const
	{
		return (this->soflags & regex_constants::optimize) ? true : false;
	}

private:

	typedef re_object_core<charT, traits> base_type;
	typedef typename base_type::utf_traits utf_traits;
	typedef typename base_type::state_type state_type;
	typedef typename base_type::state_array state_array;
#if !defined(SRELL_NO_NAMEDCAPTURE)
	typedef typename base_type::gname_string gname_string;
#endif
#if !defined(SRELL_NO_UNICODE_PROPERTY)
	typedef typename re_character_class::pstring pstring;
#endif
	typedef typename state_array::size_type state_size_type;

	typedef simple_array<ui_l32> u32array;
	typedef typename u32array::size_type u32array_size_type;

	typedef re_compiler_state<charT> cvars_type;

	bool compile_core(const ui_l32 *begin, const ui_l32 *const end, const regex_constants::syntax_option_type flags)
	{
		re_quantifier piecesize;
		cvars_type cvars;
		state_type flstate;

		this->reset(flags);
//		this->soflags = flags;
		cvars.reset(flags, begin);

		flstate.reset(st_epsilon);
		flstate.next2 = 1;
		this->NFA_states.push_back(flstate);

		if (!make_nfa_states(this->NFA_states, piecesize, begin, end, cvars))
		{
			return false;
		}

		if (begin != end)
			return this->set_error(regex_constants::error_paren);	//  ')'s are too many.

		if (!check_backreferences(cvars))
			return this->set_error(regex_constants::error_backref);

#if !defined(SRELL_NO_ICASE)
		if (this->is_icase())
			this->NFA_states[0].icase = check_if_really_needs_icase_search();
#endif

#if !defined(SRELLDBG_NO_BMH)
		setup_bmhdata();
#endif

		flstate.type = st_success;
		flstate.next1 = 0;
		flstate.next2 = 0;
		this->NFA_states.push_back(flstate);

		optimise();
		relativejump_to_absolutejump();

		return true;
	}

	bool make_nfa_states(state_array &piece, re_quantifier &piecesize, const ui_l32 *&curpos, const ui_l32 *const end, cvars_type &cvars)
	{
		state_size_type prevbranch_end = 0;
		state_type bstate;
		state_array branch;
		re_quantifier branchsize;

		piecesize.set(constants::infinity, 0u);

		bstate.reset(st_epsilon, epsilon_type::et_alt);

		for (;;)
		{
			branch.clear();

			if (!make_branch(branch, branchsize, curpos, end, cvars))
				return false;

			if (!piecesize.is_valid() || piecesize.atleast > branchsize.atleast)
				piecesize.atleast = branchsize.atleast;

			if (piecesize.atmost < branchsize.atmost)
				piecesize.atmost = branchsize.atmost;

			if (curpos != end && *curpos == meta_char::mc_bar)
			{
				bstate.next2 = static_cast<std::ptrdiff_t>(branch.size()) + 2;
				branch.insert(0, bstate);
			}

			if (prevbranch_end)
			{
				state_type &pbend = piece[prevbranch_end];

				pbend.next1 = static_cast<std::ptrdiff_t>(branch.size()) + 1;
				pbend.char_num = epsilon_type::et_brnchend;	//  '/'
			}

			piece += branch;

			if (curpos == end || *curpos == meta_char::mc_rbracl)
				break;

			//  *curpos == '|'

			prevbranch_end = piece.size();
			bstate.next2 = 0;
			piece.push_back(bstate);

			++curpos;
		}
		return true;
	}

	bool make_branch(state_array &branch, re_quantifier &branchsize, const ui_l32 *&curpos, const ui_l32 *const end, cvars_type &cvars)
	{
		state_array piece;
		state_array piece_with_quantifier;
		re_quantifier quantifier;
		re_quantifier piecesize;
		state_type astate;

		branchsize.reset(0);

		for (;;)
		{

			if (curpos == end || *curpos == meta_char::mc_bar || *curpos == meta_char::mc_rbracl /* || *curpos == char_ctrl::cc_nul */)	//  '|', ')', '\0'.
				return true;

			piece.clear();
			piece_with_quantifier.clear();

			astate.reset(st_character, *curpos++);

			switch (astate.char_num)
			{
			case meta_char::mc_rbraop:	//  '(':
				if (!parse_group(piece, piecesize, curpos, end, cvars))
					return false;
				goto AFTER_PIECE_SET;

			case meta_char::mc_sbraop:	//  '[':
#if !defined(SRELL_NO_VMODE) && !defined(SRELL_NO_UNICODE_PROPERTY)
				if (this->is_vmode())	//  vmode.
				{
					if (!parse_charclass_v(piece, piecesize, curpos, end, cvars))
						return false;
					goto AFTER_PIECE_SET;
				}
#endif
				if (!register_character_class(astate, curpos, end, cvars))
					return false;

				break;

			case meta_char::mc_escape:	//  '\\':
				if (curpos == end)
					return this->set_error(regex_constants::error_escape);

				astate.char_num = *curpos;

#if !defined(SRELL_NO_VMODE) && !defined(SRELL_NO_UNICODE_PROPERTY)
				if (this->is_vmode() && ((astate.char_num | constants::asc_icase) == char_alnum::ch_p))
				{
					posdata_holder pos;

					if (!parse_escape_p_vmode(pos, astate, ++curpos, end, cvars))
						return false;

					if (astate.type == st_character_class)
						astate.char_num = this->character_class.register_newclass(pos.ranges);
					else
						transform_seqdata(piece, pos);

					piecesize.set(astate.quantifier.atleast, astate.quantifier.atmost);

					if (piece.size())
						goto AFTER_PIECE_SET;

					break;
				}
#endif	//  !defined(SRELL_NO_VMODE) && !defined(SRELL_NO_UNICODE_PROPERTY)

				switch (astate.char_num)
				{
				case char_alnum::ch_B:	//  'B':
					astate.is_not = 1u;
					//@fallthrough@

				case char_alnum::ch_b:	//  'b':
					astate.type = st_boundary;	//  \b, \B.
					astate.quantifier.reset(0);
					if (this->is_icase())
					{
						this->character_class.setup_icase_word();
						astate.char_num = static_cast<ui_l32>(re_character_class::icase_word);
					}
					else
						astate.char_num = static_cast<ui_l32>(re_character_class::word);	//  \w, \W.
					break;

//				case char_alnum::ch_A:	//  'A':
//					astate.type   = st_bol;	//  '\A'
//				case char_alnum::ch_Z:	//  'Z':
//					astate.type   = st_eol;	//  '\Z'
//				case char_alnum::ch_z:	//  'z':
//					astate.type   = st_eol;	//  '\z'
//				case char_alnum::ch_R:	//  'R':
				//  (?>\r\n?|[\x0A-\x0C\x85\u{2028}\u{2029}])

				//  Backreferences.

#if !defined(SRELL_NO_NAMEDCAPTURE)
				//  Prepared for named captures.
				case char_alnum::ch_k:	//  'k':
					if (++curpos == end || *curpos != meta_char::mc_lt)
						return this->set_error(regex_constants::error_escape);
					else
					{
						const gname_string groupname = get_groupname(++curpos, end, cvars);

						if (groupname.size() == 0)
							return this->set_error(regex_constants::error_escape);

						astate.char_num = this->namedcaptures[groupname];

						if (astate.char_num != groupname_mapper<charT>::notfound)
							astate.icase_backrefno_unresolved = 0u;
						else
						{
							astate.icase_backrefno_unresolved = 2u;
							astate.char_num = static_cast<ui_l32>(cvars.unresolved_gnames.size());
							cvars.unresolved_gnames.push_back(groupname, astate.char_num);
						}
						goto BACKREF_POSTPROCESS;
					}
#endif
				default:

					if (astate.char_num >= char_alnum::ch_1 && astate.char_num <= char_alnum::ch_9)	//  \1, \9.
					{
						astate.char_num = translate_numbers(curpos, end, 10, 0, 0, 0xfffffffe);
							//  22.2.1.1 Static Semantics: Early Errors:
							//  It is a Syntax Error if NcapturingParens >= 23^2 - 1.

						if (astate.char_num == constants::invalid_u32value)
							return this->set_error(regex_constants::error_escape);

						astate.icase_backrefno_unresolved = 0u;

#if !defined(SRELL_NO_NAMEDCAPTURE)
						BACKREF_POSTPROCESS:
#endif
						astate.next2 = 1;
						astate.type = st_backreference;
						astate.quantifier.atleast = 0;

						if (this->is_icase())
							astate.icase |= 1u;

						goto AFTER_INCREMENT;
					}

					++curpos;
					if (!translate_escape(NULL, astate, curpos, end, false, false))
						return false;
					goto AFTER_INCREMENT;
				}

				++curpos;
				AFTER_INCREMENT:

				break;

			case meta_char::mc_period:	//  '.':
				astate.type = st_character_class;
#if !defined(SRELL_NO_SINGLELINE)
				if (this->is_dotall())
				{
					astate.char_num = static_cast<ui_l32>(re_character_class::dotall);
				}
				else
#endif
				{
//					astate.char_num = static_cast<ui_l32>(re_character_class::newline);
					range_pairs nlclass = this->character_class[static_cast<ui_l32>(re_character_class::newline)];

					nlclass.negation();
					astate.char_num = this->character_class.register_newclass(nlclass);
				}
				break;

			case meta_char::mc_caret:	//  '^':
				astate.type = st_bol;
				astate.char_num = static_cast<ui_l32>(re_character_class::newline);
				astate.quantifier.reset(0);
//				if (current_flags.m)
				if (is_multiline())
					astate.multiline = 1u;
				break;

			case meta_char::mc_dollar:	//  '$':
				astate.type = st_eol;
				astate.char_num = static_cast<ui_l32>(re_character_class::newline);
				astate.quantifier.reset(0);
//				if (current_flags.m)
				if (is_multiline())
					astate.multiline = 1u;
				break;

			case meta_char::mc_astrsk:	//  '*':
			case meta_char::mc_plus:	//  '+':
			case meta_char::mc_query:	//  '?':
			case meta_char::mc_cbraop:	//  '{'
				return this->set_error(regex_constants::error_badrepeat);

			default:;
			}

			if (astate.type == st_character)
			{
				if (this->is_icase())
					astate.char_num = unicode_case_folding::do_casefolding(astate.char_num);
			}

			piece.push_back(astate);
			piecesize = astate.quantifier;
			AFTER_PIECE_SET:

			if (piece.size())
			{
				const state_type &firststate = piece[0];

				quantifier.reset();	//  quantifier.atleast = quantifier.atmost = 1;

				if (firststate.has_quantifier() && curpos != end)
				{
					switch (*curpos)
					{
					case meta_char::mc_astrsk:	//  '*':
						--quantifier.atleast;
						//@fallthrough@

					case meta_char::mc_plus:	//  '+':
						quantifier.set_infinity();
						break;

					case meta_char::mc_query:	//  '?':
						--quantifier.atleast;
						break;

					case meta_char::mc_cbraop:	//  '{':
						++curpos;
						quantifier.atleast = translate_numbers(curpos, end, 10, 1, 0, constants::max_u32value);

						if (quantifier.atleast == constants::invalid_u32value)
							return this->set_error(regex_constants::error_brace);

						if (curpos == end)
							return this->set_error(regex_constants::error_brace);

						if (*curpos == meta_char::mc_comma)	//  ','
						{
							++curpos;
							quantifier.atmost = translate_numbers(curpos, end, 10, 1, 0, constants::max_u32value);

							if (quantifier.atmost == constants::invalid_u32value)
								quantifier.set_infinity();

							if (!quantifier.is_valid())
								return this->set_error(regex_constants::error_badbrace);
						}
						else
							quantifier.atmost = quantifier.atleast;

						if (curpos == end || *curpos != meta_char::mc_cbracl)	//  '}'
							return this->set_error(regex_constants::error_brace);

						//  *curpos == '}'
						break;

					default:
						goto AFTER_GREEDINESS_CHECK;
					}

					if (++curpos != end && *curpos == meta_char::mc_query)	//  '?'
					{
						quantifier.is_greedy = 0u;
						++curpos;
					}
					AFTER_GREEDINESS_CHECK:;
				}

				if (piece.size() == 2 && firststate.is_noncapturinggroup())
				{
					//  (?:) alone or followed by a quantifier.
//					piece_with_quantifier += piece;
					;	//  Does nothing.
				}
				else
					combine_piece_with_quantifier(piece_with_quantifier, piece, quantifier, piecesize);

				piecesize.multiply(quantifier);
				branchsize.add(piecesize);

#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)

				if (!cvars.back)
					branch += piece_with_quantifier;
				else
					branch.insert(0, piece_with_quantifier);
#else
				branch += piece_with_quantifier;
#endif
			}
		}
	}

	//  '('.

	bool parse_group(state_array &piece, re_quantifier &piecesize, const ui_l32 *&curpos, const ui_l32 *const end, cvars_type &cvars)
	{
		const re_flags originalflags(cvars);
		state_type rbstate;

		if (curpos == end)
			return this->set_error(regex_constants::error_paren);

		rbstate.reset(st_roundbracket_open);

		if (*curpos == meta_char::mc_query)	//  '?'
		{
#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
			bool lookbehind = false;
#endif

			if (++curpos == end)
				return this->set_error(regex_constants::error_paren);

			rbstate.char_num = *curpos;

			if (rbstate.char_num == meta_char::mc_lt)	//  '<'
			{
				if (++curpos == end)
					return this->set_error(regex_constants::error_paren);

				rbstate.char_num = *curpos;

				if (rbstate.char_num != meta_char::mc_eq && rbstate.char_num != meta_char::mc_exclam)
				{
#if !defined(SRELL_NO_NAMEDCAPTURE)
					const gname_string groupname = get_groupname(curpos, end, cvars);

					if (groupname.size() == 0)
						return this->set_error(regex_constants::error_escape);

					if (!this->namedcaptures.push_back(groupname, this->number_of_brackets))
						return this->set_error(regex_constants::error_backref);

					goto AFTER_EXTRB;
#else
					return this->set_error(regex_constants::error_paren);
#endif	//  !defined(SRELL_NO_NAMEDCAPTURE)
				}
#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
				lookbehind = true;
#endif
			}
			else
				rbstate.quantifier.atleast = 0;
				//  Sets .atleast to 0 for other assertions than lookbehinds. The automaton
				//  checks .atleast to know whether lookbehinds or other assertions.

			switch (rbstate.char_num)
			{
			case meta_char::mc_exclam:	//  '!':
				rbstate.is_not = 1u;
				//@fallthrough@

			case meta_char::mc_eq:	//  '=':
#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
				cvars.back = lookbehind;
#else
//				rbstate.reverse = lookbehind;
#endif

#if defined(SRELL_ENABLE_GT)
				//@fallthrough@
			case meta_char::mc_gt:
#endif
				rbstate.type = st_lookaround_open;
				rbstate.next2 = 1;
				break;

			default:
#if defined(SRELL_NO_UBMOD)
				return this->set_error(regex_constants::error_paren);
#else
				{
					const u32array_size_type boffset = curpos - cvars.begin;
					regex_constants::syntax_option_type modified = regex_constants::ECMAScript;
					regex_constants::syntax_option_type localflags = this->soflags;
					bool negate = false;
					bool flagerror = false;

					for (;;)
					{
						switch (rbstate.char_num)
						{
#if 0
						case meta_char::mc_colon:	//  ':':
							//  (?ims-ims:...)
							break;
#endif
						case meta_char::mc_rbracl:	//  ')':
							if (boffset == 2 && modified != regex_constants::ECMAScript)
							{
								this->soflags = localflags;
								rbstate.type = st_roundbracket_close;
								++curpos;
								return true;
							}
							flagerror = true;	//  "(?)" or "(?-)"
							break;

						case meta_char::mc_minus:	//  '-':
							if (negate)
								flagerror = true;
							else
								negate = true;
							break;

						case char_alnum::ch_i:	//  'i':
							if (modified & regex_constants::icase)
								flagerror = true;
							modified |= regex_constants::icase;
							if (!negate)
								localflags |= regex_constants::icase;
							else
								localflags &= ~regex_constants::icase;
							break;

						case char_alnum::ch_m:	//  'm':
							if (modified & regex_constants::multiline)
								flagerror = true;
							modified |= regex_constants::multiline;
							if (!negate)
								localflags |= regex_constants::multiline;
							else
								localflags &= ~regex_constants::multiline;
							break;

						case char_alnum::ch_s:	//  's':
							if (modified & regex_constants::dotall)
								flagerror = true;
							modified |= regex_constants::dotall;
							if (!negate)
								localflags |= regex_constants::dotall;
							else
								localflags &= ~regex_constants::dotall;
							break;

#if 0
#if !defined(SRELL_NO_VMODE) && !defined(SRELL_NO_UNICODE_PROPERTY)

						case char_alnum::ch_v:	//  'v':
							if (modified & regex_constants::unicodesets)
								flagerror = true;
							modified |= regex_constants::unicodesets;
							if (!negate)
								localflags |= regex_constants::unicodesets;
							else
								localflags &= ~regex_constants::unicodesets;
							break;
#endif
#endif
						default:
							return this->set_error(regex_constants::error_paren);
						}

						if (flagerror)
							return this->set_error(regex_constants::error_modifier);

						if (++curpos == end)
							return this->set_error(regex_constants::error_paren);

						rbstate.char_num = *curpos;
					}
				}
#endif	//  defined(SRELL_NO_UBMOD)
				//@fallthrough@

			case meta_char::mc_colon:
				rbstate.type = st_epsilon;
				rbstate.char_num = epsilon_type::et_ncgopen;
				rbstate.quantifier.atleast = this->number_of_brackets;
			}

			++curpos;
			piece.push_back(rbstate);
		}
#if !defined(SRELL_NO_NAMEDCAPTURE)
		AFTER_EXTRB:
#endif

		if (rbstate.type == st_roundbracket_open)
		{
			rbstate.char_num = this->number_of_brackets;
			rbstate.next1  = 2;
			rbstate.next2  = 1;
			piece.push_back(rbstate);
			++this->number_of_brackets;

			rbstate.type  = st_roundbracket_pop;
			rbstate.next1 = 0;
			rbstate.next2 = 0;
			piece.push_back(rbstate);
		}

		if (!make_nfa_states(piece, piecesize, curpos, end, cvars))
			return false;

		//  end or ')'?
		if (curpos == end)
			return this->set_error(regex_constants::error_paren);

		++curpos;

		cvars.restore_from(originalflags);

		switch (rbstate.type)
		{
		case st_epsilon:
			{
				state_type &firststate = piece[0];

				if (piece.size() == 2)	//  ':' + something.
				{
					piece.erase(0);
					return true;
				}

				firststate.quantifier.atmost = this->number_of_brackets - 1;
				firststate.quantifier.is_greedy = piecesize.atleast != 0u;
				rbstate.char_num = epsilon_type::et_ncgclose;
			}
			break;

//		case st_lookaround_pop:
		case st_lookaround_open:
			{
				state_type &firststate = piece[0];

#if defined(SRELL_FIXEDWIDTHLOOKBEHIND)
//				if (firststate.reverse)
				if (firststate.quantifier.atleast)	//  > 0 means lookbehind.
				{
					if (!piecesize.is_same() || piecesize.is_infinity())
						return this->set_error(regex_constants::error_lookbehind);

					firststate.quantifier = piecesize;
				}
#endif

#if defined(SRELL_ENABLE_GT)
				if (firststate.char_num != meta_char::mc_gt)
#endif
					piecesize.reset(0);

				firststate.next1 = static_cast<std::ptrdiff_t>(piece.size()) + 1;

				rbstate.type  = st_lookaround_close;
				rbstate.next1 = 0;
				rbstate.next2 = 0;
			}
			break;

		default:
			rbstate.type = st_roundbracket_close;
			rbstate.next1 = 1;
			rbstate.next2 = 1;

			re_quantifier &rb_open = piece[0].quantifier;
			re_quantifier &rb_pop = piece[1].quantifier;

			rb_open.atleast = rb_pop.atleast = rbstate.char_num + 1;
			rb_open.atmost = rb_pop.atmost = this->number_of_brackets - 1;
			rb_open.is_greedy = piecesize.atleast != 0u;
		}

		piece.push_back(rbstate);
		return true;
	}

	void combine_piece_with_quantifier(state_array &piece_with_quantifier, state_array &piece, const re_quantifier &quantifier, const re_quantifier &piecesize)
	{
		state_type &firststate = piece[0];
		const bool piece_has_0widthchecker = firststate.has_0widthchecker();
		const bool piece_is_noncapturinggroup_containing_capturinggroup = firststate.is_noncapturinggroup_containing_capturinggroup();
		state_type qstate;

		if (quantifier.atmost == 0)
			return;

		qstate.reset();
		qstate.quantifier = quantifier;

		if (firststate.is_character_or_class())
			qstate.char_num = epsilon_type::et_ccastrsk;

		if (quantifier.atmost == 1)
		{
			if (quantifier.atleast == 0)
			{
				qstate.type  = st_epsilon;
				qstate.next2 = static_cast<std::ptrdiff_t>(piece.size()) + 1;

				if (!quantifier.is_greedy)
				{
					qstate.next1 = qstate.next2;
					qstate.next2 = 1;
				}

				piece[piece.size() - 1].quantifier = quantifier;

				piece_with_quantifier.push_back(qstate);
			}

			piece_with_quantifier += piece;
			return;
		}

		//  atmost >= 2

#if !defined(SRELLDBG_NO_SIMPLEEQUIV)

		//  A counter requires at least 6 states: save, restore, check, inc, dec, ATOM(s).
		//  A character or charclass quantified by one of these has a simple equivalent representation:
		//  a{0,2}  1.epsilon(2|5), 2.CHorCL(3), 3.epsilon(4|5), 4.CHorCL(5), [5].
		//  a{0,3}  1.epsilon(2|7), 2.CHorCL(3), 3.epsilon(4|7), 4.CHorCL(5), 5.epsilon(6|7), 6.CHorCL(7), [7].
		//  a{1,2}  1.CHorCL(2), 2.epsilon(3|4), 3.CHorCL(4), [4].
		//  a{1,3}  1.CHorCL(2), 2.epsilon(3|6), 3.CHorCL(4), 4.epsilon(5|6), 5.CHorCL(6), [6].
		//  a{2,3}  1.CHorCL(2), 2.CHorCL(3), 3.epsilon(4|5), 4.CHorCL(5), [5].
		//  a{2,4}  1.CHorCL(2), 2.CHorCL(3), 3.epsilon(4|7), 4.CHorCL(5), 5.epsilon(6|7), 6.CHorCL(7), [7].
		if (firststate.is_character_or_class() && quantifier.has_simple_equivalence())
		{
			const state_size_type branchsize = piece.size() + 1;

			for (ui_l32 i = 0; i < quantifier.atleast; ++i)
				piece_with_quantifier += piece;

			if (qstate.char_num == epsilon_type::et_ccastrsk)
				firststate.quantifier.set(0, 1, quantifier.is_greedy);

			qstate.type = st_epsilon;
			qstate.next2 = (quantifier.atmost - quantifier.atleast) * branchsize;
			if (!quantifier.is_greedy)
			{
				qstate.next1 = qstate.next2;
				qstate.next2 = 1;
			}
			for (ui_l32 i = quantifier.atleast; i < quantifier.atmost; ++i)
			{
				piece_with_quantifier.push_back(qstate);
				piece_with_quantifier += piece;
				quantifier.is_greedy ? (qstate.next2 -= branchsize) : (qstate.next1 -= branchsize);
			}
			return;
		}
#endif	//  !defined(SRELLDBG_NO_SIMPLEEQUIV)

		qstate.type = st_epsilon;

		if (firststate.is_noncapturinggroup() && piecesize.atleast == 0)
			goto USE_COUNTER;

		if (quantifier.is_asterisk())	//  {0,}
		{
			//  greedy:  1.epsilon(2|4), 2.piece, 3.LAorC0WR(1|0), 4.OutOfLoop.
			//  !greedy: 1.epsilon(4|2), 2.piece, 3.LAorC0WR(1|0), 4.OutOfLoop.
			//  LAorC0WR: LastAtomOfPiece or Check0WidthRepeat.
			//  qstate.type points to 1.
		}
		else if (quantifier.is_plus())	//  {1,}
		{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)

			if (firststate.is_character_or_class())
			{
				piece_with_quantifier += piece;
				--qstate.quantifier.atleast;	//  /.+/ -> /..*/.
			}
			else
#endif
			{
				const ui_l32 backup = qstate.char_num;

				qstate.next1 = 2;
				qstate.next2 = 0;
				qstate.char_num = epsilon_type::et_jmpinlp;
				piece_with_quantifier.push_back(qstate);
				qstate.char_num = backup;
				//  greedy:  1.epsilon(3), 2.epsilon(3|5), 3.piece, 4.LAorC0WR(2|0), 5.OutOfLoop.
				//  !greedy: 1.epsilon(3), 2.epsilon(5|3), 3.piece, 4.LAorC0WR(2|0), 5.OutOfLoop.
				//  qstate.type points to 2.
			}
		}
		else
		{
			USE_COUNTER:

			qstate.char_num = this->number_of_counters;
			++this->number_of_counters;

			qstate.type = st_save_and_reset_counter;
			qstate.next1 = 2;
			qstate.next2 = 1;
			piece_with_quantifier.push_back(qstate);

			qstate.type  = st_restore_counter;
			qstate.next1 = 0;
			qstate.next2 = 0;
			piece_with_quantifier.push_back(qstate);
			//  1.save_and_reset_counter(3|2), 2.restore_counter(0|0),

			qstate.next1 = 0;
			qstate.next2 = 0;
			qstate.type = st_decrement_counter;
			piece.insert(0, qstate);

			qstate.next1 = 2;
			qstate.next2 = piece[1].is_character_or_class() ? 0 : 1;
			qstate.type = st_epsilon;	//  st_increment_counter;
			piece.insert(0, qstate);
			piece[0].char_num = epsilon_type::et_default;

			qstate.type = st_check_counter;
			//  greedy:  3.check_counter(4|6), 4.piece, 5.LAorC0WR(3|0), 6.OutOfLoop.
			//  !greedy: 3.check_counter(6|4), 4.piece, 5.LAorC0WR(3|0), 6.OutOfLoop.
			//  4.piece = { 4a.increment_counter(4c|4b), 4b.decrement_counter(0|0), 4c.OriginalPiece }.
		}

		//  qstate.type is epsilon or check_counter.
		//  Its "next"s point to piece and OutOfLoop.

		if (!piece_is_noncapturinggroup_containing_capturinggroup && (piecesize.atleast || piece_has_0widthchecker))
		{
			const state_size_type piece_size = piece.size();
			state_type &laststate = piece[piece_size - 1];

			laststate.quantifier = qstate.quantifier;
			laststate.next1 = 0 - static_cast<std::ptrdiff_t>(piece_size);
				//  Points to the one immediately before piece, which will be pushed last in this block.

			//  qstate.type has already been set. epsilon or check_counter.
			qstate.next1 = 1;
			qstate.next2 = static_cast<std::ptrdiff_t>(piece_size) + 1;
			if (!quantifier.is_greedy)
			{
				qstate.next1 = qstate.next2;
				qstate.next2 = 1;
			}
			piece_with_quantifier.push_back(qstate);
		}
		else
		{
			//  qstate.type has already been set. epsilon or check_counter.
			qstate.next1 = 1;
			qstate.next2 = static_cast<std::ptrdiff_t>(piece.size()) + 4;	//  To OutOfLoop.
				//  The reason for +3 than above is that push, pop, and check_0_width are added below.
			if (!quantifier.is_greedy)
			{
				qstate.next1 = qstate.next2;
				qstate.next2 = 1;
			}
			piece_with_quantifier.push_back(qstate);	//  *1

			qstate.char_num = this->number_of_repeats;
			++this->number_of_repeats;

			const state_size_type org1stpos = (qstate.type == st_check_counter) ? 2 : 0;

			qstate.type  = st_check_0_width_repeat;
			qstate.next1 = 0 - static_cast<std::ptrdiff_t>(piece.size()) - 3;	//  3 for push, pop, and this. Points to *1.
			qstate.next2 = 1;
			piece.push_back(qstate);

			if (piece_is_noncapturinggroup_containing_capturinggroup)
				qstate.quantifier = piece[org1stpos].quantifier;
			else
				qstate.quantifier.set(1, 0);

			qstate.type = st_repeat_in_pop;
			qstate.next1 = 0;
			qstate.next2 = 0;
			piece.insert(org1stpos, qstate);

			qstate.type = st_repeat_in_push;
			qstate.next1 = 2;
			qstate.next2 = 1;
			piece.insert(org1stpos, qstate);

				//  greedy:  1.epsilon(2|6),
				//  !greedy: 1.epsilon(6|2),
				//    2.repeat_in_push(4|3), 3.repeat_in_pop(0|0), 4.piece,
				//    5.check_0_width_repeat(1|6), 6.OutOfLoop.
				//  or
				//  greedy:  1.check_counter(2|8),
				//  !greedy: 1.check_counter(8|2),
				//    2.increment_counter(4|3), 3.decrement_counter(0|0)
				//    4.repeat_in_push(6|5), 5.repeat_in_pop(0|0), 6.piece,
				//    7.check_0_width_repeat(1|8), 8.OutOfLoop.
		}
		piece_with_quantifier += piece;
	}

	//  '['.

	bool register_character_class(state_type &castate, const ui_l32 *&curpos, const ui_l32 *const end, const cvars_type & /* cvars */)
	{
		range_pairs ranges;
		range_pair code_range;
		state_type rstate;
		range_pairs curranges;

		if (curpos == end)
			return this->set_error(regex_constants::error_brack);

		castate.type = st_character_class;

		if (*curpos == meta_char::mc_caret)	//  '^'
		{
			castate.is_not = 1u;
			++curpos;
		}

		for (;;)
		{
			if (curpos == end)
				return this->set_error(regex_constants::error_brack);

			if (*curpos == meta_char::mc_sbracl)	//   ']'
				break;

			rstate.reset();

			if (!get_character_in_class(curranges, rstate, curpos, end))
				return false;

			if (rstate.type == st_character_class)
			{
				ranges.merge(curranges);

				if (curpos != end && *curpos == meta_char::mc_minus)	//  '-'
				{
					if (++curpos == end)
						return this->set_error(regex_constants::error_brack);

					if (*curpos == meta_char::mc_sbracl)
						break;	// OK.

					return this->set_error(regex_constants::error_brack);
				}
				continue;
			}

			code_range.first = code_range.second = rstate.char_num;

			if (curpos == end)
				return this->set_error(regex_constants::error_brack);

			if (*curpos == meta_char::mc_minus)	//  '-'
			{
				++curpos;

				if (curpos == end)
					return this->set_error(regex_constants::error_brack);

				if (*curpos == meta_char::mc_sbracl)
				{
					PUSH_SEPARATELY:
					ranges.join(code_range);
					code_range.first = code_range.second = meta_char::mc_minus;
				}
				else
				{
					if (!get_character_in_class(curranges, rstate, curpos, end))
						return false;

					if (rstate.type == st_character_class)
					{
						ranges.merge(curranges);
						goto PUSH_SEPARATELY;
					}

					code_range.second = rstate.char_num;

					if (!code_range.is_range_valid())
						return this->set_error(regex_constants::error_range);
				}
			}
			ranges.join(code_range);
		}

		//  *curpos == ']'
		++curpos;
		if (this->is_icase())
			ranges.make_caseunfoldedcharset();

		if (castate.is_not)
		{
			ranges.negation();
			castate.is_not = 0u;
		}

//		castate.char_num = this->is_icase() ? ranges.template consists_of_one_character<unicode_case_folding>() : ranges.template consists_of_one_character<nocase_faketraits>();
		castate.char_num = ranges.consists_of_one_character(this->is_icase());

		if (castate.char_num != constants::invalid_u32value)
		{
			castate.type = st_character;
			return true;
		}

		castate.char_num = this->character_class.register_newclass(ranges);

		return true;
	}

	bool get_character_in_class(range_pairs &rp, state_type &rstate, const ui_l32 *&curpos, const ui_l32 *const end /* , const re_compiler_state &cvars */)
	{
		rstate.char_num = *curpos++;

		if (rstate.char_num != meta_char::mc_escape)	//  '\\'
			return true;

		rp.clear();

		if (curpos == end)
			return this->set_error(regex_constants::error_escape);

		rstate.char_num = *curpos++;

		return translate_escape(&rp, rstate, curpos, end, true, false);
	}

	void add_predefclass_to_charclass(range_pairs &cls, const state_type &castate)
	{
		range_pairs predefclass = this->character_class[castate.char_num];

		if (castate.is_not)
			predefclass.negation();

		cls.merge(predefclass);
	}

#if !defined(SRELL_NO_VMODE) && !defined(SRELL_NO_UNICODE_PROPERTY)

	bool parse_charclass_v(state_array &piece, re_quantifier &piecesize, const ui_l32 *&curpos, const ui_l32 *const end, cvars_type &cvars)
	{
		posdata_holder pos;

		if (!parse_unicharset(pos, curpos, end, cvars))
			return false;

		if (!pos.may_contain_strings())
		{
			state_type castate;

			castate.reset(st_character, pos.ranges.consists_of_one_character(this->is_icase()));

			if (castate.char_num == constants::invalid_u32value)
			{
				castate.type = st_character_class;
				castate.char_num = this->character_class.register_newclass(pos.ranges);
			}

			piece.push_back(castate);
		}
		else
		{
			transform_seqdata(piece, pos);

		}
		piecesize.atleast = pos.length.first;
		piecesize.atmost = pos.length.second;
		return true;
	}

	bool parse_unicharset(posdata_holder &basepos, const ui_l32 *&curpos, const ui_l32 *const end, const cvars_type &cvars)
	{
		enum operation_type
		{
			op_init, op_firstcc, op_union, op_intersection, op_subtraction
		};
		operation_type otype = op_init;
		posdata_holder newpos;
		range_pair code_range;
		state_type castate;
		bool invert;

		if (curpos == end)
			goto ERROR_NOT_CLOSED;

		if (*curpos == meta_char::mc_caret)	//  '^'
		{
			invert = true;
			++curpos;
		}
		else
			invert = false;

		//  ClassSetCharacter ::
		//  \ CharacterEscape[+UnicodeMode]
		//  \ ClassSetReservedPunctuator
		//  \ b

		for (;;)
		{
			if (curpos == end)
				goto ERROR_NOT_CLOSED;

			if (*curpos == meta_char::mc_sbracl)	//   ']'
				break;

			ui_l32 next2chars = constants::invalid_u32value;

			if (curpos + 1 != end && *curpos == curpos[1])
			{
				switch (*curpos)
				{
				//  ClassSetReservedDoublePunctuator :: one of
				//  && !! ## $$ %% ** ++ ,, .. :: ;; << == >> ?? @@ ^^ `` ~~
				case char_other::co_amp:	//  '&'
				case meta_char::mc_exclam:	//  '!'
				case meta_char::mc_sharp:	//  '#'
				case meta_char::mc_dollar:	//  '$'
				case char_other::co_perc:	//  '%'
				case meta_char::mc_astrsk:	//  '*'
				case meta_char::mc_plus:	//  '+'
				case meta_char::mc_comma:	//  ','
				case meta_char::mc_period:	//  '.'
				case meta_char::mc_colon:	//  ':'
				case char_other::co_smcln:	//  ';'
				case meta_char::mc_lt:		//  '<'
				case meta_char::mc_eq:		//  '='
				case meta_char::mc_gt:		//  '>'
				case meta_char::mc_query:	//  '?'
				case char_other::co_atmrk:	//  '@'
				case meta_char::mc_caret:	//  '^'
				case char_other::co_grav:	//  '`'
				case char_other::co_tilde:	//  '~'
				case meta_char::mc_minus:	//  '-'
					next2chars = *curpos;
					//@fallthrough@
				default:;
				}
			}

			switch (otype)
			{
			case op_intersection:
				if (next2chars != char_other::co_amp)
					goto ERROR_DOUBLE_PUNCT;
				curpos += 2;
				break;

			case op_subtraction:
				if (next2chars != meta_char::mc_minus)
					goto ERROR_DOUBLE_PUNCT;
				curpos += 2;
				break;

			case op_firstcc:
				if (next2chars == char_other::co_amp)
					otype = op_intersection;
				else if (next2chars == meta_char::mc_minus)
					otype = op_subtraction;
				else if (next2chars == constants::invalid_u32value)
					break;
				else
					goto ERROR_DOUBLE_PUNCT;

				curpos += 2;
				break;

//			case op_union:
//			case op_init:
			default:
				if (next2chars != constants::invalid_u32value)
					goto ERROR_DOUBLE_PUNCT;
			}

			AFTER_OPERATOR:

			if (curpos == end)
				goto ERROR_NOT_CLOSED;

			castate.reset();

			if (*curpos == meta_char::mc_sbraop)	//  '['
			{
				++curpos;
				if (!parse_unicharset(newpos, curpos, end, cvars))
					return false;
			}
			else if (!get_character_in_class_vmode(newpos, castate, curpos, end, cvars, false))
				return false;

			if (otype == op_init)
				otype = op_firstcc;
			else if (otype == op_firstcc)
				otype = op_union;

			if (curpos == end)
				goto ERROR_NOT_CLOSED;

			if (castate.type == st_character_class)
			{
			}
			else if (castate.type == st_character)
			{
				if (!newpos.has_data())
				{
					code_range.set(castate.char_num);

					if (otype <= op_union)
					{
						if (*curpos == meta_char::mc_minus)	//  '-'
						{
							++curpos;

							if (curpos == end)
								goto ERROR_BROKEN_RANGE;

							if (otype < op_union && *curpos == meta_char::mc_minus)		//  '-'
							{
								otype = op_subtraction;
								++curpos;
								basepos.ranges.join(code_range);
								goto AFTER_OPERATOR;
							}

							if (!get_character_in_class_vmode(newpos, castate, curpos, end, cvars, true))
								return false;

							otype = op_union;
							code_range.second = castate.char_num;
							if (!code_range.is_range_valid())
								goto ERROR_BROKEN_RANGE;
						}
					}

					newpos.ranges.join(code_range);
					if (this->is_icase())
						newpos.ranges.make_caseunfoldedcharset();
				}
			}

			switch (otype)
			{
			case op_union:
				basepos.do_union(newpos);
				break;

			case op_intersection:
				basepos.do_and(newpos);
				break;

			case op_subtraction:
				basepos.do_subtract(newpos);
				break;

//			case op_firstcc:
			default:
				basepos.swap(newpos);
			}
		}

		//  *curpos == ']'
		++curpos;

		if (this->is_icase())
			basepos.ranges.make_caseunfoldedcharset();

		if (invert)
		{
			if (basepos.may_contain_strings())
				goto ERROR_NOT_INVERTIBLE;

			basepos.ranges.negation();
		}

		return true;

		ERROR_NOT_CLOSED:
		return this->set_error(regex_constants::error_brack);

		ERROR_BROKEN_RANGE:
		return this->set_error(regex_constants::error_range);

		ERROR_NOT_INVERTIBLE:
		return this->set_error(regex_constants::error_complement);

		ERROR_DOUBLE_PUNCT:
		return this->set_error(regex_constants::error_operator);
	}

	bool get_character_in_class_vmode(
		posdata_holder &pos,
		state_type &castate,
		const ui_l32 *&curpos,
		const ui_l32 *const end,
		const cvars_type &cvars,
		const bool no_ccesc
	)
	{
		pos.clear();

		castate.char_num = *curpos++;

		switch (castate.char_num)
		{
		//  ClassSetSyntaxCharacter :: one of
		//  ( ) [ ] { } / - \ |
		case meta_char::mc_rbraop:	//  '('
		case meta_char::mc_rbracl:	//  ')'
		case meta_char::mc_sbraop:	//  '['
		case meta_char::mc_sbracl:	//  ']'
		case meta_char::mc_cbraop:	//  '{'
		case meta_char::mc_cbracl:	//  '}'
		case char_other::co_slash:	//  '/'
		case meta_char::mc_minus:	//  '-'
		case meta_char::mc_bar:		//  '|'
			return this->set_error(regex_constants::error_noescape);
			//@fallthrough@

		case meta_char::mc_escape:	//  '\\'
			break;

		default:
			return true;
		}

		if (curpos == end)
			return this->set_error(regex_constants::error_escape);

		castate.char_num = *curpos++;

		if (!no_ccesc)
		{
			if (((castate.char_num | constants::asc_icase) == char_alnum::ch_p))
			{
				return parse_escape_p_vmode(pos, castate, curpos, end, cvars);
			}
			else if (castate.char_num == char_alnum::ch_q)
			{
				if (curpos == end || *curpos != meta_char::mc_cbraop)	//  '{'
					return this->set_error(regex_constants::error_escape);

				simple_array<ui_l32> seqs;
				simple_array<ui_l32> curseq;
				posdata_holder dummypos;
				state_type castate;

				++curpos;

				for (;;)
				{
					if (curpos == end)
						return this->set_error(regex_constants::error_escape);

					if (*curpos == meta_char::mc_bar || *curpos == meta_char::mc_cbracl)	//  '|' or '}'.
					{
						const ui_l32 seqlen = static_cast<ui_l32>(curseq.size());

						if (seqlen <= 1)
						{
							seqs.push_backncr(2);
							seqs.push_backncr(seqlen != 0 ? curseq[0] : constants::ccstr_empty);
						}
						else	//  >= 2
						{
							seqs.push_backncr(seqlen + 1);
							seqs.append(curseq);
						}

						if (*curpos == meta_char::mc_cbracl)	//  '}'
							break;

						curseq.clear();
						++curpos;
					}
					else
					{
						castate.reset();
						if (!get_character_in_class_vmode(dummypos, castate, curpos, end, cvars, true))
							return false;

						curseq.push_backncr(castate.char_num);
					}
				}

				++curpos;
#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
				pos.split_seqs_and_ranges(seqs, this->is_icase(), cvars.back);
#else
				pos.split_seqs_and_ranges(seqs, this->is_icase(), false);
#endif

				return true;
			}
		}

		switch (castate.char_num)
		{
		//  ClassSetReservedPunctuator :: one of
		//  & - ! # % , : ; < = > @ ` ~
		case char_other::co_amp:	//  '&'
		case meta_char::mc_exclam:	//  '!'
		case meta_char::mc_sharp:	//  '#'
		case char_other::co_perc:	//  '%'
		case meta_char::mc_comma:	//  ','
		case meta_char::mc_colon:	//  ':'
		case char_other::co_smcln:	//  ';'
		case meta_char::mc_lt:		//  '<'
		case meta_char::mc_eq:		//  '='
		case meta_char::mc_gt:		//  '>'
		case char_other::co_atmrk:	//  '@'
		case char_other::co_grav:	//  '`'
		case char_other::co_tilde:	//  '~'
			return true;

		default:;
		}

		return translate_escape(&pos.ranges, castate, curpos, end, true, no_ccesc);
	}

#endif	//  !defined(SRELL_NO_VMODE) && !defined(SRELL_NO_UNICODE_PROPERTY)

	bool translate_escape(range_pairs *const rp, state_type &eastate, const ui_l32 *&curpos, const ui_l32 *const end, const bool insidecharclass, const bool no_ccesc)
	{
		if (!no_ccesc)
		{
			//  Predefined classes.
			switch (eastate.char_num)
			{
			case char_alnum::ch_D:	//  'D':
				eastate.is_not = 1u;
				//@fallthrough@

			case char_alnum::ch_d:	//  'd':
				eastate.char_num = static_cast<ui_l32>(re_character_class::digit);	//  \d, \D.
				break;

			case char_alnum::ch_S:	//  'S':
				eastate.is_not = 1u;
				//@fallthrough@

			case char_alnum::ch_s:	//  's':
				eastate.char_num = static_cast<ui_l32>(re_character_class::space);	//  \s, \S.
				break;

			case char_alnum::ch_W:	//  'W':
				eastate.is_not = 1u;
				//@fallthrough@

			case char_alnum::ch_w:	//  'w':
				if (this->is_icase())
				{
					this->character_class.setup_icase_word();
					eastate.char_num = static_cast<ui_l32>(re_character_class::icase_word);
				}
				else
					eastate.char_num = static_cast<ui_l32>(re_character_class::word);	//  \w, \W.
				break;

#if !defined(SRELL_NO_UNICODE_PROPERTY)
			//  Prepared for Unicode properties and script names.
			case char_alnum::ch_P:	//  \P{...}
				eastate.is_not = 1u;
				//@fallthrough@

			case char_alnum::ch_p:	//  \p{...}
				{
					range_pairs lranges;
					range_pairs *const pranges = (rp != NULL) ? rp : &lranges;
					const ui_l32 pnumber = lookup_propertynumber(curpos, end);

					if (pnumber == up_constants::error_property || this->character_class.is_pos(pnumber))
						return this->set_error(regex_constants::error_property);

					this->character_class.load_upranges(*pranges, pnumber);

					if (eastate.is_not)
					{
						pranges->negation();
						eastate.is_not = 0u;
					}

					if (!insidecharclass && this->is_icase())
						pranges->make_caseunfoldedcharset();

					if (rp == NULL)
						eastate.char_num = this->character_class.register_newclass(*pranges);
				}
				eastate.type = st_character_class;
				return true;
#endif	//  !defined(SRELL_NO_UNICODE_PROPERTY)

			default:
				goto CLASS_OR_CHARACTER_ESCAPE;
			}

			if (rp != NULL)
				add_predefclass_to_charclass(*rp, eastate);
			else
			{
				if (eastate.is_not)
				{
					range_pairs lranges;

					add_predefclass_to_charclass(lranges, eastate);
					eastate.char_num = this->character_class.register_newclass(lranges);
				}
			}

			eastate.is_not = 0u;
			eastate.type = st_character_class;
			return true;
		}

		CLASS_OR_CHARACTER_ESCAPE:

		switch (eastate.char_num)
		{
		case char_alnum::ch_b:
			eastate.char_num = char_ctrl::cc_bs;	//  '\b' 0x08:BS
			break;

		case char_alnum::ch_t:
			eastate.char_num = char_ctrl::cc_htab;	//  '\t' 0x09:HT
			break;

		case char_alnum::ch_n:
			eastate.char_num = char_ctrl::cc_nl;	//  '\n' 0x0a:LF
			break;

		case char_alnum::ch_v:
			eastate.char_num = char_ctrl::cc_vtab;	//  '\v' 0x0b:VT
			break;

		case char_alnum::ch_f:
			eastate.char_num = char_ctrl::cc_ff;	//  '\f' 0x0c:FF
			break;

		case char_alnum::ch_r:
			eastate.char_num = char_ctrl::cc_cr;	//  '\r' 0x0d:CR
			break;

		case char_alnum::ch_c:	//  \cX
			if (curpos != end)
			{
				eastate.char_num = static_cast<ui_l32>(*curpos | constants::asc_icase);

				if (eastate.char_num >= char_alnum::ch_a && eastate.char_num <= char_alnum::ch_z)
					eastate.char_num = static_cast<ui_l32>(*curpos++ & 0x1f);
				else
				{
					return this->set_error(regex_constants::error_escape);	//  Strict.
//					eastate.char_num = char_alnum::ch_c;	//  Loose.
				}
			}
			break;

		case char_alnum::ch_0:
			eastate.char_num = char_ctrl::cc_nul;	//  '\0' 0x00:NUL
			break;

		case char_alnum::ch_x:	//  \xhh
			eastate.char_num = translate_numbers(curpos, end, 16, 2, 2, 0xff);
			break;

		case char_alnum::ch_u:	//  \uhhhh, \u{h~hhhhhh}
			eastate.char_num = parse_escape_u(curpos, end);
			break;

		//  SyntaxCharacter, '/', and '-'.
		case meta_char::mc_caret:	//  '^'
		case meta_char::mc_dollar:	//  '$'
		case meta_char::mc_escape:	//  '\\'
		case meta_char::mc_period:	//  '.'
		case meta_char::mc_astrsk:	//  '*'
		case meta_char::mc_plus:	//  '+'
		case meta_char::mc_query:	//  '?'
		case meta_char::mc_rbraop:	//  '('
		case meta_char::mc_rbracl:	//  ')'
		case meta_char::mc_sbraop:	//  '['
		case meta_char::mc_sbracl:	//  ']'
		case meta_char::mc_cbraop:	//  '{'
		case meta_char::mc_cbracl:	//  '}'
		case meta_char::mc_bar:		//  '|'
		case char_other::co_slash:	//  '/'
			break;

		case meta_char::mc_minus:	//  '-' allowed only in charclass.
			if (insidecharclass)
				break;
			//@fallthrough@

		default:
			eastate.char_num = constants::invalid_u32value;
		}

		if (eastate.char_num == constants::invalid_u32value)
			return this->set_error(regex_constants::error_escape);

		return true;
	}

	ui_l32 parse_escape_u(const ui_l32 *&curpos, const ui_l32 *const end) const
	{
		ui_l32 ucp;

		if (curpos == end)
			return constants::invalid_u32value;

		if (*curpos == meta_char::mc_cbraop)
		{
//			ucp = translate_numbers(++curpos, end, 16, 1, 6, constants::unicode_max_codepoint, true);
			ucp = translate_numbers(++curpos, end, 16, 1, 0, constants::unicode_max_codepoint);

			if (curpos == end || *curpos != meta_char::mc_cbracl)
				return constants::invalid_u32value;

			++curpos;
		}
		else
		{
			ucp = translate_numbers(curpos, end, 16, 4, 4, 0xffff);

			if (ucp >= 0xd800 && ucp <= 0xdbff)
			{
				const ui_l32 *prefetch = curpos;

				if (prefetch != end && *prefetch == meta_char::mc_escape && ++prefetch != end && *prefetch == char_alnum::ch_u)
				{
					++prefetch;

					const ui_l32 nextucp = translate_numbers(prefetch, end, 16, 4, 4, 0xffff);

					if (nextucp >= 0xdc00 && nextucp <= 0xdfff)
					{
						curpos = prefetch;
						ucp = (((ucp << 10) & 0xffc00) | (nextucp & 0x3ff)) + 0x10000;
					}
				}
			}
		}
		return ucp;
	}

#if !defined(SRELL_NO_UNICODE_PROPERTY)

	ui_l32 lookup_propertynumber(const ui_l32 *&curpos, const ui_l32 *const end)
	{
		pstring pname;
		pstring pvalue;

		if (curpos == end || *curpos != meta_char::mc_cbraop)	//  '{'
			return up_constants::error_property;

		const bool digit_seen = get_property_name_or_value(pvalue, ++curpos, end);

		if (!pvalue.size())
			return up_constants::error_property;

		if (!digit_seen)
		{
			if (curpos == end)
				return up_constants::error_property;

			if (*curpos == meta_char::mc_eq)	//  '='
			{
				pname = pvalue;
				get_property_name_or_value(pvalue, ++curpos, end);
				if (!pvalue.size())
					return up_constants::error_property;
			}
		}

		if (curpos == end || *curpos != meta_char::mc_cbracl)	//  '}'
			return up_constants::error_property;

		++curpos;

		pname.push_backncr(0);
		pvalue.push_backncr(0);

		return this->character_class.get_propertynumber(pname, pvalue);
	}

	bool get_property_name_or_value(pstring &name_or_value, const ui_l32 *&curpos, const ui_l32 *const end) const
	{
		bool number_found = false;

		name_or_value.clear();

		for (;; ++curpos)
		{
			if (curpos == end)
				break;

			const ui_l32 curchar = *curpos;

			if (curchar >= char_alnum::ch_A && curchar <= char_alnum::ch_Z)
				;
			else if (curchar >= char_alnum::ch_a && curchar <= char_alnum::ch_z)
				;
			else if (curchar == char_other::co_ll)	//  '_'
				;
			else if (curchar >= char_alnum::ch_0 && curchar <= char_alnum::ch_9)
				number_found = true;
			else
				break;

			name_or_value.append(1, static_cast<typename pstring::value_type>(curchar));
		}

		//  A string containing a digit cannot be a property name.
		return number_found;
	}

#endif	//  !defined(SRELL_NO_UNICODE_PROPERTY)

#if !defined(SRELL_NO_NAMEDCAPTURE)

#if !defined(SRELL_NO_UNICODE_PROPERTY)
	gname_string get_groupname(const ui_l32 *&curpos, const ui_l32 *const end, cvars_type &cvars)
#else
	gname_string get_groupname(const ui_l32 *&curpos, const ui_l32 *const end, cvars_type &)
#endif
	{
		charT mbstr[utf_traits::maxseqlen];
		gname_string groupname;

#if !defined(SRELL_NO_UNICODE_PROPERTY)
		cvars.idchecker.setup();
#endif
		for (;;)
		{
			if (curpos == end)
			{
				groupname.clear();
				break;
			}

			ui_l32 curchar = *curpos++;

			if (curchar == meta_char::mc_gt)	//  '>'
				break;

			if (curchar == meta_char::mc_escape && curpos != end && *curpos == char_alnum::ch_u)	//  '\\', 'u'.
				curchar = parse_escape_u(++curpos, end);

#if defined(SRELL_NO_UNICODE_PROPERTY)
			if (curchar != meta_char::mc_escape)
#else
			if (cvars.idchecker.is_identifier(curchar, groupname.size() != 0))
#endif
				;	//  OK.
			else
				curchar = constants::invalid_u32value;

			if (curchar == constants::invalid_u32value)
			{
				groupname.clear();
				break;
			}

			const ui_l32 seqlen = utf_traits::to_codeunits(mbstr, curchar);

			for (ui_l32 i = 0; i < seqlen; ++i)
				groupname.append(1, mbstr[i]);
		}

		return groupname;
	}
#endif	//  !defined(SRELL_NO_NAMEDCAPTURE)

#if !defined(SRELL_NO_VMODE) && !defined(SRELL_NO_UNICODE_PROPERTY)

	bool parse_escape_p_vmode(posdata_holder &pos, state_type &ccepastate, const ui_l32 *&curpos, const ui_l32 *const end,
#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
		const cvars_type &cvars)
#else
		const cvars_type &)
#endif
	{
		if (curpos == end)
			return this->set_error(regex_constants::error_escape);

//		ccepastate.is_not = (ccepastate.char_num & constants::asc_icase) ? false : true;
		if (ccepastate.char_num == char_alnum::ch_P)	//  \P{...}
			ccepastate.is_not = 1u;

		ccepastate.char_num = lookup_propertynumber(curpos, end);

		if (ccepastate.char_num == up_constants::error_property)
			return this->set_error(regex_constants::error_property);

		if (!this->character_class.is_pos(ccepastate.char_num))
		{
			pos.clear();

			this->character_class.load_upranges(pos.ranges, ccepastate.char_num);

			if (this->is_icase() && ccepastate.char_num >= static_cast<ui_l32>(re_character_class::number_of_predefcls))
				pos.ranges.make_caseunfoldedcharset();

			if (ccepastate.is_not)
			{
				pos.ranges.negation();
				ccepastate.is_not = 0u;
			}

			ccepastate.type = st_character_class;
			ccepastate.quantifier.reset(1);
		}
		else
		{
			simple_array<ui_l32> sequences;

			this->character_class.get_prawdata(sequences, ccepastate.char_num);
#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
			pos.split_seqs_and_ranges(sequences, this->is_icase(), cvars.back);
#else
			pos.split_seqs_and_ranges(sequences, this->is_icase(), false);
#endif

			ccepastate.quantifier.set(pos.length.first, pos.length.second);

			if (ccepastate.is_not)
				return this->set_error(regex_constants::error_complement);
		}
		return true;
	}

	ui_l32 transform_seqdata(state_array &piece, const posdata_holder &pos)
	{
		ui_l32 seqlen = static_cast<ui_l32>(pos.indices.size());
		state_type castate;

		castate.reset(st_character_class);
		castate.char_num = this->character_class.register_newclass(pos.ranges);

		if (seqlen > 0)
		{
			const bool has_empty = pos.has_empty();
#if !defined(SRELLDBG_NO_ASTERISK_OPT) && !defined(SRELLDBG_NO_POS_OPT) && !defined(SRELLDBG_NO_STATEHOOK)
			state_size_type prevbranch_end = 0;
#else
			state_size_type prevbranch_alt = 0;
#endif
			state_type branchstate;
			state_type jumpstate;
			state_array branch;

			branch.resize(seqlen);
			for (ui_l32 i = 0; i < seqlen; ++i)
				branch[i].reset();

			branchstate.reset(st_epsilon, epsilon_type::et_alt);

			jumpstate.reset(st_epsilon, epsilon_type::et_brnchend);	//  '/'

			--seqlen;

			for (; seqlen >= 2; --seqlen)
			{
				ui_l32 offset = pos.indices[seqlen];
				const ui_l32 seqend = pos.indices[seqlen - 1];

				if (offset != seqend)
				{
					branch.resize(seqlen + 1);
					branch[seqlen] = jumpstate;

					for (ui_l32 count = 0; offset < seqend; ++offset)
					{
						branch[count++].char_num = pos.seqs[offset];

						if (count == seqlen)
						{
#if !defined(SRELLDBG_NO_ASTERISK_OPT) && !defined(SRELLDBG_NO_POS_OPT) && !defined(SRELLDBG_NO_STATEHOOK)
							state_size_type bpos = 0;

							for (state_size_type ppos = 0; ppos < piece.size();)
							{
								if (bpos + 1 == branch.size())
								{
									piece.push_backncr(piece[ppos]);

									state_type &pst = piece[ppos];

									pst.reset(st_epsilon, epsilon_type::et_hooked);
									pst.next1 = static_cast<std::ptrdiff_t>(piece.size()) - ppos - 1;
									pst.next2 = static_cast<std::ptrdiff_t>(prevbranch_end) - ppos;

									state_type &bst = piece[piece.size() - 1];

									bst.next1 = bst.next1 - pst.next1;
									bst.next2 = bst.next2 ? (bst.next2 - pst.next1) : 0;
									goto SKIP_APPEND;
								}

								state_type &pst = piece[ppos];

#if 0
								if (pst.type == st_epsilon)
									ppos += pst.next1;
								else
#endif
								if (pst.char_num == branch[bpos].char_num)
								{
									++bpos;
									ppos += pst.next1;
								}
								else if (pst.next2)
									ppos += pst.next2;
								else
								{
									pst.next2 = static_cast<std::ptrdiff_t>(piece.size()) - ppos;
									break;
								}
							}

							{
								const state_size_type alen = branch.size() - bpos;

								if (piece.size())
									piece[prevbranch_end].next1 = piece.size() + alen - 1 - prevbranch_end;

								piece.append(branch, bpos, alen);
								prevbranch_end = piece.size() - 1;
							}
							SKIP_APPEND:
							count = 0;

#else	//  defined(SRELLDBG_NO_ASTERISK_OPT) || defined(SRELLDBG_NO_POS_OPT) || defined(SRELLDBG_NO_STATEHOOK)

							if (piece.size())
							{
								state_type &laststate = piece[piece.size() - 1];

								laststate.next1 = seqlen + 2;
								piece[prevbranch_alt].next2 = static_cast<std::ptrdiff_t>(piece.size()) - prevbranch_alt;
							}
							prevbranch_alt = piece.size();
							piece.push_back(branchstate);
							piece.append(branch);
							count = 0;

#endif	//  !defined(SRELLDBG_NO_ASTERISK_OPT) && !defined(SRELLDBG_NO_POS_OPT) && !defined(SRELLDBG_NO_STATEHOOK)
						}
					}
				}
			}

			if (piece.size())
			{
#if !defined(SRELLDBG_NO_ASTERISK_OPT) && !defined(SRELLDBG_NO_POS_OPT) && !defined(SRELLDBG_NO_STATEHOOK)
				state_type &laststate = piece[prevbranch_end];

				laststate.next1 = piece.size() + (has_empty ? 2 : 1) - prevbranch_end;

				branchstate.next2 = static_cast<std::ptrdiff_t>(piece.size()) + 1;
				piece.insert(0, branchstate);
#else
				state_type &laststate = piece[piece.size() - 1];

				laststate.next1 = has_empty ? 3 : 2;

				piece[prevbranch_alt].next2 = static_cast<std::ptrdiff_t>(piece.size()) - prevbranch_alt;
#endif
			}

			if (has_empty)
			{
				branchstate.next2 = 2;
				piece.push_back(branchstate);
			}

			piece.push_back(castate);

			branchstate.char_num = epsilon_type::et_ncgopen;
			branchstate.next1 = 1;
			branchstate.next2 = 0;
			branchstate.quantifier.set(1, 0);
			piece.insert(0, branchstate);

			branchstate.char_num = epsilon_type::et_ncgclose;
			branchstate.quantifier.atmost = 1;
			piece.push_back(branchstate);

#if !defined(SRELLDBG_NO_ASTERISK_OPT) && !defined(SRELLDBG_NO_POS_OPT) && !defined(SRELLDBG_NO_STATEHOOK)
			reorder_piece(piece);
#endif

		}
		return castate.char_num;
	}

#endif	//  !defined(SRELL_NO_VMODE) && !defined(SRELL_NO_UNICODE_PROPERTY)

	ui_l32 translate_numbers(const ui_l32 *&curpos, const ui_l32 *const end, const int radix, const std::size_t minsize, const std::size_t maxsize, const ui_l32 maxvalue) const
	{
		std::size_t count = 0;
		ui_l32 u32value = 0;
		ui_l32 num;

		for (; maxsize == 0 || count < maxsize; ++curpos, ++count)
		{

			if (curpos == end)
				break;

			const ui_l32 ch = *curpos;

			if ((ch >= char_alnum::ch_0 && ch <= char_alnum::ch_7) || (radix >= 10 && (ch == char_alnum::ch_8 || ch == char_alnum::ch_9)))
				num = ch - char_alnum::ch_0;
			else if (radix == 16)
			{
				if (ch >= char_alnum::ch_A && ch <= char_alnum::ch_F)
					num = ch - char_alnum::ch_A + 10;
				else if (ch >= char_alnum::ch_a && ch <= char_alnum::ch_f)
					num = ch - char_alnum::ch_a + 10;
				else
					break;
			}
			else
				break;

			const ui_l32 nextvalue = u32value * radix + num;

			if ((/* maxvalue != 0 && */ nextvalue > maxvalue) || nextvalue < u32value)
				break;

			u32value = nextvalue;
		}

		if (count >= minsize)
			return u32value;

		return constants::invalid_u32value;
	}

	bool check_backreferences(cvars_type &cvars)
	{
		for (state_size_type backrefpos = 1; backrefpos < this->NFA_states.size(); ++backrefpos)
		{
			state_type &brs = this->NFA_states[backrefpos];

			if (brs.type == st_backreference)
			{
				const ui_l32 &backrefno = brs.char_num;

#if !defined(SRELL_NO_NAMEDCAPTURE)
				if (brs.icase_backrefno_unresolved & 2u)
				{
					if (backrefno >= cvars.unresolved_gnames.size())
						return false;	//  Internal error.

					brs.char_num = this->namedcaptures[cvars.unresolved_gnames[backrefno]];

					if (backrefno == groupname_mapper<charT>::notfound)
						return false;

					brs.icase_backrefno_unresolved &= ~2u;
				}
#endif

				for (state_size_type roundbracket_closepos = 0;; ++roundbracket_closepos)
				{
					if (roundbracket_closepos < this->NFA_states.size())
					{
						const state_type &rbcs = this->NFA_states[roundbracket_closepos];

						if (rbcs.type == st_roundbracket_close && rbcs.char_num == backrefno)
						{
							if (roundbracket_closepos > backrefpos)
							{
								if (brs.next1 == -1)
								{
									state_type &prevstate = this->NFA_states[backrefpos + brs.next1];

									if (prevstate.is_asterisk_or_plus_for_onelen_atom())
									{
										prevstate.next1 = 2;
										prevstate.next2 = 0;
									}
								}

								brs.type = st_epsilon;
								brs.next2 = 0;
								brs.char_num = epsilon_type::et_fmrbckrf;
							}
							break;
						}
					}
					else
						return false;
				}
			}
		}
		return true;
	}

#if !defined(SRELLDBG_NO_1STCHRCLS)

	void create_firstchar_class()
	{
#if !defined(SRELLDBG_NO_BITSET)
		range_pairs fcc;
#else
		range_pairs &fcc = this->firstchar_class;
#endif

		const bool canbe0length = gather_nextchars(fcc, static_cast<state_size_type>(this->NFA_states[0].next1), 0u, false);

		if (canbe0length)
		{
			fcc.set_solerange(range_pair_helper(0, constants::unicode_max_codepoint));
			//  Expressions would consist of assertions only, such as /^$/.
			//  We cannot but accept every codepoint.
		}

#if !defined(SRELLDBG_NO_BITSET)
		this->NFA_states[0].quantifier.atleast = this->character_class.register_newclass(fcc);
#endif

#if !defined(SRELLDBG_NO_BITSET) || !defined(SRELLDBG_NO_SCFINDER)
		set_bitset_table(fcc);
#endif
	}

#if !defined(SRELLDBG_NO_BITSET) || !defined(SRELLDBG_NO_SCFINDER)
	void set_bitset_table(const range_pairs &fcc)
	{
#if !defined(SRELLDBG_NO_SCFINDER)
		ui_l32 entrychar = constants::max_u32value;
#endif

		for (typename range_pairs::size_type i = 0; i < fcc.size(); ++i)
		{
			const range_pair &range = fcc[i];

#if 0
			ui_l32 second = range.second <= constants::unicode_max_codepoint ? range.second : constants::unicode_max_codepoint;

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(push)
#pragma warning(disable:4127)
#endif
			if (utf_traits::utftype == 16)
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(pop)
#endif
			{
				if (second >= 0x10000 && range.first < 0x10000)
				{
					this->firstchar_class_bs.set_range(utf_traits::firstcodeunit(0x10000) & utf_traits::bitsetmask, utf_traits::firstcodeunit(second) & utf_traits::bitsetmask);
					second = 0xffff;
				}
			}
			this->firstchar_class_bs.set_range(utf_traits::firstcodeunit(range.first) & utf_traits::bitsetmask, utf_traits::firstcodeunit(second) & utf_traits::bitsetmask);

#else
			for (ui_l32 ucp = range.first; ucp <= utf_traits::maxcpvalue; ++ucp)
			{
				const ui_l32 firstcu = utf_traits::firstcodeunit(ucp) & utf_traits::bitsetmask;

#if !defined(SRELLDBG_NO_BITSET)
				this->firstchar_class_bs.set(firstcu);
#endif

#if !defined(SRELLDBG_NO_SCFINDER)
				if (entrychar != constants::invalid_u32value)
				{
					if (entrychar != firstcu)
					{
						if (entrychar == constants::max_u32value)
							entrychar = firstcu;
						else
							entrychar = constants::invalid_u32value;
					}
				}
#endif
				if (ucp == range.second)
					break;
			}
#endif
		}
#if !defined(SRELLDBG_NO_SCFINDER)
		this->NFA_states[0].char_num = entrychar;
#endif
	}
#endif	//  !defined(SRELLDBG_NO_BITSET) || !defined(SRELLDBG_NO_SCFINDER)
#endif	//  !defined(SRELLDBG_NO_1STCHRCLS)

	bool gather_nextchars(range_pairs &nextcharclass, state_size_type pos, simple_array<bool> &checked, const ui_l32 bracket_number, const bool subsequent) const
	{
		bool canbe0length = false;

		for (;;)
		{
			const state_type &state = this->NFA_states[pos];

			if (checked[pos])
				break;

			checked[pos] = true;

			if (state.next2
					&& (state.type != st_check_counter || !state.quantifier.is_greedy || state.quantifier.atleast == 0)
					&& (state.type != st_save_and_reset_counter)
					&& (state.type != st_roundbracket_open)
					&& (state.type != st_roundbracket_close || state.char_num != bracket_number)
					&& (state.type != st_repeat_in_push)
					&& (state.type != st_backreference || (state.next1 != state.next2))
					&& (state.type != st_lookaround_open))
				if (gather_nextchars(nextcharclass, pos + state.next2, checked, bracket_number, subsequent))
					canbe0length = true;

			switch (state.type)
			{
			case st_character:
				if (!this->is_ricase())
				{
					nextcharclass.join(range_pair_helper(state.char_num));
				}
				else
				{
					ui_l32 table[ucf_constants::rev_maxset] = {};
					const ui_l32 setnum = unicode_case_folding::do_caseunfolding(table, state.char_num);

					for (ui_l32 j = 0; j < setnum; ++j)
						nextcharclass.join(range_pair_helper(table[j]));
				}
				return canbe0length;

			case st_character_class:
				nextcharclass.merge(this->character_class[state.char_num]);
				return canbe0length;

			case st_backreference:
				{
					const state_size_type nextpos = find_next1_of_bracketopen(state.char_num);

					gather_nextchars(nextcharclass, nextpos, state.char_num, subsequent);
				}
				break;

			case st_eol:
			case st_bol:
				if (!subsequent)
					break;

				//@fallthrough@

			case st_boundary:
				if (subsequent)
					nextcharclass.set_solerange(range_pair_helper(0, constants::unicode_max_codepoint));

				break;

			case st_lookaround_open:
//				if (!state.is_not && !state.reverse)
				if (!state.is_not && state.quantifier.atleast == 0)
				{
					gather_nextchars(nextcharclass, pos + 1, checked, 0u, subsequent);
				}
				else if (subsequent)
					nextcharclass.set_solerange(range_pair_helper(0, constants::unicode_max_codepoint));

				break;

			case st_roundbracket_close:
				if (/* bracket_number == 0 || */ state.char_num != bracket_number)
					break;
				//@fallthrough@

			case st_success:	//  == st_lookaround_close.
				return true;

			case st_check_counter:
				if (!state.quantifier.is_greedy && state.quantifier.atleast >= 1)
					return canbe0length;
				//@fallthrough@

			default:;
			}

			if (state.next1)
				pos += state.next1;
			else
				break;
		}
		return canbe0length;
	}

	bool gather_nextchars(range_pairs &nextcharclass, const state_size_type pos, const ui_l32 bracket_number, const bool subsequent) const
	{
		simple_array<bool> checked;

		checked.resize(this->NFA_states.size(), false);
		return gather_nextchars(nextcharclass, pos, checked, bracket_number, subsequent);
	}

	state_size_type find_next1_of_bracketopen(const ui_l32 bracketno) const
	{
		for (state_size_type no = 0; no < this->NFA_states.size(); ++no)
		{
			const state_type &state = this->NFA_states[no];

			if (state.type == st_roundbracket_open && state.char_num == bracketno)
				return no + state.next1;
		}
		return 0;
	}

	void relativejump_to_absolutejump()
	{
		for (state_size_type pos = 0; pos < this->NFA_states.size(); ++pos)
		{
			state_type &state = this->NFA_states[pos];

#if !defined(SRELLDBG_NO_ASTERISK_OPT)
			if (state.next1 || state.type == st_character || state.type == st_character_class)
#else
			if (state.next1)
#endif
				state.next_state1 = &this->NFA_states[pos + state.next1];
			else
				state.next_state1 = NULL;

			if (state.next2)
				state.next_state2 = &this->NFA_states[pos + state.next2];
			else
				state.next_state2 = NULL;
		}
	}

	void optimise()
	{
#if !defined(SRELLDBG_NO_BRANCH_OPT2) && !defined(SRELLDBG_NO_STATEHOOK)
		branch_optimisation2();
#endif

#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND) && !defined(SRELLDBG_NO_MPREWINDER)
		if (!this->bmdata)
			find_entrypoint();
#endif

#if !defined(SRELLDBG_NO_ASTERISK_OPT)
		asterisk_optimisation();
#endif

#if !defined(SRELLDBG_NO_BRANCH_OPT) && !defined(SRELLDBG_NO_ASTERISK_OPT)
		branch_optimisation();
#endif

#if !defined(SRELLDBG_NO_1STCHRCLS)
		create_firstchar_class();
#endif

#if !defined(SRELLDBG_NO_SKIP_EPSILON)
		skip_epsilon();
#endif

#if !defined(SRELLDBG_NO_CCPOS)
		set_charclass_posinfo();
#endif
	}

#if !defined(SRELLDBG_NO_SKIP_EPSILON)

	void skip_epsilon()
	{
		for (state_size_type pos = 0; pos < this->NFA_states.size(); ++pos)
		{
			state_type &state = this->NFA_states[pos];

			if (state.next1)
				state.next1 = static_cast<std::ptrdiff_t>(skip_nonbranch_epsilon(pos + state.next1)) - pos;

			if (state.next2)
				state.next2 = static_cast<std::ptrdiff_t>(skip_nonbranch_epsilon(pos + state.next2)) - pos;
		}
	}

	state_size_type skip_nonbranch_epsilon(state_size_type pos) const
	{
		for (;;)
		{
			const state_type &state = this->NFA_states[pos];

			if (state.type == st_epsilon && state.next2 == 0)
			{
				pos += state.next1;
				continue;
			}
			break;
		}
		return pos;
	}

#endif

#if !defined(SRELLDBG_NO_ASTERISK_OPT)

	void asterisk_optimisation()
	{
		for (state_size_type pos = 1u; pos < this->NFA_states.size(); ++pos)
		{
			state_type &curstate = this->NFA_states[pos];

			switch (curstate.type)
			{
			case st_character:
			case st_character_class:
				if (this->NFA_states[pos - 1].is_question_or_asterisk_before_corcc())
				{
					state_type &estate = this->NFA_states[pos - 1];
					const state_size_type nextno = pos + estate.farnext() - 1;

					if (is_exclusive_sequence(estate.quantifier, pos, nextno))
					{
						state_type &estate2 = this->NFA_states[pos - 1];
						state_type &corccstate = this->NFA_states[pos];

						estate2.next1 = 1;
						estate2.next2 = 0;
						estate2.char_num = char_ctrl::cc_nul;

						if (corccstate.next1 < 0)
							corccstate.next1 = 0;

						corccstate.next2 = nextno - pos;
					}
				}
				continue;

			default:;
			}
		}
	}

	bool is_exclusive_sequence(const re_quantifier &eq, const state_size_type curno, const state_size_type nextno)	//  const
	{
		const state_type &curstate = this->NFA_states[curno];
		range_pairs curchar_class;
		range_pairs nextchar_class;

		if (curstate.type == st_character)
		{
			curchar_class.join(range_pair_helper(curstate.char_num));
		}
		else if (curstate.type == st_character_class)
		{
			curchar_class = this->character_class[curstate.char_num];
			if (curchar_class.size() == 0)	//  Means [], which always makes matching fail.
				return true;	//  For preventing the automaton from pushing bt data.
		}
		else
		{
			return false;
		}

		const bool canbe0length = gather_nextchars(nextchar_class, nextno, 0u, true);

		if (nextchar_class.size())
		{
			if (!canbe0length || eq.is_greedy)
			{
#if !defined(SRELLDBG_NO_SPLITCC)

				range_pairs kept;
				range_pairs removed;

				curchar_class.split_ranges(kept, removed, nextchar_class);

				if (removed.size() == 0)	//  !curchar_class.is_overlap(nextchar_class)
					return true;

				if (curstate.type == st_character_class && kept.size() && eq.is_infinity())
				{
					{
						state_type &curstate2 = this->NFA_states[curno];

						curstate2.char_num = kept.consists_of_one_character(this->is_icase());
						if (curstate2.char_num != constants::invalid_u32value)
							curstate2.type = st_character;
						else
							curstate2.char_num = this->character_class.register_newclass(kept);
					}
					const re_quantifier backupeq(eq);

					insert_at(nextno, 2);
					state_type &n0 = this->NFA_states[nextno];
					state_type &n1 = this->NFA_states[nextno + 1];

					n0.reset(st_epsilon, epsilon_type::et_ccastrsk);
					n0.quantifier = backupeq;
//					n0.next2 = 1;
					n0.next2 = 2;
					if (!n0.quantifier.is_greedy)
					{
						n0.next1 = n0.next2;
						n0.next2 = 1;
					}

					n1.reset(st_character_class, removed.consists_of_one_character(this->is_icase()));

					if (n1.char_num != constants::invalid_u32value)
						n1.type = st_character;
					else
						n1.char_num = this->character_class.register_newclass(removed);

					n1.next1 = -2;
//					n1.next2 = 0;
					return true;
				}

#else	//  defined(SRELLDBG_NO_SPLITCC)

				if (!curchar_class.is_overlap(nextchar_class))
				{
					return true;
				}

#endif	//  !defined(SRELLDBG_NO_SPLITCC)
			}
		}
		else if (/* nextchar_class.size() == 0 && */ (!canbe0length || only_success_left(nextno)))
		{
			//  (size() == 0 && !canbe0length) means [].
			return eq.is_greedy ? true : false;
		}

		return false;
	}

	bool only_success_left(state_size_type pos) const
	{
		for (;;)
		{
			const state_type &state = this->NFA_states[pos];

			switch (state.type)
			{
			case st_success:
				return true;

			case st_roundbracket_close:
			case st_backreference:
				if (state.next2 != 0 && state.next1 != state.next2)
					return false;
				break;

			case st_epsilon:
				if (state.next2 != 0 && !only_success_left(pos + state.next2))
					return false;
				break;

			case st_roundbracket_open:
				break;	//  /a*()/

			default:
				return false;
			}
			if (state.next1)
				pos += state.next1;
			else
				return false;
		}
	}
#endif	//  !defined(SRELLDBG_NO_ASTERISK_OPT)

	void insert_at(const state_size_type pos, const std::ptrdiff_t len)
	{
		state_type newstate;

		for (state_size_type cur = 0; cur < pos; ++cur)
		{
			state_type &state = this->NFA_states[cur];

			if (state.next1 && (cur + state.next1) >= pos)
				state.next1 += len;

			if (state.next2 && (cur + state.next2) >= pos)
				state.next2 += len;
		}

		for (state_size_type cur = pos; cur < this->NFA_states.size(); ++cur)
		{
			state_type &state = this->NFA_states[cur];

			if ((cur + state.next1) < pos)
				state.next1 -= len;

			if ((cur + state.next2) < pos)
				state.next2 -= len;
		}

		newstate.reset(st_epsilon);
		for (std::ptrdiff_t count = 0; count < len; ++count)
			this->NFA_states.insert(pos, newstate);
	}

#if !defined(SRELLDBG_NO_STATEHOOK)

	void reorder_piece(state_array &piece) const
	{
		simple_array<ui_l32> newpos;
		ui_l32 offset = 0;

		newpos.resize(piece.size() + 1, 0);

		for (ui_l32 indx = 0; indx <= piece.size(); ++indx)
		{
			if (newpos[indx] == 0)
			{
				newpos[indx] = indx + offset;

				state_type &st = piece[indx];

				if (st.type == st_epsilon && st.char_num == epsilon_type::et_hooked)
				{
					st.char_num = epsilon_type::et_alt;
					++offset;
					newpos[indx + st.next1] = indx + offset;
				}
			}
			else
				--offset;
		}

		for (state_size_type indx = 0; indx < piece.size(); ++indx)
		{
			state_type &st = piece[indx];

			if (st.next1 != 0)
			{
				const ui_l32 newn1abs = newpos[indx + st.next1];
				st.next1 = static_cast<std::ptrdiff_t>(newn1abs) - newpos[indx];
			}

			if (st.next2 != 0)
			{
				const ui_l32 newn2abs = newpos[indx + st.next2];
				st.next2 = static_cast<std::ptrdiff_t>(newn2abs) - newpos[indx];
			}
		}

		state_array newpiece(piece.size());

		for (state_size_type indx = 0; indx < piece.size(); newpiece[newpos[indx]] = piece[indx], ++indx);

		newpiece.swap(piece);

	}

#endif	//  !defined(SRELLDBG_NO_STATEHOOK)

	bool check_if_backref_used(state_size_type pos, const ui_l32 number) const
	{
		for (; pos < this->NFA_states.size(); ++pos)
		{
			const state_type &state = this->NFA_states[pos];

			if (state.type == st_backreference && state.char_num == number)
				return true;
		}
		return false;
	}

#if !defined(SRELLDBG_NO_BRANCH_OPT) || !defined(SRELLDBG_NO_BRANCH_OPT2)

	state_size_type gather_if_char_or_charclass(range_pairs &charclass, state_size_type pos, const bool strictly) const
	{
		for (;;)
		{
			const state_type &curstate = this->NFA_states[pos];

			if (curstate.type == st_character && curstate.next2 == 0)
			{
				charclass.set_solerange(range_pair_helper(curstate.char_num));
				return pos;
			}
			else if (curstate.type == st_character_class && curstate.next2 == 0)
			{
				charclass = this->character_class[curstate.char_num];
				return pos;
			}
			else if (curstate.type == st_epsilon && curstate.next2 == 0 && !strictly)
			{
			}
			else
				break;

			pos += curstate.next1;
		}
		return 0;
	}
#endif	//  !defined(SRELLDBG_NO_BRANCH_OPT) || !defined(SRELLDBG_NO_BRANCH_OPT2)

#if !defined(SRELLDBG_NO_BRANCH_OPT)
	void branch_optimisation()
	{
		range_pairs nextcharclass1;

		for (state_size_type pos = 1; pos < this->NFA_states.size(); ++pos)
		{
			const state_type &state = this->NFA_states[pos];

			if (state.is_branch())
			{
				const state_size_type nextcharpos = gather_if_char_or_charclass(nextcharclass1, pos + state.next1, false);

				if (nextcharpos)
				{
					range_pairs nextcharclass2;
					const bool canbe0length = gather_nextchars(nextcharclass2, pos + state.next2, 0u /* bracket_number */, true);

					if (!canbe0length && !nextcharclass1.is_overlap(nextcharclass2))
					{
						state_type &branch = this->NFA_states[pos];
						state_type &next1 = this->NFA_states[nextcharpos];

						next1.next2 = pos + branch.next2 - nextcharpos;
						branch.next2 = 0;
						branch.char_num = epsilon_type::et_bo1fmrbr;
					}
				}
			}
		}
	}
#endif	//  !defined(SRELLDBG_NO_BRANCH_OPT)

#if !defined(SRELL_NO_ICASE)
	ui_l32 check_if_really_needs_icase_search()
	{
		for (state_size_type i = 0; i < this->NFA_states.size(); ++i)
		{
			const state_type &state = this->NFA_states[i];

			if (state.type == st_character)
			{
				if (unicode_case_folding::count_caseunfolding(state.char_num) > 1)
					return 1u;
			}
		}
		return 0u;
	}
#endif	//  !defined(SRELL_NO_ICASE)

#if !defined(SRELLDBG_NO_BMH)
	void setup_bmhdata()
	{
		simple_array<ui_l32> u32s;

		for (state_size_type i = 1; i < this->NFA_states.size(); ++i)
		{
			const state_type &state = this->NFA_states[i];

			if (state.type == st_character)
				u32s.push_backncr(state.char_num);
			else
			{
				u32s.clear();
				break;
			}
		}

		if (u32s.size() > 1)
//		if ((u32s.size() > 1 && !this->is_ricase()) || (u32s.size() > 2 && this->is_ricase()))
		{
			if (this->bmdata)
				this->bmdata->clear();
			else
				this->bmdata = new re_bmh<charT, utf_traits>;

			this->bmdata->setup(u32s, this->is_ricase());
			return /* false */;
		}

		if (this->bmdata)
			delete this->bmdata;
		this->bmdata = NULL;
//		return true;
	}
#endif	//  !defined(SRELLDBG_NO_BMH)

#if !defined(SRELLDBG_NO_CCPOS)
	void set_charclass_posinfo()
	{
		this->character_class.finalise();
		for (state_size_type i = 1; i < this->NFA_states.size(); ++i)
		{
			state_type &state = this->NFA_states[i];

			if (state.type == st_character_class || state.type == st_bol || state.type == st_eol || state.type == st_boundary)
			{
				const range_pair &posinfo = this->character_class.charclasspos(state.char_num);
				state.quantifier.set(posinfo.first, posinfo.second);
			}
		}
	}
#endif	//  !defined(SRELLDBG_NO_CCPOS)

#if !defined(SRELLDBG_NO_BRANCH_OPT2) && !defined(SRELLDBG_NO_STATEHOOK)

	void branch_optimisation2()
	{
		range_pairs basealt1stch;
		range_pairs nextalt1stch;

		for (state_size_type pos = 1; pos < this->NFA_states.size(); ++pos)
		{
			const state_type &curstate = this->NFA_states[pos];

			if (curstate.type == st_epsilon && curstate.next2 != 0 && (curstate.char_num == epsilon_type::et_alt || curstate.char_num == epsilon_type::et_hooked))	//  '|' or 'h'
			{
				const state_size_type next1pos = pos + curstate.next1;
				state_size_type precharchainpos = pos;

				if (gather_if_char_or_charclass(basealt1stch, next1pos, true) != 0)
				{
					state_size_type next2pos = precharchainpos + curstate.next2;
					state_size_type postcharchainpos = 0;

					for (;;)
					{
						state_size_type next2next1pos = next2pos;
						state_type &nstate2 = this->NFA_states[next2pos];
						state_size_type next2next2pos = 0;

						if (nstate2.type == st_epsilon)
						{
							if (nstate2.next2 != 0)
							{
								if (nstate2.char_num == epsilon_type::et_alt || nstate2.char_num == epsilon_type::et_hooked)	//  '|', 'h'
									next2next2pos = next2pos + nstate2.next2;
							}
							next2next1pos += nstate2.next1;
						}

						if (gather_if_char_or_charclass(nextalt1stch, next2next1pos, true) != 0)
						{
							const int relation = basealt1stch.relationship(nextalt1stch);

							if (relation == 0)
							{
								if (next2next2pos)	//  if (nstate2.is_branch())
								{
									nstate2.next2 = 0;
									nstate2.char_num = epsilon_type::et_bo2skpd;	//  '!'
								}

								const state_size_type next2next1next1pos = next2next1pos + this->NFA_states[next2next1pos].next1;
								state_type &becomes_unused = this->NFA_states[next2next1pos];

								postcharchainpos = postcharchainpos == 0
									? next1pos + this->NFA_states[next1pos].next1
									: postcharchainpos + this->NFA_states[postcharchainpos].next2;

								state_type &becomes_branch = this->NFA_states[postcharchainpos];

								becomes_unused = becomes_branch;

								becomes_unused.next1 = postcharchainpos + becomes_branch.next1 - next2next1pos;
								becomes_unused.next2 = becomes_unused.next2 ? (postcharchainpos + becomes_branch.next2 - next2next1pos) : 0;

								becomes_branch.reset(st_epsilon, epsilon_type::et_hooked);
								becomes_branch.next1 = next2next1pos - postcharchainpos;
								becomes_branch.next2 = next2next1next1pos - postcharchainpos;

								state_type &prechainbranchpoint = this->NFA_states[precharchainpos];

								prechainbranchpoint.next2 = next2next2pos
									? next2next2pos - precharchainpos
									: (prechainbranchpoint.char_num = epsilon_type::et_bo2fmrbr, 0);	//  '2'
							}
							else if (relation == 1)
							{
								break;
							}
							else
								precharchainpos = next2pos;
						}
						else
						{
							//  Fix for bug210428.
							//  Original: /mm2|m|mm/
							//  1st step: /m(?:m2||m)/ <- No more optimisation can be performed. Must quit.
							//  2nd step: /mm(?:2||)/ <- BUG.
							break;
						}

						if (next2next2pos == 0)
							break;

						next2pos = next2next2pos;
					}
				}
			}
		}

		reorder_piece(this->NFA_states);
	}
#endif	//   !defined(SRELLDBG_NO_BRANCH_OPT2) && !defined(SRELLDBG_NO_STATEHOOK)

#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND) && !defined(SRELLDBG_NO_MPREWINDER)

	void find_entrypoint()
	{
		if (find_singlechar_ep(1u))
			return;

		find_better_ep(1u);
	}

	bool find_singlechar_ep(state_size_type cur)
	{
		state_size_type curatompos = 0;
		state_size_type singlecharpos = 0;
		state_size_type seqpos = 0;
		ui_l32 prevchar = constants::invalid_u32value;
		ui_l32 charcount = 0;
		bool needs_rerun = false;

		for (; cur < this->NFA_states.size(); ++cur)
		{
			const state_type &state = this->NFA_states[cur];

			switch (state.type)
			{
			case st_character:
				curatompos = cur;
				ST_CHARACTER:
				if (prevchar != constants::invalid_u32value)
					seqpos = curatompos;

				singlecharpos = curatompos;
				++charcount;
				prevchar = this->NFA_states[cur].char_num;
				continue;

			case st_character_class:
				prevchar = constants::invalid_u32value;
				continue;

			case st_epsilon:
				if (state.next2 == 0)
				{
					if (state.char_num == epsilon_type::et_jmpinlp)
					{
						const state_size_type rapos = cur + state.next1;
						const state_type &repatom = this->NFA_states[rapos];

						if (repatom.type == st_character)
						{
							curatompos = cur;
							cur = rapos;
							needs_rerun = true;
							goto ST_CHARACTER;
						}
						prevchar = constants::invalid_u32value;
						cur = rapos - 1;
					}
					continue;
				}

				if ((state.char_num == epsilon_type::et_ccastrsk)
					|| ((state.char_num == epsilon_type::et_default)
						&& (is_reversible_atom(cur + state.nearnext(), true))))
				{
					cur += state.farnext() - 1;
					needs_rerun = true;
					prevchar = constants::invalid_u32value;
					continue;
				}
				break;

			case st_check_counter:
				{
					const state_size_type rapos = cur + 3;
					const state_type &repatom = this->NFA_states[rapos];

					if (repatom.type == st_character)
					{
						curatompos = cur;
						cur = rapos;
						needs_rerun = state.quantifier.is_same();
						goto ST_CHARACTER;
					}

					if (is_reversible_atom(rapos, state.quantifier.atleast == 0))
					{
						cur = cur + state.farnext() - 1;
						needs_rerun = true;
						prevchar = constants::invalid_u32value;
						continue;
					}
				}
				break;

			case st_save_and_reset_counter:
			case st_repeat_in_push:
				cur += state.next1 - 1;
				continue;

			case st_check_0_width_repeat:
				cur += state.next2 - 1;
				continue;

			case st_backreference:
			case st_lookaround_open:
				break;	//  Gives up.

			case st_restore_counter:
			case st_decrement_counter:
			case st_repeat_in_pop:
				break;	//  NFA_states broken.

			case st_roundbracket_open:
				if (check_if_backref_used(cur + 1, state.char_num))
					break;

				needs_rerun = true;
				continue;

			default:
				continue;
			}
			break;
		}

		return seqpos != 0
			? (create_rewinder(seqpos, needs_rerun), true)
			: (charcount > 1 ? (create_rewinder(singlecharpos, needs_rerun), true) : false);
	}

	bool is_reversible_atom(const state_size_type pos, const bool check_optseq) const
	{
		const state_type &s = this->NFA_states[pos];
		state_size_type end = 0u;

		if (s.type == st_character || s.type == st_character_class)
			return true;

		if (check_optseq)
			return false;	//  Optional sequence (?, *, {0,m}) found.
			//  Not only at the beginning but also at any position does
			//  an optional sequence need measures to be taken:
			//  "2000-1-1" =~ /\d(\d+-)?\d{1,2}-\d{1,2}/

		switch (s.type)
		{
		case st_epsilon:
			if (s.next2 == 0 && s.char_num == epsilon_type::et_ncgopen)
				end = skip_group(this->NFA_states, pos);
			break;

		case st_roundbracket_open:
			if (check_if_backref_used(pos + 1, s.char_num))
				return false;

			end = skip_bracket(s.char_num, this->NFA_states, pos);
			break;

		case st_repeat_in_push:
			end = skip_0width_checker(s.char_num, this->NFA_states, pos);
			//@fallthrough@

		default:;
		}

		return end != 0u && !has_obstacle_to_reverse(pos, end, false);
	}

	bool has_obstacle_to_reverse(state_size_type pos, const state_size_type end, const bool check_optseq) const
	{
		for (; pos < end;)
		{
			const state_type &s = this->NFA_states[pos];

			if (s.type == st_epsilon)
			{
				if (s.char_num == epsilon_type::et_alt)
					return true;
					//  The rewinder cannot support Disjunction because forward matching
					//  and backward matching can go through different routes:
					//  * In a forward search /(?:.|ab)c/ against "abc" matches "abc",
					//    while in a backward search from 'c' it matches "bc".

				//  Because of the same reason, the rewinder cannot support an optional
				//  group either. Semantically, /(\d+-)?\d{1,2}-\d{1,2}/ is equivalent to
				//  /(\d+-|)\d{1,2}-\d{1,2}/.
				if (check_optseq)
				{
					if (s.char_num == epsilon_type::et_jmpinlp)
					{
						pos += s.next1;
						continue;
					}

					if (s.char_num == epsilon_type::et_default && s.next2 != 0 && !this->NFA_states[pos + s.nearnext()].is_character_or_class())
						return true;
				}

			}
			else if (s.type == st_backreference)
				return true;
			else if (s.type == st_lookaround_open)
				return true;
			else if (check_optseq && s.type == st_check_counter)
			{
				if (s.quantifier.atleast == 0 && !this->NFA_states[pos + 3].is_character_or_class())
					return true;
				pos += 3;
				continue;
			}
			++pos;
		}
		return false;
	}

	state_size_type skip_bracket(const ui_l32 no, const state_array &NFAs, state_size_type pos) const
	{
		return find_pair(st_roundbracket_close, NFAs, no, pos);
	}

	state_size_type skip_0width_checker(const ui_l32 no, const state_array &NFAs, state_size_type pos) const
	{
		return find_pair(st_check_0_width_repeat, NFAs, no, pos);
	}

	state_size_type find_pair(const re_state_type type, const state_array &NFAs, const ui_l32 no, state_size_type pos) const
	{
		for (++pos; pos < NFAs.size(); ++pos)
		{
			const state_type &s = NFAs[pos];

			if (s.type == type && s.char_num == no)
				return pos;
		}
		return 0u;
	}

	state_size_type skip_group(const state_array &NFAs, state_size_type pos) const
	{
		ui_l32 depth = 1u;

		for (++pos; pos < NFAs.size(); ++pos)
		{
			const state_type &s = NFAs[pos];

			if (s.type == st_epsilon)
			{
				if (s.char_num == epsilon_type::et_ncgopen)
					++depth;
				else if (s.char_num == epsilon_type::et_ncgclose)
				{
					if (--depth == 0u)
						return pos;
				}
			}
		}
		return 0u;
	}

	void create_rewinder(const state_size_type end, const bool needs_rerun)
	{
		state_array newNFAs;
		state_type rwstate;

		newNFAs.append(this->NFA_states, 1u, end - 1u);
		if (!reverse_atoms(newNFAs) || newNFAs.size() == 0u)
			return;

		rwstate.reset(st_lookaround_open, meta_char::mc_eq);
		rwstate.next1 = static_cast<std::ptrdiff_t>(end + newNFAs.size() + 2) - 1;
		rwstate.next2 = 1;
		rwstate.quantifier.atleast = needs_rerun ? 3 : 2; //  Match point rewinder.
			//  "singing" problem: /\w+ing/ against "singing" matches the
			//  entire "singing". However, if modified like /(?<=\K\w+)ing/
			//  it matches "sing" only, which is not correct (but correct if
			//  /\w+?ing/). To avoid this problem, after rewinding is
			//  finished rerunning the automaton forwards from the found
			//  point is needed if the reversed states contain a variable
			//  length (non fixed length) atom.
			//  TODO: This rerunning can be avoided if the reversed atoms
			//  are an exclusive sequence, like /\d+[:,]+\d+abcd/.
		newNFAs.insert(0, rwstate);

		rwstate.type = st_lookaround_close;
		rwstate.next1 = 0;
		rwstate.next2 = 0;
		newNFAs.append(1, rwstate);

		this->NFA_states.insert(1, newNFAs);
		this->NFA_states[0].next2 = static_cast<std::ptrdiff_t>(newNFAs.size()) + 1;
	}

	bool reverse_atoms(state_array &NFAs)
	{
		state_array revNFAs;
		state_array atomseq;
		state_type epsilon;

		epsilon.reset(st_epsilon);

		for (state_size_type cur = 0u; cur < NFAs.size();)
		{
			const state_type &state = NFAs[cur];

			switch (state.type)
			{
			case st_epsilon:
				if (state.is_noncapturinggroup_begin_or_end())
				{
					revNFAs.insert(0, epsilon);
					++cur;
					continue;
				}
				break;

			case st_roundbracket_open:
				atomseq.clear();
				atomseq.push_back(epsilon);
				atomseq.push_back(epsilon);
				revNFAs.insert(0, atomseq);
				cur += 2;
				continue;

			case st_roundbracket_close:
				revNFAs.insert(0, epsilon);
				cur += 1;
				continue;

			default:;
			}

			const state_size_type boundary = find_atom_boundary(NFAs, cur, NFAs.size());

			if (boundary == 0u || cur == boundary)
				return false;

			atomseq.clear();
			atomseq.append(NFAs, cur, boundary - cur);

			if (!mend_for_reverse(atomseq))
			{
				return false;
			}

			cur = boundary;
			revNFAs.insert(0, atomseq);
		}
		revNFAs.swap(NFAs);
		return true;
	}

	bool mend_for_reverse(state_array &atoms)
	{
		for (state_size_type pos = 0; pos < atoms.size(); ++pos)
		{
			state_type &s = atoms[pos];

			switch (s.type)
			{
			case st_roundbracket_open:
				if (!check_if_backref_used(pos + 1, s.char_num))
				{
					const state_size_type end = skip_bracket(s.char_num, atoms, pos);

					if (end != 0u)
					{
						pos += 2;
						state_array rev(atoms, pos, end - pos);

						if (reverse_atoms(rev) && (end - pos) == rev.size())
						{
							if (s.quantifier.is_greedy)
							{
								atoms[pos - 2].reset(st_epsilon);
								atoms[pos - 1].reset(st_epsilon);
								atoms[end].type = st_epsilon;
								atoms[end].char_num = char_ctrl::cc_nul;
								atoms[end].next2 = 0;
							}
							else
							{
								state_type &bro = atoms[pos - 2];
								state_type &brp = atoms[pos - 1];
								state_type &brc = atoms[end];

								bro.type = st_repeat_in_push;
								brp.type = st_repeat_in_pop;
								brc.type = st_check_0_width_repeat;

								bro.char_num = this->number_of_repeats;
								brp.char_num = this->number_of_repeats;
								brc.char_num = this->number_of_repeats;
								++this->number_of_repeats;
							}
							atoms.replace(pos, end - pos, rev);
							pos += rev.size();
							continue;
						}
					}
				}
				return false;

			case st_epsilon:
				if (s.char_num == epsilon_type::et_ncgopen)
				{
					state_size_type end = skip_group(atoms, pos);

					if (end != 0u)
					{
						++pos;
						state_array rev(atoms, pos, end - pos);

						if (reverse_atoms(rev) && (end - pos) == rev.size())
						{
							atoms.replace(pos, end - pos, rev);
							pos += rev.size();
							continue;
						}
					}
					return false;
				}
				else if ((s.char_num == epsilon_type::et_ccastrsk || s.char_num == epsilon_type::et_default)
					&& s.next2 != 0 && !s.quantifier.is_greedy)
				{
					s.next2 = s.next1;
					s.next1 = 1;
					s.quantifier.is_greedy = 1u;
				}
				continue;

			case st_save_and_reset_counter:
				if (pos + 5 < atoms.size())
				{
					state_type &ccstate = atoms[pos + 2];

					if (!ccstate.quantifier.is_greedy)
					{
//						for (ui_l32 j = 0; j < 5; ++j)
//							atoms[pos + j].quantifier.is_greedy = true;

						ccstate.next2 = ccstate.next1;
						ccstate.next1 = 1;
						ccstate.quantifier.is_greedy = 1u;
					}
					continue;
				}
				return false;

			default:;
			}
		}
		return true;
	}

	state_size_type find_atom_boundary(const state_array &NFAs, state_size_type cur, const state_size_type end) const
	{
		const state_size_type begin = cur;
		state_size_type charatomseq_endpos = cur;
		const state_type *charatomseq_begin = NULL;

		for (; cur < end; ++cur)
		{
			const state_type &state = NFAs[cur];

			switch (state.type)
			{
			case st_character:
			case st_character_class:
				if (charatomseq_begin == NULL)
					charatomseq_begin = &state;
				else if (!charatomseq_begin->is_same_character_or_charclass(state))
					return charatomseq_endpos;

				charatomseq_endpos = cur + 1;
				continue;

			case st_epsilon:
				if (state.next2 == 0)
				{
					if (charatomseq_begin)
						return charatomseq_endpos;

					if (state.char_num == epsilon_type::et_jmpinlp)
						continue;
					else if (state.char_num == epsilon_type::et_ncgopen)
					{
						const state_size_type gend = skip_group(NFAs, cur);

						return gend != 0u ? gend + 1 : 0u;
					}
					else if (state.char_num != epsilon_type::et_brnchend)
						return cur + 1;

					return 0u;
				}

				if (state.char_num == epsilon_type::et_ccastrsk)
				{
					if (cur + 1 < end)
					{
						const state_type &repatom = NFAs[cur + 1];

						if (charatomseq_begin == NULL)
							charatomseq_begin = &repatom;
						else if (!charatomseq_begin->is_same_character_or_charclass(repatom))
							return charatomseq_endpos;

						return cur + state.farnext();
					}
					return 0u;
				}
				else if (state.char_num == epsilon_type::et_alt)	//  '|'
				{
					if (charatomseq_begin)
						return charatomseq_endpos;

					state_size_type altend = cur + state.next2 - 1;

					for (; this->NFA_states[altend].type == st_epsilon && this->NFA_states[altend].char_num == epsilon_type::et_brnchend; altend += this->NFA_states[altend].next1);

					return altend;
				}

				if (state.char_num == epsilon_type::et_default)
					return charatomseq_begin ? charatomseq_endpos : cur + state.farnext();

				return 0u;

			case st_save_and_reset_counter:
				if (charatomseq_begin)
					return charatomseq_endpos;

				cur += 2;
				return cur + NFAs[cur].farnext();

			case st_bol:
			case st_eol:
			case st_boundary:
			case st_backreference:
				if (charatomseq_begin)
					return charatomseq_endpos;
				return cur + 1;

			case st_roundbracket_open:
				if (charatomseq_begin)
					return charatomseq_endpos;
				else
				{
					const state_size_type rbend = skip_bracket(state.char_num, NFAs, cur);

					if (rbend != 0u)
						return rbend + 1;
				}
				return 0u;

			case st_repeat_in_push:
				if (charatomseq_begin)
					return charatomseq_endpos;
				else
				{
					const state_size_type rpend = skip_0width_checker(state.char_num, NFAs, cur);

					if (rpend != 0u)
						return rpend + 1;
				}
				return 0u;

			case st_lookaround_open:
				if (charatomseq_begin)
					return charatomseq_endpos;
				return cur + state.next1;

			case st_roundbracket_close:
			case st_check_0_width_repeat:
			case st_lookaround_close:
				return charatomseq_endpos;

			case st_roundbracket_pop:
			case st_check_counter:
			case st_decrement_counter:
			case st_restore_counter:
			case st_repeat_in_pop:
				return 0u;

			default:
				return 0u;
			}
		}
		return begin != charatomseq_endpos ? charatomseq_endpos : 0u;
	}

	bool find_better_ep(state_size_type cur)
	{
		state_size_type betterpos = 0u;
		ui_l32 bp_cpnum = 0u;
		ui_l32 charcount = 0u;
		range_pairs nextcc;

		for (; cur < this->NFA_states.size();)
		{
			const state_type &state = this->NFA_states[cur];

			if (state.type == st_epsilon)
			{
				if (state.char_num == epsilon_type::et_ncgopen || (state.next2 == 0 && state.char_num != epsilon_type::et_jmpinlp))
				{
					++cur;
					continue;
				}
			}
			else if (state.type == st_roundbracket_open)
			{
				cur += state.next1;
				continue;
			}
			else if (state.type == st_bol || state.type == st_eol || state.type == st_boundary)
			{
				cur += state.next1;
				continue;
			}
			else if (state.type == st_roundbracket_close)
			{
				cur += state.next2;
				continue;
			}
			else if (state.type == st_backreference || state.type == st_lookaround_open)
				break;

			const state_size_type boundary = find_atom_boundary(this->NFA_states, cur, this->NFA_states.size());

			if (boundary == 0u || cur == boundary)
				break;

			nextcc.clear();
			const bool canbe0length = gather_nextchars(nextcc, cur, 0u, false);

			if (canbe0length)
				break;

			const ui_l32 cpnum = nextcc.total_codepoints();
			const bool has_obstacle = has_obstacle_to_reverse(cur, boundary, true);

			if (betterpos != 0u)
			{
				if (bp_cpnum > cpnum || (bp_cpnum == cpnum && charcount == 1u))
				{
					betterpos = cur;
					bp_cpnum = cpnum;
					++charcount;
				}
			}
			else
			{
				betterpos = cur;
				bp_cpnum = cpnum;
				++charcount;
			}

			if (has_obstacle)
				break;

			cur = boundary;
		}

		return (charcount > 1u) ? (create_rewinder(betterpos, true), true) : false;
	}

#endif	//  !defined(SRELL_FIXEDWIDTHLOOKBEHIND) && !defined(SRELLDBG_NO_MPREWINDER)

public:	//  For debug.

	void print_NFA_states(const int) const;
};
//  re_compiler

	}	//  namespace re_detail

//  ... "rei_compiler.hpp"]
//  ["regex_sub_match.hpp" ...

//  28.9, class template sub_match:
template <class BidirectionalIterator>
class sub_match : public std::pair<BidirectionalIterator, BidirectionalIterator>
{
public:

	typedef typename std::iterator_traits<BidirectionalIterator>::value_type value_type;
	typedef typename std::iterator_traits<BidirectionalIterator>::difference_type difference_type;
	typedef BidirectionalIterator iterator;
	typedef std::basic_string<value_type> string_type;

	bool matched;

//	constexpr sub_match();	//  C++11.

	sub_match() : matched(false)
	{
	}

	difference_type length() const
	{
		return matched ? std::distance(this->first, this->second) : 0;
	}

	operator string_type() const
	{
		return matched ? string_type(this->first, this->second) : string_type();
	}

	string_type str() const
	{
		return matched ? string_type(this->first, this->second) : string_type();
	}

	int compare(const sub_match &s) const
	{
		return str().compare(s.str());
	}

	int compare(const string_type &s) const
	{
		return str().compare(s);
	}

	int compare(const value_type *const s) const
	{
		return str().compare(s);
	}

	void swap(sub_match &s)
	{
		if (this != &s)
		{
			this->std::pair<BidirectionalIterator, BidirectionalIterator>::swap(s);
			std::swap(matched, s.matched);
		}
	}

#if !defined(SRELL_NO_APIEXT)

	template <typename ST, typename SA>
	operator std::basic_string<value_type, ST, SA>() const
	{
		typedef std::basic_string<value_type, ST, SA> string_type2;
		return matched ? string_type2(this->first, this->second) : string_type2();
	}

	template <typename ST, typename SA>
	std::basic_string<value_type, ST, SA> str() const
	{
		typedef std::basic_string<value_type, ST, SA> string_type2;
		return matched ? string_type2(this->first, this->second) : string_type2();
	}

#endif	//  !defined(SRELL_NO_APIEXT)

	void set_(const typename re_detail::re_submatch_type<BidirectionalIterator> &br)
	{
		this->first = br.core.open_at;
		this->second = br.core.close_at;
		this->matched = br.counter != 0;
	}
};

//  28.9.2, sub_match non-member operators:
//  [7.9.2] sub_match non-member operators

//  Compares sub_match & with sub_match &.
template <class BiIter>
bool operator==(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) == 0;	//  1
}

template <class BiIter>
bool operator!=(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) != 0;	//  2
}

template <class BiIter>
bool operator<(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) < 0;	//  3
}

template <class BiIter>
bool operator<=(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) <= 0;	//  4
}

template <class BiIter>
bool operator>=(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) >= 0;	//  5
}

template <class BiIter>
bool operator>(const sub_match<BiIter> &lhs, const sub_match<BiIter> &rhs)
{
	return lhs.compare(rhs) > 0;	//  6
}

//  Compares basic_string & with sub_match &.
template <class BiIter, class ST, class SA>
bool operator==(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(lhs.c_str()) == 0;	//  7
}

template <class BiIter, class ST, class SA>
bool operator!=(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs == rhs);	//  8
}

template <class BiIter, class ST, class SA>
bool operator<(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(lhs.c_str()) > 0;	//  9
}

template <class BiIter, class ST, class SA>
bool operator>(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs < lhs;	//  10
}

template <class BiIter, class ST, class SA>
bool operator>=(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs < rhs);	//  11
}

template <class BiIter, class ST, class SA>
bool operator<=(
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(rhs < lhs);	//  12
}

//  Compares sub_match & with basic_string &.
template <class BiIter, class ST, class SA>
bool operator==(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return lhs.compare(rhs.c_str()) == 0;	//  13
}

template <class BiIter, class ST, class SA>
bool operator!=(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return !(lhs == rhs);	//  14
}

template <class BiIter, class ST, class SA>
bool operator<(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return lhs.compare(rhs.c_str()) < 0;	//  15
}

template <class BiIter, class ST, class SA>
bool operator>(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return rhs < lhs;	//  16
}

template <class BiIter, class ST, class SA>
bool operator>=(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return !(lhs < rhs);	//  17
}

template <class BiIter, class ST, class SA>
bool operator<=(
	const sub_match<BiIter> &lhs,
	const std::basic_string<typename std::iterator_traits<BiIter>::value_type, ST, SA> &rhs
)
{
	return !(rhs < lhs);	//  18
}

//  Compares iterator_traits::value_type * with sub_match &.
template <class BiIter>
bool operator==(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(lhs) == 0;	//  19
}

template <class BiIter>
bool operator!=(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs == rhs);	//  20
}

template <class BiIter>
bool operator<(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(lhs) > 0;	//  21
}

template <class BiIter>
bool operator>(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs < lhs;	//  22
}

template <class BiIter>
bool operator>=(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs < rhs);	//  23
}

template <class BiIter>
bool operator<=(
	typename std::iterator_traits<BiIter>::value_type const *lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(rhs < lhs);	//  24
}

//  Compares sub_match & with iterator_traits::value_type *.
template <class BiIter>
bool operator==(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return lhs.compare(rhs) == 0;	//  25
}

template <class BiIter>
bool operator!=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return !(lhs == rhs);	//  26
}

template <class BiIter>
bool operator<(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return lhs.compare(rhs) < 0;	//  27
}

template <class BiIter>
bool operator>(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return rhs < lhs;	//  28
}

template <class BiIter>
bool operator>=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return !(lhs < rhs);	//  29
}

template <class BiIter>
bool operator<=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const *rhs
)
{
	return !(rhs < lhs);	//  30
}

//  Compares iterator_traits::value_type & with sub_match &.
template <class BiIter>
bool operator==(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(typename sub_match<BiIter>::string_type(1, lhs)) == 0;	//  31
}

template <class BiIter>
bool operator!=(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs == rhs);	//  32
}

template <class BiIter>
bool operator<(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs.compare(typename sub_match<BiIter>::string_type(1, lhs)) > 0;	//  33
}

template <class BiIter>
bool operator>(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return rhs < lhs;	//  34
}

template <class BiIter>
bool operator>=(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(lhs < rhs);	//  35
}

template <class BiIter>
bool operator<=(
	typename std::iterator_traits<BiIter>::value_type const &lhs,
	const sub_match<BiIter> &rhs
)
{
	return !(rhs < lhs);	//  36
}

//  Compares sub_match & with iterator_traits::value_type &.
template <class BiIter>
bool operator==(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return lhs.compare(typename sub_match<BiIter>::string_type(1, rhs)) == 0;	//  37
}

template <class BiIter>
bool operator!=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return !(lhs == rhs);	//  38
}

template <class BiIter>
bool operator<(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return lhs.compare(typename sub_match<BiIter>::string_type(1, rhs)) < 0;	//  39
}

template <class BiIter>
bool operator>(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return rhs < lhs;	//  40
}

template <class BiIter>
bool operator>=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return !(lhs < rhs);	//  41
}

template <class BiIter>
bool operator<=(
	const sub_match<BiIter> &lhs,
	typename std::iterator_traits<BiIter>::value_type const &rhs
)
{
	return !(rhs < lhs);	//  42
}

template <class charT, class ST, class BiIter>
std::basic_ostream<charT, ST> &operator<<(std::basic_ostream<charT, ST> &os, const sub_match<BiIter> &m)
{
	return (os << m.str());
}

typedef sub_match<const char *> csub_match;
typedef sub_match<const wchar_t *> wcsub_match;
typedef sub_match<std::string::const_iterator> ssub_match;
typedef sub_match<std::wstring::const_iterator> wssub_match;

typedef csub_match u8ccsub_match;
typedef ssub_match u8cssub_match;

#if defined(WCHAR_MAX)
	#if WCHAR_MAX >= 0x10ffff
		typedef wcsub_match u32wcsub_match;
		typedef wssub_match u32wssub_match;
		typedef u32wcsub_match u1632wcsub_match;
		typedef u32wssub_match u1632wssub_match;
	#elif WCHAR_MAX >= 0xffff
		typedef wcsub_match u16wcsub_match;
		typedef wssub_match u16wssub_match;
		typedef u16wcsub_match u1632wcsub_match;
		typedef u16wssub_match u1632wssub_match;
	#endif
#endif

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef sub_match<const char16_t *> u16csub_match;
	typedef sub_match<const char32_t *> u32csub_match;
	typedef sub_match<std::u16string::const_iterator> u16ssub_match;
	typedef sub_match<std::u32string::const_iterator> u32ssub_match;
#endif

#if defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef sub_match<const char8_t *> u8csub_match;
#else
	typedef u8ccsub_match u8csub_match;
#endif
#if defined(SRELL_CPP20_CHAR8_ENABLED) && SRELL_CPP20_CHAR8_ENABLED >= 2
	typedef sub_match<std::u8string::const_iterator> u8ssub_match;
#else	//  !defined(SRELL_CPP20_CHAR8_ENABLED) || SRELL_CPP20_CHAR8_ENABLED < 2
	typedef u8cssub_match u8ssub_match;
#endif

//  ... "regex_sub_match.hpp"]
//  ["regex_match_results.hpp" ...

//  28.10, class template match_results:
template <class BidirectionalIterator, class Allocator = std::allocator<sub_match<BidirectionalIterator> > >
class match_results
{
public:

	typedef sub_match<BidirectionalIterator> value_type;
	typedef const value_type & const_reference;
	typedef const_reference reference;
//	typedef implementation defined const_iterator;
	typedef typename std::vector<value_type, Allocator>::const_iterator const_iterator;
	typedef const_iterator iterator;
	typedef typename std::iterator_traits<BidirectionalIterator>::difference_type difference_type;

#if defined(__cplusplus) && __cplusplus >= 201103L
	typedef typename std::allocator_traits<Allocator>::size_type size_type;
#else
	typedef typename Allocator::size_type size_type;	//  TR1.
#endif

	typedef Allocator allocator_type;
	typedef typename std::iterator_traits<BidirectionalIterator>::value_type char_type;
	typedef std::basic_string<char_type> string_type;

public:

	//  28.10.1, construct/copy/destroy:
	//  [7.10.1] construct/copy/destroy
	explicit match_results(const Allocator &a = Allocator()) : ready_(0u), sub_matches_(a)
	{
	}

	match_results(const match_results &m)
	{
		operator=(m);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	match_results(match_results &&m) SRELL_NOEXCEPT
	{
		operator=(std::move(m));
	}
#endif

	match_results &operator=(const match_results &m)
	{
		if (this != &m)
		{
//			this->sstate_ = m.sstate_;
			this->ready_ = m.ready_;
			this->sub_matches_ = m.sub_matches_;
			this->prefix_ = m.prefix_;
			this->suffix_ = m.suffix_;
			this->base_ = m.base_;
#if !defined(SRELL_NO_NAMEDCAPTURE)
			this->gnames_ = m.gnames_;
#endif
		}
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	match_results &operator=(match_results &&m) SRELL_NOEXCEPT
	{
		if (this != &m)
		{
//			this->sstate_ = std::move(m.sstate_);
			this->ready_ = m.ready_;
			this->sub_matches_ = std::move(m.sub_matches_);
			this->prefix_ = std::move(m.prefix_);
			this->suffix_ = std::move(m.suffix_);
			this->base_ = m.base_;
#if !defined(SRELL_NO_NAMEDCAPTURE)
			this->gnames_ = std::move(m.gnames_);
#endif
		}
		return *this;
	}
#endif

//	~match_results();

	//  28.10.2, state:
	bool ready() const
	{
		return (ready_ & 1u) ? true : false;
	}

	//  28.10.3, size:
	//  [7.10.2] size
	size_type size() const
	{
		return sub_matches_.size();
	}

	size_type max_size() const
	{
		return sub_matches_.max_size();
//		return static_cast<size_type>(~0) / sizeof (value_type);
	}

	bool empty() const
	{
		return size() == 0;
	}

	//  28.10.4, element access:
	//  [7.10.3] element access
	difference_type length(const size_type sub = 0) const
	{
		return (*this)[sub].length();
	}

	difference_type position(const size_type sub = 0) const
	{
		const_reference ref = (*this)[sub];

		return std::distance(base_, ref.first);
	}

	string_type str(const size_type sub = 0) const
	{
		return string_type((*this)[sub]);
	}

	const_reference operator[](const size_type n) const
	{
		return n < sub_matches_.size() ? sub_matches_[n] : unmatched_;
	}

#if !defined(SRELL_NO_NAMEDCAPTURE)

	//  Helpers for overload resolution of the integer literal 0 of signed types.
	template <typename IntegerType>
	difference_type length(const IntegerType zero) const
	{
		return length(static_cast<size_type>(zero));
	}
	template <typename IntegerType>
	difference_type position(const IntegerType zero) const
	{
		return position(static_cast<size_type>(zero));
	}
	template <typename IntegerType>
	string_type str(const IntegerType zero) const
	{
		return str(static_cast<size_type>(zero));
	}
	template <typename IntegerType>
	const_reference operator[](const IntegerType zero) const
	{
		return operator[](static_cast<size_type>(zero));
	}

	difference_type length(const string_type &sub) const
	{
		return (*this)[sub].length();
	}

	difference_type position(const string_type &sub) const
	{
		const_reference ref = (*this)[sub];

		return std::distance(base_, ref.first);
	}

	string_type str(const string_type &sub) const
	{
		return string_type((*this)[sub]);
	}

	const_reference operator[](const string_type &sub) const
	{
		const re_detail::ui_l32 backrefno = lookup_backref_number(sub.data(), sub.data() + sub.size());

		return backrefno != re_detail::groupname_mapper<char_type>::notfound ? sub_matches_[backrefno] : unmatched_;
	}

	difference_type length(const char_type *sub) const
	{
		return (*this)[sub].length();
	}

	difference_type position(const char_type *sub) const
	{
		const_reference ref = (*this)[sub];

		return std::distance(base_, ref.first);
	}

	string_type str(const char_type *sub) const
	{
		return string_type((*this)[sub]);
	}

	const_reference operator[](const char_type *sub) const
	{
		const re_detail::ui_l32 backrefno = lookup_backref_number(sub, sub + std::char_traits<char_type>::length(sub));

		return backrefno != re_detail::groupname_mapper<char_type>::notfound ? sub_matches_[backrefno] : unmatched_;
	}

#endif	//  !defined(SRELL_NO_NAMEDCAPTURE)

	const_reference prefix() const
	{
		return prefix_;
	}

	const_reference suffix() const
	{
		return suffix_;
	}

	const_iterator begin() const
	{
		return sub_matches_.begin();
	}

	const_iterator end() const
	{
		return sub_matches_.end();
	}

	const_iterator cbegin() const
	{
		return sub_matches_.begin();
	}

	const_iterator cend() const
	{
		return sub_matches_.end();
	}

	//  28.10.5, format:
	//  [7.10.4] format
	template <class OutputIter>
	OutputIter format(
		OutputIter out,
		const char_type *fmt_first,
		const char_type *const fmt_last,
		regex_constants::match_flag_type /* flags */ = regex_constants::format_default
	) const
	{
		if (this->ready() && !this->empty())
		{
#if !defined(SRELL_NO_NAMEDCAPTURE)
			const bool no_groupnames = gnames_.size() == 0;
#endif
			const value_type &m0 = (*this)[0];

			while (fmt_first != fmt_last)
			{
				if (*fmt_first != static_cast<char_type>(re_detail::meta_char::mc_dollar))	//  '$'
				{
					*out++ = *fmt_first++;
				}
				else
				{
					++fmt_first;
					if (fmt_first == fmt_last)
					{
						*out++ = re_detail::meta_char::mc_dollar;	//  '$';
					}
					else if (*fmt_first == static_cast<char_type>(re_detail::char_other::co_amp))	//  '&', $&
					{
						out = std::copy(m0.first, m0.second, out);
						++fmt_first;
					}
					else if (*fmt_first == static_cast<char_type>(re_detail::char_other::co_grav))	//  '`', $`, prefix.
					{
						out = std::copy(this->prefix().first, this->prefix().second, out);
						++fmt_first;
					}
					else if (*fmt_first == static_cast<char_type>(re_detail::char_other::co_apos))	//  '\'', $', suffix.
					{
						out = std::copy(this->suffix().first, this->suffix().second, out);
						++fmt_first;
					}
#if !defined(SRELL_NO_NAMEDCAPTURE)
					else if (*fmt_first == static_cast<char_type>(re_detail::meta_char::mc_lt) && !no_groupnames)	//  '<', $<
					{
						const char_type *const current_backup = fmt_first;
						bool replaced = false;

						if (++fmt_first == fmt_last)
							;	//  Do nothing.
						else
						{
							const char_type *const name_begin = fmt_first;

							for (;; ++fmt_first)
							{
								if (*fmt_first == static_cast<char_type>(re_detail::meta_char::mc_gt))
								{
									const re_detail::ui_l32 backref_number = lookup_backref_number(name_begin, fmt_first);

									if (backref_number != re_detail::groupname_mapper<char_type>::notfound)
									{
										const value_type &mn = (*this)[backref_number];

										if (mn.matched)
											out = std::copy(mn.first, mn.second, out);
//										replaced = true;
									}
									replaced = true;
									++fmt_first;
									break;
								}
								if (fmt_first == fmt_last)
									break;
							}
						}
						if (!replaced)
						{
							fmt_first = current_backup;
							*out++ = re_detail::meta_char::mc_dollar;	//  '$';
						}
					}
#endif	//  !defined(SRELL_NO_NAMEDCAPTURE)
					else
					{
						const char_type *const backup_pos = fmt_first;
						size_type backref_number = 0;

						if (fmt_first != fmt_last && *fmt_first >= static_cast<char_type>(re_detail::char_alnum::ch_0) && *fmt_first <= static_cast<char_type>(re_detail::char_alnum::ch_9))	//  '0'-'9'
						{
							backref_number += *fmt_first - re_detail::char_alnum::ch_0;	//  '0';

							if (++fmt_first != fmt_last && *fmt_first >= static_cast<char_type>(re_detail::char_alnum::ch_0) && *fmt_first <= static_cast<char_type>(re_detail::char_alnum::ch_9))	//  '0'-'9'
							{
								backref_number *= 10;
								backref_number += *fmt_first - re_detail::char_alnum::ch_0;	//  '0';
								++fmt_first;
							}
						}

						if (backref_number && backref_number < this->size())
						{
							const value_type &mn = (*this)[backref_number];

							if (mn.matched)
								out = std::copy(mn.first, mn.second, out);
						}
						else
						{
							*out++ = re_detail::meta_char::mc_dollar;	//  '$';

							fmt_first = backup_pos;
							if (*fmt_first == static_cast<char_type>(re_detail::meta_char::mc_dollar))
								++fmt_first;
						}
					}
				}
			}
		}
		return out;
	}

	template <class OutputIter, class ST, class SA>
	OutputIter format(
		OutputIter out,
		const std::basic_string<char_type, ST, SA> &fmt,
		regex_constants::match_flag_type flags = regex_constants::format_default
	) const
	{
		return format(out, fmt.data(), fmt.data() + fmt.size(), flags);
	}

	template <class ST, class SA>
	std::basic_string<char_type, ST, SA> format(
		const string_type &fmt,
		regex_constants::match_flag_type flags = regex_constants::format_default
	) const
	{
		std::basic_string<char_type, ST, SA> result;

//		format(std::back_insert_iterator<string_type>(result), fmt, flags);
		format(std::back_inserter(result), fmt, flags);
		return result;
	}

	string_type format(const char_type *fmt, regex_constants::match_flag_type flags = regex_constants::format_default) const
	{
		string_type result;

		format(std::back_inserter(result), fmt, fmt + std::char_traits<char_type>::length(fmt), flags);
		return result;
	}

	//  28.10.6, allocator:
	//  [7.10.5] allocator
	allocator_type get_allocator() const
	{
		return allocator_type();
	}

	//  28.10.7, swap:
	//  [7.10.6] swap
	void swap(match_results &that)
	{
		const match_results tmp(that);
		that = *this;
		*this = tmp;
	}

	regex_constants::error_type ecode() const
	{
		return static_cast<regex_constants::error_type>(ready_ >> 1);
	}

public:	//  For internal.

	typedef match_results<BidirectionalIterator> match_results_type;
	typedef typename match_results_type::size_type match_results_size_type;
	typedef typename re_detail::re_search_state</*charT, */BidirectionalIterator> search_state_type;

	search_state_type sstate_;

	void clear_()
	{
		ready_ = 0u;
		sub_matches_.clear();
//		prefix_.matched = false;
//		suffix_.matched = false;
#if !defined(SRELL_NO_NAMEDCAPTURE)
		gnames_.clear();
#endif
	}

//	template <typename charT>
#if !defined(SRELL_NO_NAMEDCAPTURE)
	bool set_match_results_(const re_detail::groupname_mapper<char_type> &gnames)
#else
	bool set_match_results_()
#endif
	{
		sub_matches_.resize(sstate_.bracket.size());
//		value_type &m0 = sub_matches_[0];

		sub_matches_[0].matched = true;

		for (re_detail::ui_l32 i = 1; i < static_cast<re_detail::ui_l32>(sstate_.bracket.size()); ++i)
			sub_matches_[i].set_(sstate_.bracket[i]);

		base_ = sstate_.lblim;
		prefix_.first = sstate_.srchbegin;
		prefix_.second = sub_matches_[0].first = sstate_.bracket[0].core.open_at;
		suffix_.first = sub_matches_[0].second = sstate_.ssc.iter;
		suffix_.second = sstate_.srchend;

		prefix_.matched = prefix_.first != prefix_.second;
		suffix_.matched = suffix_.first != suffix_.second;

#if !defined(SRELL_NO_NAMEDCAPTURE)
		gnames_ = gnames;
#endif
		ready_ = 1u;
		return true;
	}

	bool set_match_results_bmh_()
	{
		sub_matches_.resize(1);
//		value_type &m0 = sub_matches_[0];

		sub_matches_[0].matched = true;

		base_ = sstate_.lblim;
		prefix_.first = sstate_.srchbegin;
		prefix_.second = sub_matches_[0].first = sstate_.ssc.iter;
		suffix_.first = sub_matches_[0].second = sstate_.nextpos;
		suffix_.second = sstate_.srchend;

		prefix_.matched = prefix_.first != prefix_.second;
		suffix_.matched = suffix_.first != suffix_.second;

		ready_ = 1u;
		return true;
	}

	void set_prefix1_(const BidirectionalIterator pf)
	{
		prefix_.first = pf;
	}

	void update_prefix1_(const BidirectionalIterator pf)
	{
		prefix_.first = pf;
		prefix_.matched = prefix_.first != prefix_.second;
	}

	void update_prefix2_(const BidirectionalIterator ps)
	{
		prefix_.second = ps;
		prefix_.matched = prefix_.first != prefix_.second;
	}

	void update_m0_(const BidirectionalIterator mf, const BidirectionalIterator ms)
	{
		sub_matches_.resize(1);

		sub_matches_[0].first = mf;
		sub_matches_[0].second = ms;
		sub_matches_[0].matched = true;

		prefix_.first = prefix_.second = mf;
	}

	bool mark_as_failed_(const int reason)
	{
		ready_ = reason ? (reason << 1) : 1u;
		return false;
	}

private:

#if !defined(SRELL_NO_NAMEDCAPTURE)

	re_detail::ui_l32 lookup_backref_number(const char_type *begin, const char_type *const end) const
	{
		typename re_detail::groupname_mapper<char_type>::gname_string key(end - begin);

		for (std::size_t i = 0; begin != end; ++begin, ++i)
			key[i] = *begin;

		return gnames_[key];
	}

#endif	//  !defined(SRELL_NO_NAMEDCAPTURE)

public:	//  For debug.

	template <typename BasicRegexT>
	void print_sub_matches(const BasicRegexT &, const int) const;
	void print_addresses(const value_type &, const char *const) const;

private:

	typedef std::vector<value_type, Allocator> sub_match_array;

	unsigned int ready_;
	sub_match_array sub_matches_;
	value_type prefix_;
	value_type suffix_;
	value_type unmatched_;
	BidirectionalIterator base_;

#if !defined(SRELL_NO_NAMEDCAPTURE)
	re_detail::groupname_mapper<char_type> gnames_;
#endif
};

//  28.10.7, match_results swap:
//  [7.10.6] match_results swap
template <class BidirectionalIterator, class Allocator>
void swap(
	match_results<BidirectionalIterator, Allocator> &m1,
	match_results<BidirectionalIterator, Allocator> &m2
)
{
	m1.swap(m2);
}

//  28.10.8, match_results comparisons
template <class BidirectionalIterator, class Allocator>
bool operator==(
	const match_results<BidirectionalIterator, Allocator> &m1,
	const match_results<BidirectionalIterator, Allocator> &m2
)
{
	if (!m1.ready() && !m2.ready())
		return true;

	if (m1.ready() && m2.ready())
	{
		if (m1.empty() && m2.empty())
			return true;

		if (!m1.empty() && !m2.empty())
		{
			return m1.prefix() == m2.prefix() && m1.size() == m2.size() && std::equal(m1.begin(), m1.end(), m2.begin()) && m1.suffix() == m2.suffix();
		}
	}
	return false;
}

template <class BidirectionalIterator, class Allocator>
bool operator!=(
	const match_results<BidirectionalIterator, Allocator> &m1,
	const match_results<BidirectionalIterator, Allocator> &m2
)
{
	return !(m1 == m2);
}

typedef match_results<const char *> cmatch;
typedef match_results<const wchar_t *> wcmatch;
typedef match_results<std::string::const_iterator> smatch;
typedef match_results<std::wstring::const_iterator> wsmatch;

typedef cmatch u8ccmatch;
typedef smatch u8csmatch;

#if defined(WCHAR_MAX)
	#if WCHAR_MAX >= 0x10ffff
		typedef wcmatch u32wcmatch;
		typedef wsmatch u32wsmatch;
		typedef u32wcmatch u1632wcmatch;
		typedef u32wsmatch u1632wsmatch;
	#elif WCHAR_MAX >= 0xffff
		typedef wcmatch u16wcmatch;
		typedef wsmatch u16wsmatch;
		typedef u16wcmatch u1632wcmatch;
		typedef u16wsmatch u1632wsmatch;
	#endif
#endif

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef match_results<const char16_t *> u16cmatch;
	typedef match_results<const char32_t *> u32cmatch;
	typedef match_results<std::u16string::const_iterator> u16smatch;
	typedef match_results<std::u32string::const_iterator> u32smatch;
#endif

#if defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef match_results<const char8_t *> u8cmatch;
#else
	typedef u8ccmatch u8cmatch;
#endif
#if defined(SRELL_CPP20_CHAR8_ENABLED) && SRELL_CPP20_CHAR8_ENABLED >= 2
	typedef match_results<std::u8string::const_iterator> u8smatch;
#else	//  !defined(SRELL_CPP20_CHAR8_ENABLED) || SRELL_CPP20_CHAR8_ENABLED < 2
	typedef u8csmatch u8smatch;
#endif

//  ... "regex_match_results.hpp"]
//  ["rei_algorithm.hpp" ...

	namespace re_detail
	{

#if !defined(SRELL_NO_APIEXT)

template <typename charT>
struct repoptions
{
	const charT *fmt_begin;
	const charT *fmt_end;
	bool global;

	repoptions(const charT *const b, const charT *const e, const bool g)
		: fmt_begin(b), fmt_end(e), global(g)
	{
	}
};

template <typename charT, typename ST, typename SA, typename BidiIter>
bool call_mrformat(std::basic_string<charT, ST, SA> &s, const match_results<BidiIter> &m, void *p)
{
	const repoptions<charT> *const opts = reinterpret_cast<const repoptions<charT> *>(p);

	m.format(std::back_inserter(s), opts->fmt_begin, opts->fmt_end);	//, flags);
	return opts->global;
}

template <typename charT, typename StringLike, typename iteratorTag>
iteratorTag pos0_(const StringLike &s, iteratorTag)
{
	return s.begin();
}
template <typename charT, typename StringLike>
const charT *pos0_(const StringLike &s, const charT *)
{
	return s.data();
}

template <typename charT, typename StringLike, typename iteratorTag>
iteratorTag pos1_(const StringLike &s, iteratorTag)
{
	return s.end();
}
template <typename charT, typename StringLike>
const charT *pos1_(const StringLike &s, const charT *)
{
	return s.data() + s.size();
}

#endif	//  !defined(SRELL_NO_APIEXT)

template <typename charT, typename traits>
class re_object : public re_compiler<charT, traits>
{
public:

	template <typename BidirectionalIterator, typename Allocator>
	bool search
	(
		const BidirectionalIterator begin,
		const BidirectionalIterator end,
		const BidirectionalIterator lookbehind_limit,
		match_results<BidirectionalIterator, Allocator> &results,
		const regex_constants::match_flag_type flags
	) const
	{
		int reason = 0;

		results.clear_();

		if (this->NFA_states.size())
		{
			re_search_state<BidirectionalIterator> &sstate = results.sstate_;

			sstate.init(begin, end, lookbehind_limit, flags);

#if !defined(SRELLDBG_NO_BMH)
			if (this->bmdata && !sstate.match_continuous_flag())
			{
#if !defined(SRELL_NO_ICASE)
				if (!this->is_ricase() ? this->bmdata->do_casesensitivesearch(sstate, typename std::iterator_traits<BidirectionalIterator>::iterator_category()) : this->bmdata->do_icasesearch(sstate, typename std::iterator_traits<BidirectionalIterator>::iterator_category()))
#else
				if (this->bmdata->do_casesensitivesearch(sstate, typename std::iterator_traits<BidirectionalIterator>::iterator_category()))
#endif	//  !defined(SRELL_NO_ICASE)
					return results.set_match_results_bmh_();

				goto NOT_FOUND;
			}
#endif	//  !defined(SRELLDBG_NO_BMH)

			sstate.init_for_automaton(this->number_of_brackets, this->number_of_counters, this->number_of_repeats);

			if (sstate.match_continuous_flag())
			{
				sstate.entry_state = this->NFA_states[0].next_state2;

				sstate.ssc.iter = sstate.nextpos;

#if defined(SRELL_NO_LIMIT_COUNTER)
				sstate.reset();
#else
				sstate.reset(this->limit_counter);
#endif
				reason = !this->is_ricase() ? run_automaton<false, false>(sstate) : run_automaton<true, false>(sstate);

				goto CHECK_REASON;
			}

			sstate.entry_state = this->NFA_states[0].next_state1;

#if !defined(SRELLDBG_NO_SCFINDER)
			if (this->NFA_states[0].char_num != constants::invalid_u32value)
			{
				reason = !this->is_ricase() ? do_search_sc<false>(sstate, typename std::iterator_traits<BidirectionalIterator>::iterator_category()) : do_search_sc<true>(sstate, typename std::iterator_traits<BidirectionalIterator>::iterator_category());

				goto CHECK_REASON;
			}
#endif	//  !defined(SRELLDBG_NO_SCFINDER)

#if !defined(SRELL_NO_ICASE)
			reason = !this->is_ricase() ? do_search<false>(sstate) : do_search<true>(sstate);
#else
			reason = do_search<false>(results);
#endif
			CHECK_REASON:
			if (reason == 1)
			{
#if !defined(SRELL_NO_NAMEDCAPTURE)
				return results.set_match_results_(this->namedcaptures);
#else
				return results.set_match_results_();
#endif
			}
		}
#if !defined(SRELLDBG_NO_BMH)
		NOT_FOUND:
#endif
		return results.mark_as_failed_(reason);
	}

private:

	typedef typename traits::utf_traits utf_traits;

	template <const bool icase, typename BidirectionalIterator>
	int do_search(re_search_state<BidirectionalIterator> &sstate) const
	{
		for (;;)
		{
			const bool final = sstate.nextpos == sstate.srchend;

			sstate.ssc.iter = sstate.nextpos;

			if (!final)
			{
#if defined(SRELLDBG_NO_1STCHRCLS)
				utf_traits::codepoint_inc(sstate.nextpos, sstate.srchend);
#else
	#if !defined(SRELLDBG_NO_BITSET)
				if (!this->firstchar_class_bs.test((*sstate.nextpos++) & utf_traits::bitsetmask))
	#else
				const ui_l32 firstchar = utf_traits::codepoint_inc(sstate.nextpos, sstate.srchend);

				if (!this->firstchar_class.is_included(firstchar))
	#endif
					continue;
#endif	//  defined(SRELLDBG_NO_1STCHRCLS)
			}
			//  Even when final == true, we have to try for such expressions
			//  as "" =~ /^$/ or "..." =~ /$/.

#if defined(SRELL_NO_LIMIT_COUNTER)
			sstate.reset(/* first */);
#else
			sstate.reset(/* first, */ this->limit_counter);
#endif
			const int reason = run_automaton<icase, false>(sstate /* , false */);
			if (reason)
				return reason;

			if (final)
				break;
		}
		return 0;
	}

#if !defined(SRELLDBG_NO_SCFINDER)

	template <const bool icase, typename ContiguousIterator>
	int do_search_sc(re_search_state<ContiguousIterator> &sstate, const std::random_access_iterator_tag) const
	{
		if (is_contiguous(sstate.srchbegin))
		{
			typedef typename std::iterator_traits<ContiguousIterator>::value_type char_type;
			const char_type ec = static_cast<char_type>(this->NFA_states[0].char_num);

			for (;;)
			{
				if (sstate.nextpos >= sstate.srchend)
					break;

				sstate.ssc.iter = sstate.nextpos;

				const char_type *const bgnpos = std::char_traits<char_type>::find(&*sstate.nextpos, sstate.srchend - sstate.nextpos, ec);

				if (bgnpos)
				{
//					sstate.ssc.iter = bgnpos;
					sstate.ssc.iter += bgnpos - &*sstate.nextpos;
//					sstate.nextpos = bgnpos + 1;
					sstate.nextpos = sstate.ssc.iter + 1;

#if defined(SRELL_NO_LIMIT_COUNTER)
					sstate.reset();
#else
					sstate.reset(this->limit_counter);
#endif
					const int reason = run_automaton<icase, false>(sstate);
					if (reason)
						return reason;
				}
				else
					break;
			}
			return 0;
		}
		return do_search_sc<icase>(sstate, std::bidirectional_iterator_tag());
	}

	template <const bool icase, typename BidirectionalIterator>
	int do_search_sc(re_search_state<BidirectionalIterator> &sstate, const std::bidirectional_iterator_tag) const
	{
		typedef typename std::iterator_traits<BidirectionalIterator>::value_type char_type;
		const char_type ec = static_cast<char_type>(this->NFA_states[0].char_num);

		for (; sstate.nextpos != sstate.srchend;)
		{
			sstate.ssc.iter = find(sstate.nextpos, sstate.srchend, ec);

			if (sstate.ssc.iter != sstate.srchend)
			{
				sstate.nextpos = sstate.ssc.iter;
				++sstate.nextpos;

#if defined(SRELL_NO_LIMIT_COUNTER)
				sstate.reset();
#else
				sstate.reset(this->limit_counter);
#endif
				const int reason = run_automaton<icase, false>(sstate);
				if (reason)
					return reason;
			}
			else
				break;
		}
		return 0;
	}

	template <typename BidirectionalIterator, typename CharT0>
	BidirectionalIterator find(BidirectionalIterator begin, const BidirectionalIterator end, const CharT0 c) const
	{
		for (; begin != end; ++begin)
			if ((*begin & utf_traits::bitsetmask) == (c & utf_traits::bitsetmask))
				break;

		return begin;
	}

	template <typename BidirectionalIterator>
	bool is_contiguous(BidirectionalIterator) const
	{
		return false;
	}

#if !defined(SRELL_NO_CONCEPTS) && defined(__cpp_concepts)

	template <std::contiguous_iterator I>
	bool is_contiguous(I) const
	{
		return true;
	}

#else

	bool is_contiguous(const charT *) const
	{
		return true;
	}

	bool is_contiguous(typename std::basic_string<charT>::const_iterator) const
	{
		return true;
	}
#endif	//  !defined(SRELL_NO_CONCEPTS) && defined(__cpp_concepts)
#endif	//  !defined(SRELLDBG_NO_SCFINDER)

	template <typename T, const bool>
	struct casehelper
	{
		static T canonicalise(const T t)
		{
			return t;
		}
	};

	template <typename T>
	struct casehelper<T, true>
	{
		static T canonicalise(const T t)
		{
			return unicode_case_folding::do_casefolding(t);
		}
	};

	template <const bool icase, const bool reverse, typename BidirectionalIterator>
	int run_automaton
	(
		re_search_state</*charT, */BidirectionalIterator> &sstate
	) const
	{
		typedef casehelper<ui_l32, icase> casehelper_type;
		typedef typename re_object_core<charT, traits>::state_type state_type;
		typedef re_search_state</*charT, */BidirectionalIterator> ss_type;
//		typedef typename ss_type::search_state_core ssc_type;
		typedef typename ss_type::submatch_type submatch_type;
		typedef typename ss_type::submatchcore_type submatchcore_type;
		typedef typename ss_type::counter_type counter_type;
		typedef typename ss_type::position_type position_type;
		ui_l32 is_matched;

		goto START;

		JUDGE:
		if (is_matched)
		{
			MATCHED:
			sstate.ssc.state = sstate.ssc.state->next_state1;
		}
		else
		{
			NOT_MATCHED:

#if !defined(SRELL_NO_LIMIT_COUNTER)
			if (--sstate.failure_counter)
			{
#endif
				if (sstate.bt_stack.size() > sstate.btstack_size)
				{
					sstate.ssc = sstate.bt_stack.back();
					sstate.bt_stack.pop_back();

					sstate.ssc.state = sstate.ssc.state->next_state2;
				}
				else
					return 0;

#if !defined(SRELL_NO_LIMIT_COUNTER)
			}
			else
#if defined(SRELL_NO_THROW)
				return static_cast<int>(regex_constants::error_complexity);
#else
				throw regex_error(regex_constants::error_complexity);
#endif
#endif
		}

//		START:
		for (;;)
		{
			START:

			switch (sstate.ssc.state->type)
			{
			case st_character:

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(push)
#pragma warning(disable:4127)
#endif
				if (!reverse)
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(pop)
#endif
				{
					if (!sstate.is_at_srchend())
					{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						const BidirectionalIterator prevpos = sstate.ssc.iter;
#endif
						const ui_l32 uchar = casehelper_type::canonicalise(utf_traits::codepoint_inc(sstate.ssc.iter, sstate.srchend));
						RETRY_CF:

						if (sstate.ssc.state->char_num == uchar)
							goto MATCHED;

#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						if (sstate.ssc.state->next_state2)
						{
							sstate.ssc.state = sstate.ssc.state->next_state2;

							if (sstate.ssc.state->type == st_character)
								goto RETRY_CF;

							sstate.ssc.iter = prevpos;
							continue;
						}
#endif
					}
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
					else if (sstate.ssc.state->next_state2)
					{
						sstate.ssc.state = sstate.ssc.state->next_state2;
						continue;
					}
#endif
				}
				else	//  reverse == true.
				{
					if (!sstate.is_at_lookbehindlimit())
					{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						const BidirectionalIterator prevpos = sstate.ssc.iter;
#endif
						const ui_l32 uchar = casehelper_type::canonicalise(utf_traits::dec_codepoint(sstate.ssc.iter, sstate.lblim));
						RETRY_CB:

						if (sstate.ssc.state->char_num == uchar)
							goto MATCHED;

#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						if (sstate.ssc.state->next_state2)
						{
							sstate.ssc.state = sstate.ssc.state->next_state2;

							if (sstate.ssc.state->type == st_character)
								goto RETRY_CB;

							sstate.ssc.iter = prevpos;
							continue;
						}
#endif
					}
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
					else if (sstate.ssc.state->next_state2)
					{
						sstate.ssc.state = sstate.ssc.state->next_state2;
						continue;
					}
#endif
				}
				goto NOT_MATCHED;

			case st_character_class:

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(push)
#pragma warning(disable:4127)
#endif
				if (!reverse)
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(pop)
#endif
				{
					if (!sstate.is_at_srchend())
					{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						const BidirectionalIterator prevpos = sstate.ssc.iter;
#endif
						const ui_l32 uchar = utf_traits::codepoint_inc(sstate.ssc.iter, sstate.srchend);
//						RETRY_CCF:

#if !defined(SRELLDBG_NO_CCPOS)
						if (this->character_class.is_included(sstate.ssc.state->quantifier.atleast, sstate.ssc.state->quantifier.atmost, uchar))
#else
						if (this->character_class.is_included(sstate.ssc.state->char_num, uchar))
#endif
							goto MATCHED;

#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						if (sstate.ssc.state->next_state2)
						{
							sstate.ssc.state = sstate.ssc.state->next_state2;

//							if (sstate.ssc.state->type == st_character_class)
//								goto RETRY_CCF;

							sstate.ssc.iter = prevpos;
							continue;
						}
#endif
					}
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
					else if (sstate.ssc.state->next_state2)
					{
						sstate.ssc.state = sstate.ssc.state->next_state2;
						continue;
					}
#endif
				}
				else	//  reverse == true.
				{
					if (!sstate.is_at_lookbehindlimit())
					{
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						const BidirectionalIterator prevpos = sstate.ssc.iter;
#endif
						const ui_l32 uchar = utf_traits::dec_codepoint(sstate.ssc.iter, sstate.lblim);
//						RETRY_CCB:

#if !defined(SRELLDBG_NO_CCPOS)
						if (this->character_class.is_included(sstate.ssc.state->quantifier.atleast, sstate.ssc.state->quantifier.atmost, uchar))
#else
						if (this->character_class.is_included(sstate.ssc.state->char_num, uchar))
#endif
							goto MATCHED;

#if !defined(SRELLDBG_NO_ASTERISK_OPT)
						if (sstate.ssc.state->next_state2)
						{
							sstate.ssc.state = sstate.ssc.state->next_state2;

//							if (sstate.ssc.state->type == st_character_class)
//								goto RETRY_CCB;

							sstate.ssc.iter = prevpos;
							continue;
						}
#endif
					}
#if !defined(SRELLDBG_NO_ASTERISK_OPT)
					else if (sstate.ssc.state->next_state2)
					{
						sstate.ssc.state = sstate.ssc.state->next_state2;
						continue;
					}
#endif
				}
				goto NOT_MATCHED;

			case st_epsilon:

#if defined(SRELLDBG_NO_SKIP_EPSILON)
				if (sstate.ssc.state->next_state2)
#endif
				{
					sstate.bt_stack.push_back(sstate.ssc);	//	sstate.push();
				}

				sstate.ssc.state = sstate.ssc.state->next_state1;
				continue;

			default:
				switch (sstate.ssc.state->type)
				{

			case st_check_counter:
				{
					ST_CHECK_COUNTER:
					ui_l32 &counter = sstate.counter[sstate.ssc.state->char_num];

					if (counter < sstate.ssc.state->quantifier.atmost)
					{
						++counter;

						if (counter > sstate.ssc.state->quantifier.atleast)
						{
							LOOP_WITHOUT_INCREMENT:	//  counter >= atmost && atmost == infinity.

							sstate.bt_stack.push_back(sstate.ssc);
							sstate.ssc.state = sstate.ssc.state->next_state1;
						}
						else
						{
							sstate.ssc.state
								= sstate.ssc.state->quantifier.is_greedy
								? sstate.ssc.state->next_state1
								: sstate.ssc.state->next_state2;
						}
					}
					else
					{
						if (sstate.ssc.state->quantifier.is_infinity())
							goto LOOP_WITHOUT_INCREMENT;

						sstate.ssc.state
							= sstate.ssc.state->quantifier.is_greedy
							? sstate.ssc.state->next_state2
							: sstate.ssc.state->next_state1;
					}
				}
				continue;

			case st_decrement_counter:
				--sstate.counter[sstate.ssc.state->char_num];
				goto NOT_MATCHED;

			case st_save_and_reset_counter:
				{
					counter_type &c = sstate.counter[sstate.ssc.state->char_num];

					sstate.counter_stack.push_back(c);
					sstate.bt_stack.push_back(sstate.ssc);
					c = 0;
				}
				sstate.ssc.state = sstate.ssc.state->next_state1;
				goto ST_CHECK_COUNTER;

			case st_restore_counter:
				sstate.counter[sstate.ssc.state->char_num] = sstate.counter_stack.back();
				sstate.counter_stack.pop_back();
				goto NOT_MATCHED;

			case st_roundbracket_open:	//  '(':
				{
					submatch_type &bracket = sstate.bracket[sstate.ssc.state->char_num];

					++bracket.counter;
					sstate.capture_stack.push_back(bracket.core);

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(push)
#pragma warning(disable:4127)
#endif
					if (!reverse)
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(pop)
#endif
					{
						bracket.core.open_at = sstate.ssc.iter;
					}
					else
						bracket.core.close_at = sstate.ssc.iter;

					for (ui_l32 brno = sstate.ssc.state->quantifier.atleast; brno <= sstate.ssc.state->quantifier.atmost; ++brno)
					{
						submatch_type &inner_bracket = sstate.bracket[brno];

						sstate.capture_stack.push_back(inner_bracket.core);
						sstate.counter_stack.push_back(inner_bracket.counter);
						inner_bracket.core.open_at = inner_bracket.core.close_at = sstate.srchend;
						inner_bracket.counter = 0;
						//  ECMAScript spec (3-5.1) 15.10.2.5, NOTE 3.
						//  ECMAScript 2018 (ES9) 21.2.2.5.1, Note 3.
					}

					sstate.bt_stack.push_back(sstate.ssc);
				}
				goto MATCHED;

			case st_roundbracket_pop:	//  '/':
				{
					for (ui_l32 brno = sstate.ssc.state->quantifier.atmost; brno >= sstate.ssc.state->quantifier.atleast; --brno)
					{
						submatch_type &inner_bracket = sstate.bracket[brno];

						inner_bracket.counter = sstate.counter_stack.back();
						inner_bracket.core = sstate.capture_stack.back();
						sstate.counter_stack.pop_back();
						sstate.capture_stack.pop_back();
					}

					submatch_type &bracket = sstate.bracket[sstate.ssc.state->char_num];

					bracket.core = sstate.capture_stack.back();
					sstate.capture_stack.pop_back();
					--bracket.counter;
				}
				goto NOT_MATCHED;

			case st_roundbracket_close:	//  ')':
				{
					submatch_type &bracket = sstate.bracket[sstate.ssc.state->char_num];
					submatchcore_type &brc = bracket.core;

					if ((!reverse ? brc.open_at : brc.close_at) != sstate.ssc.iter)
					{
						sstate.ssc.state = sstate.ssc.state->next_state1;
					}
					else	//  0 width match, breaks from the loop.
					{
						if (sstate.ssc.state->next_state1->type != st_check_counter)
						{
							if (bracket.counter > 1)
								goto NOT_MATCHED;	//  ECMAScript spec 15.10.2.5, note 4.

							sstate.ssc.state = sstate.ssc.state->next_state2;
								//  Accepts 0 width match and exits.
						}
						else
						{
							//  A pair with check_counter.
							const counter_type counter = sstate.counter[sstate.ssc.state->next_state1->char_num];

							if (counter > sstate.ssc.state->next_state1->quantifier.atleast)
								goto NOT_MATCHED;	//  Takes a captured string in the previous loop.

							sstate.ssc.state = sstate.ssc.state->next_state1;
								//  Accepts 0 width match and continues.
						}
					}
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(push)
#pragma warning(disable:4127)
#endif
					if (!reverse)
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(pop)
#endif
					{
						brc.close_at = sstate.ssc.iter;
					}
					else	//  reverse == true.
					{
						brc.open_at  = sstate.ssc.iter;
					}
				}
				continue;

			case st_repeat_in_push:
				{
					position_type &r = sstate.repeat[sstate.ssc.state->char_num];

					sstate.repeat_stack.push_back(r);
					r = sstate.ssc.iter;

					for (ui_l32 brno = sstate.ssc.state->quantifier.atleast; brno <= sstate.ssc.state->quantifier.atmost; ++brno)
					{
						submatch_type &inner_bracket = sstate.bracket[brno];

						sstate.capture_stack.push_back(inner_bracket.core);
						sstate.counter_stack.push_back(inner_bracket.counter);
						inner_bracket.core.open_at = inner_bracket.core.close_at = sstate.srchend;
						inner_bracket.counter = 0;
						//  ECMAScript 2019 (ES10) 21.2.2.5.1, Note 3.
					}
					sstate.bt_stack.push_back(sstate.ssc);
				}
				goto MATCHED;

			case st_repeat_in_pop:
				for (ui_l32 brno = sstate.ssc.state->quantifier.atmost; brno >= sstate.ssc.state->quantifier.atleast; --brno)
				{
					submatch_type &inner_bracket = sstate.bracket[brno];

					inner_bracket.counter = sstate.counter_stack.back();
					inner_bracket.core = sstate.capture_stack.back();
					sstate.counter_stack.pop_back();
					sstate.capture_stack.pop_back();
				}

				sstate.repeat[sstate.ssc.state->char_num] = sstate.repeat_stack.back();
				sstate.repeat_stack.pop_back();
				goto NOT_MATCHED;

			case st_check_0_width_repeat:
				if (sstate.ssc.iter != sstate.repeat[sstate.ssc.state->char_num])
					goto MATCHED;

				if (sstate.ssc.state->next_state1->type == st_check_counter)
				{
					const counter_type counter = sstate.counter[sstate.ssc.state->next_state1->char_num];

					if (counter > sstate.ssc.state->next_state1->quantifier.atleast)
						goto NOT_MATCHED;

					sstate.ssc.state = sstate.ssc.state->next_state1;
				}
				else
					sstate.ssc.state = sstate.ssc.state->next_state2;

				continue;

			case st_backreference:	//  '\\':
				{
					const submatch_type &bracket = sstate.bracket[sstate.ssc.state->char_num];

					if (bracket.counter == 0)	//  Undefined.
					{
						ESCAPE_FROM_ZERO_WIDTH_MATCH:
						sstate.ssc.state = sstate.ssc.state->next_state2;
						continue;
					}

					const submatchcore_type &brc = bracket.core;

					if (brc.open_at == brc.close_at)
						goto ESCAPE_FROM_ZERO_WIDTH_MATCH;

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(push)
#pragma warning(disable:4127)
#endif
					if (!reverse)
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(pop)
#endif
					{
						BidirectionalIterator backrefpos = brc.open_at;

						if (!sstate.ssc.state->icase_backrefno_unresolved)	//  !icase.
						{
							for (; backrefpos != brc.close_at;)
							{
								if (sstate.is_at_srchend() || *sstate.ssc.iter++ != *backrefpos++)
									goto NOT_MATCHED;
							}
						}
						else	//  icase.
						{
							for (; backrefpos != brc.close_at;)
							{
								if (!sstate.is_at_srchend())
								{
									const ui_l32 uchartxt = utf_traits::codepoint_inc(sstate.ssc.iter, sstate.srchend);
									const ui_l32 ucharref = utf_traits::codepoint_inc(backrefpos, brc.close_at);

									if (unicode_case_folding::do_casefolding(uchartxt) == unicode_case_folding::do_casefolding(ucharref))
										continue;
								}
								goto NOT_MATCHED;
							}
						}
					}
					else	//  reverse == true.
					{
						BidirectionalIterator backrefpos = brc.close_at;

						if (!sstate.ssc.state->icase_backrefno_unresolved)	//  !icase.
						{
							for (; backrefpos != brc.open_at;)
							{
								if (sstate.is_at_lookbehindlimit() || *--sstate.ssc.iter != *--backrefpos)
									goto NOT_MATCHED;
							}
						}
						else	//  icase.
						{
							for (; backrefpos != brc.open_at;)
							{
								if (!sstate.is_at_lookbehindlimit())
								{
									const ui_l32 uchartxt = utf_traits::dec_codepoint(sstate.ssc.iter, sstate.lblim);
									const ui_l32 ucharref = utf_traits::dec_codepoint(backrefpos, brc.open_at);

									if (unicode_case_folding::do_casefolding(uchartxt) == unicode_case_folding::do_casefolding(ucharref))
										continue;
								}
								goto NOT_MATCHED;
							}
						}
					}
				}
				goto MATCHED;

			case st_lookaround_open:
				{
					const state_type *const lostate = sstate.ssc.state;

					for (ui_l32 i = 1; i < this->number_of_brackets; ++i)
					{
						const submatch_type &sm = sstate.bracket[i];
						sstate.capture_stack.push_back(sm.core);
						sstate.counter_stack.push_back(sm.counter);
					}

					for (ui_l32 i = 0; i < this->number_of_counters; ++i)
						sstate.counter_stack.push_back(sstate.counter[i]);

					for (ui_l32 i = 0; i < this->number_of_repeats; ++i)
						sstate.repeat_stack.push_back(sstate.repeat[i]);

					const typename ss_type::bottom_state backup_bottom(sstate.btstack_size, sstate.capture_stack.size(), sstate.counter_stack.size(), sstate.repeat_stack.size());
					const BidirectionalIterator orgpos = sstate.ssc.iter;

					sstate.btstack_size = sstate.bt_stack.size();

#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND) && !defined(SRELLDBG_NO_MPREWINDER)
					if (lostate->quantifier.atleast >= 2)
					{
						sstate.repeat_stack.push_back(sstate.lblim);
						sstate.lblim = sstate.srchbegin;
					}
#endif

#if defined(SRELL_FIXEDWIDTHLOOKBEHIND)

//					if (lostate->reverse)
					{
						for (ui_l32 i = 0; i < lostate->quantifier.atleast; ++i)
						{
							if (!sstate.is_at_lookbehindlimit())
							{
								utf_traits::dec_codepoint(sstate.ssc.iter, sstate.lblim);
								continue;
							}
							is_matched = false;
							goto AFTER_LOOKAROUND;
						}
					}
#endif
					sstate.ssc.state = lostate->next_state2;

					//  sstate.ssc.state is no longer pointing to lookaround_open!

#if defined(SRELL_NO_THROW)
					const int reason =
#else
					is_matched =
#endif
#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND)
						(lostate->quantifier.atleast == 0 ? run_automaton<icase, false>(sstate) : run_automaton<icase, true>(sstate));
#else
						run_automaton<icase, false>(sstate);
#endif

#if defined(SRELL_NO_THROW)
					if (reason & ~1)
						return reason;

					is_matched = reason ? 1 : 0;
#endif

#if defined(SRELL_FIXEDWIDTHLOOKBEHIND)
					AFTER_LOOKAROUND:
#endif

#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND) && !defined(SRELLDBG_NO_MPREWINDER)
					if (lostate->quantifier.atleast >= 2)
					{
						sstate.lblim = sstate.repeat_stack[backup_bottom.repeatstack_size];
						if (is_matched)
							sstate.bracket[0].core.open_at = sstate.ssc.iter;
					}
#endif

#if defined(SRELL_ENABLE_GT)
					if (lostate->char_num != meta_char::mc_gt)	//  '>'
#endif
					{
#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND) && !defined(SRELLDBG_NO_MPREWINDER)
						if (lostate->quantifier.atleast < 3)
#endif
							sstate.ssc.iter = orgpos;
					}
					sstate.bt_stack.resize(sstate.btstack_size);

					sstate.btstack_size = backup_bottom.btstack_size;
					sstate.capture_stack.resize(backup_bottom.capturestack_size);
					sstate.counter_stack.resize(backup_bottom.counterstack_size);
					sstate.repeat_stack.resize(backup_bottom.repeatstack_size);

					is_matched ^= lostate->is_not;

					if (is_matched)
					{
#if !defined(SRELL_FIXEDWIDTHLOOKBEHIND) && !defined(SRELLDBG_NO_MPREWINDER)
						if (lostate->quantifier.atleast == 3)
							sstate.ssc.state = this->NFA_states[0].next_state2;
						else
#endif
							sstate.ssc.state = lostate->next_state1;
						continue;
					}
				}

//			case st_lookaround_pop:
				for (ui_l32 i = this->number_of_repeats; i;)
				{
					sstate.repeat[--i] = sstate.repeat_stack.back();
					sstate.repeat_stack.pop_back();
				}

				for (ui_l32 i = this->number_of_counters; i;)
				{
					sstate.counter[--i] = sstate.counter_stack.back();
					sstate.counter_stack.pop_back();
				}

				for (ui_l32 i = this->number_of_brackets; i > 1;)
				{
					submatch_type &sm = sstate.bracket[--i];

					sm.counter = sstate.counter_stack.back();
					sm.core = sstate.capture_stack.back();
					sstate.counter_stack.pop_back();
					sstate.capture_stack.pop_back();
				}
				goto NOT_MATCHED;

			case st_bol:	//  '^':
				if (sstate.is_at_lookbehindlimit() && !sstate.is_prev_avail())
				{
					if (!sstate.match_not_bol_flag())
						goto MATCHED;
				}
					//  !sstate.is_at_lookbehindlimit() || sstate.match_prev_avail_flag()
				else if (sstate.ssc.state->multiline)
				{
					const ui_l32 prevchar = utf_traits::prevcodepoint(sstate.ssc.iter, sstate.reallblim);

#if !defined(SRELLDBG_NO_CCPOS)
					if (this->character_class.is_included(sstate.ssc.state->quantifier.atleast, sstate.ssc.state->quantifier.atmost, prevchar))
#else
					if (this->character_class.is_included(re_character_class::newline, prevchar))
#endif
						goto MATCHED;
				}
				goto NOT_MATCHED;

			case st_eol:	//  '$':
				if (sstate.is_at_srchend())
				{
					if (!sstate.match_not_eol_flag())
						goto MATCHED;
				}
				else if (sstate.ssc.state->multiline)
				{
					const ui_l32 nextchar = utf_traits::codepoint(sstate.ssc.iter, sstate.srchend);

#if !defined(SRELLDBG_NO_CCPOS)
					if (this->character_class.is_included(sstate.ssc.state->quantifier.atleast, sstate.ssc.state->quantifier.atmost, nextchar))
#else
					if (this->character_class.is_included(re_character_class::newline, nextchar))
#endif
						goto MATCHED;
				}
				goto NOT_MATCHED;

			case st_boundary:	//  '\b' '\B'
				is_matched = sstate.ssc.state->is_not;
//				is_matched = sstate.ssc.state->char_num == char_alnum::ch_B;

				//  First, suppose the previous character is not \w but \W.

				if (sstate.is_at_srchend())
				{
					if (sstate.match_not_eow_flag())
						is_matched = is_matched ? 0u : 1u;
				}
				else
				{
#if !defined(SRELLDBG_NO_CCPOS)
					if (this->character_class.is_included(sstate.ssc.state->quantifier.atleast, sstate.ssc.state->quantifier.atmost, utf_traits::codepoint(sstate.ssc.iter, sstate.srchend)))
#else
					if (this->character_class.is_included(sstate.ssc.state->char_num, utf_traits::codepoint(sstate.ssc.iter, sstate.srchend)))
#endif
					{
						is_matched = is_matched ? 0u : 1u;
					}
				}
				//      \W/last     \w
				//  \b  false       true
				//  \B  true        false

				//  Second, if the actual previous character is \w, flip is_matched.

				if (sstate.is_at_lookbehindlimit() && !sstate.is_prev_avail())
				{
					if (sstate.match_not_bow_flag())
						is_matched = is_matched ? 0u : 1u;
				}
				else
				{
					//  !sstate.is_at_lookbehindlimit() || sstate.match_prev_avail_flag()
#if !defined(SRELLDBG_NO_CCPOS)
					if (this->character_class.is_included(sstate.ssc.state->quantifier.atleast, sstate.ssc.state->quantifier.atmost, utf_traits::prevcodepoint(sstate.ssc.iter, sstate.reallblim)))
#else
					if (this->character_class.is_included(sstate.ssc.state->char_num, utf_traits::prevcodepoint(sstate.ssc.iter, sstate.reallblim)))
#endif
					{
						is_matched = is_matched ? 0u : 1u;
					}
				}
				//  \b                          \B
				//  pre cur \W/last \w          pre cur \W/last \w
				//  \W/base false   true        \W/base true    false
				//  \w      true    false       \w      false   true

				goto JUDGE;

			case st_success:	//  == lookaround_close.
//				if (is_recursive)
				if (sstate.btstack_size)
					return 1;

				if
				(
					(!sstate.match_not_null_flag() || !sstate.is_null())
					&&
					(!sstate.match_match_flag() || sstate.is_at_srchend())
				)
					return 1;

				goto NOT_MATCHED;

#if defined(SRELLTEST_NEXTPOS_OPT)
			case st_move_nextpos:
#if !defined(SRELLDBG_NO_1STCHRCLS) && !defined(SRELLDBG_NO_BITSET)
				sstate.nextpos = sstate.ssc.iter;
				if (!sstate.is_at_srchend())
					++sstate.nextpos;
#else	//  defined(SRELLDBG_NO_1STCHRCLS) || defined(SRELLDBG_NO_BITSET)
				if (sstate.ssc.iter != sstate.bracket[0].core.open_at)
				{
					sstate.nextpos = sstate.ssc.iter;
					if (!sstate.is_at_srchend())
						utf_traits::codepoint_inc(sstate.nextpos, sstate.srchend);
				}
#endif
				goto MATCHED;
#endif

			default:
				//  Reaching here means that this->NFA_states is corrupted.
#if defined(SRELL_NO_THROW)
				return static_cast<int>(regex_constants::error_internal);
#else
				throw regex_error(regex_constants::error_internal);
#endif
				}
			}
		}
	}

#if !defined(SRELL_NO_APIEXT)

protected:

	template <typename StringLike, typename ST, typename SA, typename RAIter, typename MA>
	void do_replace(
		StringLike &s,
		bool (*repfunc)(std::basic_string<charT, ST, SA> &, const match_results<RAIter, MA> &, void *),
		void *ptr
	) const
	{
		typedef std::basic_string<charT, ST, SA> string_type;
		typedef typename string_type::size_type size_type;
		typedef typename traits::utf_traits utf_traits;
		typedef match_results<RAIter, MA> match_type;
		regex_constants::match_flag_type flags = regex_constants::match_default;
		string_type subst;
		match_type match;
		size_type offset = 0;
		size_type prevend = offset;

		for (;;)
		{
			if (!this->search(pos0_<charT>(s, RAIter()) + offset, pos1_<charT>(s, RAIter()), pos0_<charT>(s, RAIter()), match, flags))
				break;

			const typename match_type::size_type matchlen = match.length(0);

			match.update_prefix1_(pos0_<charT>(s, RAIter()) + prevend);
			offset = match[0].second - pos0_<charT>(s, RAIter());

			const bool continuable = repfunc(subst, match, ptr);

			s.replace(match[0].first - pos0_<charT>(s, RAIter()), matchlen, subst);
			if (!continuable)
				break;

			offset += subst.size() - matchlen;
			prevend = offset;

			if (matchlen == 0)
			{
				if (offset == s.size())
				{
					break;
				}
				else
				{
					RAIter it = pos0_<charT>(s, RAIter()) + offset;

					utf_traits::codepoint_inc(it, pos1_<charT>(s, RAIter()));
					offset = it - pos0_<charT>(s, RAIter());
				}
			}
			subst.clear();
			flags |= regex_constants::match_prev_avail;
		}
	}

	template <typename BidiIter>
	struct submatch_helper : public sub_match<BidiIter>
	{
		submatch_helper(const BidiIter f, const BidiIter s, const bool m = true)
		{
			this->first = f;
			this->second = s;
			this->matched = m;
		}
	};

	template <typename MatchResults, typename container, typename BidiIter>
	void do_split(
		container &c,
		const BidiIter begin,
		const BidiIter end,
		const std::size_t limit /* = -1 */
	) const
	{
		typedef typename traits::utf_traits utf_traits;
		typedef MatchResults match_type;
		typedef submatch_helper<BidiIter> helper;
		regex_constants::match_flag_type flags = regex_constants::match_default;
		BidiIter offset = begin;
		BidiIter prevend = offset;
		std::size_t count = 0;
		match_type match;

		if (limit == 0)	//  22.2.5.14 RegExp.prototype [ @@split ] ( string, limit ), step 14:
			return;

		if (offset == end)	//  22.2.5.14 RegExp.prototype [ @@split ] ( string, limit ), step 16:
		{
			if (!this->search(offset, end, begin, match, flags))
				c.push_back(helper(begin, end));

			return;
		}

		for (; offset < end;)
		{
			if (!this->search(offset, end, begin, match, flags) || match[0].first == end)
				break;

			if (match[0].second != prevend)
			{
				if (++count == limit)
					break;
				c.push_back(helper(prevend, match[0].first));

				prevend = match[0].second;

				for (typename match_type::size_type i = 1; i < match.size(); ++i)
				{
					if (++count == limit)
						goto FINAL_PUSH;
					c.push_back(match[i]);
				}

				offset = prevend;
			}
			else
			{
				utf_traits::codepoint_inc(offset, end);
			}
			flags |= regex_constants::match_prev_avail;
		}

		FINAL_PUSH:
		c.push_back(helper(prevend, end));
	}

#endif	//  !defined(SRELL_NO_APIEXT)
};
//  re_object

	}	//  namespace re_detail

//  ... "rei_algorithm.hpp"]
//  ["basic_regex.hpp" ...

//  28.8, class template basic_regex:
template <class charT, class traits = regex_traits<charT> >
class basic_regex : public re_detail::re_object<charT, traits>
{
public:

	//  Types:
	typedef charT value_type;
	typedef traits traits_type;
	typedef typename traits::string_type string_type;
	typedef regex_constants::syntax_option_type flag_type;
	typedef typename traits::locale_type locale_type;

	//  28.8.1, constants:
	//  [7.8.1] constants
	static const regex_constants::syntax_option_type icase = regex_constants::icase;
	static const regex_constants::syntax_option_type nosubs = regex_constants::nosubs;
	static const regex_constants::syntax_option_type optimize = regex_constants::optimize;
	static const regex_constants::syntax_option_type collate = regex_constants::collate;
	static const regex_constants::syntax_option_type ECMAScript = regex_constants::ECMAScript;
	static const regex_constants::syntax_option_type basic = regex_constants::basic;
	static const regex_constants::syntax_option_type extended = regex_constants::extended;
	static const regex_constants::syntax_option_type awk = regex_constants::awk;
	static const regex_constants::syntax_option_type grep = regex_constants::grep;
	static const regex_constants::syntax_option_type egrep = regex_constants::egrep;
	static const regex_constants::syntax_option_type multiline = regex_constants::multiline;

	static const regex_constants::syntax_option_type dotall = regex_constants::dotall;
	static const regex_constants::syntax_option_type unicodesets = regex_constants::unicodesets;

	//  28.8.2, construct/copy/destroy:
	//  [7.8.2] construct/copy/destroy
	basic_regex()
	{
	}

	explicit basic_regex(const charT *const p, const flag_type f = regex_constants::ECMAScript)
	{
		assign(p, p + std::char_traits<charT>::length(p), f);
	}

	basic_regex(const charT *const p, const std::size_t len, const flag_type f = regex_constants::ECMAScript)
	{
		assign(p, p + len, f);
	}

	basic_regex(const basic_regex &e)
	{
		assign(e);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	basic_regex(basic_regex &&e) SRELL_NOEXCEPT
	{
		assign(std::move(e));
	}
#endif

	template <class ST, class SA>
	explicit basic_regex(const std::basic_string<charT, ST, SA> &p, const flag_type f = regex_constants::ECMAScript)
	{
		assign(p, f);
	}

	template <class ForwardIterator>
	basic_regex(ForwardIterator first, ForwardIterator last, const flag_type f = regex_constants::ECMAScript)
	{
		assign(first, last, f);
	}

#if defined(SRELL_CPP11_INITIALIZER_LIST_ENABLED)
	basic_regex(std::initializer_list<charT> il, const flag_type f = regex_constants::ECMAScript)
	{
		assign(il, f);
	}
#endif

//	~basic_regex();

	basic_regex &operator=(const basic_regex &right)
	{
		return assign(right);
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	basic_regex &operator=(basic_regex &&e) SRELL_NOEXCEPT
	{
		return assign(std::move(e));
	}
#endif

	basic_regex &operator=(const charT *const ptr)
	{
		return assign(ptr);
	}

#if defined(SRELL_CPP11_INITIALIZER_LIST_ENABLED)
	basic_regex &operator=(std::initializer_list<charT> il)
	{
		return assign(il.begin(), il.end());
	}
#endif

	template <class ST, class SA>
	basic_regex &operator=(const std::basic_string<charT, ST, SA> &p)
	{
		return assign(p);
	}

	//  28.8.3, assign:
	//  [7.8.3] assign
	basic_regex &assign(const basic_regex &right)
	{
		re_detail::re_object_core<charT, traits>::operator=(right);
		return *this;
	}

#if defined(SRELL_CPP11_MOVE_ENABLED)
	basic_regex &assign(basic_regex &&right) SRELL_NOEXCEPT
	{
		re_detail::re_object_core<charT, traits>::operator=(std::move(right));
		return *this;
	}
#endif

	basic_regex &assign(const charT *const ptr, const flag_type f = regex_constants::ECMAScript)
	{
		return assign(ptr, ptr + std::char_traits<charT>::length(ptr), f);
	}

	basic_regex &assign(const charT *const p, std::size_t len, const flag_type f = regex_constants::ECMAScript)
	{
		return assign(p, p + len, f);
	}

	template <class string_traits, class A>
	basic_regex &assign(const std::basic_string<charT, string_traits, A> &s, const flag_type f = regex_constants::ECMAScript)
	{
		return assign(s.c_str(), s.c_str() + s.size(), f);
	}

	template <class InputIterator>
	basic_regex &assign(InputIterator first, InputIterator last, const flag_type f = regex_constants::ECMAScript)
	{
#if defined(SRELL_STRICT_IMPL)
		basic_regex tmp;
		tmp.compile(first, last, f);
		tmp.swap(*this);
#else
		this->compile(first, last, f);
#endif
		return *this;
	}

#if defined(SRELL_CPP11_INITIALIZER_LIST_ENABLED)
	basic_regex &assign(std::initializer_list<charT> il, const flag_type f = regex_constants::ECMAScript)
	{
		return assign(il.begin(), il.end(), f);
	}
#endif

	//  28.8.4, const operations:
	//  [7.8.4] const operations
	unsigned mark_count() const
	{
		return this->number_of_brackets - 1;
	}

	flag_type flags() const
	{
		return this->soflags;
	}

	//  28.8.5, locale:
	//  [7.8.5] locale
	locale_type imbue(locale_type loc)
	{
		return this->traits_inst.imbue(loc);
	}

	locale_type getloc() const
	{
		return this->traits_inst.getloc();
	}

	//  28.8.6, swap:
	//  [7.8.6] swap
	void swap(basic_regex &e)
	{
		re_detail::re_object_core<charT, traits>::swap(e);
	}

	regex_constants::error_type ecode() const
	{
		return re_detail::re_object_core<charT, traits>::ecode();
	}

#if !defined(SRELL_NO_APIEXT)

	template <typename BidirectionalIterator, typename Allocator>
	bool match(
		const BidirectionalIterator begin,
		const BidirectionalIterator end,
		match_results<BidirectionalIterator, Allocator> &m,
		const regex_constants::match_flag_type flags = regex_constants::match_default
	) const
	{
		return base_type::search(begin, end, begin, m, flags | regex_constants::match_continuous | regex_constants::match_match_);
	}

	template <typename BidirectionalIterator>
	bool match(
		const BidirectionalIterator begin,
		const BidirectionalIterator end,
		const regex_constants::match_flag_type flags = regex_constants::match_default
	) const
	{
		match_results<BidirectionalIterator> m;
		return this->match(begin, end, m, flags);
	}

	template <typename Allocator>
	bool match(
		const charT *const str,
		match_results<const charT *, Allocator> &m,
		const regex_constants::match_flag_type flags = regex_constants::match_default
	) const
	{
		return this->match(str, str + std::char_traits<charT>::length(str), m, flags);
	}

	bool match(
		const charT *const str,
		const regex_constants::match_flag_type flags = regex_constants::match_default
	) const
	{
		return this->match(str, str + std::char_traits<charT>::length(str), flags);
	}

	template <typename ST, typename SA, typename MA>
	bool match(
		const std::basic_string<charT, ST, SA> &s,
		match_results<typename std::basic_string<charT, ST, SA>::const_iterator, MA> &m,
		const regex_constants::match_flag_type flags = regex_constants::match_default
	) const
	{
		return this->match(s.begin(), s.end(), m, flags);
	}

	template <typename ST, typename SA>
	bool match(
		const std::basic_string<charT, ST, SA> &s,
		const regex_constants::match_flag_type flags = regex_constants::match_default
	) const
	{
		return this->match(s.begin(), s.end(), flags);
	}

	template <typename BidirectionalIterator, typename Allocator>
	bool search(
		const BidirectionalIterator begin,
		const BidirectionalIterator end,
		const BidirectionalIterator lookbehind_limit,
		match_results<BidirectionalIterator, Allocator> &m,
		const regex_constants::match_flag_type flags = regex_constants::match_default
	) const
	{
		return base_type::search(begin, end, lookbehind_limit, m, flags);
	}

	template <typename BidirectionalIterator>
	bool search(
		const BidirectionalIterator begin,
		const BidirectionalIterator end,
		const BidirectionalIterator lookbehind_limit,
		const regex_constants::match_flag_type flags = regex_constants::match_default
	) const
	{
		match_results<BidirectionalIterator> m;
		return base_type::search(begin, end, lookbehind_limit, m, flags);
	}

	template <typename BidirectionalIterator, typename Allocator>
	bool search(
		const BidirectionalIterator begin,
		const BidirectionalIterator end,
		match_results<BidirectionalIterator, Allocator> &m,
		const regex_constants::match_flag_type flags = regex_constants::match_default
	) const
	{
		return base_type::search(begin, end, begin, m, flags);
	}

	template <typename BidirectionalIterator>
	bool search(
		const BidirectionalIterator begin,
		const BidirectionalIterator end,
		const regex_constants::match_flag_type flags = regex_constants::match_default
	) const
	{
		return this->search(begin, end, begin, flags);
	}

	template <typename Allocator>
	bool search(
		const charT *const str,
		match_results<const charT *, Allocator> &m,
		const regex_constants::match_flag_type flags = regex_constants::match_default
	) const
	{
		return this->search(str, str + std::char_traits<charT>::length(str), m, flags);
	}

	bool search(
		const charT *const str,
		const regex_constants::match_flag_type flags = regex_constants::match_default
	) const
	{
		return this->search(str, str + std::char_traits<charT>::length(str), flags);
	}

	template <typename ST, typename SA, typename MA>
	bool search(
		const std::basic_string<charT, ST, SA> &s,
		match_results<typename std::basic_string<charT, ST, SA>::const_iterator, MA> &m,
		const regex_constants::match_flag_type flags = regex_constants::match_default
	) const
	{
		return this->search(s.begin(), s.end(), m, flags);
	}

	template <typename ST, typename SA>
	bool search(
		const std::basic_string<charT, ST, SA> &s,
		const regex_constants::match_flag_type flags = regex_constants::match_default
	) const
	{
		return this->search(s.begin(), s.end(), flags);
	}

	template <typename StringLike>
	void replace(
		StringLike &s,
		const charT *const fmt_begin,
		const charT *const fmt_end,
		const bool global = false
	) const
	{
		typedef typename StringLike::traits_type ST;
		typedef typename StringLike::allocator_type SA;
		re_detail::repoptions<charT> opts(fmt_begin, fmt_end, global);

		this->do_replace(s, re_detail::call_mrformat<charT, ST, SA, const charT *>, reinterpret_cast<void *>(&opts));
	}

	template <typename StringLike>
	void replace(
		StringLike &s,
		const charT *const fmt,
		const bool global = false
	) const
	{
		replace(s, fmt, fmt + std::char_traits<charT>::length(fmt), global);
	}

	template <typename StringLike, typename FST, typename FSA>
	void replace(
		StringLike &s,
		const std::basic_string<charT, FST, FSA> &fmt,
		const bool global = false
	) const
	{
		replace(s, fmt.data(), fmt.data() + fmt.size(), global);
	}

	template <typename StringLike, typename RandomAccessIterator, typename MA>
	void replace(
		StringLike &s,
		bool (*repfunc)(std::basic_string<charT, typename StringLike::traits_type, typename StringLike::allocator_type> &, const match_results<RandomAccessIterator, MA> &, void *),
		void *ptr = NULL
	) const
	{
		this->do_replace(s, repfunc, ptr);
	}

	//  re.replace<my_match_type>(...):
	//  Overload for match_results<BidiIter, CustomAllocator> (my_match_type)
	//  to be used internally and passed to a callback function instead of default
	//  match_results<BidiIter, std::allocator<sub_match<BidiIter> >.
//	template <typename S, typename MR>
	template <typename MatchResults, typename StringLike>
	void replace(
		StringLike &s,
//		bool (*repfunc)(std::basic_string<charT, typename S::traits_type, typename S::allocator_type> &, const match_results<typename MR::value_type::iterator, typename MR::allocator_type> &, void *),
		bool (*repfunc)(std::basic_string<charT, typename StringLike::traits_type, typename StringLike::allocator_type> &, const MatchResults &, void *),
		void *ptr = NULL
	) const
	{
		this->do_replace(s, repfunc, ptr);
	}

	template <typename container, typename ST, typename SA>
	void split(
		container &c,
		const std::basic_string<charT, ST, SA> &s,
		const std::size_t limit = static_cast<std::size_t>(-1)
	) const
	{
		typedef typename container::value_type::iterator BidiIter;
		typedef match_results<BidiIter> match_type;	//  match_type::value_type == container::value_type.
		this->template do_split<match_type>(c, re_detail::pos0_<charT>(s, BidiIter()), re_detail::pos1_<charT>(s, BidiIter()), limit);
	}

	template <typename container, typename BidirectionalIterator>
	void split(
		container &c,
		const BidirectionalIterator begin,	//  The same as or convertible to container::value_type::iterator.
		const BidirectionalIterator end,
		const std::size_t limit = static_cast<std::size_t>(-1)
	) const
	{
		typedef typename container::value_type::iterator const_iterator;
		typedef match_results<const_iterator> match_type;

		this->template do_split<match_type>(c, static_cast<const_iterator>(begin), static_cast<const_iterator>(end), limit);
		//  container::value_type::iterator should be const_iterator,
		//  whereas BidirectionalIterator can be iterator.
	}

	template <typename container>
	void split(
		container &c,
		const charT *const str,
		const std::size_t limit = static_cast<std::size_t>(-1)
	) const
	{
		typedef match_results<const charT *> match_type;
		this->template do_split<match_type>(c, str, str + std::char_traits<charT>::length(str), limit);
	}

	//  re.split<my_match_type>(listcontainer, string):
	//  Overload for match_results<BidiIter, CustomAllocator> (my_match_type)
	//  to be used internally instead of default
	//  match_results<BidiIter, std::allocator<sub_match<BidiIter> >.
	//  In general, container::value_type == sub_match<BidiIter> == MatchResults::value_type.
	template <typename MatchResults, typename container, typename ST, typename SA>
	void split(
		container &c,
		const std::basic_string<charT, ST, SA> &s,
		const std::size_t limit = static_cast<std::size_t>(-1)
	) const
	{
		typedef typename container::value_type::iterator BidiIter;
		this->template do_split<MatchResults>(c, re_detail::pos0_<charT>(s, BidiIter()), re_detail::pos1_<charT>(s, BidiIter()), limit);
	}

	template <typename MatchResults, typename container, typename BidirectionalIterator>
	void split(
		container &c,
		const BidirectionalIterator begin,	//  The same as or convertible to MatchResults::value_type::iterator and container::value_type::iterator.
		const BidirectionalIterator end,
		const std::size_t limit = static_cast<std::size_t>(-1)
	) const
	{
		typedef typename container::value_type::iterator const_iterator;

		this->template do_split<MatchResults>(c, static_cast<const_iterator>(begin), static_cast<const_iterator>(end), limit);
	}

	template <typename MatchResults, typename container>
	void split(
		container &c,
		const charT *const str,
		const std::size_t limit = static_cast<std::size_t>(-1)
	) const
	{
		this->template do_split<MatchResults>(c, str, str + std::char_traits<charT>::length(str), limit);
	}

private:

	typedef re_detail::re_object<charT, traits> base_type;

#endif	//  !defined(SRELL_NO_APIEXT)
};
template <class charT, class traits>
	const regex_constants::syntax_option_type basic_regex<charT, traits>::icase;
template <class charT, class traits>
	const regex_constants::syntax_option_type basic_regex<charT, traits>::nosubs;
template <class charT, class traits>
	const regex_constants::syntax_option_type basic_regex<charT, traits>::optimize;
template <class charT, class traits>
	const regex_constants::syntax_option_type basic_regex<charT, traits>::collate;
template <class charT, class traits>
	const regex_constants::syntax_option_type basic_regex<charT, traits>::ECMAScript;
template <class charT, class traits>
	const regex_constants::syntax_option_type basic_regex<charT, traits>::basic;
template <class charT, class traits>
	const regex_constants::syntax_option_type basic_regex<charT, traits>::extended;
template <class charT, class traits>
	const regex_constants::syntax_option_type basic_regex<charT, traits>::awk;
template <class charT, class traits>
	const regex_constants::syntax_option_type basic_regex<charT, traits>::grep;
template <class charT, class traits>
	const regex_constants::syntax_option_type basic_regex<charT, traits>::egrep;
template <class charT, class traits>
	const regex_constants::syntax_option_type basic_regex<charT, traits>::multiline;

template <class charT, class traits>
	const regex_constants::syntax_option_type basic_regex<charT, traits>::dotall;
template <class charT, class traits>
	const regex_constants::syntax_option_type basic_regex<charT, traits>::unicodesets;

//  28.8.6, basic_regex swap:
template <class charT, class traits>
void swap(basic_regex<charT, traits> &lhs, basic_regex<charT, traits> &rhs)
{
	lhs.swap(rhs);
}

typedef basic_regex<char> regex;
typedef basic_regex<wchar_t> wregex;

typedef basic_regex<char, u8regex_traits<char> > u8cregex;

#if defined(WCHAR_MAX)
	#if WCHAR_MAX >= 0x10ffff
		typedef wregex u32wregex;
		typedef u32wregex u1632wregex;
	#elif WCHAR_MAX >= 0xffff
		typedef basic_regex<wchar_t, u16regex_traits<wchar_t> > u16wregex;
		typedef u16wregex u1632wregex;
	#endif
#endif

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef basic_regex<char16_t> u16regex;
	typedef basic_regex<char32_t> u32regex;
#endif

#if defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef basic_regex<char8_t> u8regex;
#else
	typedef u8cregex u8regex;
#endif

//  ... "basic_regex.hpp"]
//  ["regex_iterator.hpp" ...

//  28.12.1, class template regex_iterator:
template <class BidirectionalIterator, class charT = typename std::iterator_traits<BidirectionalIterator>::value_type, class traits = regex_traits<charT> >
class regex_iterator
{
public:

	typedef basic_regex<charT, traits> regex_type;
	typedef match_results<BidirectionalIterator> value_type;
	typedef std::ptrdiff_t difference_type;
	typedef const value_type *pointer;
	typedef const value_type &reference;
	typedef std::forward_iterator_tag iterator_category;

	regex_iterator()
	{
		//  28.12.1.1: Constructs an end-of-sequence iterator.
	}

	regex_iterator(
		const BidirectionalIterator a,
		const BidirectionalIterator b,
		const regex_type &re,
		const regex_constants::match_flag_type m = regex_constants::match_default)
		: begin(a), end(b), pregex(&re), flags(m)
	{
		regex_search(begin, end, begin, match, *pregex, flags);
			//  28.12.1.1: If this call returns false the constructor
			//    sets *this to the end-of-sequence iterator.
	}

	regex_iterator(const regex_iterator &that)
	{
		operator=(that);
	}

	regex_iterator &operator=(const regex_iterator &that)
	{
		if (this != &that)
		{
			this->match = that.match;
			if (this->match.size() > 0)
			{
				this->begin = that.begin;
				this->end = that.end;
				this->pregex = that.pregex;
				this->flags = that.flags;
			}
		}
		return *this;
	}

	bool operator==(const regex_iterator &right) const
	{
		if (right.match.size() == 0 || this->match.size() == 0)
			return this->match.size() == right.match.size();

		return this->begin == right.begin
			&& this->end == right.end
			&& this->pregex == right.pregex
			&& this->flags == right.flags
			&& this->match[0] == right.match[0];
	}

	bool operator!=(const regex_iterator &right) const
	{
		return !(*this == right);
	}

	const value_type &operator*() const
	{
		return match;
	}

	const value_type *operator->() const
	{
		return &match;
	}

	regex_iterator &operator++()
	{
		if (this->match.size())
		{
			BidirectionalIterator start = match[0].second;

			if (match[0].first == start)	//  The iterator holds a 0-length match.
			{
				if (start == end)
				{
					match.clear_();
					//    28.12.1.4.2: If the iterator holds a zero-length match and
					//  start == end the operator sets *this to the end-ofsequence
					//  iterator and returns *this.
				}
				else
				{
					//    28.12.1.4.3: Otherwise, if the iterator holds a zero-length match
					//  the operator calls regex_search(start, end, match, *pregex, flags
					//  | regex_constants::match_not_null | regex_constants::match_continuous).
					//  If the call returns true the operator returns *this. [Cont...]

					if (!regex_search(start, end, begin, match, *pregex, flags | regex_constants::match_not_null | regex_constants::match_continuous))
					{
						const BidirectionalIterator prevend = start;

						//  [...Cont] Otherwise the operator increments start and continues
						//  as if the most recent match was not a zero-length match.
//						++start;
						utf_traits::codepoint_inc(start, end);

						flags |= regex_constants::match_prev_avail;

						if (regex_search(start, end, begin, match, *pregex, flags))
						{
							//    28.12.1.4.5-6: In all cases in which the call to regex_search
							//  returns true, match.prefix().first shall be equal to the previous
							//  value of match[0].second, ... match[i].position() shall return
							//  distance(begin, match[i].first).
							//    This means that match[i].position() gives the offset from the
							//  beginning of the target sequence, which is often not the same as
							//  the offset from the sequence passed in the call to regex_search.
							//
							//  To satisfy this:
							match.update_prefix1_(prevend);
						}
					}
				}
			}
			else
			{
				//    28.12.1.4.4: If the most recent match was not a zero-length match,
				//  the operator sets flags to flags | regex_constants::match_prev_avail
				//  and calls regex_search(start, end, match, *pregex, flags). [Cont...]
				flags |= regex_constants::match_prev_avail;

				regex_search(start, end, begin, match, *pregex, flags);
				//  [...Cont] If the call returns false the iterator sets *this to
				//  the end-of-sequence iterator. The iterator then returns *this.
				//
				//    28.12.1.4.5-6: In all cases in which the call to regex_search
				//  returns true, match.prefix().first shall be equal to the previous
				//  value of match[0].second, ... match[i].position() shall return
				//  distance(begin, match[i].first).
				//    This means that match[i].position() gives the offset from the
				//  beginning of the target sequence, which is often not the same as
				//  the offset from the sequence passed in the call to regex_search.
				//
				//  These should already be done in regex_search.
			}
		}
		return *this;
	}

	regex_iterator operator++(int)
	{
		const regex_iterator tmp = *this;
		++(*this);
		return tmp;
	}

private:

	BidirectionalIterator                begin;
	BidirectionalIterator                end;
	const regex_type                    *pregex;
	regex_constants::match_flag_type     flags;
	match_results<BidirectionalIterator> match;

	typedef typename traits::utf_traits utf_traits;
};

typedef regex_iterator<const char *> cregex_iterator;
typedef regex_iterator<const wchar_t *> wcregex_iterator;
typedef regex_iterator<std::string::const_iterator> sregex_iterator;
typedef regex_iterator<std::wstring::const_iterator> wsregex_iterator;

typedef regex_iterator<const char *, typename std::iterator_traits<const char *>::value_type, u8regex_traits<typename std::iterator_traits<const char *>::value_type> > u8ccregex_iterator;
typedef regex_iterator<std::string::const_iterator, typename std::iterator_traits<std::string::const_iterator>::value_type, u8regex_traits<typename std::iterator_traits<std::string::const_iterator>::value_type> > u8csregex_iterator;

#if defined(WCHAR_MAX)
	#if WCHAR_MAX >= 0x10ffff
		typedef wcregex_iterator u32wcregex_iterator;
		typedef wsregex_iterator u32wsregex_iterator;
		typedef u32wcregex_iterator u1632wcregex_iterator;
		typedef u32wsregex_iterator u1632wsregex_iterator;
	#elif WCHAR_MAX >= 0xffff
		typedef regex_iterator<const wchar_t *, typename std::iterator_traits<const wchar_t *>::value_type, u16regex_traits<typename std::iterator_traits<const wchar_t *>::value_type> > u16wcregex_iterator;
		typedef regex_iterator<std::wstring::const_iterator, typename std::iterator_traits<std::wstring::const_iterator>::value_type, u16regex_traits<typename std::iterator_traits<std::wstring::const_iterator>::value_type> > u16wsregex_iterator;
		typedef u16wcregex_iterator u1632wcregex_iterator;
		typedef u16wsregex_iterator u1632wsregex_iterator;
	#endif
#endif

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef regex_iterator<const char16_t *> u16cregex_iterator;
	typedef regex_iterator<const char32_t *> u32cregex_iterator;
	typedef regex_iterator<std::u16string::const_iterator> u16sregex_iterator;
	typedef regex_iterator<std::u32string::const_iterator> u32sregex_iterator;
#endif

#if defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef regex_iterator<const char8_t *> u8cregex_iterator;
#else
	typedef u8ccregex_iterator u8cregex_iterator;
#endif
#if defined(SRELL_CPP20_CHAR8_ENABLED) && SRELL_CPP20_CHAR8_ENABLED >= 2
	typedef regex_iterator<std::u8string::const_iterator> u8sregex_iterator;
#else	//  !defined(SRELL_CPP20_CHAR8_ENABLED) || SRELL_CPP20_CHAR8_ENABLED < 2
	typedef u8csregex_iterator u8sregex_iterator;
#endif

#if !defined(SRELL_NO_APIEXT)

template <typename BidirectionalIterator, typename BasicRegex = basic_regex<typename std::iterator_traits<BidirectionalIterator>::value_type, regex_traits<typename std::iterator_traits<BidirectionalIterator>::value_type> >, typename MatchResults = match_results<BidirectionalIterator> >
class regex_iterator2
{
public:

	typedef typename std::iterator_traits<BidirectionalIterator>::value_type char_type;
	typedef BasicRegex regex_type;
	typedef MatchResults value_type;
	typedef std::ptrdiff_t difference_type;
	typedef const value_type *pointer;
	typedef const value_type &reference;
	typedef std::input_iterator_tag iterator_category;

	regex_iterator2() {}

	regex_iterator2(
		const BidirectionalIterator b,
		const BidirectionalIterator e,
		const regex_type &re,
		const regex_constants::match_flag_type m = regex_constants::match_default)
		: begin_(b), end_(e), pregex_(&re)
	{
		rewind(m);
	}

	template <typename ST, typename SA>
	regex_iterator2(
		const std::basic_string<char_type, ST, SA> &s,
		const regex_type &re,
		const regex_constants::match_flag_type m = regex_constants::match_default)
		: begin_(re_detail::pos0_<char_type>(s, BidirectionalIterator()))
		, end_(re_detail::pos1_<char_type>(s, BidirectionalIterator()))
		, pregex_(&re)
	{
		rewind(m);
	}

	regex_iterator2(const regex_iterator2 &right)
	{
		operator=(right);
	}

	regex_iterator2 &operator=(const regex_iterator2 &right)
	{
		if (this != &right)
		{
			this->match_ = right.match_;
			if (this->match_.size() > 0)
			{
				this->begin_ = right.begin_;
				this->end_ = right.end_;
				this->pregex_ = right.pregex_;
				this->flags_ = right.flags_;
				this->prevmatch_empty_ = right.prevmatch_empty_;
				this->submatch_ = right.submatch_;
			}
		}
		return *this;
	}

	bool operator==(const regex_iterator2 &right) const
	{
		if (right.match_.size() == 0 || this->match_.size() == 0)
			return this->match_.size() == right.match_.size();

		return this->begin_ == right.begin_
			&& this->end_ == right.end_
			&& this->pregex_ == right.pregex_
			&& this->flags_ == right.flags_
			&& this->match_[0] == right.match_[0];
	}

	bool operator!=(const regex_iterator2 &right) const
	{
//		return !(*this == right);
		return !operator==(right);
	}

	const value_type &operator*() const
	{
		return match_;
	}

	const value_type *operator->() const
	{
		return &match_;
	}

	bool done() const
	{
		return match_.size() == 0;
	}

	bool empty() const
	{
		return begin_ == end_;
	}

	void assign(
		const BidirectionalIterator b,
		const BidirectionalIterator e,
		const regex_type &re,
		const regex_constants::match_flag_type m = regex_constants::match_default)
	{
		begin_ = b;
		end_ = e;
		pregex_ = &re;
		rewind(m);
	}
	template <typename ST, typename SA>
	void assign(
		const std::basic_string<char_type, ST, SA> &s,
		const regex_type &re,
		const regex_constants::match_flag_type m = regex_constants::match_default)
	{
		begin_ = re_detail::pos0_<char_type>(s, BidirectionalIterator());
		end_ = re_detail::pos1_<char_type>(s, BidirectionalIterator());
		pregex_ = &re;
		rewind(m);
	}
	void assign(const regex_iterator2 &right)
	{
		operator=(right);
	}

	void rewind(const regex_constants::match_flag_type m = regex_constants::match_default)
	{
		flags_ = m;

		if (regex_search(begin_, end_, begin_, match_, *pregex_, flags_))
		{
			prevmatch_empty_ = match_[0].first == match_[0].second;
		}
		else
			match_.set_prefix1_(begin_);

		submatch_ = 0u;
	}

	regex_iterator2 &operator++()
	{
		if (match_.size())
		{
			const BidirectionalIterator prevend = match_[0].second;
			BidirectionalIterator start = prevend;

			if (prevmatch_empty_)
			{
				if (start == end_)
				{
					match_.clear_();
					return *this;
				}
				utf_traits::codepoint_inc(start, end_);
			}

			flags_ |= regex_constants::match_prev_avail;
			if (regex_search(start, end_, begin_, match_, *pregex_, flags_))
				prevmatch_empty_ = match_[0].first == match_[0].second;

			match_.update_prefix1_(prevend);
		}
		return *this;
	}

	regex_iterator2 operator++(int)
	{
		const regex_iterator2 tmp = *this;
		++(*this);
		return tmp;
	}

	//  For replace.

	//  Replaces [match_[0].first, match_[0].second) in
	//  [entire_string.begin(), entire_string.end()) with replacement,
	//  and adjusts all the internal iterators accordingly.
	template <typename ST, typename SA>
	void replace(std::basic_string<char_type, ST, SA> &entire_string, const std::basic_string<char_type, ST, SA> &replacement)
	{
		typedef std::basic_string<char_type, ST, SA> string_type;

		if (match_.size())
		{
			const BidirectionalIterator oldbegin = re_detail::pos0_<char_type>(entire_string, BidirectionalIterator());
			const typename string_type::size_type oldbeginoffset = begin_ - oldbegin;
			const typename string_type::size_type oldendoffset = end_ - oldbegin;
			const typename string_type::size_type pos = match_[0].first - oldbegin;
			const typename string_type::size_type count = match_[0].second - match_[0].first;
			const typename match_type::difference_type addition = replacement.size() - match_.length(0);

			entire_string.replace(pos, count, replacement);

			const BidirectionalIterator newbegin = re_detail::pos0_<char_type>(entire_string, BidirectionalIterator());

			begin_ = newbegin + oldbeginoffset;
			end_ = newbegin + (oldendoffset + addition);	//  VC checks if an iterator exceeds end().

			match_.update_m0_(newbegin + pos, newbegin + (pos + count + addition));

			prevmatch_empty_ = count == 0;
		}
	}

	template <typename ST, typename SA>
	void replace(std::basic_string<char_type, ST, SA> &entire_string, const BidirectionalIterator b, const BidirectionalIterator e)
	{
		typedef std::basic_string<char_type, ST, SA> string_type;

		replace(entire_string, string_type(b, e));
	}

	template <typename ST, typename SA>
	void replace(std::basic_string<char_type, ST, SA> &entire_string, const char_type *const replacement)
	{
		typedef std::basic_string<char_type, ST, SA> string_type;

		replace(entire_string, string_type(replacement));
	}

	//  For split.

	//  1. Until done() returns true, gather this->prefix() when
	//     split_ready() returns true,
	//  2. Once done() becomes true, get remainder().

	//  Returns if this->prefix() holds a range that is worthy of being
	//  treated as a split substring.
	bool split_ready()	//const
	{
		if (match_.size())
		{
			if (match_[0].first != end_)
				return match_.prefix().first != match_[0].second;

			//  [end_, end_) is not appropriate as a split range. Invalidates the current match.
			match_.clear_();
		}
		return false;	//  Iterating complete.
	}

	//  When the only_after_match parameter is false, returns
	//  [prefix().first, end); otherwise (when true) returns
	//  [match_[0].second, end).
	//  This function is intended to be called after iterating is
	//  finished, to receive the range of suffix() of the last match.
	//  If iterating is aborted during processing (e.g. pushing to a
	//  list container) a captured subsequence (match_[n] where n >= 1),
	//  then should be called with only_after_match being true,
	//  otherwise [prefix().first, prefix().second) would be duplicated.
	const typename value_type::value_type &remainder(const bool only_after_match = false)
	{
		if (only_after_match && match_.size())
			match_.set_prefix1_(match_[0].second);

		match_.update_prefix2_(end_);
		return match_.prefix();
	}

	//  The following 4 split_* functions are intended to be used
	//  together, as follows:
	//
	//    for (it.split_begin(); !it.done(); it.split_next()) {
	//      if (++count == LIMIT)
	//        break;
	//      list.push_back(it.split_range());
	//    }
	//    list.push_back(it.split_remainder());

	//  Moves to a first subsequence for which split_ready() returns
	//  true. This should be called only once before beginning iterating
	//  (or after calling rewind()).
	bool split_begin()
	{
		if (split_ready())
			return true;

		operator++();
		return split_ready();
	}

	//  Moves to a next subsequence for which split_ready() returns
	//  true.
	//  This function is intended to be used instead of the ordinary
	//  increment operator (++).
	bool split_next()
	{
		if (++submatch_ >= match_.size())
		{
			submatch_ = 0u;
			operator++();
			return split_begin();
		}
		return !done();
	}

	//  Returns a current subsequence to which the iterator points.
	const typename value_type::value_type &split_range() const
	{
		return submatch_ == 0u ? match_.prefix() : match_[submatch_];
	}

	//  Returns the final subsequence immediately following the last
	//  match range. This should be called after iterating is complete
	//  or aborted.
	//  Unlike remainder() above, a boolean value corresponding to
	//  only_after_match is automatically calculated.
	const typename value_type::value_type &split_remainder()
	{
		if (submatch_ > 0u)
			match_.set_prefix1_(match_[0].second);

		match_.update_prefix2_(end_);
		return match_.prefix();
	}

private:

	typedef match_results<BidirectionalIterator> match_type;
	typedef typename regex_type::traits_type::utf_traits utf_traits;

	BidirectionalIterator begin_;
	BidirectionalIterator end_;
	const regex_type *pregex_;
	regex_constants::match_flag_type flags_;
	match_type match_;
	bool prevmatch_empty_;
	typename match_type::size_type submatch_;

};

typedef regex_iterator2<const char *> cregex_iterator2;
typedef regex_iterator2<const wchar_t *> wcregex_iterator2;
typedef regex_iterator2<std::string::const_iterator> sregex_iterator2;
typedef regex_iterator2<std::wstring::const_iterator> wsregex_iterator2;

typedef regex_iterator2<const char *, u8cregex> u8ccregex_iterator2;
typedef regex_iterator2<std::string::const_iterator, u8cregex> u8csregex_iterator2;

#if defined(WCHAR_MAX)
	#if (WCHAR_MAX >= 0x10ffff)
		typedef wcregex_iterator2 u32wcregex_iterator2;
		typedef wsregex_iterator2 u32wsregex_iterator2;
		typedef u32wcregex_iterator2 u1632wcregex_iterator2;
		typedef u32wsregex_iterator2 u1632wsregex_iterator2;
	#elif (WCHAR_MAX >= 0xffff)
		typedef regex_iterator2<const wchar_t *, u16wregex> u16wcregex_iterator2;
		typedef regex_iterator2<std::wstring::const_iterator, u16wregex> u16wsregex_iterator2;
		typedef u16wcregex_iterator2 u1632wcregex_iterator2;
		typedef u16wsregex_iterator2 u1632wsregex_iterator2;
	#endif
#endif

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef regex_iterator2<const char16_t *> u16cregex_iterator2;
	typedef regex_iterator2<const char32_t *> u32cregex_iterator2;
	typedef regex_iterator2<std::u16string::const_iterator> u16sregex_iterator2;
	typedef regex_iterator2<std::u32string::const_iterator> u32sregex_iterator2;
#endif

#if defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef regex_iterator2<const char8_t *> u8cregex_iterator2;
#else
	typedef u8ccregex_iterator2 u8cregex_iterator2;
#endif
#if defined(SRELL_CPP20_CHAR8_ENABLED) && (SRELL_CPP20_CHAR8_ENABLED >= 2)
	typedef regex_iterator2<std::u8string::const_iterator> u8sregex_iterator2;
#else	//  !defined(SRELL_CPP20_CHAR8_ENABLED) || (SRELL_CPP20_CHAR8_ENABLED < 2)
	typedef u8csregex_iterator2 u8sregex_iterator2;
#endif

#endif	//  !defined(SRELL_NO_APIEXT)

//  ... "regex_iterator.hpp"]
//  ["regex_algorithm.hpp" ...

//  28.11.2, function template regex_match:
//  [7.11.2] Function template regex_match
template <class BidirectionalIterator, class Allocator, class charT, class traits>
bool regex_match(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	match_results<BidirectionalIterator, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return e.search(first, last, first, m, flags | regex_constants::match_continuous | regex_constants::match_match_);
}

template <class BidirectionalIterator, class charT, class traits>
bool regex_match(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
//  4 Effects: Behaves "as if" by constructing an instance of
//  match_results<BidirectionalIterator> what, and then returning the
//  result of regex_match(first, last, what, e, flags).

	match_results<BidirectionalIterator> what;

	return regex_match(first, last, what, e, flags);
}

template <class charT, class Allocator, class traits>
bool regex_match(
	const charT *const str,
	match_results<const charT *, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_match(str, str + std::char_traits<charT>::length(str), m, e, flags);
}

template <class ST, class SA, class Allocator, class charT, class traits>
bool regex_match(
	const std::basic_string<charT, ST, SA> &s,
	match_results<typename std::basic_string<charT, ST, SA>::const_iterator, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_match(s.begin(), s.end(), m, e, flags);
}

template <class charT, class traits>
bool regex_match(
	const charT *const str,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_match(str, str + std::char_traits<charT>::length(str), e, flags);
}

template <class ST, class SA, class charT, class traits>
bool regex_match(
	const std::basic_string<charT, ST, SA> &s,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_match(s.begin(), s.end(), e, flags);
}

template <class BidirectionalIterator, class Allocator, class charT, class traits>
bool regex_search(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const BidirectionalIterator lookbehind_limit,
	match_results<BidirectionalIterator, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return e.search(first, last, lookbehind_limit, m, flags);
}

template <class BidirectionalIterator, class charT, class traits>
bool regex_search(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const BidirectionalIterator lookbehind_limit,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
//  6 Effects: Behaves "as if" by constructing an object what of type
//  match_results<iterator> and then returning the result of
//  regex_search(first, last, what, e, flags).

	match_results<BidirectionalIterator> what;
	return regex_search(first, last, lookbehind_limit, what, e, flags);
}

//  28.11.3, function template regex_search:
//  7.11.3 regex_search [tr.re.alg.search]
template <class BidirectionalIterator, class Allocator, class charT, class traits>
bool regex_search(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	match_results<BidirectionalIterator, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return e.search(first, last, first, m, flags);
}

template <class BidirectionalIterator, class charT, class traits>
bool regex_search(
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
//  6 Effects: Behaves "as if" by constructing an object what of type
//  match_results<iterator> and then returning the result of
//  regex_search(first, last, what, e, flags).

	match_results<BidirectionalIterator> what;
	return regex_search(first, last, what, e, flags);
}

template <class charT, class Allocator, class traits>
bool regex_search(
	const charT *const str,
	match_results<const charT *, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_search(str, str + std::char_traits<charT>::length(str), m, e, flags);
}

template <class charT, class traits>
bool regex_search(
	const charT *const str,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_search(str, str + std::char_traits<charT>::length(str), e, flags);
}

template <class ST, class SA, class charT, class traits>
bool regex_search(
	const std::basic_string<charT, ST, SA> &s,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_search(s.begin(), s.end(), e, flags);
}

template <class ST, class SA, class Allocator, class charT, class traits>
bool regex_search(
	const std::basic_string<charT, ST, SA> &s,
	match_results<typename std::basic_string<charT, ST, SA>::const_iterator, Allocator> &m,
	const basic_regex<charT, traits> &e,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	return regex_search(s.begin(), s.end(), m, e, flags);
}

//  28.11.4, function template regex_replace:
//  [7.11.4] Function template regex_replace
template <class OutputIterator, class BidirectionalIterator, class traits, class charT, class ST, class SA>
OutputIterator regex_replace(
	OutputIterator out,
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const basic_regex<charT, traits> &e,
	const std::basic_string<charT, ST, SA> &fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	typedef regex_iterator<BidirectionalIterator, charT, traits> iterator_type;

	const bool do_copy = !(flags & regex_constants::format_no_copy);
	const iterator_type eos;
	iterator_type i(first, last, e, flags);
	typename iterator_type::value_type::value_type last_m_suffix;

	last_m_suffix.first = first;
	last_m_suffix.second = last;

	for (; i != eos; ++i)
	{
		if (do_copy)
			out = std::copy(i->prefix().first, i->prefix().second, out);

		out = i->format(out, fmt, flags);
		last_m_suffix = i->suffix();

		if (flags & regex_constants::format_first_only)
			break;
	}

	if (do_copy)
		out = std::copy(last_m_suffix.first, last_m_suffix.second, out);

	return out;
}

template <class OutputIterator, class BidirectionalIterator, class traits, class charT>
OutputIterator regex_replace(
	OutputIterator out,
	const BidirectionalIterator first,
	const BidirectionalIterator last,
	const basic_regex<charT, traits> &e,
	const charT *const fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	//  Strictly speaking, this should be implemented as a version different
	//  from the above with changing the line i->format(out, fmt, flags) to
	//  i->format(out, fmt, fmt + char_traits<charT>::length(fmt), flags).

	const std::basic_string<charT> fs(fmt, fmt + std::char_traits<charT>::length(fmt));

	return regex_replace(out, first, last, e, fs, flags);
}

template <class traits, class charT, class ST, class SA, class FST, class FSA>
std::basic_string<charT, ST, SA> regex_replace(
	const std::basic_string<charT, ST, SA> &s,
	const basic_regex<charT, traits> &e,
	const std::basic_string<charT, FST, FSA> &fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	std::basic_string<charT, ST, SA> result;

	regex_replace(std::back_inserter(result), s.begin(), s.end(), e, fmt, flags);
	return result;
}

template <class traits, class charT, class ST, class SA>
std::basic_string<charT, ST, SA> regex_replace(
	const std::basic_string<charT, ST, SA> &s,
	const basic_regex<charT, traits> &e,
	const charT *const fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	std::basic_string<charT, ST, SA> result;

	regex_replace(std::back_inserter(result), s.begin(), s.end(), e, fmt, flags);
	return result;
}

template <class traits, class charT, class ST, class SA>
std::basic_string<charT> regex_replace(
	const charT *const s,
	const basic_regex<charT, traits> &e,
	const std::basic_string<charT, ST, SA> &fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	std::basic_string<charT> result;

	regex_replace(std::back_inserter(result), s, s + std::char_traits<charT>::length(s), e, fmt, flags);
	return result;
}

template <class traits, class charT>
std::basic_string<charT> regex_replace(
	const charT *const s,
	const basic_regex<charT, traits> &e,
	const charT *const fmt,
	const regex_constants::match_flag_type flags = regex_constants::match_default
)
{
	std::basic_string<charT> result;

	regex_replace(std::back_inserter(result), s, s + std::char_traits<charT>::length(s), e, fmt, flags);
	return result;
}

#if !defined(SRELL_NO_APIEXT)

template <typename BasicStringLike>
struct str_clip
{
public:

	typedef BasicStringLike string_type;
	typedef typename string_type::traits_type traits_type;
	typedef typename string_type::allocator_type allocator_type;
	typedef typename string_type::size_type size_type;
	typedef typename string_type::iterator iterator;
	typedef typename string_type::const_iterator const_iterator;
	typedef typename string_type::const_pointer const_pointer;

	str_clip(const str_clip &s)
		: ptr_(s.ptr_), offset_(s.offset_), roffset_(s.roffset_)
	{
	}

	str_clip(string_type &s)
		: ptr_(&s), offset_(0), roffset_(0)
	{
	}

	str_clip(string_type &s, const size_type pos, const size_type count = string_type::npos)
		: ptr_(&s), offset_(pos)
	{
		const size_type remainder = s.size() - pos;

		roffset_ = count < remainder ? remainder - count : 0;
	}

	str_clip(string_type &s, const_iterator b, const_iterator e)
		: ptr_(&s), offset_(b - s.begin()), roffset_(s.end() - e)
	{
	}

	const str_clip &clip(const size_type pos, const size_type count = string_type::npos)
	{
		const size_type remainder = ptr_->size() - pos;

		offset_ = pos;
		roffset_ = count < remainder ? remainder - count : 0;
		return *this;
	}

	const str_clip &clip(const_iterator b, const_iterator e)
	{
		offset_ = b - ptr_->begin();
		roffset_ = ptr_->end() - e;
		return *this;
	}

	const_pointer data() const
	{
		return ptr_->data() + offset_;
	}

	size_type size() const
	{
		return ptr_->size() - offset_ - roffset_;
	}

	iterator begin() const
	{
		return ptr_->begin() + offset_;
	}

	iterator end() const
	{
		return ptr_->end() - roffset_;
	}

	void replace(const size_type pos, const size_type count, const string_type &r) const
	{
		ptr_->replace(pos + offset_, count, r);
	}

private:

	string_type *ptr_;
	size_type offset_;
	size_type roffset_;
};

#endif	//  !defined(SRELL_NO_APIEXT)

//  ... "regex_algorithm.hpp"]
//  ["regex_token_iterator.hpp" ...

//  28.12.2, class template regex_token_iterator:
template <class BidirectionalIterator, class charT = typename std::iterator_traits<BidirectionalIterator>::value_type, class traits = regex_traits<charT> >
class regex_token_iterator
{
public:

	typedef basic_regex<charT, traits> regex_type;
	typedef sub_match<BidirectionalIterator> value_type;
	typedef std::ptrdiff_t difference_type;
	typedef const value_type *pointer;
	typedef const value_type &reference;
	typedef std::forward_iterator_tag iterator_category;

	regex_token_iterator() : result_(NULL)
	{
		//  Constructs the end-of-sequence iterator.
	}

	regex_token_iterator(
		const BidirectionalIterator a,
		const BidirectionalIterator b,
		const regex_type &re,
		int submatch = 0,
		regex_constants::match_flag_type m = regex_constants::match_default
	) : position_(a, b, re, m), result_(NULL), subs_(1, submatch)
	{
		post_constructor_(a, b);
	}

	regex_token_iterator(
		const BidirectionalIterator a,
		const BidirectionalIterator b,
		const regex_type &re,
		const std::vector<int> &submatches,
		regex_constants::match_flag_type m = regex_constants::match_default
	) : position_(a, b, re, m), result_(NULL), subs_(submatches)
	{
		post_constructor_(a, b);
	}

#if defined(SRELL_CPP11_INITIALIZER_LIST_ENABLED)
	regex_token_iterator(
		const BidirectionalIterator a,
		const BidirectionalIterator b,
		const regex_type &re,
		std::initializer_list<int> submatches,
		regex_constants::match_flag_type m = regex_constants::match_default
	) : position_(a, b, re, m), result_(NULL), subs_(submatches)
	{
		post_constructor_(a, b);
	}
#endif

	template <std::size_t N>	//  Was R in TR1.
	regex_token_iterator(
		const BidirectionalIterator a,
		const BidirectionalIterator b,
		const regex_type &re,
		const int (&submatches)[N],
		regex_constants::match_flag_type m = regex_constants::match_default
	) : position_(a, b, re, m), result_(NULL), subs_(submatches, submatches + N)
	{
		post_constructor_(a, b);
	}

	regex_token_iterator(const regex_token_iterator &that)
	{
		operator=(that);
	}

	regex_token_iterator &operator=(const regex_token_iterator &that)
	{
		if (this != &that)
		{
			this->result_ = that.result_;
			if (this->result_)
			{
				this->position_ = that.position_;
				this->suffix_ = that.suffix_;
				this->N_ = that.N_;
				this->subs_ = that.subs_;
			}
		}
		return *this;
	}

	bool operator==(const regex_token_iterator &right)
	{
		if (right.result_ == NULL || this->result_ == NULL)
			return this->result_ == right.result_;

		if (this->result_ == &this->suffix_ || right.result_ == &right.suffix_)
			return this->suffix_ == right.suffix_;

		return this->position_ == right.position_
			&& this->N_ == right.N_
			&& this->subs_ == right.subs_;
	}

	bool operator!=(const regex_token_iterator &right)
	{
		return !(*this == right);
	}

	const value_type &operator*()
	{
		return *result_;
	}

	const value_type *operator->()
	{
		return result_;
	}

	regex_token_iterator &operator++()
	{
		if (result_ == &suffix_)
			result_ = NULL;
		else if (result_ != NULL)
		{
			if (++this->N_ >= subs_.size())
			{
				position_iterator eos_iterator;

				this->N_ = 0;
				suffix_ = position_->suffix();
				if (++position_ == eos_iterator)
				{
					result_ = (suffix_.matched && minus1_in_subs_()) ? &suffix_ : NULL;
					return *this;
				}
			}
			result_ = subs_[this->N_] != -1 ? &((*position_)[subs_[this->N_]]) : &((*position_).prefix());
		}
		return *this;
	}

	regex_token_iterator operator++(int)
	{
		const regex_token_iterator tmp(*this);
		++(*this);
		return tmp;
	}

private:

	void post_constructor_(const BidirectionalIterator a, const BidirectionalIterator b)
	{
		position_iterator eos_iterator;

		this->N_ = 0;

		if (position_ != eos_iterator && subs_.size())
		{
			result_ = subs_[this->N_] != -1 ? &((*position_)[subs_[this->N_]]) : &((*position_).prefix());
			return;
		}

		if (minus1_in_subs_())
		{
			suffix_.matched = a != b;

			if (suffix_.matched)
			{
				suffix_.first = a;
				suffix_.second = b;
				result_ = &suffix_;
				return;
			}
		}
		result_ = NULL;
	}

	bool minus1_in_subs_() const
	{
		for (std::size_t i = 0; i < subs_.size(); ++i)
			if (subs_[i] == -1)
				return true;

		return false;
	}

private:

	typedef regex_iterator<BidirectionalIterator, charT, traits> position_iterator;
	position_iterator position_;
	const value_type *result_;
	value_type suffix_;
	std::size_t N_;
	std::vector<int> subs_;
};

typedef regex_token_iterator<const char *> cregex_token_iterator;
typedef regex_token_iterator<const wchar_t *> wcregex_token_iterator;
typedef regex_token_iterator<std::string::const_iterator> sregex_token_iterator;
typedef regex_token_iterator<std::wstring::const_iterator> wsregex_token_iterator;

typedef regex_token_iterator<const char *, typename std::iterator_traits<const char *>::value_type, u8regex_traits<typename std::iterator_traits<const char *>::value_type> > u8ccregex_token_iterator;
typedef regex_token_iterator<std::string::const_iterator, typename std::iterator_traits<std::string::const_iterator>::value_type, u8regex_traits<typename std::iterator_traits<std::string::const_iterator>::value_type> > u8csregex_token_iterator;

#if defined(WCHAR_MAX)
	#if WCHAR_MAX >= 0x10ffff
		typedef wcregex_token_iterator u32wcregex_token_iterator;
		typedef wsregex_token_iterator u32wsregex_token_iterator;
		typedef u32wcregex_token_iterator u1632wcregex_token_iterator;
		typedef u32wsregex_token_iterator u1632wsregex_token_iterator;
	#elif WCHAR_MAX >= 0xffff
		typedef regex_token_iterator<const wchar_t *, typename std::iterator_traits<const wchar_t *>::value_type, u16regex_traits<typename std::iterator_traits<const wchar_t *>::value_type> > u16wcregex_token_iterator;
		typedef regex_token_iterator<std::wstring::const_iterator, typename std::iterator_traits<std::wstring::const_iterator>::value_type, u16regex_traits<typename std::iterator_traits<std::wstring::const_iterator>::value_type> > u16wsregex_token_iterator;
		typedef u16wcregex_token_iterator u1632wcregex_token_iterator;
		typedef u16wsregex_token_iterator u1632wsregex_token_iterator;
	#endif
#endif

#if defined(SRELL_CPP11_CHAR1632_ENABLED)
	typedef regex_token_iterator<const char16_t *> u16cregex_token_iterator;
	typedef regex_token_iterator<const char32_t *> u32cregex_token_iterator;
	typedef regex_token_iterator<std::u16string::const_iterator> u16sregex_token_iterator;
	typedef regex_token_iterator<std::u32string::const_iterator> u32sregex_token_iterator;
#endif

#if defined(SRELL_CPP20_CHAR8_ENABLED)
	typedef regex_token_iterator<const char8_t *> u8cregex_token_iterator;
#else
	typedef u8ccregex_token_iterator u8cregex_token_iterator;
#endif
#if defined(SRELL_CPP20_CHAR8_ENABLED) && SRELL_CPP20_CHAR8_ENABLED >= 2
	typedef regex_token_iterator<std::u8string::const_iterator> u8sregex_token_iterator;
#else	//  !defined(SRELL_CPP20_CHAR8_ENABLED) || SRELL_CPP20_CHAR8_ENABLED < 2
	typedef u8csregex_token_iterator u8sregex_token_iterator;
#endif

//  ... "regex_token_iterator.hpp"]

}		//  namespace srell
#endif	//  SRELL_REGEX_TEMPLATE_LIBRARY
