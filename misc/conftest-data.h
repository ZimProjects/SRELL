//  Generated on 2025/05/18.

struct testdata
{
	int type;
	const char *title;
	const char *flags;
	const char_type *re;
	const char_type *str;
	unsigned int offset;
	unsigned int number;
	const char_type *expected;
};

testdata tests[] = {
	//  Compilation error.
{
	0, "Compilation error 01: Bad range.\n",
	"E",
	RE("[b-a]"),
	STR(""),
	srell::regex_constants::error_range, 0,
	STR0("")
},
{
	0, "Compilation error 02: Unknown escape in charclass.\n",
	"E",
	RE("[\\1]()"),
	STR(""),
	srell::regex_constants::error_escape, 0,
	STR0("")
},

#if !defined(NO_VMODE)
{
	0, "Compilation error 03: Complement of pos.\n",
	"Ev",
	RE("\\P{Basic_Emoji}"),
	STR(""),
	srell::regex_constants::error_complement, 0,
	STR0("")
},
{
	0, "Compilation error 04: Complement of pos charclass.\n",
	"Ev",
	RE("[^\\p{Basic_Emoji}]"),
	STR(""),
	srell::regex_constants::error_complement, 0,
	STR0("")
},
#endif	//  !defined(NO_VMODE)

{
	0, "Compilation error 05: Out of range backref number 1.\n",
	"E",
	RE("a\\1"),
	STR(""),
	srell::regex_constants::error_backref, 0,
	STR0("")
},
{
	0, "Compilation error 06: Out of range backref number 2.\n",
	"E",
	RE("(?<=\\1a)"),
	STR(""),
	srell::regex_constants::error_backref, 0,
	STR0("")
},
	//  Backreference.
{
	0, "Backref 01.\n",
	"",
	RE("^(.*)*b\\1$"),
	STR("aaaabaa"),
	0, 2,
	STR0("aaaabaa")
	STR0("aa")
},
{
	0, "Backref 02.\n",
	"",
	RE("^(.*)*b\\1\\1$"),
	STR("aaaabaaaa"),
	0, 2,
	STR0("aaaabaaaa")
	STR0("aa")
},
{
	0, "Backref 03.\n",
	"",
	RE("(.*?)*b\\1"),
	STR("ab"),
	0, 2,
	STR0("b")
	STR0("(undefined)")
},
{
	0, "Backref 04.\n",
	"",
	RE("(a(.)a|\\2(.)b){2}"),
	STR("acaaabbb"),
	0, 4,
	STR0("aaabb")
	STR0("bb")
	STR0("(undefined)")
	STR0("b")
},
{
	0, "Backref 05.\n",
	"",
	RE("(a*)(b)*\\1\\1\\1"),
	STR("aabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaa"),
	0, 3,
	STR0("aabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaa")
	STR0("aa")
	STR0("b")
},
{
	0, "Backref 06.\n",
	"",
	RE("()(?:\\1+)*"),
	STR(""),
	0, 2,
	STR0("")
	STR0("")
},
	//  Capture.
{
	0, "Capture 01.\n",
	"",
	RE("(.*)*b"),
	STR("aaaaaaaaaab"),
	0, 2,
	STR0("aaaaaaaaaab")
	STR0("aaaaaaaaaa")
},
{
	0, "Capture 02.\n",
	"",
	RE("(.*)+b"),
	STR("aaaaaaaaaab"),
	0, 2,
	STR0("aaaaaaaaaab")
	STR0("aaaaaaaaaa")
},
{
	0, "Capture 03.\n",
	"",
	RE("(.*){2,}b"),
	STR("aaaaaaaaaab"),
	0, 2,
	STR0("aaaaaaaaaab")
	STR0("")
},
{
	0, "Capture 04.\n",
	"",
	RE("(?=(a+))(abc)"),
	STR("aaaaaaaaaabc"),
	0, 3,
	STR0("abc")
	STR0("a")
	STR0("abc")
},
{
	0, "Capture 05.\n",
	"",
	RE("(\\d{4}[-]){3}\\d{3,4}"),
	STR("1234-5678-9012-345"),
	0, 2,
	STR0("1234-5678-9012-345")
	STR0("9012-")
},

	//  Repeat.
{
	0, "Repeat 01: Capturing group.\n",
	"",
	RE("(([A-Z]+)|([a-z]+))+"),
	STR("ABCabcDEFdef"),
	0, 4,
	STR0("ABCabcDEFdef")
	STR0("def")
	STR0("(undefined)")
	STR0("def")
},
{
	0, "Repeat 02: Non-capturing group.\n",
	"",
	RE("(?:([A-Z]+)|([a-z]+))+"),
	STR("ABCabcDEFdef"),
	0, 3,
	STR0("ABCabcDEFdef")
	STR0("(undefined)")
	STR0("def")
},
	//  Non-ASCII character.
{
	0, "Non-ASCII 01.\n",
	"",
	RE("\\u3042*\\u3044"),
	STR("\\u3042\\u3042\\u3042\\u3044"),
	0, 1,
	STR0("\\u3042\\u3042\\u3042\\u3044")
},
{
	0, "Non-ASCII 02.\n",
	"",
	RE("(.).\\1"),
	STR("\\u3068\\u307E\\u307E\\u3068\\u30C8\\u30DE\\u30DE\\u30C8\\u3068\\u307E\\u3068"),
	0, 2,
	STR0("\\u3068\\u307E\\u3068")
	STR0("\\u3068")
},
	//  Case-insensitive, icase.
{
	0, "Icase 01: Icase range.\n",
	"iG",
	RE("[Z-a]+"),
	STR("0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"),
	0, 3,
	STR0("A")
	STR0("Z[\\]^_`a")
	STR0("z")
},
{
	0, "Icase 02: Sigma, case-sensitive.\n",
	"",
	RE("\\u03C3+"),
	STR("\\u03A3\\u03C3\\u03C2\\u30A2\\u30A4\\u30A6\\u{1B000}\\u30AA\\u3084\\u3086\\u{1B001}\\u3088"),
	0, 1,
	STR0("\\u03C3")
},
{
	0, "Icase 03: Sigma, nocase.\n",
	"i",
	RE("\\u03C3+"),
	STR("\\u03A3\\u03C3\\u03C2\\u30A2\\u30A4\\u30A6\\u{1B000}\\u30AA\\u3084\\u3086\\u{1B001}\\u3088"),
	0, 1,
	STR0("\\u03A3\\u03C3\\u03C2")
},
	//  Unicode property.
{
	0, "Unicode property 01: Kana.\n",
	"",
	RE("(?:(\\p{sc=Hiragana}+)|(\\p{sc=Katakana}+))+"),
	STR("\\u03A3\\u03C3\\u03C2\\u30A2\\u30A4\\u30A6\\u{1B000}\\u30AA\\u3084\\u3086\\u{1B001}\\u3088"),
	0, 3,
	STR0("\\u30A2\\u30A4\\u30A6\\u{1B000}\\u30AA\\u3084\\u3086\\u{1B001}\\u3088")
	STR0("\\u3084\\u3086\\u{1B001}\\u3088")
	STR0("(undefined)")
},
{
	0, "Unicode property 02: Kana in charclass.\n",
	"",
	RE("[\\p{sc=Hiragana}\\p{sc=Katakana}]+"),
	STR("\\u03A3\\u03C3\\u03C2\\u30A2\\u30A4\\u30A6\\u{1B000}\\u30AA\\u3084\\u3086\\u{1B001}\\u3088"),
	0, 1,
	STR0("\\u30A2\\u30A4\\u30A6\\u{1B000}\\u30AA\\u3084\\u3086\\u{1B001}\\u3088")
},
{
	0, "Unicode property 03: Kana in complement charclass.\n",
	"",
	RE("[^\\p{sc=Hiragana}]+"),
	STR("\\u03A3\\u03C3\\u03C2\\u30A2\\u30A4\\u30A6\\u{1B000}\\u30AA\\u3084\\u3086\\u{1B001}\\u3088"),
	0, 1,
	STR0("\\u03A3\\u03C3\\u03C2\\u30A2\\u30A4\\u30A6\\u{1B000}\\u30AA")
},
	//  regex_match, match_continuous.
{
	0, "regex_match 01.\n",
	"M",
	RE("\\w*"),
	STR("abcd"),
	0, 1,
	STR0("abcd")
},
{
	0, "regex_match 02.\n",
	"M",
	RE("\\w*"),
	STR("@abcd"),
	0, 0,
	STR0("")
},
{
	0, "regex_match 03.\n",
	"M",
	RE("\\w*"),
	STR("abcd@"),
	0, 0,
	STR0("")
},
{
	0, "match_continous 01.\n",
	"y",
	RE("\\w+"),
	STR("abcd"),
	0, 1,
	STR0("abcd")
},
{
	0, "match_continuous 02.\n",
	"y",
	RE("\\w+"),
	STR("@abcd"),
	0, 0,
	STR0("")
},
{
	0, "match_continuous 03.\n",
	"y",
	RE("\\w+"),
	STR("abcd@"),
	0, 1,
	STR0("abcd")
},
	//  Boyer-Moore-Horspool. BMH.
{
	0, "BMH 01s: Case-sensitive search 1.\n",
	"",
	RE("AbCd"),
	STR("AbCd"),
	0, 1,
	STR0("AbCd")
},
{
	0, "BMH 01m: Case-sensitive match 1.\n",
	"M",
	RE("AbCd"),
	STR("AbCd"),
	0, 1,
	STR0("AbCd")
},
{
	0, "BMH 02s: Case-sensitive search 2.\n",
	"",
	RE("AbCd"),
	STR("aBcD"),
	0, 0,
	STR0("")
},
{
	0, "BMH 02m: Case-sensitive match 2.\n",
	"M",
	RE("AbCd"),
	STR("aBcD"),
	0, 0,
	STR0("")
},
{
	0, "BMH 03s: Icase search.\n",
	"i",
	RE("AbCd"),
	STR("aBcD"),
	0, 1,
	STR0("aBcD")
},
{
	0, "BMH 03m: Icase match.\n",
	"iM",
	RE("AbCd"),
	STR("aBcD"),
	0, 1,
	STR0("aBcD")
},
{
	0, "BMH 04a: Case-folded UTF-8 representation becomes longer in code unit.\n",
	"8i",
	RE("\\u023Az"),	//  023a (2 octets) -> 2c65 (3 octets).
	STR("zzz\\u{023A}zzzz"),	//  Code unit(s): 1, 1, 1, 2, 1, 1, 1, 1.
	0, 1,
	STR0("\\u{023A}z")
},
{
	0, "BMH 04b: Case-folded UTF-8 representation becomes longer in code unit.\n",
	"8i",
	RE("\\u023Ez"),	//  023e (2 octets) -> 2c66 (3 octets).
	STR("zzz\\u{023E}zzzz"),
	0, 1,
	STR0("\\u{023E}z")
},
	//  Broken/corrupted UTF-8.
{
	0, "Broken UTF-8 01: Match found.\n",
	"M",
	RE("[\\w ]+"),
	STR("ABC 0123456789 xyz"),
	0, 1,
	STR0("ABC 0123456789 xyz")
},
{
	8, "Broken UTF-8 02: Match not found. \\x80 prevents fullmatch.\n",
	"M",
	RE("\\S*"),
	STR("\x80""ABC 0123456789 xyz"),
	0, 0,
	STR0("")
},
{
	8, "Broken UTF-8 03: Search failure. \\x80 prevents search with ^.\n",
	"",
	RE("^\\S*$"),
	STR("\x80""ABC 0123456789 xyz"),
	0, 0,
	STR0("")
},
{
	8, "Broken UTF-8 04: Search failure. \\x80 prevents search with ^ and match_continuous.\n",
	"y",
	RE("^\\S*$"),
	STR("\x80""ABC 0123456789 xyz"),
	0, 0,
	STR0("")
},
{
	8, "Broken UTF-8 05: Search failure. \\x80 prevents search with match_continuous.\n",
	"y",
	RE("\\S*$"),
	STR("\x80""ABC 0123456789 xyz"),
	0, 0,
	STR0("")
},
{
	8, "Broken UTF-8 06: Search failure. \\x80 prevents search with $.\n",
	"",
	RE("\\S+$"),
	STR("ABC 0123456789 xyz\x80"),
	0, 0,
	STR0("")
},
{
	8, "Broken UTF-8 07a: 0 width match after \\x80 succeeds 1.\n",
	"",
	RE("\\S*$"),
	STR("ABC 0123456789 xyz\x80"),
	0, 1,
	STR0("")
},
{
	8, "Broken UTF-8 07b: 0 width match after \\x80 succeeds 2.\n",
	"",
	RE("$"),
	STR("\x80"),
	0, 1,
	STR0("")
},
{
	0, "Broken UTF-8 08: Match found, empty string.\n",
	"M",
	RE("^$"),
	STR(""),
	0, 1,
	STR0("")
},
{
	8, "Broken UTF-8 09: Match not found. \\x80 must not be ignored by match with /^$/.\n",
	"M",
	RE("^$"),
	STR("\x80"),
	0, 0,
	STR0("")
},
{
	8, "Broken UTF-8 10: Search failure. \\x80 must not be ignored by search with /^$/.\n",
	"",
	RE("^$"),
	STR("\x80"),
	0, 0,
	STR0("")
},
{
	8, "Broken UTF-8 11: Match not found. \\x80 prevents match with /$/.\n",
	"M",
	RE("$"),
	STR("\x80"),
	0, 0,
	STR0("")
},
{
	8, "Broken UTF-8 & BMH 01: Preceding \\x80 must be ignored.\n",
	"",
	RE("ABC"),
	STR("\x80""ABC"),
	0, 1,
	STR0("ABC")
},
{
	8, "Broken UTF-8 & BMH 02: Trailing \\x80 must be ignored.\n",
	"",
	RE("ABC"),
	STR("ABC\x80"),
	0, 1,
	STR0("ABC")
},
{
	8, "Broken UTF-8 & BMH 03: Icase, preceding \\x80 must be igored.\n",
	"i",
	RE("ABC"),
	STR("\x80""abc"),
	0, 1,
	STR0("abc")
},
{
	8, "Broken UTF-8 & BMH 04: Icase, trailing \\x80 must be igored.\n",
	"i",
	RE("ABC"),
	STR("abc\x80"),
	0, 1,
	STR0("abc")
},
{
	8, "Broken UTF-8 & BMH 05: Non-ASCII, trailing \\x80.\n",
	"",
	RE("\\u3042\\u3044"),
	STR("\\u3042\\u3044\x80"),
	0, 1,
	STR0("\\u3042\\u3044")
},
{
	8, "Broken UTF-8 & BMH 06: Non-ASCII, icase, trailing \\x80.\n",
	"i",
	RE("\\u3042\\u3044"),
	STR("\\u3042\\u3044\x80"),
	0, 1,
	STR0("\\u3042\\u3044")
},
{
	8, "Broken UTF-8 & BMH 07: Non-ASCII, embedded \\x80 1.\n",
	"",
	RE("\\u3042\\u3044"),
	STR("\\u3042")STR("\x80")STR("\\u3044"),
	0, 0,
	STR0("")
},
{
	8, "Broken UTF-8 & BMH 08: Non-ASCII, icase, embedded \\x80 1.\n",
	"i",
	RE("\\u3042\\u3044"),
	STR("\\u3042")STR("\x80")STR("\\u3044"),
	0, 0,
	STR0("")
},
{
	8, "Broken UTF-8 & BMH 09: Non-ASCII, embedded \\x80 2.\n",
	"",
	RE("\\u3042\\u3044"),
	STR("\\u3042\x80\\u3044\\u3042\\u3044"),
	0, 1,
	STR0("\\u3042\\u3044")
},
{
	8, "Broken UTF-8 & BMH 10: Non-ASCII, icase, embedded \\x80 2.\n",
	"i",
	RE("\\u3042\\u3044"),
	STR("\\u3042\x80\\u3044\\u3042\\u3044"),
	0, 1,
	STR0("\\u3042\\u3044")
},

	//  Duplicate named capturing group.
{
	0, "Duplicate named capture 01.\n",
	"N",
	RE("(?<year>\\d{4})[/-](?<month>[A-Za-z]+)[/-](?<day>\\d{1,2})|(?<day>\\d{1,2})[/-](?<month>[A-Za-z]+)[/-](?<year>\\d{4})|(?<month>[A-Za-z]+)[/-](?<day>\\d{1,2})[/-](?<year>\\d{4})"),
	STR("2024-May-26"),
	0, 10,
	STR0("2024-May-26")
	STR0("2024 <year>")
	STR0("May <month>")
	STR0("26 <day>")
	STR0("(undefined)")
	STR0("(undefined)")
	STR0("(undefined)")
	STR0("(undefined)")
	STR0("(undefined)")
	STR0("(undefined)")
},
{
	0, "Duplicate named capture 02.\n",
	"N",
	RE("(?<year>\\d{4})[/-](?<month>[A-Za-z]+)[/-](?<day>\\d{1,2})|(?<day>\\d{1,2})[/-](?<month>[A-Za-z]+)[/-](?<year>\\d{4})|(?<month>[A-Za-z]+)[/-](?<day>\\d{1,2})[/-](?<year>\\d{4})"),
	STR("26-May-2024"),
	0, 10,
	STR0("26-May-2024")
	STR0("(undefined)")
	STR0("(undefined)")
	STR0("(undefined)")
	STR0("26 <day>")
	STR0("May <month>")
	STR0("2024 <year>")
	STR0("(undefined)")
	STR0("(undefined)")
	STR0("(undefined)")
},
{
	0, "Duplicate named capture 03.\n",
	"N",
	RE("(?<year>\\d{4})[/-](?<month>[A-Za-z]+)[/-](?<day>\\d{1,2})|(?<day>\\d{1,2})[/-](?<month>[A-Za-z]+)[/-](?<year>\\d{4})|(?<month>[A-Za-z]+)[/-](?<day>\\d{1,2})[/-](?<year>\\d{4})"),
	STR("May-26-2024"),
	0, 10,
	STR0("May-26-2024")
	STR0("(undefined)")
	STR0("(undefined)")
	STR0("(undefined)")
	STR0("(undefined)")
	STR0("(undefined)")
	STR0("(undefined)")
	STR0("May <month>")
	STR0("26 <day>")
	STR0("2024 <year>")
},
	//  Lookbehind.

{
	0, "Three iterators 01: Search succeeds.\n",
	"3",
	RE("(?<=^\\d+).+"),
	STR("0123abcd"),
	4, 1,
	STR0("abcd")
},
{
	0, "Three iterators 02: Search fails.\n",
	"3",
	RE("(?<=^\\d+).+"),
	STR("0123abcd"),
	5, 0,
	STR0("")
},
{
	0, "Three iteratorts 03: Match fails.\n",
	"3M",
	RE("(?<=^\\d+).+"),
	STR("0123abcd"),
	4, 0,
	STR0("")
},
	//  Character class escapes.
{
	0, "CharacterClassEscape 01.\n",
	"",
	RE("\\w+"),
	STR("abcd,efgh"),
	0, 1,
	STR0("abcd")
},
{
	0, "CharacterClassEscape 02.\n",
	"",
	RE("\\W+"),
	STR("abcd,efgh"),
	0, 1,
	STR0(",")
},
{
	0, "CharacterClassEscape 03.\n",
	"",
	RE("[\\w]+"),
	STR("abcd,efgh"),
	0, 1,
	STR0("abcd")
},
{
	0, "CharacterClassEscape 04.\n",
	"",
	RE("[\\W]+"),
	STR("abcd,efgh"),
	0, 1,
	STR0(",")
},
	//  Icase and compelement of \p problem:
	//  https://github.com/tc39/proposal-regexp-v-flag/issues/30
{
	0, "U-mode icase property 01: Complement.\n",
	"i",
	RE("\\P{Ll}+"),
	STR("aAbBcCdD4#"),
	0, 1,
	STR0("aAbBcCdD4#")
},
{
	0, "U-mode icase property 02: In charclass.\n",
	"i",
	RE("[\\p{Ll}]+"),
	STR("aAbBcCdD4#"),
	0, 1,
	STR0("aAbBcCdD")
},
{
	0, "U-mode icase peroperty 03: Double complements.\n",
	"i",
	RE("[^\\P{Ll}]+"),
	STR("aAbBcCdD4#"),
	0, 0,
	STR0("")
},

#if !defined(NO_VMODE)
	//  Vmode, v-mode, v mode, v-flag mode, v flag, unicodesets, pos.
{
	0, "Property complement in charclass 01: U-mode.\n",
	"",
	RE("[\\P{Ll}]+"),
	STR("ABCDefgh"),
	0, 1,
	STR0("ABCD")
},
{
	0, "Property complement in charclass 02: V-mode.\n",
	"v",
	RE("[\\P{Ll}]+"),
	STR("ABCDefgh"),
	0, 1,
	STR0("ABCD")
},
{
	0, "Property complement in charclass 03: U-mode, icase.\n",
	"i",
	RE("[\\P{Ll}]+"),
	STR("ABCDefgh"),
	0, 1,
	STR0("ABCDefgh")
},
{
	0, "Property complement in charclass 04: V-mode, icase (Different from u-mode).\n",
	"iv",
	RE("[\\P{Ll}]+"),
	STR("ABCDefgh"),
	0, 0,
	STR0("")
},
	//  V-mode, properties.
{
	0, "V-mode property 01.\n",
	"vA",
	RE("\\p{RGI_Emoji}"),
	STR("\\u{1F468}\\u200D\\u2764\\uFE0F\\u200D\\u{1F48B}\\u200D\\u{1F468}\\u{1F3FB}"),
	0, 2,
	STR0("\\u{1F468}\\u200D\\u2764\\uFE0F\\u200D\\u{1F48B}\\u200D\\u{1F468}")
	STR0("\\u{1F3FB}")
},
{
	0, "V-mode property 02.\n",
	"v",
	RE("^\\p{RGI_Emoji}$"),
	STR("\\u{1F468}\\u200D\\u2764\\uFE0F\\u200D\\u{1F48B}\\u200D\\u{1F468}\\u{1F3FB}"),
	0, 0,
	STR0("")
},
	//  V-mode, strings.
{
	0, "V-mode string 01: Longest string first matched 01.\n",
	"v",
	RE("[\\q{abc|cde|ab|d|f}]+"),	//  abc -> d. Not ab -> cde -> f.
	STR("abcde"),
	0, 1,
	STR0("abcd")
},
{
	0, "V-mode string 02: $ causes backtracking.\n",
	"v",
	RE("[\\q{abc|cde|ab|d|f}]+$"),	//  ab -> cde -> f.
	STR("abcdef"),
	0, 1,
	STR0("abcdef")
},
{
	0, "V-mode 03: String icase 01.\n",
	"iv",
	RE("[\\q{cH|C}]+"),
	STR("cch"),
	0, 1,
	STR0("cch")
},
{
	0, "V-mode 04: String icase 02.\n",
	"iv",
	RE("[\\q{ChH|cH}]+"),
	STR("chchh"),
	0, 1,
	STR0("chchh")
},
#endif	//  !defined(NO_VMODE)

	//  Undefined backreferences.
{
	0, "UndefinedBackref 01.\n",
	"",
	RE("(\\1*)"),
	STR(""),
	0, 2,
	STR0("")
	STR0("")
},
{
	0, "UndefinedBackref 02.\n",
	"",
	RE("\\1+()"),
	STR(""),
	0, 2,
	STR0("")
	STR0("")
},
{
	0, "UndefinedBackref 03.\n",
	"",
	RE("^((\\1+)|\\d)+123$"),
	STR("000123"),
	0, 3,
	STR0("000123")
	STR0("0")
	STR0("(undefined)")
},
{
	0, "UndefinedBackref 04.\n",
	"",
	RE("(?:\\1+)*()"),
	STR(""),
	0, 2,
	STR0("")
	STR0("")
},
{
	0, "UndefinedBackref 05.\n",
	"",
	RE("(\\2)(\\1)"),
	STR(""),
	0, 3,
	STR0("")
	STR0("")
	STR0("")
},
{
	0, "UndefinedBackref 06.\n",
	"",
	RE("(?:(\\d+)|([a-z]*))\\1_"),
	STR("_"),
	0, 3,
	STR0("_")
	STR0("(undefined)")
	STR0("")
},
	//  Flag modifiers.
{
	0, "UBModifiers 01: (?i) 01.\n",
	"",
	RE("(?i)aeiou"),
	STR("AEIOU"),
	0, 1,
	STR0("AEIOU")
},
{
	0, "UBModifiers 02: (?-i) 01.\n",
	"i",
	RE("(?-i)aeiou"),
	STR("AEIOU"),
	0, 0,
	STR0("")
},
{
	0, "UBModifiers 03: (?m) 01.\n",
	"",
	RE("(?m)abc$\\n^def"),
	STR("abc\ndef"),
	0, 1,
	STR0("abc\ndef")
},
{
	0, "UBModifiers 04: (?-m) 01.\n",
	"",
	RE("(?-m)abc$\\n^def"),
	STR("abc\ndef"),
	0, 0,
	STR0("")
},
{
	0, "UBModifiers 05: (?s) 01.\n",
	"",
	RE("(?s)abc.def"),
	STR("abc\ndef"),
	0, 1,
	STR0("abc\ndef")
},
{
	0, "UBModifiers 06: (?-s) 01.\n",
	"",
	RE("(?-s)abc$^def"),
	STR("abc\ndef"),
	0, 0,
	STR0("")
},
{
	0, "Modifiers 01: m-flag #1.\n",
	"",
	RE("(?m:[^]*?$)"),
	STR("abcd\nefgh\n"),
	0, 1,
	STR0("abcd")
},
{
	0, "Modifiers 02: m-flag #2.\n",
	"m",
	RE("(?-m:[^]*?$)"),
	STR("abcd\nefgh\n"),
	0, 1,
	STR0("abcd\nefgh\n")
},
{
	0, "Modifiers 03: s-flag #1.\n",
	"",
	RE("(.*)(?s:.*)"),
	STR("abcd\nefgh\n"),
	0, 2,
	STR0("abcd\nefgh\n")
	STR0("abcd")
},
{
	0, "Modifiers 04: s-flag #2.\n",
	"s",
	RE("(.*)(?-s:.)"),
	STR("abcd\nefgh\n"),
	0, 2,
	STR0("abcd\nefgh")
	STR0("abcd\nefg")
},
{
	0, "Modifiers 05: i-flag #1.\n",
	"",
	RE("(?i:[a-z]+)"),
	STR("aBcD"),
	0, 1,
	STR0("aBcD")
},
{
	0, "Modifiers 06: i-flag #2.\n",
	"i",
	RE("(?-i:[a-z]+)"),
	STR("aBcD"),
	0, 1,
	STR0("a")
},
{
	0, "Modifiers 07: i-flag #3.\n",
	"G",
	RE("(a)(?i:\\1)"),
	STR("aa aA Aa AA"),
	0, 2,
	STR0("aa")
	STR0("aA")
},
{
	0, "Modifiers 08: i-flag #4.\n",
	"iG",
	RE("(a)(?-i:\\1)"),
	STR("aa aA Aa AA"),
	0, 2,
	STR0("aa")
	STR0("AA")
},
{
	0, "Modifiers 09: Nested flags #1.\n",
	"",
	RE("(?i:a(?s:a))"),
	STR("AA"),
	0, 1,
	STR0("AA")
},
	//  Optimisations' side effect check.
	//  gather_nextchars().
{
	0, "OSEC, GNC 01: Greedy and ^.\n",
	"m",
	RE("[^;]*^;?"),
	STR("\n0"),
	0, 1,
	STR0("\n")
},
{
	0, "GNS 02: Non-greedy and ^.\n",
	"m",
	RE("[^;]*?^;?"),
	STR("\n0"),
	0, 1,
	STR0("")
},
{
	0, "OSEC, GNC 03: Greedy and $.\n",
	"m",
	RE("[^;]*$;?"),
	STR("\n;"),
	0, 1,
	STR0("")
},
{
	0, "OSEC, GNC 04: Non-greedy and $.\n",
	"m",
	RE("[^;]*?$;?"),
	STR("\n;"),
	0, 1,
	STR0("")
},
{
	0, "OSEC, GNC 05: Non-multiline.\n",
	"",
	RE("[^;]*^;?"),
	STR("\n0"),
	0, 1,
	STR0("")
},
{
	0, "OSEC, GNC 06: Lookaround, greedy.\n",
	"",
	RE("[^;]*(?<=abc);?"),
	STR("abcd"),
	0, 1,
	STR0("abc")
},
{
	0, "OSEC, GNC 07: Lookaround, non-greedy.\n",
	"",
	RE("[^;]*?(?<=abc);?"),
	STR("abcd"),
	0, 1,
	STR0("abc")
},
{
	0, "OSEC, GNC 08: Lookbehind with $.\n",
	"m",
	RE("(?<=$.*)"),
	STR("abcd"),
	0, 1,
	STR0("")
},
	//  Entry point selector version 1 (Implemented in SRELL 3.000).
{
	0, "OSEC, EPS 01.\n",
	"M",
	RE("[a-z]*, [a-z]"),
	STR("abcd, e"),
	0, 1,
	STR0("abcd, e")
},
{
	0, "OSEC, EPS 02.\n",
	"y",
	RE("[a-z]*, [a-z]"),
	STR("abc, d\nefg, h"),
	0, 1,
	STR0("abc, d")
},
{
	0, "OSEC, EPS 03.\n",
	"",
	RE("[a-z]*?, [a-z]"),
	STR("abc, d\nefg, h"),
	0, 1,
	STR0("abc, d")
},
	//  is_exclusive_sequence().
{
	0, "OSEC, ES 01: Char question char asterisk.\n",
	"",
	RE("a?b*"),
	STR("aaaabbbb"),
	0, 1,
	STR0("a")
},
{
	0, "OSEC, ES 02: Charclass question char asterisk.\n",
	"",
	RE("[AaC-Zc-z]?b*"),
	STR("aaaabbbb"),
	0, 1,
	STR0("a")
},
{
	0, "OSEC, ES 03: Char question char plus.\n",
	"",
	RE("a?b+"),
	STR("aaaabbbb"),
	0, 1,
	STR0("abbbb")
},
{
	0, "OSEC, ES 04: Charclass question char plus.\n",
	"",
	RE("[AaC-Zc-z]?b+"),
	STR("aaaabbbb"),
	0, 1,
	STR0("abbbb")
},
{
	0, "OSEC, ES 05: Non-greedy.\n",
	"",
	RE("a*?"),
	STR("aaaa"),
	0, 1,
	STR0("")
},
{
	0, "OSEC, ES 06: Non-greedy. (Bug190617).\n",
	"",
	RE("a{1,2}?b"),
	STR("aab"),
	0, 1,
	STR0("aab")
},
	//  is_exclusive_sequence(), splitting char class.
{
	0, "OSEC, ES-SCC 01.\n",
	"i",
	RE("[a-z]*ing"),
	STR("SKIING"),
	0, 1,
	STR0("SKIING")
},
{
	0, "OSEC, ES-SCC 02.\n",
	"i",
	RE("S*[a-z]*ING"),
	STR("SKIING"),
	0, 1,
	STR0("SKIING")
},
	//  is_exclusive_sequence(), i-modifiers.
{
	0, "OSEC, ES-IMOD 01.\n",
	"",
	RE("(?i:a*)(A+)"),
	STR("aAaA"),
	0, 2,
	STR0("aAaA")
	STR0("A")
},
{
	0, "OSEC, ES-IMOD 02.\n",
	"",
	RE("a*((?i:A+))"),
	STR("aaaa"),
	0, 2,
	STR0("aaaa")
	STR0("a")
},
{
	0, "OSEC, ES-IMOD 03.\n",
	"i",
	RE("(?-i:a*)(A+)"),
	STR("aAaA"),
	0, 2,
	STR0("aAaA")
	STR0("AaA")
},
{
	0, "OSEC, ES-IMOD 04.\n",
	"i",
	RE("A*((?-i:a+))"),
	STR("aaaa"),
	0, 2,
	STR0("aaaa")
	STR0("a")
},
	//  Simplified counter mechanism.
{
	0, "OSEC, SC 01.\n",
	"i",
	RE("(a[ab]?){4,6}?\\1$"),
	STR("ababababaaa"),
	0, 2,
	STR0("ababababaaa")
	STR0("a")
},
{
	0, "OSEC, SC 02: Greedy.\n",
	"i",
	RE("(a[ab]?){4,6}"),
	STR("aaa"),
	0, 0,
	STR0("")
},
{
	0, "OSEC, SC 03: Non-greedy.\n",
	"i",
	RE("(a[ab]?){4,6}?"),
	STR("aaa"),
	0, 0,
	STR0("")
},
	//  Entry point selector version 2 (Implemented in SRELL 4.019).
{
	0, "OSEC, EPS-v2 01: Greedy counter.\n",
	"",
	RE("[ab]\\w{0,12}c"),
	STR("a0b0c"),
	0, 1,
	STR0("a0b0c")
},
{
	0, "OSEC, EPS-v2 02: Non-greedy counter.\n",
	"",
	RE("[ab]\\w{0,12}?c"),
	STR("a0b0c"),
	0, 1,
	STR0("a0b0c")
},
{
	0, "OSEC, EPS-v2 03: Bug230820-1.\n",
	"",
	RE("(?:(\\d+)[/-])?(\\d{1,2})[/-](\\d{1,2})"),
	STR("2023/8/20"),
	0, 4,
	STR0("2023/8/20")
	STR0("2023")
	STR0("8")
	STR0("20")
},
{
	0, "OSEC, EPS-v2 04: Bug230820-2.\n",
	"",
	RE("(?:(\\d+)[/-])?(\\d{1,2})[/-](\\d{1,2})"),
	STR("2023/08/20"),
	0, 4,
	STR0("2023/08/20")
	STR0("2023")
	STR0("08")
	STR0("20")
},
{
	0, "OSEC, EPS-v2 05: Bug230820-3.\n",
	"",
	RE("\\d(?:(\\d+)[/-])?(\\d{1,2})[/-](\\d{1,2})"),
	STR("2023/08/20"),
	0, 4,
	STR0("2023/08/20")
	STR0("023")
	STR0("08")
	STR0("20")
},
{
	0, "OSEC, EPS-v2 06: Bug230820-4.\n",
	"",
	RE("(?:(?:(\\d+)-)?)+(\\d{1,2})-(\\d{1,2})"),
	STR("2023-8-21"),
	0, 4,
	STR0("2023-8-21")
	STR0("2023")
	STR0("8")
	STR0("21")
},
{
	0, "OSEC, EPS-v2 07: Replacement and restoration of lblim.\n",
	"3",
	RE(".*e(?<=(.*))"),
	STR("0123abcdef"),
	4, 2,
	STR0("abcde")
	STR0("0123abcde")
},
	//  branch_optimisation().
{
	0, "OSEC, BO1 01: Do not enter repeated group.\n",
	"",
	RE("(?:ab)+|cd"),
	STR("ababcd"),
	0, 1,
	STR0("abab")
},
	//  branch_optimisation2().
{
	0, "OSEC, BO2 01: Wrong shortcut.\n",
	"M",
	RE("a|ab|abc"),
	STR("ac"),
	0, 0,
	STR0("")
},
	//  Bugfixes.
{
	0, "Bug210423-1.\n",
	"",
	RE("abc|ab|ac"),
	STR("abc"),
	0, 1,
	STR0("abc")
},
{
	0, "Bug210423-2.\n",
	"",
	RE("abc|ab|ac"),
	STR("ab"),
	0, 1,
	STR0("ab")
},
{
	0, "Bug210423-3.\n",
	"",
	RE("abc|ab|ac"),
	STR("ac"),
	0, 1,
	STR0("ac")
},
{
	0, "Bug210429.\n",
	"",
	RE("mm2|m|mm"),
	STR("m"),
	0, 1,
	STR0("m")
},
{
	0, "Bug220509: Nomikomi.\n",
	"",
	RE("abcd|ab"),
	STR("abc"),
	0, 1,
	STR0("ab")
},
{
	0, "Bug230729: Modification of Lookbehind 16d1.\n",
	"3",
	RE("\\B.ef"),
	STR("abcdefdef"),
	6, 1,
	STR0("def")
},
{
	0, "Bug240113: Lookbehind.\n",
	"",
	RE("(?:(?=(\\w))|b)c$"),
	STR("abc"),
	0, 2,
	STR0("bc")
	STR0("(undefined)")
},
{
	0, "Bug4.037-4.038(240127): Lookaround must be 0-width.\n",
	"",
	RE("(?:(?=abc))*"),
	STR("abc"),
	0, 1,
	STR0("")
},
{
	0, "Bug4.037-4.039(240130): Non-greedy ? ({0,1}?).\n",
	"",
	RE("a??b"),
	STR("b"),
	0, 1,
	STR0("b")
},
	//  From ECMAScript Specification.
{
	0, "Test 1 (ECMAScript 2019 Language Specification 21.2.2.3, NOTE).\n",
	"",
	RE("((a)|(ab))((c)|(bc))"),
	STR("abc"),
	0, 7,
	STR0("abc")
	STR0("a")
	STR0("a")
	STR0("(undefined)")
	STR0("bc")
	STR0("(undefined)")
	STR0("bc")
},
{
	0, "Test 2a (ECMAScript 2019 Language Specification 21.2.2.5.1, NOTE 2).\n",
	"",
	RE("a[a-z]{2,4}"),
	STR("abcdefghi"),
	0, 1,
	STR0("abcde")
},
{
	0, "Test 2b (ECMAScript 2019 Language Specification 21.2.2.5.1, NOTE 2).\n",
	"",
	RE("a[a-z]{2,4}?"),
	STR("abcdefghi"),
	0, 1,
	STR0("abc")
},
{
	0, "Test 3 (ECMAScript 2019 Language Specification 21.2.2.5.1, NOTE 2).\n",
	"",
	RE("(aa|aabaac|ba|b|c)*"),
	STR("aabaac"),
	0, 2,
	STR0("aaba")
	STR0("ba")
},
{
	0, "Test 4 (ECMAScript 2019 Language Specification 21.2.2.5.1, NOTE 3).\n",
	"",
	RE("(z)((a+)?(b+)?(c))*"),
	STR("zaacbbbcac"),
	0, 6,
	STR0("zaacbbbcac")
	STR0("z")
	STR0("ac")
	STR0("a")
	STR0("(undefined)")
	STR0("c")
},
{
	0, "Test 5a (ECMAScript 2019 Language Specification 21.2.2.5.1, NOTE 4).\n",
	"",
	RE("(a*)*"),
	STR("b"),
	0, 2,
	STR0("")
	STR0("(undefined)")
},
{
	0, "Test 5b (ECMAScript 2019 Language Specification 21.2.2.5.1, NOTE 4).\n",
	"",
	RE("(a*)b\\1+"),
	STR("baaaac"),
	0, 2,
	STR0("b")
	STR0("")
},
{
	0, "Test 6a (ECMAScript 2019 Language Specification 21.2.2.8.2, NOTE 2).\n",
	"",
	RE("(?=(a+))"),
	STR("baaabac"),
	0, 2,
	STR0("")
	STR0("aaa")
},
{
	0, "Test 6b (ECMAScript 2019 Language Specification 21.2.2.8.2, NOTE 2).\n",
	"",
	RE("(?=(a+))a*b\\1"),
	STR("baaabac"),
	0, 2,
	STR0("aba")
	STR0("a")
},
{
	0, "Test 7 (ECMAScript 2019 Language Specification 21.2.2.8.2, NOTE 3).\n",
	"",
	RE("(.*?)a(?!(a+)b\\2c)\\2(.*)"),
	STR("baaabaac"),
	0, 4,
	STR0("baaabaac")
	STR0("ba")
	STR0("(undefined)")
	STR0("abaac")
},
	//  Lookbehind.
	//  https://github.com/tc39/proposal-regexp-lookbehind
{
	0, "Lookbehind 01.\n",
	"",
	RE("(?<=(\\d+)(\\d+))$"),
	STR("1053"),
	0, 3,
	STR0("")
	STR0("1")
	STR0("053")
},
	//  https://github.com/tc39/test262/tree/master/test/built-ins/RegExp/lookBehind
{
	0, "Lookbehind 02a: alternations.js #1.\n",
	"",
	RE(".*(?<=(..|...|....))(.*)"),
	STR("xabcd"),
	0, 3,
	STR0("xabcd")
	STR0("cd")
	STR0("")
},
{
	0, "Lookbehind 02b: alternations.js #2.\n",
	"",
	RE(".*(?<=(xx|...|....))(.*)"),
	STR("xabcd"),
	0, 3,
	STR0("xabcd")
	STR0("bcd")
	STR0("")
},
{
	0, "Lookbehind 02c: alternations.js #3.\n",
	"",
	RE(".*(?<=(xx|...))(.*)"),
	STR("xxabcd"),
	0, 3,
	STR0("xxabcd")
	STR0("bcd")
	STR0("")
},
{
	0, "Lookbehind 02d: alternations.js #4.\n",
	"",
	RE(".*(?<=(xx|xxx))(.*)"),
	STR("xxabcd"),
	0, 3,
	STR0("xxabcd")
	STR0("xx")
	STR0("abcd")
},
{
	0, "Lookbehind 03a: back-references-to-captures.js #1.\n",
	"i",
	RE("(?<=\\1(\\w))d"),
	STR("abcCd"),
	0, 2,
	STR0("d")
	STR0("C")
},
{
	0, "Lookbehind 03b: back-references-to-captures.js #2.\n",
	"",
	RE("(?<=\\1([abx]))d"),
	STR("abxxd"),
	0, 2,
	STR0("d")
	STR0("x")
},
{
	0, "Lookbehind 03c: back-references-to-captures.js #3.\n",
	"",
	RE("(?<=\\1(\\w+))c"),
	STR("ababc"),
	0, 2,
	STR0("c")
	STR0("ab")
},
{
	0, "Lookbehind 03d: back-references-to-captures.js #4.\n",
	"",
	RE("(?<=\\1(\\w+))c"),
	STR("ababbc"),
	0, 2,
	STR0("c")
	STR0("b")
},
{
	0, "Lookbehind 03e: back-references-to-captures.js #5.\n",
	"",
	RE("(?<=\\1(\\w+))c"),
	STR("ababdc"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 03f: back-references-to-captures.js #6.\n",
	"",
	RE("(?<=(\\w+)\\1)c"),
	STR("ababc"),
	0, 2,
	STR0("c")
	STR0("abab")
},
{
	0, "Lookbehind 04a: back-references.js #1.\n",
	"",
	RE("(.)(?<=(\\1\\1))"),
	STR("abb"),
	0, 3,
	STR0("b")
	STR0("b")
	STR0("bb")
},
{
	0, "Lookbehind 04b: back-references.js #2.\n",
	"i",
	RE("(.)(?<=(\\1\\1))"),
	STR("abB"),
	0, 3,
	STR0("B")
	STR0("B")
	STR0("bB")
},
{
	0, "Lookbehind 04c: back-references.js #3.\n",
	"i",
	RE("((\\w)\\w)(?<=\\1\\2\\1)"),
	STR("aabAaBa"),
	0, 3,
	STR0("aB")
	STR0("aB")
	STR0("a")
},
{
	0, "Lookbehind 04d: back-references.js #4.\n",
	"i",
	RE("(\\w(\\w))(?<=\\1\\2\\1)"),
	STR("aabAaBa"),
	0, 3,
	STR0("Ba")
	STR0("Ba")
	STR0("a")
},
{
	0, "Lookbehind 04e: back-references.js #5.\n",
	"i",
	RE("(?=(\\w))(?<=(\\1))."),
	STR("abaBbAa"),
	0, 3,
	STR0("b")
	STR0("b")
	STR0("B")
},
{
	0, "Lookbehind 04f: back-references.js #6.\n",
	"",
	RE("(?<=(.))(\\w+)(?=\\1)"),
	STR("  'foo'  "),
	0, 3,
	STR0("foo")
	STR0("'")
	STR0("foo")
},
{
	0, "Lookbehind 04g: back-references.js #7.\n",
	"",
	RE("(?<=(.))(\\w+)(?=\\1)"),
	STR("  \"foo\"  "),
	0, 3,
	STR0("foo")
	STR0("\"")
	STR0("foo")
},
{
	0, "Lookbehind 04h: back-references.js #8.\n",
	"",
	RE("(.)(?<=\\1\\1\\1)"),
	STR("abbb"),
	0, 2,
	STR0("b")
	STR0("b")
},
{
	0, "Lookbehind 04i: back-references.js #9.\n",
	"",
	RE("(..)(?<=\\1\\1\\1)"),
	STR("fababab"),
	0, 2,
	STR0("ab")
	STR0("ab")
},
{
	0, "Lookbehind 04j: back-references.js #10.\n",
	"",
	RE("(?<=(.))(\\w+)(?=\\1)"),
	STR("  .foo\"  "),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 04k: back-references.js #11.\n",
	"",
	RE("(.)(?<=\\1\\1\\1)"),
	STR("ab"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 04l: back-references.js #12.\n",
	"",
	RE("(.)(?<=\\1\\1\\1)"),
	STR("abb"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 04l: back-references.js #13.\n",
	"",
	RE("(..)(?<=\\1\\1\\1)"),
	STR("ab"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 04m: back-references.js #14.\n",
	"",
	RE("(..)(?<=\\1\\1\\1)"),
	STR("abb"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 04n: back-references.js #15.\n",
	"",
	RE("(..)(?<=\\1\\1\\1)"),
	STR("aabb"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 04o: back-references.js #16.\n",
	"",
	RE("(..)(?<=\\1\\1\\1)"),
	STR("abab"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 04p: back-references.js #17.\n",
	"",
	RE("(..)(?<=\\1\\1\\1)"),
	STR("fabxbab"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 04q: back-references.js #18.\n",
	"",
	RE("(..)(?<=\\1\\1\\1)"),
	STR("faxabab"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 05: captures-negative.js.\n",
	"",
	RE("(?<!(^|[ab]))\\w{2}"),
	STR("abcdef"),
	0, 2,
	STR0("de")
	STR0("(undefined)")
},
{
	0, "Lookbehind 06a: captures.js #1.\n",
	"",
	RE("(?<=(c))def"),
	STR("abcdef"),
	0, 2,
	STR0("def")
	STR0("c")
},
{
	0, "Lookbehind 06b: captures.js #2.\n",
	"",
	RE("(?<=(\\w{2}))def"),
	STR("abcdef"),
	0, 2,
	STR0("def")
	STR0("bc")
},
{
	0, "Lookbehind 06c: captures.js #3.\n",
	"",
	RE("(?<=(\\w(\\w)))def"),
	STR("abcdef"),
	0, 3,
	STR0("def")
	STR0("bc")
	STR0("c")
},
{
	0, "Lookbehind 06d: captures.js #4.\n",
	"",
	RE("(?<=(\\w){3})def"),
	STR("abcdef"),
	0, 2,
	STR0("def")
	STR0("a")
},
{
	0, "Lookbehind 06e: captures.js #5.\n",
	"",
	RE("(?<=(bc)|(cd))."),
	STR("abcdef"),
	0, 3,
	STR0("d")
	STR0("bc")
	STR0("(undefined)")
},
{
	0, "Lookbehind 06f: captures.js #6.\n",
	"",
	RE("(?<=([ab]{1,2})\\D|(abc))\\w"),
	STR("abcdef"),
	0, 3,
	STR0("c")
	STR0("a")
	STR0("(undefined)")
},
{
	0, "Lookbehind 06g: captures.js #7.\n",
	"",
	RE("\\D(?<=([ab]+))(\\w)"),
	STR("abcdef"),
	0, 3,
	STR0("ab")
	STR0("a")
	STR0("b")
},
{
	0, "Lookbehind 06h: captures.js #8.\n",
	"G",
	RE("(?<=b|c)\\w"),
	STR("abcdef"),
	0, 2,
	STR0("c")
	STR0("d")
},
{
	0, "Lookbehind 06i: captures.js #9.\n",
	"G",
	RE("(?<=[b-e])\\w{2}"),
	STR("abcdef"),
	0, 2,
	STR0("cd")
	STR0("ef")
},
{
	0, "Lookbehind 07: do-not-backtrack.js.\n",
	"",
	RE("(?<=([abc]+)).\\1"),
	STR("abcdbc"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 08a: greedy-loop.js #1.\n",
	"",
	RE("(?<=(b+))c"),
	STR("abbbbbbc"),
	0, 2,
	STR0("c")
	STR0("bbbbbb")
},
{
	0, "Lookbehind 08b: greedy-loop.js #2.\n",
	"",
	RE("(?<=(b\\d+))c"),
	STR("ab1234c"),
	0, 2,
	STR0("c")
	STR0("b1234")
},
{
	0, "Lookbehind 08c: greedy-loop.js #3.\n",
	"",
	RE("(?<=((?:b\\d{2})+))c"),
	STR("ab12b23b34c"),
	0, 2,
	STR0("c")
	STR0("b12b23b34")
},
{
	0, "Lookbehind 09a: misc.js #1.\n",
	"",
	RE("(?<=$abc)def"),
	STR("abcdef"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 09b: misc.js #2.\n",
	"",
	RE("^f.o(?<=foo)$"),
	STR("fno"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 09c: misc.js #3.\n",
	"",
	RE("^foo(?<!foo)$"),
	STR("foo"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 09d: misc.js #4.\n",
	"",
	RE("^f.o(?<!foo)$"),
	STR("foo"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 09e: misc.js #5.\n",
	"",
	RE("^foo(?<=foo)$"),
	STR("foo"),
	0, 1,
	STR0("foo")
},
{
	0, "Lookbehind 09f: misc.js #6.\n",
	"",
	RE("^f.o(?<=foo)$"),
	STR("foo"),
	0, 1,
	STR0("foo")
},
{
	0, "Lookbehind 09g: misc.js #7.\n",
	"",
	RE("^f.o(?<!foo)$"),
	STR("fno"),
	0, 1,
	STR0("fno")
},
{
	0, "Lookbehind 09h: misc.js #8.\n",
	"",
	RE("^foooo(?<=fo+)$"),
	STR("foooo"),
	0, 1,
	STR0("foooo")
},
{
	0, "Lookbehind 09i: misc.js #9.\n",
	"",
	RE("^foooo(?<=fo*)$"),
	STR("foooo"),
	0, 1,
	STR0("foooo")
},
{
	0, "Lookbehind 09j: misc.js #10.\n",
	"",
	RE("(abc\\1)"),
	STR("abc"),
	0, 2,
	STR0("abc")
	STR0("abc")
},
{
	0, "Lookbehind 09k: misc.js #11.\n",
	"",
	RE("(abc\\1)"),
	STR("abc\\u1234"),
	0, 2,
	STR0("abc")
	STR0("abc")
},
{
	0, "Lookbehind 09l: misc.js #12.\n",
	"i",
	RE("(abc\\1)"),
	STR("abc"),
	0, 2,
	STR0("abc")
	STR0("abc")
},
{
	0, "Lookbehind 09m: misc.js #13.\n",
	"i",
	RE("(abc\\1)"),
	STR("abc\\u1234"),
	0, 2,
	STR0("abc")
	STR0("abc")
},
{
	0, "Lookbehind 10a: mutual-recursive.js #1.\n",
	"",
	RE("(?<=a(.\\2)b(\\1)).{4}"),
	STR("aabcacbc"),
	0, 3,
	STR0("cacb")
	STR0("a")
	STR0("")
},
{
	0, "Lookbehind 10b: mutual-recursive.js #2.\n",
	"",
	RE("(?<=a(\\2)b(..\\1))b"),
	STR("aacbacb"),
	0, 3,
	STR0("b")
	STR0("ac")
	STR0("ac")
},
{
	0, "Lookbehind 10c: mutual-recursive.js #3.\n",
	"",
	RE("(?<=(?:\\1b)(aa))."),
	STR("aabaax"),
	0, 2,
	STR0("x")
	STR0("aa")
},
{
	0, "Lookbehind 10d: mutual-recursive.js #4.\n",
	"",
	RE("(?<=(?:\\1|b)(aa))."),
	STR("aaaax"),
	0, 2,
	STR0("x")
	STR0("aa")
},
{
	0, "Lookbehind 11a: negative.js #1.\n",
	"",
	RE("(?<!abc)\\w\\w\\w"),
	STR("abcdef"),
	0, 1,
	STR0("abc")
},
{
	0, "Lookbehind 11b: negative.js #2.\n",
	"",
	RE("(?<!a.c)\\w\\w\\w"),
	STR("abcdef"),
	0, 1,
	STR0("abc")
},
{
	0, "Lookbehind 11c: negative.js #3.\n",
	"",
	RE("(?<!a\\wc)\\w\\w\\w"),
	STR("abcdef"),
	0, 1,
	STR0("abc")
},
{
	0, "Lookbehind 11d: negative.js #4.\n",
	"",
	RE("(?<!a[a-z])\\w\\w\\w"),
	STR("abcdef"),
	0, 1,
	STR0("abc")
},
{
	0, "Lookbehind 11e: negative.js #5.\n",
	"",
	RE("(?<!a[a-z]{2})\\w\\w\\w"),
	STR("abcdef"),
	0, 1,
	STR0("abc")
},
{
	0, "Lookbehind 11f: negative.js #6.\n",
	"",
	RE("(?<!abc)def"),
	STR("abcdef"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 11g: negative.js #7.\n",
	"",
	RE("(?<!a.c)def"),
	STR("abcdef"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 11h: negative.js #8.\n",
	"",
	RE("(?<!a\\wc)def"),
	STR("abcdef"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 11i: negative.js #9.\n",
	"",
	RE("(?<!a[a-z][a-z])def"),
	STR("abcdef"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 11j: negative.js #10.\n",
	"",
	RE("(?<!a[a-z]{2})def"),
	STR("abcdef"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 11k: negative.js #11.\n",
	"",
	RE("(?<!a{1}b{1})cde"),
	STR("abcdef"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 11l: negative.js #12.\n",
	"",
	RE("(?<!a{1}[a-z]{2})def"),
	STR("abcdef"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 12a: nested-lookaround.js #1.\n",
	"",
	RE("(?<=ab(?=c)\\wd)\\w\\w"),
	STR("abcdef"),
	0, 1,
	STR0("ef")
},
{
	0, "Lookbehind 12b: nested-lookaround.js #2.\n",
	"",
	RE("(?<=a(?=([^a]{2})d)\\w{3})\\w\\w"),
	STR("abcdef"),
	0, 2,
	STR0("ef")
	STR0("bc")
},
{
	0, "Lookbehind 12c: nested-lookaround.js #3.\n",
	"",
	RE("(?<=a(?=([bc]{2}(?<!a{2}))d)\\w{3})\\w\\w"),
	STR("abcdef"),
	0, 2,
	STR0("ef")
	STR0("bc")
},
{
	0, "Lookbehind 12d: nested-lookaround.js #4.\n",
	"",
	RE("^faaao?(?<=^f[oa]+(?=o))"),
	STR("faaao"),
	0, 1,
	STR0("faaa")
},
{
	0, "Lookbehind 12e: nested-lookaround.js #5.\n",
	"",
	RE("(?<=a(?=([bc]{2}(?<!a*))d)\\w{3})\\w\\w"),
	STR("abcdef"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 13a: simple-fixed-length.js #1.\n",
	"",
	RE("^.(?<=a)"),
	STR("b"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 13b: simple-fixed-length.js #2.\n",
	"",
	RE("^f\\w\\w(?<=\\woo)"),
	STR("boo"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 13c: simple-fixed-length.js #3.\n",
	"",
	RE("^f\\w\\w(?<=\\woo)"),
	STR("fao"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 13d: simple-fixed-length.js #4.\n",
	"",
	RE("^f\\w\\w(?<=\\woo)"),
	STR("foa"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 13e: simple-fixed-length.js #5.\n",
	"",
	RE("^.(?<=a)"),
	STR("a"),
	0, 1,
	STR0("a")
},
{
	0, "Lookbehind 13f: simple-fixed-length.js #6.\n",
	"",
	RE("^f..(?<=.oo)"),
	STR("foo1"),
	0, 1,
	STR0("foo")
},
{
	0, "Lookbehind 13g: simple-fixed-length.js #7.\n",
	"",
	RE("^f\\w\\w(?<=\\woo)"),
	STR("foo2"),
	0, 1,
	STR0("foo")
},
{
	0, "Lookbehind 13h: simple-fixed-length.js #8.\n",
	"",
	RE("(?<=abc)\\w\\w\\w"),
	STR("abcdef"),
	0, 1,
	STR0("def")
},
{
	0, "Lookbehind 13i: simple-fixed-length.js #9.\n",
	"",
	RE("(?<=a.c)\\w\\w\\w"),
	STR("abcdef"),
	0, 1,
	STR0("def")
},
{
	0, "Lookbehind 13j: simple-fixed-length.js #10.\n",
	"",
	RE("(?<=a\\wc)\\w\\w\\w"),
	STR("abcdef"),
	0, 1,
	STR0("def")
},
{
	0, "Lookbehind 13k: simple-fixed-length.js #11.\n",
	"",
	RE("(?<=a[a-z])\\w\\w\\w"),
	STR("abcdef"),
	0, 1,
	STR0("cde")
},
{
	0, "Lookbehind 13l: simple-fixed-length.js #12.\n",
	"",
	RE("(?<=a[a-z][a-z])\\w\\w\\w"),
	STR("abcdef"),
	0, 1,
	STR0("def")
},
{
	0, "Lookbehind 13m: simple-fixed-length.js #13.\n",
	"",
	RE("(?<=a[a-z]{2})\\w\\w\\w"),
	STR("abcdef"),
	0, 1,
	STR0("def")
},
{
	0, "Lookbehind 13n: simple-fixed-length.js #14.\n",
	"",
	RE("(?<=a{1})\\w\\w\\w"),
	STR("abcdef"),
	0, 1,
	STR0("bcd")
},
{
	0, "Lookbehind 13o: simple-fixed-length.js #15.\n",
	"",
	RE("(?<=a{1}b{1})\\w\\w\\w"),
	STR("abcdef"),
	0, 1,
	STR0("cde")
},
{
	0, "Lookbehind 13p: simple-fixed-length.js #16.\n",
	"",
	RE("(?<=a{1}[a-z]{2})\\w\\w\\w"),
	STR("abcdef"),
	0, 1,
	STR0("def")
},
{
	0, "Lookbehind 14a: sliced-strings.js #1.\n",
	"i",
	RE("(?=(abcdefghijklmn))(?<=\\1)a"),
	STR("abcdefghijklmnabcdefghijklmn"),
	14, 0,
	STR0("")
},
{
	0, "Lookbehind 14b: sliced-strings.js #2.\n",
	"",
	RE("(?=(abcdefghijklmn))(?<=\\1)a"),
	STR("abcdefghijklmnabcdefghijklmn"),
	14, 0,
	STR0("")
},
{
	0, "Lookbehind 14c: sliced-strings.js #3.\n",
	"",
	RE("(?=(abcdefg))(?<=\\1)"),
	STR("abcdefgabcdefg"),
	1, 0,
	STR0("")
},
{
	0, "Lookbehind 15a: start-of-line.js #1.\n",
	"",
	RE("(?<=^[^a-c]{3})def"),
	STR("abcdef"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 15b: start-of-line.js #2.\n",
	"",
	RE("\"^foooo(?<=^o+)$"),
	STR("foooo"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 15c: start-of-line.js #3.\n",
	"",
	RE("\"^foooo(?<=^o*)$"),
	STR("foooo"),
	0, 0,
	STR0("")
},
{
	0, "Lookbehind 15d: start-of-line.js #4.\n",
	"",
	RE("(?<=^abc)def"),
	STR("abcdef"),
	0, 1,
	STR0("def")
},
{
	0, "Lookbehind 15e: start-of-line.js #5.\n",
	"",
	RE("(?<=^[a-c]{3})def"),
	STR("abcdef"),
	0, 1,
	STR0("def")
},
{
	0, "Lookbehind 15f: start-of-line.js #6.\n",
	"m",
	RE("(?<=^[a-c]{3})def"),
	STR("xyz\nabcdef"),
	0, 1,
	STR0("def")
},
{
	0, "Lookbehind 15g: start-of-line.js #7.\n",
	"mG",
	RE("(?<=^)\\w+"),
	STR("ab\ncd\nefg"),
	0, 3,
	STR0("ab")
	STR0("cd")
	STR0("efg")
},
{
	0, "Lookbehind 15h: start-of-line.js #8.\n",
	"mG",
	RE("\\w+(?<=$)"),
	STR("ab\ncd\nefg"),
	0, 3,
	STR0("ab")
	STR0("cd")
	STR0("efg")
},
{
	0, "Lookbehind 15i: start-of-line.js #9.\n",
	"mG",
	RE("(?<=^)\\w+(?<=$)"),
	STR("ab\ncd\nefg"),
	0, 3,
	STR0("ab")
	STR0("cd")
	STR0("efg")
},
{
	0, "Lookbehind 15j: start-of-line.js #10.\n",
	"",
	RE("^foo(?<=^fo+)$"),
	STR("foo"),
	0, 1,
	STR0("foo")
},
{
	0, "Lookbehind 15k: start-of-line.js #11.\n",
	"",
	RE("^foooo(?<=^fo*)"),
	STR("foooo"),
	0, 1,
	STR0("foooo")
},
{
	0, "Lookbehind 15l: start-of-line.js #12.\n",
	"",
	RE("^(f)oo(?<=^\\1o+)$"),
	STR("foo"),
	0, 2,
	STR0("foo")
	STR0("f")
},
{
	0, "Lookbehind 15m: start-of-line.js #13.\n",
	"i",
	RE("^(f)oo(?<=^\\1o+)$"),
	STR("foo"),
	0, 2,
	STR0("foo")
	STR0("f")
},
{
	0, "Lookbehind 15n: start-of-line.js #14.\n",
	"i",
	RE("^(f)oo(?<=^\\1o+).$"),
	STR("foo\\u1234"),
	0, 2,
	STR0("foo\\u1234")
	STR0("f")
},
{
	0, "Lookbehind 15o: start-of-line.js #15.\n",
	"",
	RE("(?<=^\\w+)def"),
	STR("abcdefdef"),
	0, 1,
	STR0("def")
},
{
	0, "Lookbehind 15p: start-of-line.js #16.\n",
	"G",
	RE("(?<=^\\w+)def"),
	STR("abcdefdef"),
	0, 2,
	STR0("def")
	STR0("def")
},
{
	0, "Lookbehind 16ab: sticky.js #1-2.\n",
	"A",
	RE("(?<=^(\\w+))def"),
	STR("abcdefdef"),
	0, 4,
	STR0("def")
	STR0("abc")
	STR0("def")
	STR0("abcdef")
},
{
	0, "Lookbehind 16cd: sticky.js #3-4.\n",
	"A",
	RE("\\Bdef"),
	STR("abcdefdef"),
	0, 2,
	STR0("def")
	STR0("def")
},
{
	0, "Lookbehind 17a: variable-length.js #1.\n",
	"",
	RE("(?<=[a|b|c]*)[^a|b|c]{3}"),
	STR("abcdef"),
	0, 1,
	STR0("def")
},
{
	0, "Lookbehind 17b: variable-length.js #2.\n",
	"",
	RE("(?<=\\w*)[^a|b|c]{3}"),
	STR("abcdef"),
	0, 1,
	STR0("def")
},
{
	0, "Lookbehind 18a: word-boundary.js #1.\n",
	"",
	RE("(?<=\\b)[d-f]{3}"),
	STR("abc def"),
	0, 1,
	STR0("def")
},
{
	0, "Lookbehind 18b: word-boundary.js #2.\n",
	"",
	RE("(?<=\\B)\\w{3}"),
	STR("ab cdef"),
	0, 1,
	STR0("def")
},
{
	0, "Lookbehind 18c: word-boundary.js #3.\n",
	"",
	RE("(?<=\\B)(?<=c(?<=\\w))\\w{3}"),
	STR("ab cdef"),
	0, 1,
	STR0("def")
},
{
	0, "Lookbehind 18d: word-boundary.js #4.\n",
	"",
	RE("(?<=\\b)[d-f]{3}"),
	STR("abcdef"),
	0, 0,
	STR0("")
},
	//  Dotall.
	//  https://github.com/tc39/test262/tree/master/test/built-ins/RegExp/dotall
{
	0, "dotall flag 01: with-dotall-unicode.js.\n",
	"s",
	RE(".*"),
	STR("a3\\u03C0\\u2027\\u0085\v\f\\u180E\\u00010300\n\r\\u2028\\u2029\\uD800\\uDFFF"),
	0, 1,
	STR0("a3\\u03C0\\u2027\\u0085\v\f\\u180E\\u00010300\n\r\\u2028\\u2029\\uD800\\uDFFF")
},
{
	0, "dotall flag 02: without-dotall-unicode.js.\n",
	"",
	RE("(^.*)([\\s]+)(.*$)"),
	STR("a3\\u03C0\\u2027\\u0085\v\f\\u180E\\u00010300\n\r\\u2028\\u2029\\uD800\\uDFFF"),
	0, 4,
	STR0("a3\\u03C0\\u2027\\u0085\v\f\\u180E\\u00010300\n\r\\u2028\\u2029\\uD800\\uDFFF")
	STR0("a3\\u03C0\\u2027\\u0085\v\f\\u180E\\u00010300")
	STR0("\n\r\\u2028\\u2029")
	STR0("\\uD800\\uDFFF")
},
	//  Named-groups.
	//  https://github.com/tc39/test262/tree/master/test/built-ins/RegExp/named-groups
{
	0, "named-groups 01a: lookbehind.js #1.\n",
	"",
	RE("(?<=(?<a>\\w){3})f"),
	STR("abcdef"),
	0, 2,
	STR0("f")
	STR0("c")
},
{
	0, "named-groups 01c: lookbehind.js #2.\n",
	"",
	RE("(?<=(?<a>\\w){4})f"),
	STR("abcdef"),
	0, 2,
	STR0("f")
	STR0("b")
},
{
	0, "named-groups 01d: lookbehind.js #3.\n",
	"",
	RE("(?<=(?<a>\\w)+)f"),
	STR("abcdef"),
	0, 2,
	STR0("f")
	STR0("a")
},
{
	0, "named-groups 01e: lookbehind.js #4.\n",
	"",
	RE("(?<=(?<a>\\w){6})f"),
	STR("abcdef"),
	0, 0,
	STR0("")
},
{
	0, "named-groups 01f: lookbehind.js #5.\n",
	"",
	RE("((?<=\\w{3}))f"),
	STR("abcdef"),
	0, 2,
	STR0("f")
	STR0("")
},
{
	0, "named-groups 01g: lookbehind.js #6.\n",
	"",
	RE("(?<a>(?<=\\w{3}))f"),
	STR("abcdef"),
	0, 2,
	STR0("f")
	STR0("")
},
{
	0, "named-groups 01h: lookbehind.js #7.\n",
	"",
	RE("(?<!(?<a>\\d){3})f"),
	STR("abcdef"),
	0, 2,
	STR0("f")
	STR0("(undefined)")
},
{
	0, "named-groups 01i: lookbehind.js #8.\n",
	"",
	RE("(?<!(?<a>\\D){3})f"),
	STR("abcdef"),
	0, 0,
	STR0("")
},
{
	0, "named-groups 01j: lookbehind.js #9.\n",
	"",
	RE("(?<!(?<a>\\D){3})f|f"),
	STR("abcdef"),
	0, 2,
	STR0("f")
	STR0("(undefined)")
},
{
	0, "named-groups 01k: lookbehind.js #10.\n",
	"",
	RE("(?<a>(?<!\\D{3}))f|f"),
	STR("abcdef"),
	0, 2,
	STR0("f")
	STR0("(undefined)")
},
{
	0, "named-groups 02a: unicode-match.js #1.\n",
	"",
	RE("(?<a>.)(?<b>.)(?<c>.)\\k<c>\\k<b>\\k<a>"),
	STR("abccba"),
	0, 4,
	STR0("abccba")
	STR0("a")
	STR0("b")
	STR0("c")
},
{
	0, "named-groups 02b: unicode-match.js #2.\n",
	"",
	RE("(?<b>b).\\1"),
	STR("bab"),
	0, 2,
	STR0("bab")
	STR0("b")
},
{
	0, "named-groups 02c: unicode-match.js #3.\n",
	"",
	RE("(.)(?<a>a)\\1\\2"),
	STR("baba"),
	0, 3,
	STR0("baba")
	STR0("b")
	STR0("a")
},
{
	0, "named-groups 02d: unicode-match.js #4.\n",
	"",
	RE("(.)(?<a>a)(?<b>\\1)(\\2)"),
	STR("baba"),
	0, 5,
	STR0("baba")
	STR0("b")
	STR0("a")
	STR0("b")
	STR0("a")
},
{
	0, "named-groups 02e: unicode-match.js #5.\n",
	"",
	RE("(?<lt><)a"),
	STR("<a"),
	0, 2,
	STR0("<a")
	STR0("<")
},
{
	0, "named-groups 02f: unicode-match.js #6.\n",
	"",
	RE("(?<gt>>)a"),
	STR(">a"),
	0, 2,
	STR0(">a")
	STR0(">")
},
{
	0, "named-groups 02g: unicode-match.js #7.\n",
	"",
	RE("(?<a>.(?<b>.(?<c>.)))"),
	STR("bab"),
	0, 4,
	STR0("bab")
	STR0("bab")
	STR0("ab")
	STR0("b")
},
	//  unicode-property-names-invalid.js, unicode-property-names-valid.js, unicode-property-names.js.
{
	0, "named-groups 03a: unicode-references.js #1.\n",
	"",
	RE("(?<b>.).\\k<b>"),
	STR("bab"),
	0, 2,
	STR0("bab")
	STR0("b")
},
{
	0, "named-groups 03b: unicode-references.js #2.\n",
	"",
	RE("(?<b>.).\\k<b>"),
	STR("baa"),
	0, 0,
	STR0("")
},
{
	0, "named-groups 03c: unicode-references.js #3.\n",
	"",
	RE("(?<a>\\k<a>\\w).."),
	STR("bab"),
	0, 2,
	STR0("bab")
	STR0("b")
},
{
	0, "named-groups 03d: unicode-references.js #4.\n",
	"",
	RE("\\k<a>(?<a>b)\\w\\k<a>"),
	STR("bab"),
	0, 2,
	STR0("bab")
	STR0("b")
},
{
	0, "named-groups 03e: unicode-references.js #5.\n",
	"",
	RE("(?<b>b)\\k<a>(?<a>a)\\k<b>"),
	STR("bab"),
	0, 3,
	STR0("bab")
	STR0("b")
	STR0("a")
},
{
	0, "named-groups 03f: unicode-references.js #6.\n",
	"",
	RE("(?<a>a)(?<b>b)\\k<a>"),
	STR("aba"),
	0, 3,
	STR0("aba")
	STR0("a")
	STR0("b")
},
{
	0, "named-groups 03g: unicode-references.js #7.\n",
	"",
	RE("(?<a>a)(?<b>b)\\k<a>|(?<c>c)"),
	STR("aba"),
	0, 4,
	STR0("aba")
	STR0("a")
	STR0("b")
	STR0("(undefined)")
},
{
	0, "named-groups 04a: duplicate-names-(exec|match|test).js #1.\n",
	"",
	RE("(?<x>a)|(?<x>b)"),
	STR("bab"),
	0, 3,
	STR0("b")
	STR0("(undefined)")
	STR0("b")
},
{
	0, "named-groups 04b: duplicate-names-(exec|match|test).js #2.\n",
	"",
	RE("(?<x>b)|(?<x>a)"),
	STR("bab"),
	0, 3,
	STR0("b")
	STR0("b")
	STR0("(undefined)")
},
{
	0, "named-groups 04c: duplicate-names-(exec|match|test).js #3.\n",
	"",
	RE("(?:(?<x>a)|(?<x>b))\\k<x>"),
	STR("aa"),
	0, 3,
	STR0("aa")
	STR0("a")
	STR0("(undefined)")
},
{
	0, "named-groups 04d: duplicate-names-(exec|match|test).js #4.\n",
	"",
	RE("(?:(?<x>a)|(?<x>b))\\k<x>"),
	STR("bb"),
	0, 3,
	STR0("bb")
	STR0("(undefined)")
	STR0("b")
},
{
	0, "named-groups 04e: duplicate-names-(exec|match|test).js #5.\n",
	"N",
	RE("(?:(?:(?<x>a)|(?<x>b))\\k<x>){2}"),
	STR("aabb"),
	0, 3,
	STR0("aabb")
	STR0("(undefined)")
	STR0("b <x>")
},
{
	0, "named-groups 04f: duplicate-names-(exec|match|test).js #6.\n",
	"",
	RE("(?:(?:(?<x>a)|(?<x>b))\\k<x>){2}"),
	STR("abab"),
	0, 0,
	STR0("")
},
{
	0, "named-groups 04g: duplicate-names-(exec|match).js #7.\n",
	"",
	RE("(?:(?<x>a)|(?<x>b))\\k<x>"),
	STR("abab"),
	0, 0,
	STR0("")
},
{
	0, "named-groups 04h: duplicate-names-(exec|match).js #8.\n",
	"",
	RE("(?:(?<x>a)|(?<x>b))\\k<x>"),
	STR("cdef"),
	0, 0,
	STR0("")
},
{
	0, "named-groups 04i: duplicate-names-(exec|match).js #9.\n",
	"",
	RE("^(?:(?<a>x)|(?<a>y)|z)\\k<a>$"),
	STR("xx"),
	0, 3,
	STR0("xx")
	STR0("x")
	STR0("(undefined)")
},
{
	0, "named-groups 04j: duplicate-names-(exec|match).js #10.\n",
	"",
	RE("^(?:(?<a>x)|(?<a>y)|z)\\k<a>$"),
	STR("z"),
	0, 3,
	STR0("z")
	STR0("(undefined)")
	STR0("(undefined)")
},
{
	0, "named-groups 04k: duplicate-names-(exec|match).js #11.\n",
	"",
	RE("^(?:(?<a>x)|(?<a>y)|z)\\k<a>$"),
	STR("zz"),
	0, 0,
	STR0("")
},
{
	0, "named-groups 04l: duplicate-names-(exec|match).js #12.\n",
	"",
	RE("(?<a>x)|(?:zy\\k<a>)"),
	STR("zy"),
	0, 2,
	STR0("zy")
	STR0("(undefined)")
},
{
	0, "named-groups 04m: duplicate-names-(exec|match).js #13.\n",
	"",
	RE("^(?:(?<a>x)|(?<a>y)|z){2}\\k<a>$"),
	STR("xz"),
	0, 3,
	STR0("xz")
	STR0("(undefined)")
	STR0("(undefined)")
},
{
	0, "named-groups 04n: duplicate-names-(exec|match).js #14.\n",
	"",
	RE("^(?:(?<a>x)|(?<a>y)|z){2}\\k<a>$"),
	STR("yz"),
	0, 3,
	STR0("yz")
	STR0("(undefined)")
	STR0("(undefined)")
},
{
	0, "named-groups 04o: duplicate-names-(exec|match).js #15.\n",
	"",
	RE("^(?:(?<a>x)|(?<a>y)|z){2}\\k<a>$"),
	STR("xzx"),
	0, 0,
	STR0("")
},
{
	0, "named-groups 04p: duplicate-names-(exec|match).js #16.\n",
	"",
	RE("^(?:(?<a>x)|(?<a>y)|z){2}\\k<a>$"),
	STR("yzy"),
	0, 0,
	STR0("")
},
{
	0, "named-groups 05a: duplicate-names-matchall.js #1.\n",
	"A",
	RE("(?<x>a)|(?<x>b)"),
	STR("bab"),
	0, 9,
	STR0("b")
	STR0("(undefined)")
	STR0("b")
	STR0("a")
	STR0("a")
	STR0("(undefined)")
	STR0("b")
	STR0("(undefined)")
	STR0("b")
},
{
	0, "named-groups 05b: duplicate-names-matchall.js #2.\n",
	"A",
	RE("(?<x>b)|(?<x>a)"),
	STR("bab"),
	0, 9,
	STR0("b")
	STR0("b")
	STR0("(undefined)")
	STR0("a")
	STR0("(undefined)")
	STR0("a")
	STR0("b")
	STR0("b")
	STR0("(undefined)")
},

{
	1, NULL,
	NULL,
	RE(""),
	STR(""),
	0, 0,
	STR0("")
}
};
