#ifndef DOCS_GEN_UTIL_H
#define DOCS_GEN_UTIL_H

#include <string>
#include <vector>

namespace docs_gen_core::util {

	char* next_arg(int* argc, char*** argv);
	std::wstring to_wstring(const std::string& s);
	void split_by(const std::wstring& s, wchar_t delim, std::vector<std::wstring>& elems);

} // docs_gen_core::util

#endif // DOCS_GEN_UTIL_H