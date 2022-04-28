#ifndef BIOS_H
#define BIOS_H

#include <string>
#include <cstdint>

constexpr uint32_t bios_size = 512 * 1024;

class Bios {
    uint8_t memory[bios_size]{0};
    
public:
    Bios(std::string filepath);

    Bios(const Bios&) = delete;
    Bios& operator=(const Bios&) = delete;

    uint32_t load32(uint32_t offset);
};



#endif //BIOS_H