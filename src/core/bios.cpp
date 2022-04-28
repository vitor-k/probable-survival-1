#include <fstream>
#include <fmt/core.h>

#include "bios.h"
#include "mips.h"
#include "log.h"

Bios::Bios(std::string filepath) {
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (file.is_open()) {
        // get length of file:
        file.seekg (0, file.end);
        auto length = file.tellg();
        file.seekg (0, file.beg);

        if (length == bios_size) {
            LOG("Reading bios...\n", length);
            file.read((char*) memory, length);

            if (file) {
                LOG("all characters read successfully.\n");
            }
            else {
                LOG("error: only {} could be read.\n", file.gcount());
            }
        }
        else {
            LOG("File size invalid.\n");
        }

        file.close();
    }
}

uint32_t Bios::load32(uint32_t offset) {
    LOG_DEBUG("BIOS: Fetching from {:x}\n", offset);
    return build32(memory[offset],memory[offset+1],memory[offset+2], memory[offset+3]);
}
