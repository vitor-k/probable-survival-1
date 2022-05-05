#ifndef MIPS_H
#define MIPS_H

#include <cstdint>

constexpr uint32_t build32(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return (static_cast<uint32_t>(d) << 24) | (static_cast<uint32_t>(c) << 16) | (static_cast<uint32_t>(b) << 8) | a;
}

constexpr uint8_t getFirstByte(uint32_t whole) {
    return whole & 0xff;
}
constexpr uint8_t getSecondByte(uint32_t whole) {
    return (whole >> 8) & 0xff;
}
constexpr uint8_t getThirdByte(uint32_t whole) {
    return (whole >> 16) & 0xff;
}
constexpr uint8_t getFourthByte(uint32_t whole) {
    return (whole >> 24) & 0xff;
}

struct Instruction {
    uint32_t whole{0};

    Instruction(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
        whole = build32(a,b,c,d);
    }
    Instruction(uint32_t a) {
        whole = a;
    }

    template<int N, int M>
    constexpr uint32_t getBits() const{
        return (whole >> (N)) & ((1<<M)-1);
    }

    constexpr uint8_t getOpcode() const{
        return getBits<26,6>();
    }
    constexpr uint8_t getRS() const{
        return getBits<21,5>();
    }
    constexpr uint8_t getBase() const{
        return getRS();
    }
    constexpr uint8_t getCopOpcode() const{
        return getRS();
    }
    constexpr uint8_t getRT() const{
        return getBits<16,5>();
    }
    constexpr uint8_t getRD() const{
        return getBits<11,5>();
    }
    constexpr uint8_t getShamt() const{
        return getBits<6,5>();
    }
    constexpr uint8_t getFunct() const{
        return getBits<0,6>();
    }
    constexpr uint16_t getImmediate() const{
        return getBits<0,16>();
    }
    constexpr int16_t getOffset() const{
        union _imm {
            uint16_t u;
            int16_t i;
        };
        return _imm{getImmediate()}.i;
    }
    constexpr uint32_t getAddress() const{
        return getBits<0,26>();
    }
};

#endif // MIPS_H
