/*
 * Summary: poorly optimized html tags stripper
 * Description: strips html tags while retrieving encased data
 * Based on regex
 *
 * Author: strmbld wlfhrtd
 */

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <algorithm>
#include <cctype>

static inline void ltrim(std::string& s) { s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); })); }
static inline void rtrim(std::string& s) { s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end()); }
static inline void trim(std::string& s) { ltrim(s); rtrim(s); }
static inline std::string ltrim_by_copy(std::string s) { ltrim(s); return s; }
static inline std::string rtrim_by_copy(std::string s) { rtrim(s); return s; }
static inline std::string trim_by_copy(std::string s) { trim(s); return s; }

static inline void replaceAll(std::string& s, const std::string& needle, const std::string& replacement)
{
	size_t start_pos = 0;
	while ((start_pos = s.find(needle, start_pos)) != std::string::npos) {
		s.replace(start_pos, needle.length(), replacement);
		start_pos += replacement.length();
	}
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "\nInvalid argument\n"
			<< "Enter input/output filenames (e.g. input.txt result.txt)\n";
		return -1;
	}

	std::cout << "Enter tags (separated by space) for data that should be extracted.\n"
		<< "(e.g. li h2 b i a)\n"
		<< "Type 'quit' (without quotes) to stop input.\n";

	std::vector<std::string> tags = {};
	std::vector<std::pair<std::string, std::string>> htmlTags = {};

	while (true)
	{
		std::string s;
		std::cin >> s;
		if (s == "quit")
		{
			break;
		}
		tags.emplace_back(s);
		
		std::string opTag = "<" + s + ">";
		std::string clTag = "</" + s + ">";
		std::pair<std::string, std::string> htmlTagPair(opTag, clTag);
		htmlTags.emplace_back(htmlTagPair);
	}

	std::ifstream txtFile;
	txtFile.open(argv[1]);
	if (!txtFile.is_open())
	{
		std::cerr << "\nUnable to load input file.\n";
		return -1;
	}

	// regex template: \<(li|i|b|h2){1}>.+<\/(li|i|b|h2){1}\>
	std::string captureGroup = "(";
	for (auto& x : tags)
	{
		captureGroup += x + "|";
	}
	captureGroup[captureGroup.size() - 1] = ')'; // (li|i|b)
	std::cout << "Capture group: " << captureGroup << std::endl;
	std::string sRegexpr = "<" + captureGroup + "{1}>.+</" + captureGroup + "{1}>";
	std::cout << "Regular expression: " << sRegexpr << std::endl;
	std::regex search(sRegexpr);

	std::string document = "";
	while (!txtFile.eof())
	{
		std::string sBuffer = "";
		std::getline(txtFile, sBuffer);
		std::smatch m;
		std::regex_search(sBuffer, m, search);
		if (!m.empty())
		{
			std::string matched = m[0];
			for (auto& pair : htmlTags)
			{
				replaceAll(matched, pair.first, " ");
				replaceAll(matched, pair.second, " ");
			}
			trim(matched);
			document += matched + "\n";
		}
	}
	txtFile.close();

	std::ofstream output;
	output.open(argv[2]);
	if (!output.is_open())
	{
		std::cerr << "\nUnable to open output file.\n";
		return -1;
	}
	output << document;
	output.close();
	return 0;
}