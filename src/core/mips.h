#ifndef MIPS_H
#define MIPS_H

#include <cstdint>

constexpr uint32_t build32(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return (static_cast<uint32_t>(d) << 24) | (static_cast<uint32_t>(c) << 16) | (static_cast<uint32_t>(b) << 8) | a;
}

struct Instruction {
    uint32_t whole{0};

    Instruction(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
        whole = build32(a,b,c,d);
    }
    Instruction(uint32_t a) {
        whole = a;
    }

    constexpr uint32_t getOpcode(){
        return (whole >> 26) & 0b111111;
    }
    constexpr uint32_t getRS(){
        return (whole >> 21) & 0b11111;
    }
    constexpr uint32_t getRT(){
        return (whole >> 16) & 0b11111;
    }
    constexpr uint32_t getRD(){
        return (whole >> 11) & 0b11111;
    }
    constexpr uint32_t getShamt(){
        return (whole >> 6) & 0b11111;
    }
    constexpr uint32_t getFunct(){
        return whole & 0b111111;
    }
    constexpr uint32_t getImmediate(){
        return whole & 0xffff;
    }
    constexpr uint32_t getAddress(){
        return whole & 0x3ffffff;
    }
};

#endif // MIPS_H
