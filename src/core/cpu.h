#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <memory>
#include <string>

#include "bios.h"
#include "mips.h"

constexpr uint32_t memory_size = 2 * 1024 * 1024;
constexpr uint32_t bios_addr = 0xbfc00000;

class CPU {
public:
    CPU(std::string bios_path);

    CPU(const CPU&) = delete;
    CPU& operator=(const CPU&) = delete;

private:
    uint32_t pc = bios_addr;

    uint8_t memory[memory_size]{0};

    std::unique_ptr<Bios> bios;

public:
    void decodeExecute(Instruction instruction);
    void mainLoop();

private:
    uint32_t load32(uint32_t addr);
};

#endif // CPU_H
