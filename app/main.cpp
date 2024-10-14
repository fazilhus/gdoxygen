#include "util/util.hpp"
#include "dir.hpp"

#include <iostream>
#include <chrono>

int main(int argc, char** argv) {
	const auto start = std::chrono::high_resolution_clock::now();
	const auto _ = docs_gen_core::util::next_arg(&argc, &argv);

	const auto path = docs_gen_core::util::next_arg(&argc, &argv);
	if (path == nullptr) {
		std::cerr << "[USAGE] <program> <root of the project>\n";
		return -1;
	}

	docs_gen_core::dir p;
	char* arg;
	while ((arg = docs_gen_core::util::next_arg(&argc, &argv)) != nullptr) {
		p.push_ignored_folder(docs_gen_core::util::to_wstring(arg));
	}

	if (!p.set_path(docs_gen_core::util::to_wstring(path))) {
		std::cerr << "[ERROR] invalid path: " << arg << '\n';
		return -1;
	}

	p.construct_file_tree();
	p.gen_docs();
	
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
	std::cout << "Time took " << static_cast<float>(duration.count()) / 1000000000.0f << " seconds\n";
}
