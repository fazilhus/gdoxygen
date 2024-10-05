#include "util.hpp"

#include <locale>
#include <vector>

namespace docs_gen_core::util {

	char* next_arg(int* argc, char*** argv) {
		if (*argc == 0) return nullptr;

		char* res = **argv;
		*argc -= 1;
		*argv += 1;
		return res;
	}

	std::wstring to_wstring(const std::string& s) {
		std::vector<wchar_t> buf(s.size());
		std::use_facet<std::ctype<wchar_t>>(std::locale()).widen(s.data(),
		                                                         s.data() + s.size(),
		                                                         buf.data());
		return {buf.data(), buf.size()};
	}
	
} // docs_gen_core::util