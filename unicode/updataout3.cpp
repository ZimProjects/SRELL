//
//  updataout.cpp: version 3.007 (2025/08/23).
//
//  This is a program that generates srell_updata3.h from:
//    DerivedCoreProperties.txt
//    DerivedNormalizationProps.txt
//    PropList.txt
//    PropertyValueAliases.txt
//    ScriptExtensions.txt
//    Scripts.txt
//    UnicodeData.txt
//    emoji-data.txt
//    emoji-sequences.txt
//    emoji-zwj-sequences.txt
//  provided by the Unicode Consortium. The latese versions of them are
//  available at:
//    emoji-data.txt: http://www.unicode.org/Public/UNIDATA/emoji/
//    emoji-sequences.txt and emoji-zwj-sequences.txt:
//      http://www.unicode.org/Public/emoji/
//    others: http://www.unicode.org/Public/UNIDATA/
//

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>	//  For std::swap in C++98/03
#include <utility>	//  For std::swap in C++11-
#define SRELL_NO_UNICODE_DATA
#include "../srell.hpp"

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(disable:4996)
#endif

namespace updata
{
static const char *const property_names[] = {	//  3
	"General_Category:gc", "Script:sc", "Script_Extensions:scx", ""
};
static const char *const binary_property_names[] = {	//  53 (52+1)
	//  *1: http://unicode.org/reports/tr18/#General_Category_Property
	//  *2: 9th field in UnicodeData.txt
	"ASCII",								//  *1
	"ASCII_Hex_Digit:AHex",					//  PropList.txt
	"Alphabetic:Alpha",						//  DerivedCoreProperties.txt
	"Any",									//  *1
	"Assigned",								//  *1
	"Bidi_Control:Bidi_C",					//  PropList.txt
	"Bidi_Mirrored:Bidi_M",					//  *2
	"Case_Ignorable:CI",					//  DerivedCoreProperties.txt
	"Cased",								//  DerivedCoreProperties.txt
	"Changes_When_Casefolded:CWCF",			//  DerivedCoreProperties.txt
	"Changes_When_Casemapped:CWCM",			//  DerivedCoreProperties.txt
	"Changes_When_Lowercased:CWL",			//  DerivedCoreProperties.txt
	"Changes_When_NFKC_Casefolded:CWKCF",	//  DerivedNormalizationProps.txt
	"Changes_When_Titlecased:CWT",			//  DerivedCoreProperties.txt
	"Changes_When_Uppercased:CWU",			//  DerivedCoreProperties.txt
	"Dash",									//  PropList.txt
	"Default_Ignorable_Code_Point:DI",		//  DerivedCoreProperties.txt
	"Deprecated:Dep",						//  PropList.txt
	"Diacritic:Dia",						//  PropList.txt
	"Emoji",								//  emoji-data.txt
	"Emoji_Component:EComp",				//  emoji-data.txt
	"Emoji_Modifier:EMod",					//  emoji-data.txt
	"Emoji_Modifier_Base:EBase",			//  emoji-data.txt
	"Emoji_Presentation:EPres",				//  emoji-data.txt
	"Extended_Pictographic:ExtPict",		//  emoji-data.txt
	"Extender:Ext",							//  PropList.txt
	"Grapheme_Base:Gr_Base",				//  DerivedCoreProperties.txt
	"Grapheme_Extend:Gr_Ext",				//  DerivedCoreProperties.txt
	"Hex_Digit:Hex",						//  PropList.txt
	"IDS_Binary_Operator:IDSB",				//  PropList.txt
	"IDS_Trinary_Operator:IDST",			//  PropList.txt
	"ID_Continue:IDC",						//  DerivedCoreProperties.txt
	"ID_Start:IDS",							//  DerivedCoreProperties.txt
	"Ideographic:Ideo",						//  PropList.txt
	"Join_Control:Join_C",					//  PropList.txt
	"Logical_Order_Exception:LOE",			//  PropList.txt
	"Lowercase:Lower",						//  DerivedCoreProperties.txt
	"Math",									//  DerivedCoreProperties.txt
	"Noncharacter_Code_Point:NChar",		//  PropList.txt
	"Pattern_Syntax:Pat_Syn",				//  PropList.txt
	"Pattern_White_Space:Pat_WS",			//  PropList.txt
	"Quotation_Mark:QMark",					//  PropList.txt
	"Radical",								//  PropList.txt
	"Regional_Indicator:RI",				//  PropList.txt
	"Sentence_Terminal:STerm",				//  PropList.txt
	"Soft_Dotted:SD",						//  PropList.txt
	"Terminal_Punctuation:Term",			//  PropList.txt
	"Unified_Ideograph:UIdeo",				//  PropList.txt
	"Uppercase:Upper",						//  DerivedCoreProperties.txt
	"Variation_Selector:VS",				//  PropList.txt
	"White_Space:space",					//  PropList.txt
	"XID_Continue:XIDC",					//  DerivedCoreProperties.txt
	"XID_Start:XIDS",						//  DerivedCoreProperties.txt
	//  ECMAScript 2019/Unicode 11:
	//    "Extended_Pictographic:ExtPict",
	//  ECMAScript 2021/Unicode 13:
	//    Aliases: EComp, EMod, EBase, EPres, and ExtPict
	""
};
static const char *const emoseq_property_names[] = {
	"RGI_Emoji",
	"Basic_Emoji",							//  emoji-sequences.txt
	"Emoji_Keycap_Sequence",				//  emoji-sequences.txt
	"RGI_Emoji_Modifier_Sequence",			//  emoji-sequences.txt
	"RGI_Emoji_Flag_Sequence",				//  emoji-sequences.txt
	"RGI_Emoji_Tag_Sequence",				//  emoji-sequences.txt
	"RGI_Emoji_ZWJ_Sequence",				//  emoji-zwj-sequences.txt
	""
};
static const char *const gc_values[] = {	//  38
	"Other:C", "Control:Cc:cntrl", "Format:Cf", "Unassigned:Cn",
	"Private_Use:Co", "Surrogate:Cs", "Letter:L", "Cased_Letter:LC",
	"Lowercase_Letter:Ll", "Titlecase_Letter:Lt", "Uppercase_Letter:Lu", "Modifier_Letter:Lm",
	"Other_Letter:Lo", "Mark:M:Combining_Mark", "Spacing_Mark:Mc", "Enclosing_Mark:Me",
	"Nonspacing_Mark:Mn", "Number:N", "Decimal_Number:Nd:digit", "Letter_Number:Nl",
	"Other_Number:No", "Punctuation:P:punct", "Connector_Punctuation:Pc", "Dash_Punctuation:Pd",
	"Close_Punctuation:Pe", "Final_Punctuation:Pf", "Initial_Punctuation:Pi", "Other_Punctuation:Po",
	"Open_Punctuation:Ps", "Symbol:S", "Currency_Symbol:Sc", "Modifier_Symbol:Sk",
	"Math_Symbol:Sm", "Other_Symbol:So", "Separator:Z", "Line_Separator:Zl",
	"Paragraph_Separator:Zp", "Space_Separator:Zs", ""
};
}	//  namespace updata

namespace unishared
{
template <typename T>
std::string to_string(T value, int radix = 10, const int precision = 1)
{
	std::string num;

	if (radix >= 2 && radix <= 16)
	{
		typedef typename std::string::size_type size_type;
		const bool minus = value < 0 ? (value = 0 - value, true) : false;

		for (; value; value /= radix)
			num.push_back("0123456789ABCDEF"[value % radix]);

		if (precision > 0 && num.size() < static_cast<size_type>(precision))
			num.append(static_cast<size_type>(precision) - num.size(), static_cast<char>('0'));

		if (minus)
			num.push_back(static_cast<char>('-'));

		const size_type mid = num.size() / 2;

		for (size_type i = 0; i < mid; ++i)
			std::swap(num[i], num[num.size() - i - 1]);
	}
	return num;
}

void throw_error(const char *const s, ...)
{
	char buffer[256];

	va_list va;
	va_start(va, s);
	std::vsprintf(buffer, s, va);
	va_end(va);
	throw std::runtime_error(buffer);
}

void read_file(std::string &str, const char *const filename, const char *const dir)
{
	const std::string path(std::string(dir ? dir : "") + filename);
	FILE *const fp = std::fopen(path.c_str(), "r");

	std::fprintf(stdout, "Reading '%s'... ", path.c_str());

	if (fp)
	{
		static const std::size_t bufsize = 4096;
		char *const buffer = static_cast<char *>(std::malloc(bufsize));

		if (buffer)
		{
			for (;;)
			{
				const std::size_t size = std::fread(buffer, 1, bufsize, fp);

				if (!size)
					break;

				str.append(buffer, size);
			}
			std::fclose(fp);
			std::fputs("done.\n", stdout);
			std::free(buffer);
			return;
		}
	}
	std::fputs("failed...", stdout);
	throw_error("could not open!");
}

bool write_file(const char *const filename, const std::string &str)
{
	FILE *const fp = std::fopen(filename, "wb");

	std::fprintf(stdout, "Writing '%s'... ", filename);

	if (fp)
	{
		const bool success = std::fwrite(str.c_str(), 1, str.size(), fp) == str.size();
		std::fclose(fp);
		if (success)
		{
			std::fputs("done.\n", stdout);
			return true;
		}
	}
	std::fputs("failed...\n", stdout);
	return false;
}
}	//  namespace unishared

struct up_options
{
	const char *outfilename;
	const char *indir;
	int version;
	int errorno;
	bool noesc;

	up_options(const int argc, const char *const *const argv)
		: outfilename("srell_updata3.h")
		, indir("")
		, version(301)
		, errorno(0)
		, noesc(false)
	{
		for (int index = 1; index < argc; ++index)
		{
			const char firstchar = argv[index][0];

			if (firstchar == '-' || firstchar == '/')
			{
				const char *const option = argv[index] + 1;

				if (std::strcmp(option, "o") == 0)
				{
					if (index + 1 >= argc)
						goto NO_ARGUMENT;
					outfilename = argv[++index];
				}
				else if (std::strcmp(option, "i") == 0 || std::strcmp(option, "id") == 0)
				{
					if (index + 1 >= argc)
						goto NO_ARGUMENT;
					indir = argv[++index];
				}
				else if (std::strcmp(option, "noesc") == 0)
				{
					if (index + 1 >= argc)
						goto NO_ARGUMENT;
					noesc = static_cast<int>(std::strtod(argv[++index], NULL)) ? true : false;
				}
				else if (std::strcmp(option, "?") == 0 || std::strcmp(option, "h") == 0)
				{
					std::fputs("Usage: updataout2 [options]\nOptions:\n", stdout);
					std::fputs("  -i <DIRECTORY>\tSame as -id.\n", stdout);
					std::fputs("  -id <DIRECTORY>\tAssume that input files exist in <DIRECTORY>.\n\t\t\t<DIRECTORY> must ends with '/' or '\\'.\n", stdout);
					std::fputs("  -o <FILE>\t\tOutput to <FILE>.\n", stdout);
					std::fputs("  -noesc [1|0]\t\tDo not escape literal strings.\n", stdout);
					errorno = 1;
					return;
				}
				else
					goto UNKNOWN_OPTION;

				continue;

				NO_ARGUMENT:
				std::fprintf(stdout, "[Error] no argument for \"%s\" specified.\n", argv[index]);
				errorno = -2;
			}
			else
			{
				UNKNOWN_OPTION:
				std::fprintf(stdout, "[Error] unknown option \"%s\" found.\n", argv[index]);
				errorno = -1;
			}
		}
	}
};
//  struct up_options

class unicode_property
{
public:

	unicode_property()
		: re_colon_(":")
	{
	}

	int create_updata(std::string &outdata, const up_options &opts)
	{
		int errorno = opts.errorno;
		const char *const unidatafilename = "UnicodeData.txt";
		const char *const propdatafiles[] = { "PropList.txt", "DerivedCoreProperties.txt", "emoji-data.txt", "DerivedNormalizationProps.txt", "" };
		const char *const emodatafiles[] = { "emoji-sequences.txt", "emoji-zwj-sequences.txt", "" };
		const char *const scfilename = "Scripts.txt";
		const char *const scxfilename = "ScriptExtensions.txt";
		const char *const pvafilename = "PropertyValueAliases.txt";
		canonicalname_mapper scriptname_maps;
		strings_type scriptname_aliases;
		std::string licensetext;
		rangeholder general_category_values;
		rangeholder binary_properties;
		seqholder emoseq_properties;
		rangeholder scripts;
		rangeholder scriptextensions;
		sortedrangeholder combined_properties;
		sortedseqholder combined_pos;
//		scriptnameholder ucs_to_scriptname;	//  codepoint->scriptname.

		if (errorno)
			return errorno;

		try
		{
			licensetext = "//  ";
			licensetext += unidatafilename;
			licensetext += "\n//\n";

			read_unidata(general_category_values, binary_properties, unidatafilename, opts.indir);
			set_additionalbinprops(binary_properties, general_category_values);	//  for ASCII, Any, Cn.
			create_compositecategories(general_category_values);	//  This needs "Cn".

			read_binprops(binary_properties, licensetext, propdatafiles, opts.indir);
#if !defined(SRELL_NO_VMODE)
			read_emoseq(emoseq_properties, licensetext, emodatafiles, opts.indir);
#endif

			read_scriptnames(scriptname_maps, scriptname_aliases, licensetext, scfilename, pvafilename, opts);

			read_scripts(scripts, licensetext, scfilename, opts.indir);

			scriptextensions = scripts;
			modify_for_scx(scriptextensions, scriptname_maps, licensetext, scxfilename, opts.indir);

			combine_properties(combined_properties, general_category_values, "gc", updata::gc_values);
			combine_properties(combined_properties, binary_properties, "bp", updata::binary_property_names);
			combine_properties(combined_properties, scripts, "sc", scriptname_aliases);
			combine_properties(combined_properties, scriptextensions, "scx", scriptname_aliases);
#if !defined(SRELL_NO_VMODE)
			combine_pos(combined_pos, emoseq_properties, "bp", updata::emoseq_property_names);
#endif

			do_formatting(outdata, combined_properties, combined_pos, opts);

			licensetext.append(1, '\n');
			outdata.insert(0, licensetext);
		}
		catch (srell::regex_error &e)
		{
			std::printf("\nError: %s,%d\n", e.what(), e.code());
			errorno = 1;
		}
		catch (std::runtime_error &e)
		{
			std::printf("\nError: %s\n", e.what());
			errorno = 2;
		}
		return errorno;
	}

private:

	typedef srell::re_detail::ui_l32 ui_l32;
	typedef srell::re_detail::range_pairs ucprange_array;
	typedef srell::re_detail::range_pair u32pair;
	typedef u32pair ucprange;
	typedef srell::re_detail::range_pair_helper u32rp_helper;
	typedef u32rp_helper ucprange_helper;
	typedef std::map<std::string, ucprange_array> rangeholder;
	typedef srell::re_detail::simple_array<ui_l32> u32array;
	typedef std::map<std::string, u32array> seqholder;
	typedef std::vector<std::string> strings_type;
	typedef std::vector<srell::csub_match> matchranges_type;
	typedef std::map<ui_l32, std::string> scriptnameholder;
	typedef std::map<std::string, std::string> name_mapper;
	typedef std::map<std::string, ui_l32> namenumber_mapper;
	typedef name_mapper canonicalname_mapper;
	static const ui_l32 invalid_u32value = srell::re_detail::constants::invalid_u32value;
	static const ui_l32 compositeclass = invalid_u32value;

	struct sorted_name_and_ranges
	{
		std::string ptype;
		std::string canonicalname;
		std::string namealiases;
		ucprange_array ucpranges;
	};
	typedef std::vector<sorted_name_and_ranges> sortedrangeholder;

	struct sorted_name_and_seqs
	{
		std::string ptype;
		std::string canonicalname;
		std::string namealiases;
		u32array ucpseqs;
	};
	typedef std::vector<sorted_name_and_seqs> sortedseqholder;

	void split2(matchranges_type &parts, const std::string &data, const char splitter)
	{
		std::string::size_type readpos = 0;
		srell::csub_match csm;

		csm.matched = true;
		for (;;)
		{
			std::string::size_type lineend = data.find(splitter, readpos);

			csm.first = data.data() + readpos;
			if (lineend == std::string::npos)
			{
				csm.second = data.data() + data.size();
				parts.push_back(csm);
				break;
			}

			csm.second = data.data() + lineend;
			parts.push_back(csm);
			++lineend;
			readpos = lineend;
		}
	}

	std::string join(const char c, const strings_type &parts, const bool add_final_also = false)
	{
		std::string out;

		for (strings_type::size_type i = 0; i < parts.size(); ++i)
			out.append(parts[i] + c);

		if (!add_final_also && out.size())
			out.resize(out.size() - 1);

		return out;
	}

	void read_unidata(rangeholder &gc, rangeholder &bp, const char *const unidatafilename, const char *const indir)
	{
		const srell::regex re_dataline("^([0-9A-F]+);([^;]*);(([^;]*);(?:[^;]*;){6}([^;]*)(?:;[^;]*){5})$");
		const srell::regex re_rangefirst("^<(.*), First>$");

		const std::string stringY("Y");
		const std::string stringN("N");
		ui_l32 prevucp = invalid_u32value;
		std::string data;
		matchranges_type lines;
		srell::cmatch cmatch;
//		matchranges_type parts;
		std::string rangename;
		std::string rangefirstproperty;
		ui_l32 rangefirstcp = 0;
		ucprange range;
		ucprange_array bidi_mirrored_ranges;

		unishared::read_file(data, unidatafilename, indir);
		split2(lines, data, '\n');

		for (matchranges_type::size_type i = 0; i < lines.size(); ++i)
		{
			const srell::csub_match &line = lines[i];

			if (srell::regex_match(line.first, line.second, cmatch, re_dataline))
			{
				const srell::cmatch::value_type &codepoint = cmatch[1];
				const srell::cmatch::value_type &name = cmatch[2];
				const std::string name_string(name.str());
				const std::string property(cmatch[3].str());

				range.first = range.second = static_cast<ui_l32>(std::strtol(codepoint.first, NULL, 16));

				if (prevucp >= range.first && prevucp != invalid_u32value)
					unishared::throw_error("Out of order: %.4lX >= %.4lX", prevucp, range.first);

//				parts.clear();
//				split2(parts, property, ';');
//				if (parts.size() != 13)
//					unishared::throw_error("number of fields is not 13, but %u\n\t[%s]", parts.size(), line.str().c_str());

//				const std::string &general_category = parts[0];
//				const std::string &bidi_mirrored = parts[7];
				const std::string general_category(cmatch[4].str());
				const std::string bidi_mirrored(cmatch[5].str());

				prevucp = range.first;

				if (rangename.size())
				{
					if (name_string.compare("<" + rangename + ", Last>") != 0)
						unishared::throw_error("<%s, Last> does not follow its First line.\n\t%s follows insteadly.", rangename.c_str(), name_string.c_str());

					if (property != rangefirstproperty)
					{
						unishared::throw_error("\"%s\": properties of First and Last are different.\n\tFirst: %s\n\tLast:  %s", rangename.c_str(), rangefirstproperty.c_str(), property.c_str());
					}

					range.first = rangefirstcp;
					rangename.clear();
				}
				else if (srell::regex_match(name.first, name.second, cmatch, re_rangefirst))
				{
					rangename = cmatch[1];
					rangefirstproperty = property;
					rangefirstcp = range.first;
					continue;
				}

				//  Registers "general_category" value.
				gc[general_category].join(range);

				//  Registers "bidi_mirrored" value.
				if (bidi_mirrored == stringY)
				{
					bidi_mirrored_ranges.join(range);
				}
				else if (bidi_mirrored != stringN)
					unishared::throw_error("Unknown Bidi_Mirrored value [%s] in %s.", bidi_mirrored.c_str(), line.str().c_str());
			}
			else if (line.first != line.second)
				unishared::throw_error("Unknown format [%s]", line.str().c_str());
		}
		bp["Bidi_Mirrored"] = bidi_mirrored_ranges;
	}

	void read_scriptnames(canonicalname_mapper &sn_maps, strings_type &sn_aliases, std::string &licensetext, const char *const scfilename, const char *const pvafilename, const up_options &opts)
	{
		const srell::regex re_scline("^[0-9A-Fa-f.]+\\s*;\\s*(\\S+)");
		const srell::regex re_pvaline("scx?\\s*;\\s*(\\S.*)\\r?\\n?");
		const srell::regex re_split("[ ;]+");
		ui_l32 count = 0;
		std::string data;
		matchranges_type lines;
		srell::cmatch cmatch;
		namenumber_mapper seennames;

		unishared::read_file(data, scfilename, opts.indir);

		lines.clear();
		split2(lines, data, '\n');

		for (matchranges_type::size_type i = 0; i < lines.size(); ++i)
		{
			const srell::csub_match &line = lines[i];

			if (srell::regex_search(line.first, line.second, cmatch, re_scline, srell::regex_constants::match_continuous))
			{
				const std::string scname(cmatch.str(1));

				if (!seennames.count(scname))
				{
					seennames[scname] = count++;
				}
			}
		}

		seennames["Unknown"] = count++;
		sn_aliases.resize(count);

		typedef std::vector<srell::csub_match> scnames_type;
		canonicalname_mapper aliases_tmp;
		scnames_type scnames;

		data.clear();
		unishared::read_file(data, pvafilename, opts.indir);

		lines.clear();
		split2(lines, data, '\n');

		matchranges_type::size_type i = read_license(licensetext, lines, 0);

		for (; i < lines.size(); ++i)
		{
			const srell::csub_match &line = lines[i];

			if (srell::regex_match(line.first, line.second, cmatch, re_pvaline, srell::regex_constants::match_continuous))
			{
				srell::cregex_iterator2 rei2s(cmatch[1].first, cmatch[1].second, re_split);

				scnames.clear();
				for (rei2s.split_begin();; rei2s.split_next())
				{
					scnames.push_back(rei2s.split_aptrange());
					if (rei2s.done())
						break;
				}

				if (scnames.size() >= 2)
				{
					const std::string canonicalname(scnames[1]);

					if (seennames.count(canonicalname))
					{
						std::string aliases(canonicalname);

						for (scnames_type::size_type j = 0; j < scnames.size(); ++j)
						{
							const std::string scname(scnames[j].str());

							sn_maps[scname] = canonicalname;

							if (scname != canonicalname)
							{
								aliases += ':';
								aliases += scname;
							}
						}
						sn_aliases[seennames[canonicalname]] = aliases;
					}
				}
			}
		}
	}

	matchranges_type::size_type read_license(std::string &licensetext, const matchranges_type &lines, matchranges_type::size_type pos)
	{
		static const srell::regex re_license("^#[ \\t]*(\\S.*)?$");
		srell::cmatch cm;

		for (; pos < lines.size(); ++pos)
		{
			const srell::csub_match &line = lines[pos];

			if (srell::regex_search(line.first, line.second, cm, re_license, srell::regex_constants::match_continuous))
			{
				const std::string comment(cm[1].str());

				if (comment.size())
					licensetext += "//  " + comment + '\n';
				else
				{
					licensetext += "//\n";
					break;
				}
			}
		}
		return pos;
	}

	//  binary properties created from UnicodeData.txt.
	void set_additionalbinprops(rangeholder &bp, rangeholder &gc)
	{
		ucprange_array assigned_ranges;

		for (rangeholder::iterator it = gc.begin(); it != gc.end(); ++it)
			assigned_ranges.merge(it->second);

		bp["Any"].join(ucprange_helper(0x0000, 0x10ffff));
		bp["ASCII"].join(ucprange_helper(0x0000, 0x007f));
		bp["Assigned"];	//  Only creates. No data.

//		bp["Assigned"] = assigned_ranges;
		assigned_ranges.negation();
		gc["Cn"] = assigned_ranges;
	}

	void create_compositecategory(rangeholder &gc, const char *const newname, const char *const *categories)
	{
		ucprange_array array;
		ui_l32 total = 0;

		array.append_newpair(ucprange_helper(compositeclass, 0));

		for (; **categories; ++categories)
		{
			const char *const c = *categories;
			const ui_l32 count = static_cast<ui_l32>(gc[*categories].size());

			array.append_newpair(ucprange_helper(c[0], c[1]));
			array.append_newpair(ucprange_helper(count, 0));
			total += count;
		}
		array[0].second = total;
		gc[newname] = array;
	}

	void create_compositecategories(rangeholder &gc)
	{
		const char *const categoryLC[] = { "Ll", "Lt", "Lu", "" };
		const char *const categoryL[] = { "Ll", "Lt", "Lu", "Lm", "Lo", "" };
		const char *const categoryM[] = { "Mc", "Me", "Mn", "" };
		const char *const categoryN[] = { "Nd", "Nl", "No", "" };
		const char *const categoryC[] = { "Cc", "Cf", "Cn", "Co", "Cs", "" };
		const char *const categoryP[] = { "Pc", "Pd", "Pe", "Pf", "Pi", "Po", "Ps", "" };
		const char *const categoryZ[] = { "Zl", "Zp", "Zs", "" };
		const char *const categoryS[] = { "Sc", "Sk", "Sm", "So", "" };

		create_compositecategory(gc, "LC", categoryLC);
		create_compositecategory(gc, "L", categoryL);
		create_compositecategory(gc, "M", categoryM);
		create_compositecategory(gc, "N", categoryN);
		create_compositecategory(gc, "C", categoryC);
		create_compositecategory(gc, "P", categoryP);
		create_compositecategory(gc, "Z", categoryZ);
		create_compositecategory(gc, "S", categoryS);
	}

	void read_binprops(rangeholder &bp, std::string &licensetext, const char *const *propdatafiles, const char *const indir)
	{
		static const srell::regex re_propfmt("^\\s*([0-9A-Fa-f]{4,})(?:\\.\\.([0-9A-Fa-f]{4,}))?\\s*;\\s*([^\\s;#]+)\\s*");	//  (#.*)?$");
		ucprange range;
		std::string data;
		matchranges_type lines;
		srell::cmatch cmatch;

		for (; **propdatafiles; ++propdatafiles)
		{
			data.clear();
			unishared::read_file(data, *propdatafiles, indir);

			lines.clear();
			split2(lines, data, '\n');

			matchranges_type::size_type i = read_license(licensetext, lines, 0);

			for (; i < lines.size(); ++i)
			{
				const srell::csub_match &line = lines[i];

				if (srell::regex_search(line.first, line.second, cmatch, re_propfmt, srell::regex_constants::match_continuous))
				{
					const srell::cmatch::value_type &begin = cmatch[1];
					const srell::cmatch::value_type &end = cmatch[2];
					const srell::cmatch::value_type &property = cmatch[3];
//					const srell::cmatch::value_type &comment = cmatch[4];

					range.first = static_cast<ui_l32>(std::strtol(begin.first, NULL, 16));
					range.second = end.matched ? static_cast<ui_l32>(std::strtol(end.first, NULL, 16)) : range.first;

					bp[property.str()].join(range);
				}
			}
		}
	}

	void read_emoseq(seqholder &emsq, std::string &licensetext, const char *const *emodatafiles, const char *const indir)
	{
		const srell::regex re_emsqfmt("^\\s*([0-9A-Fa-f]{4,})(?:\\.\\.([0-9A-Fa-f]{4,})|((?:\\s+[0-9A-Fa-f]{4,})+))?\\s*;\\s*([^\\s;#]+)\\s*");	//  (?:\\s*;[^#]*)(#.*)?$");
		const srell::regex re_emsq2fmt("\\s*([0-9A-Fa-f]{4,})");
		std::string data;
		matchranges_type lines;
		srell::cmatch cmatch;

		for (; **emodatafiles; ++emodatafiles)
		{
			data.clear();
			unishared::read_file(data, *emodatafiles, indir);

			lines.clear();
			split2(lines, data, '\n');

			matchranges_type::size_type i = read_license(licensetext, lines, 0);

			for (; i < lines.size(); ++i)
			{
				const srell::csub_match &line = lines[i];

				if (srell::regex_search(line.first, line.second, cmatch, re_emsqfmt, srell::regex_constants::match_continuous))
				{
					const srell::cmatch::value_type &begin = cmatch[1];
					const srell::cmatch::value_type &end = cmatch[2];
					const srell::cmatch::value_type &seqs = cmatch[3];
					const std::string seqname = cmatch[4].str();
//					const srell::cmatch::value_type &comment = cmatch[5];
					const ui_l32 first = static_cast<ui_l32>(std::strtol(begin.first, NULL, 16));

					if (seqs.matched)
					{
						const u32array::size_type orgsize = emsq[seqname].size();
						srell::cregex_iterator2 it(seqs.first, seqs.second, re_emsq2fmt, srell::regex_constants::match_continuous);
						ui_l32 count = 2;

						emsq[seqname].push_back_c(0);	//  Number of code points.
						emsq[seqname].push_back(first);

						for (; !it.done(); ++it, ++count)
						{
							const srell::cmatch::value_type &ucp = (*it)[1];

							emsq[seqname].push_back(static_cast<ui_l32>(std::strtol(ucp.first, NULL, 16)));
						}
						emsq[seqname][orgsize] = count;
					}
					else
					{
						if (end.matched)
						{
							emsq[seqname].push_back_c(1);	//  Range.
							emsq[seqname].push_back(first);
							emsq[seqname].push_back(static_cast<ui_l32>(std::strtol(end.first, NULL, 16)));
						}
						else
						{
							emsq[seqname].push_back_c(2);	//  Single code point.
							emsq[seqname].push_back(first);
						}
					}
				}
			}
		}

		for (seqholder::iterator it = emsq.begin(); it != emsq.end(); ++it)
		{
			if (it->second.size() & 1)
			{
				std::printf("[Info] Padding added to \"%s\" (%u).\n", it->first.c_str(), static_cast<unsigned int>(it->second.size()));
				it->second.push_back_c(0);
			}
		}

		emsq["RGI_Emoji"].push_back_c(compositeclass);	//  Dummy data.
	}

	void read_scripts(rangeholder &sc, std::string &licensetext, const char *const filename, const char *const indir)
	{
		const srell::regex re_scriptdata("^\\s*([0-9A-Fa-f]{4,})(?:\\.\\.([0-9A-Fa-f]{4,}))?\\s*;\\s*([^\\s;#]+)\\s*");	//  (#.*)?$");
		ucprange range;
		std::string data;
		matchranges_type lines;
		srell::cmatch cmatch;
		ucprange_array assigned_ranges;

		data.clear();
		unishared::read_file(data, filename, indir);

		lines.clear();
		split2(lines, data, '\n');

		matchranges_type::size_type i = read_license(licensetext, lines, 0);

		for (; i < lines.size(); ++i)
		{
			const srell::csub_match &line = lines[i];

			if (srell::regex_search(line.first, line.second, cmatch, re_scriptdata, srell::regex_constants::match_continuous))
			{
				const srell::cmatch::value_type &begin = cmatch[1];
				const srell::cmatch::value_type &end = cmatch[2];
				const srell::cmatch::value_type &scriptname = cmatch[3];
//				const srell::cmatch::value_type &comment = cmatch[4];

				range.first = static_cast<ui_l32>(std::strtol(begin.first, NULL, 16));
				range.second = end.matched ? static_cast<ui_l32>(std::strtol(end.first, NULL, 16)) : range.first;

				sc[scriptname].join(range);
				assigned_ranges.join(range);
			}
		}
		assigned_ranges.negation();
		sc["Unknown"] = assigned_ranges;
	}

	canonicalname_mapper load_canonicalnames(const char *const *names)
	{
		canonicalname_mapper canonicalnames;
		matchranges_type parts;

		for (; **names; ++names)
		{
			parts.clear();
			split2(parts, *names, ':');
			const std::string canonicalname(parts[0].str());
			for (matchranges_type::size_type i = 0; i < parts.size(); ++i)
			{
				canonicalnames[parts[i].str()] = canonicalname;
			}
		}
		return canonicalnames;
	}

	void modify_for_scx(rangeholder &scx, const canonicalname_mapper &canonicalnames, std::string &licensetext, const char *const filename, const char *const indir)
	{
		const srell::regex re_scxdata("^\\s*([0-9A-Fa-f]{4,})(?:\\.\\.([0-9A-Fa-f]{4,}))?\\s*;\\s*([^\\s;#][^;#]*[^\\s;#])\\s*", srell::regex::multiline);	//  (#.*)?$");
		const srell::regex re_space(" ");
		const std::string name_common("Common");
		const std::string name_inherited("Inherited");
		ucprange_array common = scx[name_common];
		ucprange_array inherited = scx[name_inherited];
		ucprange range;
		std::map<std::string, bool> warning_out;
		std::string data;
		matchranges_type lines;
		srell::cmatch cmatch;

		unishared::read_file(data, filename, indir);

		lines.clear();
		split2(lines, data, '\n');

		matchranges_type::size_type i = read_license(licensetext, lines, 0);

		for (; i < lines.size(); ++i)
		{
			const srell::csub_match &line = lines[i];

			if (srell::regex_search(line.first, line.second, cmatch, re_scxdata, srell::regex_constants::match_continuous))
			{
				const srell::cmatch::value_type &begin = cmatch[1];
				const srell::cmatch::value_type &end = cmatch[2];
				const srell::cmatch::value_type &scxnames = cmatch[3];
//				const srell::cmatch::value_type &comment = cmatch[4];

				range.first = static_cast<ui_l32>(std::strtol(begin.str().c_str(), NULL, 16));
				range.second = end.matched ? static_cast<ui_l32>(std::strtol(end.str().c_str(), NULL, 16)) : range.first;

				common.remove_range(range);
				inherited.remove_range(range);

				srell::cregex_iterator2 rei2s(scxnames.first, scxnames.second, re_space);

				for (rei2s.split_begin();; rei2s.split_next())
				{
					const std::string scriptname(!rei2s.done() ? rei2s.split_range() : rei2s.split_remainder());

					if (scriptname.size())
					{
						const canonicalname_mapper::const_iterator it = canonicalnames.find(scriptname);

						if (it != canonicalnames.end())
							scx[it->second].join(range);
						else
						{
//							unishared::throw_error("Canonical name for \"%s\" is not found.", scriptname.c_str());
							if (!warning_out.count(scriptname))
							{
								std::printf("[Info] Canonical name for \"%s\" is not found. New script?\n", scriptname.c_str());
								warning_out[scriptname] = true;
							}
						}
					}
					if (rei2s.done())
						break;
				}
			}
		}
		scx[name_common] = common;
		scx[name_inherited] = inherited;
	}

	void combine_properties(sortedrangeholder &base, const rangeholder &addition, const char *const ptype, const char *const *aliasnames)
	{
		strings_type aliases;

		for (; **aliasnames; ++aliasnames)
			aliases.push_back(std::string(*aliasnames));

		return combine_properties(base, addition, ptype, aliases);
	}

	void combine_properties(sortedrangeholder &base, const rangeholder &addition, const char *const ptype, const strings_type &aliasnames)
	{
		sorted_name_and_ranges elem;
		matchranges_type names;

		for (strings_type::size_type i = 0; i < aliasnames.size(); ++i)
		{
			const std::string &aliases = aliasnames[i];
			bool pdata_found = false;

			names.clear();
			split2(names, aliases, ':');

			const std::string canonicalname(names[0].str());

			for (matchranges_type::size_type j = 0; j < names.size(); ++j)
			{
				const rangeholder::const_iterator it = addition.find(names[j].str());

				if (it != addition.end())
				{
					elem.ucpranges = it->second;
					pdata_found = true;
					break;
				}
			}

			if (!pdata_found)
				unishared::throw_error("No property value for \"%s\" found.", aliases.c_str());

			elem.ptype = ptype;
			elem.canonicalname = canonicalname;
			elem.namealiases = aliases;
			base.push_back(elem);
		}
	}

#if !defined(SRELL_NO_VMODE)

	void combine_pos(sortedseqholder &base, const seqholder &addition, const char *const ptype, const char *const *aliasnames)
	{
		ui_l32 total = 0;
		sorted_name_and_seqs elem;
		matchranges_type names;
		u32array compclass;

		//  Composite class.
		compclass.push_back_c(compositeclass);
		compclass.push_back_c(0);

		elem.ptype = ptype;
		for (; **aliasnames; ++aliasnames)
		{
			const std::string aliases(*aliasnames);
			bool pdata_found = false;

			names.clear();
			split2(names, aliases, ':');

			const std::string canonicalname(names[0].str());

			for (strings_type::size_type i = 0; i < names.size(); ++i)
			{
				const seqholder::const_iterator it = addition.find(names[i].str());

				if (it != addition.end())
				{
					elem.ucpseqs = it->second;
					pdata_found = true;
					if (elem.ucpseqs.size() != 1 || elem.ucpseqs[0] != compositeclass)
					{
						compclass.push_back(static_cast<ui_l32>(elem.ucpseqs.size()));
						total += static_cast<ui_l32>(elem.ucpseqs.size());
					}
					break;
				}
			}

			if (!pdata_found)
				unishared::throw_error("No property value for \"%s\" found.", aliases.c_str());

			elem.canonicalname = canonicalname;
			elem.namealiases = aliases;
			base.push_back(elem);
		}

		//  Composite class.
		compclass[1] = total;
		base[0].ucpseqs = compclass;	//  [0] = RGI_Emoji.
	}

#endif	//  !defined(SRELL_NO_VMODE)

	name_mapper create_ptype_mappings()
	{
		name_mapper categories;

		categories["gc"] = "general_category";
		categories["bp"] = "binary";
		categories["sc"] = "script";
		categories["scx"] = "script_extensions";
		return categories;
	}

	std::string create_ptypes(const name_mapper &ptypes)
	{
		const char *names[] = { "bp", "gc", "sc", "scx", "" };
		const std::string t2prefix = "\tuptype_";
		std::string ptypedef;

		for (unsigned int i = 0; *names[i];)
		{
			const char *const name = names[i];
			const name_mapper::const_iterator it = ptypes.find(name);

			if (it == ptypes.end())
				unishared::throw_error("Name for ptype \"%s\" is not found.", name);

			ptypedef += t2prefix + name + " = " + unishared::to_string(++i) + ",\n";
		}
		return ptypedef;
	}

	std::string ranges_to_string(const ucprange_array &array, const std::string &indent, const bool composite)
	{
		std::string rangestring(indent);

		if (composite)
		{
			rangestring += "//  ";

			for (ucprange_array::size_type i = 1; i < array.size(); ++i)
			{
				const ucprange &range = array[i];

				if (i > 1)
					rangestring += " + ";
				rangestring += static_cast<char>(range.first);
				rangestring += static_cast<char>(range.second);
				rangestring += ':' + unishared::to_string(array[++i].first);
			}
		}
		else
		{
			unsigned count = 0;

			for (ucprange_array::size_type i = 0; i < array.size(); ++i)
			{
				const ucprange &range = array[i];
				if (count == 4)
				{
					count = 0;
					rangestring += '\n' + indent;
				}
				else if (count)
					rangestring += ' ';

				rangestring += "0x" + unishared::to_string(range.first, 16, 4) + ", 0x" + unishared::to_string(range.second, 16, 4) + ',';
				++count;
			}
		}
		return rangestring;
	}

#if !defined(SRELL_NO_VMODE)
	std::string seqs_to_string(const u32array &array, const std::string &indent)
	{
		std::string seqstring;

		if (array.size() == 1 && array[0] == compositeclass)
		{
		}
		else
		{
			for (u32array::size_type i = 0; i < array.size();)
			{
				const ui_l32 num = array[i];

				if (num == compositeclass)
					break;

				if (num == 0)	//  Padding.
				{
					seqstring += indent + "0,\t//  Padding.\n";
					break;
				}

				if (++i == array.size())
					unishared::throw_error("[InternalError] No data follows %u.", num);

				seqstring += indent + unishared::to_string(num);
				seqstring += ", 0x" + unishared::to_string(array[i++], 16, 4);

				if (num == 1)	//  Range.
				{
					if (i == array.size())
						unishared::throw_error("[InternalError] No pair for %.4lX.", array[i - 1]);

					seqstring += ", 0x" + unishared::to_string(array[i++], 16, 4);
				}
				else
				{
					for (ui_l32 j = 2; j < num; ++j)
					{
						if (i == array.size())
							unishared::throw_error("[InternalError] Broken after %.4lX.", array[i - 1]);

						seqstring += ", 0x" + unishared::to_string(array[i++], 16, 4);
					}
				}
				seqstring += ",\n";
			}

			if (seqstring.size())
				seqstring.resize(seqstring.size() - 1);
		}
		return seqstring;
	}
#endif	//  !defined(SRELL_NO_VMODE)

	void drop_finalcomma(std::string &data)
	{
		std::string::size_type commapos = data.rfind(',');
		if (commapos != std::string::npos)
			data.erase(commapos, 1);
	}

	std::string create_pnametable(ui_l32 &count, const std::string &indent, const up_options &opts)
	{
		const char *const *pnames = updata::property_names;
		std::string out;
		namenumber_mapper categories;

		count = 0u;
		for (unsigned int i = 2; **pnames; ++pnames, ++i)
		{
			const std::string names(*pnames);
			srell::cregex_iterator2 rei2(names, re_colon_);

			for (rei2.split_begin();; rei2.split_next())
			{
				const std::string name(!rei2.done() ? rei2.split_range() : rei2.split_remainder());
				categories[name] = i;
				++count;

				if (rei2.done())
					break;
			}
		}

		out.assign(indent + "{ \"\", " + unishared::to_string(count) + " },\n");

		for (namenumber_mapper::const_iterator it = categories.begin(); it != categories.end(); ++it)
		{
			out.append(indent);
			out.append("{ \"");
			if (!opts.noesc)
				out.append(escape_string(it->first));
			else
				out.append(it->first);
			out.append("\", " + unishared::to_string(it->second) + " },\n");
		}
		return out;
	}

	std::string join_dropcomma_append(const strings_type &s, const std::string &return_table)
	{
		std::string tmp(join('\n', s, true));

		drop_finalcomma(tmp);
		tmp.append(return_table);
		return tmp;
	}

	void do_formatting(std::string &out, const sortedrangeholder &alldata, const sortedseqholder &emsq, const up_options &opts)
	{
		const std::size_t numofproperties = sizeof (updata::property_names) / sizeof (updata::property_names[0]) + 1;
		const std::string template1("template <typename T1, typename T2, typename T3>\n");
		const std::string template2("unicode_property_data<T1, T2, T3>::");
		const std::string return_table("};\n");
		const std::string indent("\t");
		name_mapper ptype_mappings(create_ptype_mappings());
		const std::string ptypes(create_ptypes(ptype_mappings));
		const std::string t1head("\t");
		const std::string t1prefix(t1head + "upid_");

		const ui_l32 pno_base = numofproperties;
		ui_l32 offset = 0u;
		ui_l32 property_number = pno_base;
		ui_l32 property_id_number = pno_base;

		std::string pnumbers(t1prefix + "unknown = 0,\n");
		strings_type rangetable;
		strings_type lookup_ranges;
		std::string lookup_numbers;
		namenumber_mapper rangeno_map;

		pnumbers += t1prefix + "invalid = 0,\n";
		pnumbers += t1prefix + "error = 0,\n";
		pnumbers += ptypes;

		namenumber_mapper registered;
		srell::re_detail::simple_array<ucprange> rangepos;
		srell::cregex_iterator2 rei2;

		for (sortedrangeholder::size_type i = 0; i < alldata.size(); ++i)
		{
			const sorted_name_and_ranges &elem = alldata[i];
			const std::string ptype = elem.ptype;
			const std::string name = elem.canonicalname;
			const std::string aliases = elem.namealiases;
			const ucprange_array &array = elem.ucpranges;
			const std::string pnumber_keyname(ptype + '_' + name);
			const std::string position_comment(' ' + ptype + '=' + aliases);
			const bool compositeclass_found = array.size() && array[0].first == compositeclass;
			std::string rangestring(ranges_to_string(array, indent, compositeclass_found));
			ui_l32 numofranges = static_cast<ui_l32 >(array.size());
			ui_l32 pno = property_number;
			const namenumber_mapper::const_iterator rit = registered.find(rangestring);

			if (rit != registered.end())
			{
				pno = rit->second;

				lookup_ranges[pno - pno_base] += position_comment;
				rangetable[(pno - pno_base) * 2] += position_comment;

				rei2.assign(aliases, re_colon_);

				for (rei2.split_begin();; rei2.split_next())
				{
					const std::string alias(!rei2.done() ? rei2.split_range() : rei2.split_remainder());

					rangeno_map[ptype + ':' + alias] = pno;
					if (rei2.done())
						break;
				}
			}
			else
			{
				//  ucpranges of "Assigned" is empty.
				if (compositeclass_found)
				{
					std::printf("[Info] Composite property \"%s\" found.\n", aliases.c_str());
					numofranges = array[0].second;
				}
				else
					registered[rangestring] = property_number;

				rei2.assign(aliases, re_colon_);

				for (rei2.split_begin();; rei2.split_next())
				{
					const std::string alias(!rei2.done() ? rei2.split_range() : rei2.split_remainder());

					rangeno_map[ptype + ':' + alias] = property_number;
					if (rei2.done())
						break;
				}

				lookup_ranges.push_back(indent + "{ " + unishared::to_string(offset) + ", " + unishared::to_string(numofranges) + " },\t//  #" + unishared::to_string(pno) + position_comment);

				rangetable.push_back(indent + "//  #" + unishared::to_string(pno) + " (" + unishared::to_string(offset) + '+' + unishared::to_string(numofranges) + "):" + position_comment);
				rangetable.push_back(rangestring);

				rangepos.push_back(ucprange_helper(offset, numofranges));

				if (!compositeclass_found)
					offset += numofranges;

				++property_number;
			}

			pnumbers.append(t1head + pnumber_keyname + " = " + unishared::to_string(pno) + "," + (pno != property_id_number ? ("\t//  #" + unishared::to_string(property_id_number)) : "") + '\n');

			++property_id_number;
		}

		pnumbers.append(t1prefix + "max_property_number = " + unishared::to_string(property_number - 1) + ",\n");

#if !defined(SRELL_NO_VMODE)
		if (rangetable.size())
			drop_finalcomma(rangetable[rangetable.size() - 1]);

		rangetable.push_back("#if !defined(SRELL_NO_UNICODE_POS)\n" + indent + ",");

		for (sortedseqholder::size_type i = 0; i < emsq.size(); ++i)
		{
			const sorted_name_and_seqs &elem = emsq[i];
			const std::string ptype = elem.ptype;
			const std::string name = elem.canonicalname;
			const std::string aliases = elem.namealiases;
			const u32array &array = elem.ucpseqs;
			const bool compositeclass_found = array.size() && array[0] == compositeclass;
			const std::string pnumber_keyname(ptype + '_' + name);
			const std::string position_comment(' ' + ptype + '=' + aliases);
			ui_l32 numofseqs = static_cast<ui_l32>(array.size());
			std::string seqstring;

			if (compositeclass_found)
			{
				std::printf("[Info] Composite property \"%s\" found.\n", aliases.c_str());
				numofseqs = array[1];
				seqstring = indent + "//  ";

				for (u32array::size_type j = 2; j < array.size(); ++j)
				{
					if (j > 2)
						seqstring += " + ";
					seqstring += unishared::to_string(array[j]) + "/2";
				}
			}
			else
				seqstring = seqs_to_string(array, indent);

			const ui_l32 numofranges = numofseqs / 2;

			pnumbers.append(t1head + pnumber_keyname + " = " + unishared::to_string(property_number) + ",\t//  #" + unishared::to_string(property_id_number) + '\n');

			rei2.assign(aliases, re_colon_);

			for (rei2.split_begin();; rei2.split_next())
			{
				const std::string alias(!rei2.done() ? rei2.split_range() : rei2.split_remainder());

				rangeno_map[ptype + ':' + aliases] = property_number;
				if (rei2.done())
					break;
			}

			lookup_ranges.push_back(indent + "{ " + unishared::to_string(offset) + ", " + unishared::to_string(numofranges) + " },\t//  #" + unishared::to_string(property_number) + position_comment);
			rangetable.push_back(indent + "//  #" + unishared::to_string(property_number) + " (" + unishared::to_string(offset) + '+' + unishared::to_string(numofseqs) + "/2):" + position_comment);
			rangetable.push_back(seqstring);

			++property_number;
			++property_id_number;
			if (!compositeclass_found)
				offset += numofranges;
		}

		pnumbers.append(t1prefix + "max_pos_number = " + unishared::to_string(--property_number) + "\n");
		rangetable.push_back("#endif\t//  !defined(SRELL_NO_UNICODE_POS)");

#endif	//  !defined(SRELL_NO_VMODE)

		ui_l32 basepos = 0u;
		std::string pnames(create_pnametable(basepos, indent, opts));
		u32pair posinfo[numofproperties];

		sort_rangeno_table(posinfo, basepos, lookup_numbers, rangeno_map, indent, opts);
		lookup_numbers.append(return_table);
		merge_posinfo(lookup_ranges, posinfo, numofproperties, indent);
		pnames.insert(0, template1 + "const T1 " + template2 + "propertynumbertable[] =\n{\n");

		out.append("enum upid_type\n{\n");
		out.append(pnumbers);
		out.append("};\n\n");

		out.append(template1 + "struct unicode_property_data\n{\n");
		out.append("\tstatic const T1 propertynumbertable[];\n");
		out.append("\tstatic const T2 positiontable[];\n");
		out.append("\tstatic const T3 rangetable[];\n");
		out.append("};\n\n");

		out.append(pnames);
		out.append(lookup_numbers);
		out.append("\n");

		out.append(template1 + "const T2 " + template2 + "positiontable[] =\n{\n\t{ 0, 0 },\t//  #0 unknown\n");
		out.append(join_dropcomma_append(lookup_ranges, return_table));
		out.append("\n");

		out.append(template1 + "const T3 " + template2 + "rangetable[] =\n{\n");
		out.append(join_dropcomma_append(rangetable, return_table));

		out.append("#define SRELL_UPDATA_VERSION " + unishared::to_string(static_cast<unsigned int>(opts.version)) + "\n");
	}

	void sort_rangeno_table(u32pair *const posinfo, ui_l32 offset, std::string &lookup_numbers, const namenumber_mapper &rangeno_map, const std::string &indent, const up_options &opts)
	{
		strings_type names;
		name_mapper pvalues;
		namenumber_mapper pcounts;
		srell::cregex_iterator2 rei2;

		for (namenumber_mapper::const_iterator it = rangeno_map.begin(); it != rangeno_map.end(); ++it)
		{
			names.clear();
			rei2.assign(it->first, re_colon_);

			for (rei2.split_begin();; rei2.split_next())
			{
				names.push_back(rei2.split_aptrange());

				if (rei2.done() || names.size() > 2)
					break;
			}

			if (names.size() == 2)
			{
				const std::string &pname = names[0];
				const std::string &pvalue = names[1];
				if (!opts.noesc)
					pvalues[pname] += indent + "{ \"" + escape_string(pvalue) + "\", " + unishared::to_string(it->second) + " },\n";
				else
					pvalues[pname] += indent + "{ \"" + pvalue + "\", " + unishared::to_string(it->second) + " },\n";
				++pcounts[pname];
			}
		}

		offset += set_pvalue_and_count(lookup_numbers, posinfo[2], "gc", offset, pcounts, pvalues, indent);
		offset += set_pvalue_and_count(lookup_numbers, posinfo[1], "bp", offset, pcounts, pvalues, indent);
		offset += set_pvalue_and_count(lookup_numbers, posinfo[3], "sc", offset, pcounts, pvalues, indent);
		offset += set_pvalue_and_count(lookup_numbers, posinfo[4], "scx", offset, pcounts, pvalues, indent);
		drop_finalcomma(lookup_numbers);
	}

	ui_l32 set_pvalue_and_count(std::string &lookup_numbers, u32pair &posinfo, const std::string category, const ui_l32 offset, namenumber_mapper &pcounts, name_mapper &pvalues, const std::string &indent)
	{
		lookup_numbers.append(indent + "//  " + category + ": " + unishared::to_string(pcounts[category]) + "\n" + pvalues[category]);
		posinfo.set(offset + 1, pcounts[category]);
		return posinfo.second;
	}

	void merge_posinfo(strings_type &lookup_ranges, const u32pair *const posinfo, const std::size_t numofproperties, const std::string &indent)
	{
		for (std::size_t i = 1; i < numofproperties; ++i)
		{
			const u32pair &pair = posinfo[i];
			const std::string line(indent + "{ " + unishared::to_string(pair.first) + ", " + unishared::to_string(pair.second) + " },\t//  #" + unishared::to_string(i) + ' ' + (i == 1 ? "binary" : updata::property_names[i - 2]));

			lookup_ranges.insert(lookup_ranges.begin() + i - 1, line);
		}
	}

	std::string escape_string(const std::string &s)
	{
		static const char hex[] = "0123456789ABCDEF";
		std::string out;

		for (std::string::size_type i = 0; i < s.size(); ++i)
		{
			out.append("\\x");
			out.append(1, hex[(s[i] >> 4) & 15]);
			out.append(1, hex[s[i] & 15]);
		}
		return out;
	}

	srell::regex re_colon_;
};
//  class unicode_property

int main(const int argc, const char *const *const argv)
{
	up_options upopts(argc, argv);
	std::string outdata;
	unicode_property up;
	int errorno = up.create_updata(outdata, upopts);

	if (errorno == 0)
	{
		if (!unishared::write_file(upopts.outfilename, outdata))
			errorno = 2;
	}
	return errorno;
}
