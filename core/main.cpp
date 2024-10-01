#include "util/util.hpp"
#include "dir.hpp"

#include <iostream>

int main(int argc, char** argv) {
	auto program = core::util::next_arg(&argc, &argv);

	auto arg = core::util::next_arg(&argc, &argv);
	if (arg == nullptr) {
		std::cerr << "[USAGE] <program> <root of the project>\n";
		return -1;
	}

	core::dir p;
	if (!p.set_path(arg)) {
		std::cerr << "[ERROR] invalid path: " << arg << '\n';
		return -1;
	}

	std::cout << "[INFO] Creating documentation starting from " << arg << '\n';
}
