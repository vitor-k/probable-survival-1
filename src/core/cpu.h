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

    // Registers
    const uint32_t R0 = 0;
    uint32_t R1;
    uint32_t R2;
    uint32_t R3;
    uint32_t R4;
    uint32_t R5;
    uint32_t R6;
    uint32_t R7;
    uint32_t R8;
    uint32_t R9;
    uint32_t R10;
    uint32_t R11;
    uint32_t R12;
    uint32_t R13;
    uint32_t R14;
    uint32_t R15;
    uint32_t R16;
    uint32_t R17;
    uint32_t R18;
    uint32_t R19;
    uint32_t R20;
    uint32_t R21;
    uint32_t R22;
    uint32_t R23;
    uint32_t R24;
    uint32_t R25;
    uint32_t R26;
    uint32_t R27;
    uint32_t R28;
    uint32_t R29;
    uint32_t R30;
    uint32_t R31;

    uint32_t hi;
    uint32_t lo;

    // Register aliases
    const uint32_t& zero = R0;
    uint32_t& at = R1;
    uint32_t& v0 = R2;
    uint32_t& v1 = R3;
    uint32_t& a0 = R4;
    uint32_t& a1 = R5;
    uint32_t& a2 = R6;
    uint32_t& a3 = R7;
    uint32_t& t0 = R8;
    uint32_t& t1 = R9;
    uint32_t& t2 = R10;
    uint32_t& t3 = R11;
    uint32_t& t4 = R12;
    uint32_t& t5 = R13;
    uint32_t& t6 = R14;
    uint32_t& t7 = R15;
    uint32_t& s0 = R16;
    uint32_t& s1 = R17;
    uint32_t& s2 = R18;
    uint32_t& s3 = R19;
    uint32_t& s4 = R20;
    uint32_t& s5 = R21;
    uint32_t& s6 = R22;
    uint32_t& s7 = R23;
    uint32_t& t8 = R24;
    uint32_t& t9 = R25;
    uint32_t& k0 = R26;
    uint32_t& k1 = R27;
    uint32_t& gp = R28;
    uint32_t& sp = R29;
    uint32_t& fp = R30;
    uint32_t& s8 = R30;
    uint32_t& ra = R31;


    uint8_t memory[memory_size]{0};

    std::unique_ptr<Bios> bios;

public:
    void decodeExecute(Instruction instruction);
    void mainLoop();

private:
    uint32_t load32(uint32_t addr);
};

#endif // CPU_H
