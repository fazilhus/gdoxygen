#include "util.hpp"

namespace core::util {

	char* next_arg(int* argc, char*** argv) {
		if (*argc == 0) return nullptr;

		char* res = **argv;
		*argc -= 1;
		*argv += 1;
		return res;
	}

} // core::util