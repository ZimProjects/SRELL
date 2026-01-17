//
//  ucfdataout2.cpp: version 2.105 (2025/11/02).
//
//  This is a program that generates srell_ucfdata2.h from CaseFolding.txt
//  provided by the Unicode Consortium. The latese version is available at:
//  http://www.unicode.org/Public/UNIDATA/CaseFolding.txt
//

#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <algorithm>	//  For std::swap in C++98/03
#include <utility>	//  For std::swap in C++11-
#define SRELL_NO_UNICODE_DATA
#include "../srell.hpp"

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(disable:4996)
#endif

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

bool read_file(std::string &str, const char *const filename, const char *const dir)
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
			return true;
		}
	}
	std::fputs("failed...\n", stdout);
	return false;
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
}
//  namespace unishared

struct ucf_options
{
	const char *infilename;
	const char *outfilename;
	const char *indir;
	int version;
	int errorno;

	ucf_options(const int argc, const char *const *const argv)
		: infilename("CaseFolding.txt")
		, outfilename("srell_ucfdata2.h")
		, indir("")
		, version(201)
		, errorno(0)
	{

		for (int index = 1; index < argc; ++index)
		{
			const char firstchar = argv[index][0];

			if (firstchar == '-' || firstchar == '/')
			{
				const char *const option = argv[index] + 1;

				++index;
				if (std::strcmp(option, "i") == 0)
				{
					if (index >= argc)
						goto NO_ARGUMENT;
					infilename = argv[index];
				}
				else if (std::strcmp(option, "o") == 0)
				{
					if (index >= argc)
						goto NO_ARGUMENT;
					outfilename = argv[index];
				}
				else if (std::strcmp(option, "id") == 0)
				{
					if (index >= argc)
						goto NO_ARGUMENT;
					indir = argv[index];
				}
				else if (std::strcmp(option, "?") == 0 || std::strcmp(option, "h") == 0)
				{
					std::fputs("Usage: ucfdataout2 [options]\nOptions:\n", stdout);
					std::fputs("  -i <FILE>\t\tRead data from <FILE>.\n", stdout);
					std::fputs("  -id <DIRECTORY>\tAssume that input file exist in <DIRECTORY>.\n\t\t\t<DIRECTORY> must ends with '/' or '\\'.\n", stdout);
					std::fputs("  -o <FILE>\t\tOutput to <FILE>.\n", stdout);
					errorno = 1;
					return;
				}
				else
				{
					--index;
					goto UNKNOWN_OPTION;
				}

				continue;

				NO_ARGUMENT:
				std::fprintf(stdout, "[Error] no argument for \"%s\" specified.\n", argv[--index]);
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
//  struct ucf_options

class unicode_casefolding
{
public:

	unicode_casefolding()
		: maxdelta_(0L), maxdelta_cp_(0L), ucf_maxcodepoint_(0L), rev_maxcodepoint_(0L)
		, ucf_numofsegs_(1U), rev_numofsegs_(1U), numofcps_from_(0U), numofcps_to_(0U)
		, max_appearance_(0U), nextoffset_(0x100L), rev_charsets_(1, -1L)
	{
	}

	int create_ucfdata(std::string &outdata, const ucf_options &opts)
	{
		const std::string indent("\t\t\t");
		int errorno = opts.errorno;
		std::string buf;

		if (errorno)
			return errorno;

		if (unishared::read_file(buf, opts.infilename, opts.indir))
		{
			const srell::regex re_line("^.*$", srell::regex::multiline);
			const srell::regex re_license("^# (.*)$", srell::regex::sticky);
			const srell::regex re_cfdata("^\\s*([0-9A-Fa-f]+); ([CS]); ([0-9A-Fa-f]+);\\s*#\\s*(.*)$", srell::regex::sticky);
			srell::cregex_iterator2 iter(buf.c_str(), buf.c_str() + buf.size(), re_line);
			srell::cmatch match;

			for (; !iter.done(); ++iter)
			{
				if (iter->length(0))
				{
					if (!srell::regex_match((*iter)[0].first, (*iter)[0].second, match, re_license))
					{
						outdata.append(1, '\n');
						break;
					}
					outdata += "//  " + match.str(1) + "\n";
				}
			}

			outdata += "template <typename T1, typename T2>\nstruct unicode_casefolding\n{\n";

			for (; !iter.done(); ++iter)
			{
				if (srell::regex_match((*iter)[0].first, (*iter)[0].second, match, re_cfdata))
				{
					const std::string from(match[1].str());
					const std::string to(match[3].str());
					const std::string type(match[2].str());
					const std::string name(match[4].str());

					update(from, to);
				}
			}

			outdata += "\tstatic const T1 ucf_maxcodepoint = 0x" + unishared::to_string(ucf_maxcodepoint_, 16, 4) + ";\n";
			outdata += "\tstatic const T2 ucf_deltatablesize = 0x" + unishared::to_string(ucf_numofsegs_ << 8, 16) + ";\n";

			outdata += "\tstatic const T1 rev_maxcodepoint = 0x" + unishared::to_string(rev_maxcodepoint_, 16, 4) + ";\n";
			outdata += "\tstatic const T2 rev_indextablesize = 0x" + unishared::to_string(rev_numofsegs_ << 8, 16) + ";\n";
			outdata += "\tstatic const T2 rev_charsettablesize = " + unishared::to_string(numofcps_to_ * 2 + numofcps_from_ + 1) + ";\t//  1 + " + unishared::to_string(numofcps_to_) + " * 2 + " + unishared::to_string(numofcps_from_) + "\n";
			outdata += "\tstatic const T2 rev_maxset = " + unishared::to_string(maxset()) + ";\n";
			outdata += "\tstatic const T1 eos = 0;\n";

			outdata += "\n\tstatic const T1 ucf_deltatable[];\n\tstatic const T2 ucf_segmenttable[];\n\tstatic const T2 rev_indextable[];\n\tstatic const T2 rev_segmenttable[];\n\tstatic const T1 rev_charsettable[];\n";

			outdata += "};\ntemplate <typename T1, typename T2>\n\tconst T1 unicode_casefolding<T1, T2>::ucf_maxcodepoint;\ntemplate <typename T1, typename T2>\n\tconst T2 unicode_casefolding<T1, T2>::ucf_deltatablesize;\ntemplate <typename T1, typename T2>\n\tconst T1 unicode_casefolding<T1, T2>::rev_maxcodepoint;\ntemplate <typename T1, typename T2>\n\tconst T2 unicode_casefolding<T1, T2>::rev_indextablesize;\ntemplate <typename T1, typename T2>\n\tconst T2 unicode_casefolding<T1, T2>::rev_charsettablesize;\ntemplate <typename T1, typename T2>\n\tconst T2 unicode_casefolding<T1, T2>::rev_maxset;\ntemplate <typename T1, typename T2>\n\tconst T1 unicode_casefolding<T1, T2>::eos;\n\n";
			out_v2tables(outdata);
			outdata += "#define SRELL_UCFDATA_VERSION " + unishared::to_string(static_cast<unsigned int>(opts.version)) + "\n";

			std::fprintf(stdout, "MaxDelta: %+ld (U+%.4lX->U+%.4lX)\n", maxdelta_, maxdelta_cp_, maxdelta_cp_ + maxdelta_);
		}
		else
			errorno = 1;

		return errorno;
	}

private:

	void update(const std::string &from, const std::string &to)
	{
		const long cp_from = std::strtol(from.c_str(), NULL, 16);
		const long cp_to = std::strtol(to.c_str(), NULL, 16);
		const long delta = cp_to - cp_from;
		const long segno_from = cp_from >> 8;
		const long segno_to = cp_to >> 8;

		update_tables(cp_from, cp_to, segno_from);

		++numofcps_from_;
		if (std::abs(maxdelta_) < std::abs(delta))
		{
			maxdelta_cp_ = cp_from;
			maxdelta_ = delta;
		}

		if (ucf_maxcodepoint_ < cp_from)
			ucf_maxcodepoint_ = cp_from;

		if (rev_maxcodepoint_ < cp_to)
			rev_maxcodepoint_ = cp_to;

		if (rev_maxcodepoint_ < cp_from)
			rev_maxcodepoint_ = cp_from;

		if (!ucf_countedsegnos.count(segno_from))
		{
			ucf_countedsegnos[segno_from] = 1;
			++ucf_numofsegs_;
		}

		if (!rev_countedsegnos.count(segno_to))
		{
			rev_countedsegnos[segno_to] = 1;
			++rev_numofsegs_;
		}
		if (!rev_countedsegnos.count(segno_from))
		{
			rev_countedsegnos[segno_from] = 1;
			++rev_numofsegs_;
		}

		if (!cps_counted_as_foldedto.count(cp_to))
		{
			cps_counted_as_foldedto[cp_to] = 1;
			++numofcps_to_;
		}

		if (appearance_counts_.count(cp_to))
			++appearance_counts_[cp_to];
		else
			appearance_counts_[cp_to] = 1;

		if (max_appearance_ < appearance_counts_[cp_to])
			max_appearance_ = appearance_counts_[cp_to];
	}

	unsigned int maxset() const
	{
		return max_appearance_ + 1;
	}

	void out_v2tables(std::string &outdata)
	{
		const char *const headers[] = {
			"template <typename T1, typename T2>\nconst ",
			" unicode_casefolding<T1, T2>::",
			"[] =\n{\n"
		};

		create_revtables();
		out_lowertable(outdata, headers, "T1", "ucf_deltatable", ucf_deltas_, ucf_segments_);
		outdata.append(1, '\n');
		out_uppertable(outdata, headers, "T2", "ucf_segmenttable", ucf_segments_);
		outdata.append(1, '\n');
		out_lowertable(outdata, headers, "T2", "rev_indextable", rev_indices_, rev_segments_);
		outdata.append(1, '\n');
		out_uppertable(outdata, headers, "T2", "rev_segmenttable", rev_segments_);
		outdata.append(1, '\n');
		out_cstable(outdata, headers, "T1", "rev_charsettable", rev_charsets_);
	}

	//  Updates ucf_segments_, ucf_deltas_, and rev_charsets_.
	void update_tables(const long cp_from, const long cp_to, const long segno_from)
	{
		if (segno_from >= static_cast<long>(ucf_segments_.size()))
			ucf_segments_.resize(segno_from + 1, 0L);

		long &offset_of_segment = ucf_segments_[segno_from];

		if (offset_of_segment == 0L)
		{
			offset_of_segment = nextoffset_;
			nextoffset_ += 0x100L;
			ucf_deltas_.resize(nextoffset_, 0L);
		}

		ucf_deltas_[offset_of_segment + (cp_from & 0xffL)] = cp_to - cp_from;

		for (long index = 0L;; ++index)
		{
			if (index == static_cast<long>(rev_charsets_.size()))
			{
				rev_charsets_.push_back(cp_to);
				rev_charsets_.push_back(cp_from);
				rev_charsets_.push_back(-1L);
				break;
			}
			if (rev_charsets_[index] == cp_to)
			{
				for (++index; rev_charsets_[index] != -1L; ++index);

				rev_charsets_.insert(index, 1, cp_from);
				break;
			}
		}
	}

	//  Creates rev_segments_ and rev_indices_ from rev_charsets_.
	void create_revtables()
	{
		long nextoffset = 0x100L;
		for (long index = 0L; index < static_cast<long>(rev_charsets_.size()); ++index)
		{
			const long bocs = index;	//  Beginning of charset.

			for (; rev_charsets_[index] != -1L; ++index)
			{
				const long &u21ch = rev_charsets_[index];
				const long segno = u21ch >> 8L;

				if (segno >= static_cast<long>(rev_segments_.size()))
					rev_segments_.resize(segno + 1, 0L);

				long &offset_of_segment = rev_segments_[segno];

				if (offset_of_segment == 0L)
				{
					offset_of_segment = nextoffset;
					nextoffset += 0x100L;
					rev_indices_.resize(nextoffset, 0L);
				}
				rev_indices_[offset_of_segment + (u21ch & 0xffL)] = bocs;
			}
		}
	}

	void out_lowertable(std::string &outdata, const char *const headers[], const char *const type, const char *const funcname, const std::basic_string<long> &table, const std::basic_string<long> &segtable) const
	{
		const long end = static_cast<long>(table.size());

		outdata += headers[0];
		outdata += type;
		outdata += headers[1];
		outdata += funcname;
		outdata += headers[2];

		for (long i = 0L; i < end;)
		{
			const long col = i & 15L;

			if ((i & 255L) == 0)
			{
				if (i != 0L)
				{
					for (long j = 0L; j < static_cast<long>(segtable.size()); ++j)
					{
						if (segtable[j] == i)
						{
							outdata += "\n\t//  For u+" + unishared::to_string(j, 16, 2) + "xx (" + unishared::to_string(i) + ")\n";
							break;
						}
					}
				}
				else
					outdata += "\t//  For common (0)\n";
			}

			outdata += col == 0 ? "\t" : (col & 3) == 0 ? "  " : " ";
			if (table[i] >= 0L)
				outdata += unishared::to_string(table[i]);
			else
				outdata += "static_cast<", outdata += type, outdata += ">(", outdata += unishared::to_string(table[i]) + ")";

			if (++i == end)
				outdata.append(1, '\n');
			else if (col == 15L)
				outdata += ",\n";
			else
				outdata.append(1, ',');
		}
		outdata += "};\n";
	}

	void out_uppertable(std::string &outdata, const char *const headers[], const char *const type, const char *const funcname, const std::basic_string<long> &table) const
	{
		int end = static_cast<int>(table.size());

		outdata += headers[0];
		outdata += type;
		outdata += headers[1];
		outdata += funcname;
		outdata += headers[2];

		for (int i = 0; i < end;)
		{
			const int col = i & 15;

			outdata += col == 0 ? "\t" : (col & 3) == 0 ? "  " : " ";
			if (table[i] >= 0)
				outdata += unishared::to_string(table[i]);
			else
				outdata += "static_cast<", outdata += type, outdata += ">(", outdata += unishared::to_string(table[i]) + ")";

			if (++i == end)
				outdata.append(1, '\n');
			else if (col == 15)
				outdata += ",\n";
			else
				outdata.append(1, ',');
		}
		outdata += "};\n";
	}

	void out_cstable(std::string &outdata, const char *const headers[], const char *const type, const char *const funcname, const std::basic_string<long> &table) const
	{
		int end = static_cast<int>(table.size());
		bool newline = true;
		int bos = 0;
		int prevprintedbos = -1;

		outdata += headers[0];
		outdata += type;
		outdata += headers[1];
		outdata += funcname;
		outdata += headers[2];

		for (int i = 0; i < end;)
		{
			const long val = table[i];

			outdata += newline ? "\t" : " ";
			newline = false;

			if (val == -1L)
				outdata += "eos";
			else
				outdata += "0x", outdata += unishared::to_string(val, 16, 4);

			if (++i != end)
				outdata.append(1, ',');

			if (val == -1L)
			{
				if (prevprintedbos != bos / 10 || i == end)
				{
					outdata += "\t//  ";
					outdata += unishared::to_string(bos);
					prevprintedbos = bos / 10;
				}
				outdata.append(1, '\n');
				newline = true;
				bos = i;
			}
		}
		outdata += "};\n";
	}

	typedef std::map<long, char> flagset_type;

	long maxdelta_;	//  = 0L;
	long maxdelta_cp_;	//  = 0L;
	long ucf_maxcodepoint_;	//  = 0L;	//  The max code point for case-folding.
	long rev_maxcodepoint_;	//  = 0L;	//  The max code point for reverse lookup.
	unsigned int ucf_numofsegs_;	//  = 1U;	//  The number of segments in the delta table.
	unsigned int rev_numofsegs_;	//  = 1U;	//  The number of segments in the table for reverse lookup.
	unsigned int numofcps_from_;	//  = 0U;	//  The number of code points in "folded from"s.
	unsigned int numofcps_to_;	//  = 0U;	//  The number of code points in "folded to"s.

	flagset_type ucf_countedsegnos;	//  The set of segment nos marked as "counted" for case-folding.
	flagset_type rev_countedsegnos;	//  The set of segment nos marked as "counted" for reverse lookup.
	flagset_type cps_counted_as_foldedto;	//  The set of code points marked as "folded to".

	unsigned int max_appearance_;
	std::map<long, unsigned int> appearance_counts_;

	long nextoffset_;
	std::basic_string<long> ucf_deltas_;
	std::basic_string<long> ucf_segments_;
	std::basic_string<long> rev_indices_;
	std::basic_string<long> rev_segments_;
	std::basic_string<long> rev_deltas_;
	std::basic_string<long> rev_charsets_;
};
//  class unicode_casefolding

int main(const int argc, const char *const *const argv)
{
	ucf_options ucfopts(argc, argv);
	std::string outdata;
	unicode_casefolding ucf;
	int errorno = ucf.create_ucfdata(outdata, ucfopts);

	if (errorno == 0)
	{
		if (!unishared::write_file(ucfopts.outfilename, outdata))
			errorno = 2;
	}
	return errorno;
}
