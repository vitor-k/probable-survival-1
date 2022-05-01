
#include <catch2/catch_test_macros.hpp>

#include "core/mips.h"

TEST_CASE("The Bit masking should work") {
    const Instruction justOpcode{0b11111100000000000000000000000000};
    REQUIRE(justOpcode.getOpcode() == 0b111111);

    const Instruction justRS{0b00000011111000000000000000000000};
    REQUIRE(justRS.getRS() == 0b11111);

    const Instruction justRT{0b00000000000111110000000000000000};
    REQUIRE(justRT.getRT() == 0b11111);

    const Instruction justRD{0b00000000000000001111100000000000};
    REQUIRE(justRD.getRD() == 0b11111);

    const Instruction justShamt{0b00000000000000000000011111000000};
    REQUIRE(justShamt.getShamt() == 0b11111);

    const Instruction justFunct{0b00000000000000000000000000111111};
    REQUIRE(justFunct.getFunct() == 0b111111);

    const Instruction justImm{0b00000000000000001111111111111111};
    REQUIRE(justImm.getImmediate() == 0b1111111111111111);

    const Instruction justAddress{0b00000011111111111111111111111111};
    REQUIRE(justAddress.getAddress() == 0b11111111111111111111111111);
}

TEST_CASE("The Offset should be signed") {
    const auto justOffset = Instruction(0b00000000000000001111111111111111);
    const int32_t sign_test = justOffset.getOffset();
    REQUIRE(sign_test == -1);
}
