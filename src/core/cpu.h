#ifndef CPU_H
#define CPU_H

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <ostream>
#include <utility>

#include "bios.h"
#include "mips.h"

constexpr uint32_t memory_size = 2 * 1024 * 1024;
constexpr uint32_t bios_addr = 0xbfc00000;

enum class MemMap {
    Main,
    Expansion1,
    Scratchpad,
    HardwareRegs,
    BIOS,
    IO,
    Unmapped
};

enum class RegAlias : uint8_t {
    R0 = 0,
    R1 = 1,
    R2 = 2,
    R3 = 3,
    R4 = 4,
    R5 = 5,
    R6 = 6,
    R7 = 7,
    R8 = 8,
    R9 = 9,
    R10 = 10,
    R11 = 11,
    R12 = 12,
    R13 = 13,
    R14 = 14,
    R15 = 15,
    R16 = 16,
    R17 = 17,
    R18 = 18,
    R19 = 19,
    R20 = 20,
    R21 = 21,
    R22 = 22,
    R23 = 23,
    R24 = 24,
    R25 = 25,
    R26 = 26,
    R27 = 27,
    R28 = 28,
    R29 = 29,
    R30 = 30,
    R31 = 31,
    zero = 0,
    at = 1,
    v0 = 2,
    v1 = 3,
    a0 = 4,
    a1 = 5,
    a2 = 6,
    a3 = 7,
    t0 = 8,
    t1 = 9,
    t2 = 10,
    t3 = 11,
    t4 = 12,
    t5 = 13,
    t6 = 14,
    t7 = 15,
    s0 = 16,
    s1 = 17,
    s2 = 18,
    s3 = 19,
    s4 = 20,
    s5 = 21,
    s6 = 22,
    s7 = 23,
    t8 = 24,
    t9 = 25,
    k0 = 26,
    k1 = 27,
    gp = 28,
    sp = 29,
    fp = 30,
    s8 = 30,
    ra = 31,
};

enum class Cop0RegAlias : uint8_t {
    BPC = 3,
    BDA = 5,
    JUMPDEST = 6,
    DCIC = 7,
    BadVaddr = 8,
    BDAM = 9,
    BPCM = 11,
    SR = 12,
    CAUSE = 13,
    EPC = 14,
    PRID = 15,
};

inline std::ostream &operator<<(std::ostream& os, MemMap map) {
    std::string mapname;
    switch(map) {
        case MemMap::Main:
            mapname = "Main";
            break;
        case MemMap::Expansion1:
            mapname = "Expansion1";
            break;
        case MemMap::Scratchpad:
            mapname = "Scratchpad";
            break;
        case MemMap::HardwareRegs:
            mapname = "HardwareRegs";
            break;
        case MemMap::BIOS:
            mapname = "BIOS";
            break;
        case MemMap::IO:
            mapname = "IO";
            break;
        case MemMap::Unmapped:
            mapname = "Unmapped";
            break;
    }
    return os << mapname;
}

class CPU {
public:
    CPU(std::string bios_path);

    CPU(const CPU&) = delete;
    CPU& operator=(const CPU&) = delete;

    bool running = true;

private:
    // Registers

    uint32_t pc = bios_addr;

    std::array<uint32_t, 32> R;

    // Duplicate registers to emulate the load delay slot
    // TODO: possibly emulate the delay slot by separating
    // the decoding and executing step, with the decoding step
    // returning a struct with the necessary arguments and 
    // the decoded function, and then the execute step would
    // execute pending loads and call the decoded function.
    std::array<uint32_t, 32> outR;

    std::pair<uint8_t, uint32_t> load{0,0};

    uint32_t hi;
    uint32_t lo;

    // Cop0 Registers
    std::array<uint32_t, 16> Cop0R{0};

    uint8_t memory[memory_size]{0};

    std::unique_ptr<Bios> bios;

    Instruction next_instruction{0}; // Due to branch delay slots
public:
    void decodeExecute(Instruction instruction);
    void mainLoop();

private:
    MemMap decodeAddr(uint32_t addr);
    uint32_t load32(uint32_t addr);
    void store8(uint32_t addr, uint8_t val);
    void store16(uint32_t addr, uint16_t val);
    void store32(uint32_t addr, uint32_t val);
    constexpr void setR(uint8_t i, uint32_t val) {
        if(i)
            outR[i] = val;
    }
    constexpr uint32_t getR(uint8_t i) const {
        if(i)
            return R[i];
        return 0;
    }
    // RegAlias overloads
    constexpr void setR(RegAlias i, uint32_t val) {
        setR(static_cast<uint8_t>(i), val);
    }
    constexpr uint32_t getR(RegAlias i) const {
        return getR(static_cast<uint8_t>(i));
    }

    constexpr void setCop0R(uint8_t i, uint32_t val) {
        Cop0R[i] = val;
    }
    constexpr uint32_t getCop0R(uint8_t i) const {
        return Cop0R[i];
    }
    // Cop0RegAlias overloads
    constexpr void setCop0R(Cop0RegAlias i, uint32_t val) {
        setCop0R(static_cast<uint8_t>(i),val);
    }
    constexpr uint32_t getCop0R(Cop0RegAlias i) const {
        return getCop0R(static_cast<uint8_t>(i));
    }
};

#endif // CPU_H
