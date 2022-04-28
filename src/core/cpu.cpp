
#include <fmt/core.h>

#include "cpu.h"
#include "mips.h"

CPU::CPU(std::string bios_path) {
    bios = std::make_unique<Bios>(bios_path);

    R.fill(0xdeadbeef);
    R[0] = 0;
}

uint32_t CPU::load32(uint32_t addr) {
    constexpr uint32_t REGION_MASKS[] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, // The mask for KUSEG (2GB)
    0x7FFFFFFF, // the mask for KSEG0 (0.5GB)
    0x1FFFFFFF, // the mask for KSEG1 (0.5GB)
    0xFFFFFFFF, 0xFFFFFFFF // the mask for KSEG2 (1GB)
    };

    const uint8_t region_bits = addr >> 29;
    const uint32_t paddr = addr & REGION_MASKS[region_bits];

    fmt::print("CPU: Reading from {:x}.\nPaddr: {:x}\n", addr, paddr);

    if(paddr < 0x00800000) {
        // main memory
        const uint32_t offset = paddr & 0x1fffff;
        return build32(memory[offset], memory[offset+1], memory[offset+2], memory[offset+3]);
    }
    else if(paddr >= 0x1f000000 && paddr < 0x1f800000) {
        // expansion region 1
    }
    else if(paddr < 0x1f801000) {
        // scratchpad
    }
    else if(paddr < 0x1f803000) {
        // hardware registers
    }
    else if(paddr >= 0x1fc00000 && paddr < 0x1fc80000) {
        // bios
        return bios->load32(paddr & 0x7ffff);
    }
    else if(paddr >= 0xfffe0000 && paddr < 0xfffe1000) {
        // io ports
    }
    else {
        // unhandled
    }
}

void CPU::decodeExecute(Instruction instruction) {
    switch (instruction.getOpcode())
    {
    case 0x0f:
        // LUI
        {
            const uint32_t imm = instruction.getImmediate() << 16;
            setR(instruction.getRT(), imm);
        }
        break;
    case 0x0d:
        // ORI
        {
            const uint32_t imm = instruction.getImmediate();
            const uint32_t rt_val = getR(instruction.getRS());
            setR(instruction.getRT(), imm | rt_val);
        }
        break;
    default:
        fmt::print("Unhandled instruction: {:x}, opcode:{:x}\n", instruction.whole, instruction.getOpcode());
        throw;
        break;
    }
    pc+= 4;
}

void CPU::mainLoop() {
    decodeExecute(load32(pc));
}
