#include <iostream>

#include "loader.h"
#include "vm.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
        return 1;
    }

    std::vector<uint8_t> file_data;
    try {
        file_data = read_file(argv[1]);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    uint32_t start = bytes_to_uint32(file_data, 4);
    std::vector<Instruction> prog = bytes_to_prog(file_data);

    VM vm;
    vm.run(prog, start);

    return 0;
}
