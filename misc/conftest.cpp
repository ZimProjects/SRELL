//
//  Conformance test program for SRELL.
//  Version 3.007 (2025/09/18)
//
//  This needs to be compiled and run on a system that supports
//  an ISO-646/US-ASCII compatible encoding.
//

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#ifdef DUP_CHECK
#include <map>
#endif

#define SRELL_HAS_U8TYPE
#define SRELL_HAS_U1632TYPE

#ifdef __cpp_unicode_characters
typedef char16_t u16char_type;
typedef char32_t u32char_type;
#else
#define PRE_CPP11
#if defined(__clang_major__) && (__clang_major__ >= 19)
#undef SRELL_HAS_U1632TYPE
#else
typedef unsigned short u16char_type;
typedef unsigned long u32char_type;
#endif
#endif

#ifdef __cpp_char8_t
#if defined(_MSC_VER)
typedef char char_type;
#define RE(x) x
#define STR(x) x
#define STR0(x) x "\0"
#else
typedef char8_t char_type;
#define RE(x) u8##x
#define STR(x) u8##x
#define STR0(x) u8##x u8"\0"
#endif
typedef char8_t u8char_type;
#else
#define PRE_CPP20
typedef char char_type;
#define RE(x) x
#define STR(x) x
#define STR0(x) x "\0"
#if defined(__clang_major__) && (__clang_major__ >= 19)
#undef SRELL_HAS_U8TYPE
#else
typedef unsigned char u8char_type;
#endif
#endif

#include "../srell.hpp"
#include "conftest-data.h"

namespace constants
{
	enum utf_type
	{
		unknown, utf8, utf16, utf32, utf8c, utf16or32w, c, w
	};
}

struct utf0_tag {};
struct utf8_tag {};
struct utf16_tag {};
struct utf32_tag {};

#if defined(WCHAR_MAX)
	#if (WCHAR_MAX >= 0x10FFFF)
	#define SRELL_HAS_UTF32W
	#define SRELL_HAS_UTF1632W 32
	#elif (WCHAR_MAX >= 0xFFFF)
	#define SRELL_HAS_UTF16W
	#define SRELL_HAS_UTF1632W 16
	#else
	#error "wchar_t is not capable of UTF-16 or UTF-32."
	#endif
#endif

unsigned long try_unescaping(const char_type *&p)
{
	const unsigned long failure = 0x110000ul;
	unsigned long ucp = 0ul;

	if (*++p == 0x7b)	//  '{'
	{
		const char_type *const begin = ++p;

		for (;; ++p)
		{
			if (*p >= 0x30 && *p <= 0x39)
				ucp = (ucp << 4) | (*p - 0x30);
			else
			{
				const char ch = *p | 0x20;

				if (ch >= 0x61 && ch <= 0x66)
					ucp = (ucp << 4) | (ch - 0x61 + 10);
				else if (*p == 0x7d && p != begin)	//  '}'
					return ucp;
				else
					return failure;
			}
		}
	}

	for (unsigned int ui = 0;; ++p)
	{
		if (*p >= 0x30 && *p <= 0x39)
			ucp = (ucp << 4) | (*p - 0x30);
		else
		{
			const char ch = *p | 0x20;

			if (ch >= 0x61 && ch <= 0x66)
				ucp = (ucp << 4) | (ch - 0x61 + 10);
			else
				return failure;
		}

		if (++ui == 4)
			return ucp;
	}
}

template <typename Char32T, typename UtfTag>
std::basic_string<Char32T> to_utf(const char_type *&u8c, const UtfTag)
{
	std::basic_string<Char32T> out;

	for (; *u8c; ++u8c)
	{
		if (*u8c == 0x5c)	//  '\\'
		{
			const char_type *prefetch = u8c;

			if (*++prefetch == 0x75)	//  'u'
			{
				const unsigned long u32 = try_unescaping(prefetch);

				if (u32 < 0x110000ul)
				{
					out.push_back(static_cast<Char32T>(u32));

					u8c = prefetch;
					continue;
				}
			}
		}
		out.append(1, *u8c);
	}
	return out;
}

template <typename Char8T>
std::basic_string<Char8T> to_utf(const char_type *&u8c, const utf8_tag)
{
	std::basic_string<Char8T> out;

	for (; *u8c; ++u8c)
	{
		if (*u8c == 0x5c)	//  '\\'
		{
			const char_type *prefetch = u8c;

			if (*++prefetch == 0x75)	//  'u'
			{
				const unsigned long u32 = try_unescaping(prefetch);

				if (u32 < 0x110000ul)
				{
					if (u32 < 0x80ul)
					{
						out.push_back(static_cast<Char8T>(u32));
					}
					else if (u32 < 0x800ul)
					{
						out.push_back(static_cast<Char8T>(((u32 >> 6) & 0x1f) | 0xc0));
						out.push_back(static_cast<Char8T>((u32 & 0x3f) | 0x80));
					}
					else if (u32 < 0x10000ul)
					{
						out.push_back(static_cast<Char8T>(((u32 >> 12) & 0x0f) | 0xe0));
						out.push_back(static_cast<Char8T>(((u32 >> 6) & 0x3f) | 0x80));
						out.push_back(static_cast<Char8T>((u32 & 0x3f) | 0x80));
					}
					else
					{
						out.push_back(static_cast<Char8T>(((u32 >> 18) & 7) | 0xf0));
						out.push_back(static_cast<Char8T>(((u32 >> 12) & 0x3f) | 0x80));
						out.push_back(static_cast<Char8T>(((u32 >> 6) & 0x3f) | 0x80));
						out.push_back(static_cast<Char8T>((u32 & 0x3f) | 0x80));
					}

					u8c = prefetch;
					continue;
				}
			}
		}
		out.append(1, *u8c);
	}
	return out;
}

template <typename Char16T>
std::basic_string<Char16T> to_utf(const char_type *&u8c, const utf16_tag)
{
	std::basic_string<Char16T> out;

	for (; *u8c; ++u8c)
	{
		if (*u8c == 0x5c)	//  '\\'
		{
			const char_type *prefetch = u8c;

			if (*++prefetch == 0x75)	//  'u'
			{
				const unsigned long u32 = try_unescaping(prefetch);

				if (u32 < 0x110000ul)
				{
					if (u32 < 0x10000ul)
					{
						out.push_back(static_cast<Char16T>(u32));
					}
					else
					{
						out.push_back(static_cast<Char16T>(((u32 - 0x10000) >> 10) | 0xd800));
						out.push_back(static_cast<Char16T>((u32 & 0x3ff) | 0xdc00));
					}

					u8c = prefetch;
					continue;
				}
			}
		}
		out.append(1, *u8c);
	}
	return out;
}

template <typename CharT>
std::string u32ctou8c(const CharT u32, const std::size_t minsize)
{
	std::string utf8;

	//  UTF-32 -> UTF-8.
	if (u32 < 0x80)	//  00..7F
	{
		utf8.push_back(static_cast<char>((u32 >= 0x20 || u32 == 0x0a) ? u32 : 0x20));
	}
	else if (u32 < 0x800)	//  80..7FF
	{
		utf8.push_back(static_cast<char>(((u32 >> 6) & 0x1f) | 0xc0));
		utf8.push_back(static_cast<char>((u32 & 0x3f) | 0x80));
	}
	else if (u32 < 0x10000)	//  800..FFFF
	{
		utf8.push_back(static_cast<char>(((u32 >> 12) & 0x0f) | 0xe0));
		utf8.push_back(static_cast<char>(((u32 >> 6) & 0x3f) | 0x80));
		utf8.push_back(static_cast<char>((u32 & 0x3f) | 0x80));
	}
	else	//  10000..10FFFF
	{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4333)
#endif
		utf8.push_back(static_cast<char>(((u32 >> 18) & 0x7) | 0xf0));
		utf8.push_back(static_cast<char>(((u32 >> 12) & 0x3f) | 0x80));
		utf8.push_back(static_cast<char>(((u32 >> 6) & 0x3f) | 0x80));
		utf8.push_back(static_cast<char>((u32 & 0x3f) | 0x80));
#ifdef _MSC_VER
#pragma warning(pop)
#endif
	}

	if (utf8.size() < minsize)
		utf8.insert(0, minsize - utf8.size(), 0x20);

	return utf8;
}

template <typename StringT, typename CharT>
StringT simple_conv(const CharT *c)
{
	StringT out;

	for (; *c;)
		out.push_back(*c++);

	return out;
}

template <typename Char16>
std::string utf16_to_utf8c(const std::basic_string<Char16> &u16)
{
	std::string out;

	for (typename std::basic_string<Char16>::size_type index = 0; index < u16.size(); ++index)
	{
		const Char16 ucp = u16[index];

		//  UTF-16 -> UTF-32.
		if ((ucp & 0xfc00) == 0xd800)
		{
			const Char16 surtail = u16[++index];

			if ((surtail & 0xfc00) == 0xdc00)
			{
				const Char16 surlead = ((ucp & 0x3ff) + 0x40);
				//  110110aa aabbbbcc  110111cc ddddeeee
				//  11110aaa 10aabbbb  10ccccdd 10ddeeee
				out.push_back(static_cast<char>(((surlead >> 8) & 7) | 0xf0));
				out.push_back(static_cast<char>(((surlead >> 2) & 0x3f) | 0x80));
				out.push_back(static_cast<char>(0x80 | ((surlead << 4) & 0x30) | ((surtail >> 6) & 0xf)));
				out.push_back(static_cast<char>(0x80 | (surtail & 0x3f)));
				continue;
			}
			else
				--index;
		}
		out += u32ctou8c(ucp, 1);
	}
	return out;
}

template <typename Char32>
std::string utf32_to_utf8c(const std::basic_string<Char32> &u32)
{
	std::string out;

	for (typename std::basic_string<Char32>::size_type index = 0; index < u32.size(); ++index)
		out += u32ctou8c(u32[index], 1);

	return out;
}

std::string convert_to_utf8c(const std::string &s)
{
	return s;
}

std::string convert_to_utf8c(const std::wstring &ws)
{
#if (SRELL_HAS_UTF1632W == 16)
	return utf16_to_utf8c(ws);
#else
	return utf32_to_utf8c(ws);
#endif
}

#if defined(SRELL_HAS_U8TYPE)
std::string convert_to_utf8c(const std::basic_string<u8char_type> &u8)
{
	return std::string(u8.begin(), u8.end());
}
#endif
#if defined(SRELL_HAS_U1632TYPE)
std::string convert_to_utf8c(const std::basic_string<u16char_type> &u16)
{
	return utf16_to_utf8c(u16);
}
std::string convert_to_utf8c(const std::basic_string<u32char_type> &u32)
{
	return utf32_to_utf8c(u32);
}
#endif

namespace otherflags
{
	typedef unsigned int type;
	static const type none = 0;
	static const type regex_match = 1 << 0;
	static const type three_iterators = 1 << 1;
	static const type global = 1 << 2;
	static const type matchall = 1 << 3;
	static const type errortest = 1 << 4;
	static const type namedgroup = 1 << 5;
	static const type print_states = 1 << 6;
}

std::string parse_flagstring(
	srell::regex_constants::syntax_option_type &so,
	srell::regex_constants::match_flag_type &mf,
	otherflags::type &of,
	const char *flags)
{
	std::string str;

	so = srell::regex_constants::ECMAScript;
	mf = srell::regex_constants::match_default;

	for (;; ++flags)
	{
		switch (*flags)
		{
		case 0:
			return str;

		case 'i':
			so |= srell::regex_constants::icase;
			str.push_back(*flags);
			break;
		case 'm':
			so |= srell::regex_constants::multiline;
			str.push_back(*flags);
			break;
		case 's':
			so |= srell::regex_constants::dotall;
			str.push_back(*flags);
			break;
#if !defined(NO_VMODE)
		case 'v':
			so |= srell::regex_constants::unicodesets;
			str.push_back(*flags);
			break;
#endif
		case 'y':
			so |= srell::regex_constants::sticky;
			str.push_back(*flags);
			break;

		case 'n':
			so |= srell::regex_constants::nosubs;
			str.push_back(*flags);
			break;

		case '3':	//  regex_search(begin, end, start);
			of |= otherflags::three_iterators;
			break;

		case 'M':	//  regex_match()
			of |= otherflags::regex_match;
			break;

		case 'G':	//  String.prototype.match()
			of |= otherflags::global;
			break;

		case 'A':	//  String.prototype.matchAll()
			of |= otherflags::matchall;
			break;

		case 'E':
			of |= otherflags::errortest;
			break;

		case 'N':
			of |= otherflags::namedgroup;
			break;

		default:
			std::fprintf(stdout, "[Warning] Unknown flag '%c' found.\n", *flags);
			break;
		}
	}
}

template <typename RegexType, typename CharT, typename UtfTag>
bool conf_test(
	const char_type *str1,
	const char_type *exp1,
	const char *const flagstr,
	const unsigned int num,
	const char_type *expected1,
	const unsigned int offset,
	const unsigned int max)
{
	typedef RegexType regex_type;
	typedef CharT char_type2;
	typedef UtfTag utf_tag;
	typedef std::basic_string<char_type2> string_type;
	typedef srell::match_results<const CharT *> mr_type;
//	typedef srell::match_results<const typename string_type::const_iterator> smr_type;

	string_type str(to_utf<char_type2>(str1, utf_tag()));
	string_type exp(to_utf<char_type2>(exp1, utf_tag()));
	std::vector<string_type> expected;
	srell::regex_constants::syntax_option_type so = srell::regex_constants::ECMAScript;
	srell::regex_constants::match_flag_type mf = srell::regex_constants::match_default;
	otherflags::type of = otherflags::none;

	const std::string flagstr2 = parse_flagstring(so, mf, of, flagstr);
	const bool search = (of & otherflags::regex_match) ? false : true;
	const bool iterator3 = (of & otherflags::three_iterators) ? true : false;
	const bool global = (of & otherflags::global) ? true : false;
	const bool matchall = (of & otherflags::matchall) ? true : false;
	const bool errortest = (of & otherflags::errortest) ? true : false;
	const bool namedgroup = (of & otherflags::namedgroup) ? true : false;

	for (unsigned int i = 0; i < num; ++i)
	{
		const string_type s(to_utf<char_type2>(expected1, utf_tag()));

		expected.push_back(s);
		++expected1;
	}

	regex_type re;
	mr_type mr;
//	smr_type smr;
	bool b = false;
	unsigned int num_of_failures = 0;

	const std::string strfc(convert_to_utf8c(str));	//  For Console.
	const std::string expfc(convert_to_utf8c(exp));

#ifdef DUP_CHECK
	static std::map<std::string, bool> tried;
	const std::string comb(flagstr + strfc + expfc);
	if (tried.count(comb))
		std::fprintf(stdout, "[Warning] /%s/%s.exec(\"%s\") has been checked.\n", expfc.c_str(), flagstr, strfc.c_str());
	else
		tried[comb] = true;
#endif

	try
	{
		re.assign(exp, so);

		if (errortest)	//  Reaching here means that an exception has not been thrown.
		{
			std::fprintf(stdout, "\t/%s/\nResult: Failed (expected %u \"%s\", but no error thrown).\n\n", expfc.c_str(), offset, srell::regex_error(offset).what());
			return false;
		}

		const CharT *const begin = str.c_str() + offset;
		const CharT *const end = str.c_str() + str.size();
		const CharT *const lblimit = !iterator3 ? begin : str.c_str();
		string_type matched;
		string_type msg;
		string_type gname;

		if (search)
		{
			b = srell::regex_search(begin, end, lblimit, mr, re, mf);
		}
		else
		{
			b = srell::regex_match(begin, end, mr, re, mf);
		}

		std::fprintf(stdout, "\t/%s/%s.%sch(\"%s\");", expfc.c_str(), flagstr2.c_str(), search ? "sear" : "mat", strfc.c_str());

		if (offset != 0)
			std::fprintf(stdout, " offset:%u\n", offset);
		else
			std::fprintf(stdout, "\n");

		if (max > 1)
			std::fprintf(stdout, "\t%u times\n", max);

		std::fprintf(stdout, "\t%s.\n", b ? "Found" : "Not Found");

		unsigned int matchcount = 0u;

		for (; mr.size() != 0;)
		{
			if (global || matchall)
				std::fprintf(stdout, "\t#%.2u\n", static_cast<unsigned int>(matchcount / (matchall ? mr.size() : 1)));

			for (srell::cmatch::size_type i = 0; i < mr.size(); ++i)
			{
				std::fprintf(stdout, "\tm[%u] = ", static_cast<unsigned int>(i));

				if (namedgroup)
				{
					typename mr_type::gnamemap_type::gname_string gntmp0 = mr.lookup_gname_(static_cast<unsigned int>(i));

					gname.clear();
					if (gntmp0.size())
					{
						string_type gntmp1(&gntmp0[0], &gntmp0[gntmp0.size()]);

						if (&mr[i] == &mr[gntmp1])
							gname = simple_conv<string_type>(" <") + gntmp1 + simple_conv<string_type>(">");
					}
				}

				if (mr[i].matched)
				{
					matched = mr[i].str() + gname;
					msg = simple_conv<string_type>("\"") + matched + simple_conv<string_type>("\"") + simple_conv<string_type>(" (%u-%u)");
				}
				else
					msg = matched = simple_conv<string_type>("(undefined)") + gname;

				const std::size_t expno = matchcount++;

				if (expno < expected.size())
				{
					if (matched == expected.operator[](expno))
						msg += simple_conv<string_type>("; OK.");
					else
					{
						msg += simple_conv<string_type>("; failed... (expected: \"") + expected.operator[](expno) + simple_conv<string_type>("\")");
						++num_of_failures;
					}
				}
				else
				{
					msg += simple_conv<string_type>("; failed... (should not match)");
					++num_of_failures;
				}

				msg += simple_conv<string_type>("\n");
				const std::string msgfc(convert_to_utf8c(msg));
				std::fprintf(stdout, msgfc.c_str(), mr.position(i), mr.length(i));
				if (global)
					break;
			}

			if (global || matchall)
			{
				const CharT *begin2 = mr[0].second;

				if (begin2 == mr.prefix().first)
				{
					if (matchcount == expected.size())
						break;

					if (begin2 != end)
						regex_type::traits_type::utf_traits::codepoint_inc(begin2, end);
				}

				b = srell::regex_search(begin2, end, lblimit, mr, re, mf);
			}
			else
				break;
		}

		if (expected.size() != matchcount)
		{
			std::fprintf(stdout, "\tm.size() == %u; should be %u.\n", static_cast<unsigned int>(mr.size()), static_cast<unsigned int>(expected.size()));
			++num_of_failures;
		}

		std::fprintf(stdout, "Result: %s.\n\n", num_of_failures ? "Failed" : "OK");

		return num_of_failures == 0;
	}
	catch (const srell::regex_error &e)
	{
		std::fprintf(stdout, "Error (regex_error): %d \"%s\"\n\t/%s/%s;\n", e.code(), e.what(), expfc.c_str(), flagstr2.c_str());

		if (errortest)
		{
			if (e.code() == static_cast<srell::regex_constants::error_type>(offset))
			{
				std::fprintf(stdout, "Result: OK.\n\n");
				return true;
			}

			std::fprintf(stdout, "Result: Failed... (expected: %u \"%s\")\n\n", offset, srell::regex_error(offset).what());
		}
		else
			std::fprintf(stdout, "Result: Failed.\n\n");
	}
	catch (const std::exception &e)
	{
		std::fprintf(stdout, "Error (std::exception): \"%s\"\nResult: Failed.\n\n", e.what());
	}
	return false;
}

bool conf_test(
	const constants::utf_type utf_type,
	const char_type *const str,
	const char_type *const exp,
	const char *const flagstr,
	const unsigned int num,
	const char_type *const expected,
	const unsigned int offset = 0,
	const unsigned int max = 1)
{
	switch (utf_type)
	{
	case constants::utf8c:
		return conf_test<srell::u8cregex, char, utf8_tag>(str, exp, flagstr, num, expected, offset, max);

#if defined(SRELL_HAS_UTF1632W)
	case constants::utf16or32w:
		return conf_test<srell::u1632wregex, wchar_t,
#if (SRELL_HAS_UTF1632W == 16)
				utf16_tag
#else
				utf32_tag
#endif
			>(str, exp, flagstr, num, expected, offset, max);
#endif

#if defined(SRELL_HAS_U8TYPE)
	case constants::utf8:
		return conf_test<srell::basic_regex<u8char_type, srell::u8regex_traits<u8char_type> >, u8char_type, utf8_tag>(str, exp, flagstr, num, expected, offset, max);
#endif

#if defined(SRELL_HAS_U1632TYPE)
	case constants::utf16:
		return conf_test<srell::basic_regex<u16char_type, srell::u16regex_traits<u16char_type> >, u16char_type, utf16_tag>(str, exp, flagstr, num, expected, offset, max);

	case constants::utf32:
		return conf_test<srell::basic_regex<u32char_type>, u32char_type, utf32_tag>(str, exp, flagstr, num, expected, offset, max);
#endif

	case constants::w:
		return conf_test<srell::wregex, wchar_t, utf0_tag>(str, exp, flagstr, num, expected, offset, max);

	case constants::c:
	case constants::unknown:
	default:
		return conf_test<srell::regex, char, utf0_tag>(str, exp, flagstr, num, expected, offset, max);
	}
}

struct options
{
	constants::utf_type utype;
	int errorno;

	options(const int argc, const char *const *const argv)
		: utype(constants::unknown)
		, errorno(0)
	{
		if (argc >= 2)
		{
			const std::size_t len = std::strlen(argv[1]);

			if (len >= 4 && std::memcmp(argv[1], "utf", 3) == 0)
			{
				if (argv[1][3] == '8')
				{
					if (std::strcmp(argv[1] + 4, "c") == 0)
						utype = constants::utf8c;
#if defined(SRELL_HAS_U8TYPE)
					else if (argv[1][4] == 0)
						utype = constants::utf8;
#endif
					else
						goto UNKNOWN_TYPE;
				}
				else if (len >= 5 && std::memcmp(argv[1] + 3, "16", 2) == 0)
				{
#if defined(SRELL_HAS_UTF16W)
					if (std::strcmp(argv[1] + 5, "w") == 0)
						utype = constants::utf16or32w;
					else
#endif
#if defined(SRELL_HAS_U1632TYPE)
					if (argv[1][5] == 0)
						utype = constants::utf16;
#endif
					else
						goto UNKNOWN_TYPE;
				}
				else if (len >= 5 && std::memcmp(argv[1] + 3, "32", 2) == 0)
				{
#if defined(SRELL_HAS_UTF32W)
					if (std::strcmp(argv[1] + 5, "w") == 0)
						utype = constants::utf16or32w;
					else
#endif
#if defined(SRELL_HAS_U1632TYPE)
					if (argv[1][5] == 0)
						utype = constants::utf32;
#endif
					else
						goto UNKNOWN_TYPE;
				}
				else
					goto UNKNOWN_TYPE;
			}
			else
				goto UNKNOWN_TYPE;

			return;
		}

		errorno = -1;

		PRINT_USAGE:
		std::fputs("Usage: conftest testtype\n", stdout);
#if defined(SRELL_HAS_U8TYPE)
		std::fputs("    utf8    u8regex\n", stdout);
#endif
#if defined(SRELL_HAS_U1632TYPE)
		std::fputs("    utf16   u16regex\n", stdout);
		std::fputs("    utf32   u32regex\n", stdout);
#endif
		std::fputs("    utf8c   u8cregex (UTF-8 with char)\n", stdout);
#if defined(SRELL_HAS_UTF16W)
		std::fputs("    utf16w  u1632wregex (UTF-16 with wchar_t)\n", stdout);
#endif
#if defined(SRELL_HAS_UTF32W)
		std::fputs("    utf32w  u1632wregex (UTF-32 with wchar_t)\n", stdout);
#endif
		return;

		UNKNOWN_TYPE:
		std::fprintf(stdout, "[Error] Unknown type \"%s\" specified.\n", argv[1]);
		errorno = 1;
		goto PRINT_USAGE;
	}
};
//  struct options

int main(const int argc, const char *const argv[])
{
	options opts(argc, argv);
//	const unsigned int count = 100000;
	unsigned int num_of_tests = 0;
	unsigned int num_of_tests_passed = 0;
//	unsigned int num_of_benches = 0;
//	unsigned int num_of_benches_passed = 0;
	unsigned int skipped = 0;

	const char_type *re = STR("");
	const char_type *str = re;
	const char_type *expected = re;

	const constants::utf_type utype = opts.utype;
	const bool is_utf8 = utype == constants::utf8 || utype == constants::utf8c;

	if (opts.errorno)
		return opts.errorno;

	const testdata *t = tests;

	for (;; ++t)
	{
		if (t->type == 1)
			break;

		std::fputs(t->title, stdout);
		if (t->type == 8 && !is_utf8)
		{
			std::fputs("[Info] This test is specific to UTF-8. Skipped...\n\n", stdout);
			++skipped;
			continue;
		}

		if (t->re)
			re = t->re;
		if (t->str)
			str = t->str;
		if (t->expected)
			expected = t->expected;

		if (conf_test(utype, str, re, t->flags, t->number, expected, t->offset))
			++num_of_tests_passed;

		++num_of_tests;
	}

	const char *typetable[] = {
		"Unknown", "UTF-8", "UTF-16", "UTF-32", "UTF-8 with char",
#if defined(SRELL_HAS_UTF1632W)
#if (SRELL_HAS_UTF1632W == 16)
		"UTF-16 with wchar_t",
#else
		"UTF-32 with wchar_t",
#endif
#else
		"", "",
#endif
		//  These two types will pass all tests only when it has width of at least 21 bits.
		"char", "wchar_t"
	};

	std::fprintf(stdout, "TestType: %s (%d)\n", typetable[opts.utype], opts.utype);

	std::fprintf(stdout, "Results of tests: %u/%u (%.1lf%%) passed. (%u skipped).\n", num_of_tests_passed, num_of_tests, num_of_tests ? static_cast<double>(num_of_tests_passed) * 100.0 / num_of_tests : -1.0, skipped);
//	std::fprintf(stdout, "Results of benchmarks: %u/%u passed.\n", num_of_benches_passed, num_of_benches);

	return static_cast<int>(num_of_tests - num_of_tests_passed);
}
