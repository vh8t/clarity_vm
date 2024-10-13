#include <cstring>
#include <iostream>

#include "loader.h"
#include "vm.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
        return 1;
    }

    char *filename = nullptr;
    bool debug = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0)
            debug = true;
        else {
            if (filename == nullptr)
                filename = argv[i];
        }
    }

    if (filename == nullptr) {
        std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
        return 1;
    }

    std::vector<uint8_t> file_data;
    try {
        file_data = read_file(filename);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (file_data[2] != MAJOR) {
        std::cerr << "Incompatible major version, file=" << file_data[2]
                  << " vm=" << MAJOR << std::endl;
        return 1;
    }

    if (file_data[3] > MINOR) {
        std::cerr << "Incompatible minor version, file=" << file_data[3]
                  << " vm=" << MINOR << std::endl;
        return 1;
    }

    uint32_t start = bytes_to_uint32(file_data, 8);
    std::vector<Instruction> prog = bytes_to_prog(file_data);
    std::vector<uint8_t> data = bytes_to_data(file_data);

    VM vm(debug, data);
    return vm.run(prog, start);
}
