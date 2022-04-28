#ifndef CPU_H
#define CPU_H

#include <array>
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
    // Registers

    uint32_t pc = bios_addr;

    std::array<uint32_t, 32> R;

    uint32_t hi;
    uint32_t lo;

    // Register aliases
    const uint32_t& R0 = R[0];
    uint32_t& R1 = R[1];
    uint32_t& R2 = R[2];
    uint32_t& R3 = R[3];
    uint32_t& R4 = R[4];
    uint32_t& R5 = R[5];
    uint32_t& R6 = R[6];
    uint32_t& R7 = R[7];
    uint32_t& R8 = R[8];
    uint32_t& R9 = R[9];
    uint32_t& R10 = R[10];
    uint32_t& R11 = R[11];
    uint32_t& R12 = R[12];
    uint32_t& R13 = R[13];
    uint32_t& R14 = R[14];
    uint32_t& R15 = R[15];
    uint32_t& R16 = R[16];
    uint32_t& R17 = R[17];
    uint32_t& R18 = R[18];
    uint32_t& R19 = R[19];
    uint32_t& R20 = R[20];
    uint32_t& R21 = R[21];
    uint32_t& R22 = R[22];
    uint32_t& R23 = R[23];
    uint32_t& R24 = R[24];
    uint32_t& R25 = R[25];
    uint32_t& R26 = R[26];
    uint32_t& R27 = R[27];
    uint32_t& R28 = R[28];
    uint32_t& R29 = R[29];
    uint32_t& R30 = R[30];
    uint32_t& R31 = R[31];

    const uint32_t& zero = R[0];
    uint32_t& at = R[1];
    uint32_t& v0 = R[2];
    uint32_t& v1 = R[3];
    uint32_t& a0 = R[4];
    uint32_t& a1 = R[5];
    uint32_t& a2 = R[6];
    uint32_t& a3 = R[7];
    uint32_t& t0 = R[8];
    uint32_t& t1 = R[9];
    uint32_t& t2 = R[10];
    uint32_t& t3 = R[11];
    uint32_t& t4 = R[12];
    uint32_t& t5 = R[13];
    uint32_t& t6 = R[14];
    uint32_t& t7 = R[15];
    uint32_t& s0 = R[16];
    uint32_t& s1 = R[17];
    uint32_t& s2 = R[18];
    uint32_t& s3 = R[19];
    uint32_t& s4 = R[20];
    uint32_t& s5 = R[21];
    uint32_t& s6 = R[22];
    uint32_t& s7 = R[23];
    uint32_t& t8 = R[24];
    uint32_t& t9 = R[25];
    uint32_t& k0 = R[26];
    uint32_t& k1 = R[27];
    uint32_t& gp = R[28];
    uint32_t& sp = R[29];
    uint32_t& fp = R[30];
    uint32_t& s8 = R[30];
    uint32_t& ra = R[31];


    uint8_t memory[memory_size]{0};

    std::unique_ptr<Bios> bios;

public:
    void decodeExecute(Instruction instruction);
    void mainLoop();

private:
    uint32_t load32(uint32_t addr);
    constexpr void setR(uint32_t i, uint32_t val) {
        if(i)
            R[i] = val;
    }
    constexpr uint32_t getR(uint32_t i) {
        if(i)
            return R[i];
        return 0;
    }
};

#endif // CPU_H
