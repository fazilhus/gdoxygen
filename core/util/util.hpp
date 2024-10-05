#ifndef DOCS_GEN_UTIL_H
#define DOCS_GEN_UTIL_H

#include <string>

namespace docs_gen_core::util {

	char* next_arg(int* argc, char*** argv);
	std::wstring to_wstring(const std::string& s);

} // docs_gen_core::util

#endif // DOCS_GEN_UTIL_H