#include "util/util.hpp"
#include "dir.hpp"

#include <iostream>
#include <chrono>

int main(int argc, char** argv) {
	auto start = std::chrono::high_resolution_clock::now();
	auto program = core::util::next_arg(&argc, &argv);

	auto path = core::util::next_arg(&argc, &argv);
	if (path == nullptr) {
		std::cerr << "[USAGE] <program> <root of the project>\n";
		return -1;
	}

	core::dir p;
	char* arg;
	while ((arg = core::util::next_arg(&argc, &argv)) != nullptr) {
		p.push_ignored_folder(arg);
	}

	if (!p.set_path(path)) {
		std::cerr << "[ERROR] invalid path: " << arg << '\n';
		return -1;
	}

	p.construct_file_tree();
	p.gen_docs();

	std::cout << "[INFO] Creating documentation starting from " << path << '\n';
	auto stop = std::chrono::high_resolution_clock::now();
	std::cout << "Time took " << (stop - start) / 1000000000.0f << '\n';
}
