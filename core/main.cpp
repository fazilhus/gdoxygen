#include "util.hpp"

#include <iostream>

int main(int argc, char** argv) {
    auto program = core::util::next_arg(&argc, &argv);

    auto arg = core::util::next_arg(&argc, &argv);
    if (arg == nullptr)
    {
        std::cerr << "[USAGE] " << program << " <root of the project>\n";
    }

    std::cout << "Create documentation starting from " << arg << '\n';
}
