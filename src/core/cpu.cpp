
#include <fmt/core.h>

#include "cpu.h"
#include "mips.h"
#include "log.h"

// Must be included after "cpu.h"
#include <fmt/ostream.h>

namespace {
    constexpr uint32_t REGION_MASKS[] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, // The mask for KUSEG (2GB)
    0x7FFFFFFF, // the mask for KSEG0 (0.5GB)
    0x1FFFFFFF, // the mask for KSEG1 (0.5GB)
    0xFFFFFFFF, 0xFFFFFFFF // the mask for KSEG2 (1GB)
    };
} // Anonymous namespace

CPU::CPU(std::string bios_path) {
    bios = std::make_unique<Bios>(bios_path);

    R.fill(0xdeadbeef);
    R[0] = 0;
}

MemMap CPU::decodeAddr(uint32_t paddr) {
    if(paddr < 0x00800000) {
        // main memory
        return MemMap::Main;
    }
    else if(paddr >= 0x1f000000 && paddr < 0x1f800000) {
        // expansion region 1
        return MemMap::Expansion1;
    }
    else if(paddr < 0x1f801000) {
        // scratchpad
        return MemMap::Scratchpad;
    }
    else if(paddr < 0x1f803000) {
        // hardware registers
        return MemMap::HardwareRegs;
    }
    else if(paddr >= 0x1fc00000 && paddr < 0x1fc80000) {
        // bios
        return MemMap::BIOS;
    }
    else if(paddr >= 0xfffe0000 && paddr < 0xfffe1000) {
        // io ports
        return MemMap::IO;
    }
    else {
        // unhandled
        return MemMap::Unmapped;
    }
}

uint32_t CPU::load32(uint32_t addr) {
    if(addr % 4 != 0) {
        LOG("Unaligned memory read at {:x}\n", addr);
        running = false;
        return 0;
    }

    const uint8_t region_bits = addr >> 29;
    const uint32_t paddr = addr & REGION_MASKS[region_bits];

    LOG_DEBUG("CPU: Reading from {:x}.\nPaddr: {:x}\n", addr, paddr);

    switch (decodeAddr(paddr)) {
    case MemMap::Main:
    {
        const uint32_t offset = paddr & 0x1ffffc;
        return build32(memory[offset], memory[offset + 1], memory[offset + 2], memory[offset + 3]);
    }
    case MemMap::BIOS:
        return bios->load32(paddr & 0x7fffc);
    case MemMap::Unmapped:
        // fallthrough
    default:
        LOG("Unhandled memory read at {:x}, decoded as {}\n", addr, decodeAddr(paddr));
        running = false;
        break;
    }
}

void CPU::store32(uint32_t addr, uint32_t val) {
    if(addr % 4 != 0) {
        LOG("Unaligned memory store at {:x}\n", addr);
        running = false;
        return;
    }

    const uint8_t region_bits = addr >> 29;
    const uint32_t paddr = addr & REGION_MASKS[region_bits];

    LOG_DEBUG("CPU: Storing {:x} to {:x}.\nPaddr: {:x}\n", val, addr, paddr);

    switch (decodeAddr(paddr)) {
    case MemMap::Main:
        /* code */
    {
        const uint32_t offset = paddr & 0x1ffffc;
        memory[offset] = getFirstByte(val);
        memory[offset+1] = getSecondByte(val);
        memory[offset+2] = getThirdByte(val);
        memory[offset+3] = getFourthByte(val);
    }
        break;
    case MemMap::HardwareRegs:
        LOG("Ignoring writes to hardware regs for now.\n");
        break;
    case MemMap::BIOS:
        LOG("Can't write to bios!\n");
        break;
    default:
        LOG("Unhandled memory store at {:x}, decoded as: {}\n", addr, decodeAddr(paddr));
        running = false;
        break;
    }
}

void CPU::decodeExecute(Instruction instruction) {
    switch (instruction.getOpcode())
    {
    case 0x00:
        // SPECIAL
        switch(instruction.getFunct()) {
            case 0x0:
                // SLL Shift Logical Left
                LOG_DEBUG("SLL: rt:{:x}, rd:{:x}, sa:{:x}\n", instruction.getRT(), instruction.getRD(), instruction.getShamt());
                setR(instruction.getRD(), getR(instruction.getRT()) << instruction.getShamt());
                break;
            case 0x25:
                // OR
                LOG_DEBUG("OR: rs:{:x}, rt:{:x}, rd:{:x}\n", instruction.getRS(), instruction.getRT(), instruction.getRD());
                setR(instruction.getRD(), getR(instruction.getRS()) | getR(instruction.getRT()));
                break;
            default:
                LOG("Unhandled instruction: {:x}, opcode: SPECIAL, func: {:x}\n", instruction.whole, instruction.getFunct());
                running = false;
        }
        break;
    case 0x02:
        // J Jump
        LOG_DEBUG("J: addr:{:x}\n", instruction.getAddress());
        {
            const auto addr = instruction.getAddress() << 2;
            const uint32_t mask = 0xf0000000;
            pc = (pc & mask) | addr;
        }
        break;
    case 0x09:
        // ADDIU Add Immediate Unsigned Word
        LOG_DEBUG("ADDIU: rt:{:x}, rs:{:x}, I {:x}\n", instruction.getRT(), instruction.getRS(), instruction.getImmediate());
        setR(instruction.getRT(), getR(instruction.getRS()) + instruction.getImmediate());
        break;
    case 0x0f:
        // LUI
        LOG_DEBUG("LUI: rt:{:x}, I {:x}\n", instruction.getRT(), instruction.getImmediate());
        {
            const uint32_t imm = instruction.getImmediate() << 16;
            setR(instruction.getRT(), imm);
        }
        break;
    case 0x0d:
        // ORI
        LOG_DEBUG("ORI: rs:{:x}, rt:{:x}, I {:x}\n", instruction.getRS(), instruction.getRT(), instruction.getImmediate());
        {
            const uint32_t imm = instruction.getImmediate();
            const uint32_t rt_val = getR(instruction.getRS());
            setR(instruction.getRT(), imm | rt_val);
        }
        break;
    case 0x2b:
        // SW
        LOG_DEBUG("SW: base:{:x}, rt:{:x}, I {:x}\n", instruction.getBase(), instruction.getRT(), instruction.getOffset());
        {
            const int32_t offset = instruction.getOffset();
            const uint32_t rt_val = getR(instruction.getRS());
            const uint32_t base_addr = getR(instruction.getBase());
            store32(base_addr + offset, rt_val);
        }
        break;
    default:
        LOG("Unhandled instruction: {:x}, opcode:{:x}\n", instruction.whole, instruction.getOpcode());
        running = false;
        break;
    }
}

void CPU::mainLoop() {
    auto current_instruction = next_instruction;
    next_instruction = load32(pc);

    pc += 4;

    decodeExecute(current_instruction);
}
