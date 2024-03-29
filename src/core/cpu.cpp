
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <algorithm>
#include <limits>

#include "cpu.h"
#include "mips.h"
#include "log.h"

template <> struct fmt::formatter<MemMap> : ostream_formatter {};

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
        LOG("Unaligned memory read at {:#x}\n", addr);
        running = false;
        return 0;
    }

    const uint8_t region_bits = addr >> 29;
    const uint32_t paddr = addr & REGION_MASKS[region_bits];

    LOG_DEBUG("CPU: Reading from {:#x}. Paddr: {:#x}\n", addr, paddr);

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
        LOG("Unhandled memory read at {:#x}, decoded as {}\n", addr, decodeAddr(paddr));
        running = false;
        break;
    }
}

// Vide comment on store16
void CPU::store8(uint32_t addr, uint8_t val) {
    const uint8_t region_bits = addr >> 29;
    const uint32_t paddr = addr & REGION_MASKS[region_bits];

    LOG_DEBUG("CPU: Storing byte {:#x} to {:#x}. Paddr: {:#x}\n", val, addr, paddr);

    switch (decodeAddr(paddr)) {
    case MemMap::Main:
    {
        LOG("Ignoring byte writes to memory for now.\n");
        const uint32_t offset = paddr & 0x1ffffc;
        //memory[offset] = getFirstByte(val);
    }
        break;
    case MemMap::HardwareRegs:
        LOG("Ignoring byte writes to hardware regs for now.\n");
        break;
    case MemMap::BIOS:
        LOG("Can't write to bios!\n");
        break;
    case MemMap::IO:
        LOG("Ignoring byte writes to IO for now.\n");
        break;
    default:
        LOG("Unhandled byte store at {:#x}, decoded as: {}\n", addr, decodeAddr(paddr));
        running = false;
        break;
    }
}

// There is duplicate code from the store32 function, maybe the RAM writes should be abstracted away
// and merge the functions with overloading/templates
void CPU::store16(uint32_t addr, uint16_t val) {
    if(addr % 2 != 0) {
        LOG("Unaligned halfword store at {:#x}\n", addr);
        running = false;
        return;
    }

    const uint8_t region_bits = addr >> 29;
    const uint32_t paddr = addr & REGION_MASKS[region_bits];

    LOG_DEBUG("CPU: Storing halfword {:#x} to {:#x}. Paddr: {:#x}\n", val, addr, paddr);

    switch (decodeAddr(paddr)) {
    case MemMap::Main:
    {
        LOG("Ignoring halfword writes to memory for now.\n");
        const uint32_t offset = paddr & 0x1ffffc;
        //memory[offset] = getFirstByte(val);
        //memory[offset+1] = getSecondByte(val);
    }
        break;
    case MemMap::HardwareRegs:
        LOG("Ignoring halfword writes to hardware regs for now.\n");
        break;
    case MemMap::BIOS:
        LOG("Can't write to bios!\n");
        break;
    case MemMap::IO:
        LOG("Ignoring halfword writes to IO for now.\n");
        break;
    default:
        LOG("Unhandled halfword store at {:#x}, decoded as: {}\n", addr, decodeAddr(paddr));
        running = false;
        break;
    }
}

void CPU::store32(uint32_t addr, uint32_t val) {
    if(addr % 4 != 0) {
        LOG("Unaligned memory store at {:#x}\n", addr);
        running = false;
        return;
    }

    const uint8_t region_bits = addr >> 29;
    const uint32_t paddr = addr & REGION_MASKS[region_bits];

    LOG_DEBUG("CPU: Storing {:#x} to {:#x}. Paddr: {:#x}\n", val, addr, paddr);

    switch (decodeAddr(paddr)) {
    case MemMap::Main:
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
    case MemMap::IO:
        LOG("Ignoring writes to IO for now.\n");
        break;
    default:
        LOG("Unhandled memory store at {:#x}, decoded as: {}\n", addr, decodeAddr(paddr));
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
                // SLL - Shift Logical Left
                LOG_DEBUG("SLL: rt:{:#x}, rd:{:#x}, sa:{:#x}\n", instruction.getRT(), instruction.getRD(), instruction.getShamt());
                setR(instruction.getRD(), getR(instruction.getRT()) << instruction.getShamt());
                break;
            case 0x08:
                // JR - Jump Register
                LOG_DEBUG("JR: rs:{:#x}, addr:{:#x}, curr_pc:{:#x}\n", instruction.getRS(), getR(instruction.getRS()), pc);
                pc = getR(instruction.getRS());
                break;
            case 0x25:
                // OR - Bitwise OR
                LOG_DEBUG("OR: rs:{:#x}, rt:{:#x}, rd:{:#x}\n", instruction.getRS(), instruction.getRT(), instruction.getRD());
                setR(instruction.getRD(), getR(instruction.getRS()) | getR(instruction.getRT()));
                break;
            case 0x21:
                // ADDU - Add Unsigned
                LOG_DEBUG("ADDU: rs:{:#x}, rt:{:#x}, rd:{:#x}\n", instruction.getRS(), instruction.getRT(), instruction.getRD());
                setR(instruction.getRD(), getR(instruction.getRS()) + getR(instruction.getRT()));
                break;
            case 0x2b:
                // SLTU - Set on Less Than Unsigned
                LOG_DEBUG("SLTU: rs:{:#x}, rt:{:#x}, rd:{:#x}\n", instruction.getRS(), instruction.getRT(), instruction.getRD());
                setR(instruction.getRD(), getR(instruction.getRS()) < getR(instruction.getRT()));
                break;
            default:
                LOG("Unhandled instruction: {:#x}, opcode: SPECIAL, func: {:#x}\n", instruction.whole, instruction.getFunct());
                running = false;
        }
        break;
    case 0x02:
        // J - Jump
        LOG_DEBUG("J: addr:{:#x}\n", instruction.getAddress());
        {
            const auto addr = instruction.getAddress() << 2;
            const uint32_t mask = 0xf0000000;
            pc = (pc & mask) | addr;
        }
        break;
    case 0x03:
        // JAL - Jump And Link
        LOG_DEBUG("JAL: addr:{:#x}, curr_pc:{:#x}\n", instruction.getAddress(), pc);
        {
            const auto addr = instruction.getAddress() << 2;
            const uint32_t mask = 0xf0000000;
            setR(RegAlias::ra, pc);
            pc = (pc & mask) | addr;
        }
        break;
    case 0x05:
        // BNE - Branch Not Equal
        LOG_DEBUG("BNE: rs:{:#x}, rt:{:#x}, offset:{:#x}\n", instruction.getRS(), instruction.getRT(), instruction.getOffset());
        if(getR(instruction.getRS()) != getR(instruction.getRT())){
            const int32_t offset = instruction.getOffset();
            pc = pc + (offset << 2) - 4;
        }
        break;
    case 0x08:
        // ADDI - Add Immediate Word
        LOG_DEBUG("ADDI: rt:{:#x}, rs:{:#x}, I {:#x}\n", instruction.getRT(), instruction.getRS(), instruction.getImmediate());
        {
            const int32_t operand1 = getR(instruction.getRS());
            const int32_t operand2 = instruction.getImmediateS();
            const int64_t sumE = static_cast<int64_t>(operand1) + static_cast<int64_t>(operand2);
            if(sumE > std::numeric_limits<int32_t>::max() || sumE < std::numeric_limits<int32_t>::min()) {
                // exception
                LOG("ADDI Overflow! I need to implement exceptions!\n");
                running = false;
            }
            else {
                setR(instruction.getRT(), operand1 + operand2);
            }
        }
        break;
    case 0x09:
        // ADDIU - Add Immediate Unsigned Word
        LOG_DEBUG("ADDIU: rt:{:#x}, rs:{:#x}, I {:#x}\n", instruction.getRT(), instruction.getRS(), instruction.getImmediate());
        {
            const int32_t operand1 = getR(instruction.getRS());
            const int32_t operand2 = instruction.getImmediateS();
            setR(instruction.getRT(), operand1 + operand2);
        }
        break;
    case 0x0f:
        // LUI - Load Upper Immediate
        LOG_DEBUG("LUI: rt:{:#x}, I {:#x}\n", instruction.getRT(), instruction.getImmediate());
        {
            const uint32_t imm = instruction.getImmediate() << 16;
            setR(instruction.getRT(), imm);
        }
        break;
    case 0x0c:
        // ANDI - And Immediate
        LOG_DEBUG("ANDI: rs:{:#x}, rt:{:#x}, I {:#x}\n", instruction.getRS(), instruction.getRT(), instruction.getImmediate());
        {
            const uint32_t imm = instruction.getImmediate();
            const uint32_t rt_val = getR(instruction.getRS());
            setR(instruction.getRT(), imm & rt_val);
        }
        break;
    case 0x0d:
        // ORI - Bitwise OR Immediate
        LOG_DEBUG("ORI: rs:{:#x}, rt:{:#x}, I {:#x}\n", instruction.getRS(), instruction.getRT(), instruction.getImmediate());
        {
            const uint32_t imm = instruction.getImmediate();
            const uint32_t rt_val = getR(instruction.getRS());
            setR(instruction.getRT(), imm | rt_val);
        }
        break;
    case 0x10:
        {
            switch(instruction.getCopOpcode()) {
                case 0x4:
                    // MTC0 - Move To Coprocessor 0
                    LOG_DEBUG("MTC0: rt:{:#x}, rd:{:#x}\n", instruction.getRT(), instruction.getRD());
                    switch(static_cast<Cop0RegAlias>(instruction.getRD())){
                        case Cop0RegAlias::SR:
                            Cop0R[instruction.getRD()] = getR(instruction.getRT());
                            break;
                        default:
                            LOG("Unhandled write to COP0 Register {:#x}, val:{:#x}\n", instruction.getRD(), getR(instruction.getRT()));
                    }
                    //Cop0R[instruction.getRD()] = getR(instruction.getRT());
                    break;
                default:
                    LOG("Unhandled instruction: {:#x}, opcode:{:#x}, copopcode:{:#x}\n", instruction.whole, instruction.getOpcode(), instruction.getCopOpcode());
                    running = false;
                    break;
            }
        }
        break;
    case 0x23:
        // LW - Load Word
        LOG_DEBUG("LW: base:{:#x}, rt:{:#x}, offset {:#x}\n", instruction.getBase(), instruction.getRT(), instruction.getOffset());
        {
            if(getCop0R(Cop0RegAlias::SR) & 0x10000) {
                LOG_DEBUG("Ignoring loads from isolated cache\n");
                break;
            }
            const int32_t offset = instruction.getOffset();
            const uint32_t rt_val = getR(instruction.getRT());
            const uint32_t base_addr = getR(instruction.getBase());

            load = {rt_val, load32(base_addr + offset)};
        }
        break;
    case 0x28:
        // SB - Store Byte
        LOG_DEBUG("SB: base:{:#x}, rt:{:#x}, offset {:#x}\n", instruction.getBase(), instruction.getRT(), instruction.getOffset());
        {
            if(getCop0R(Cop0RegAlias::SR) & 0x10000) {
                LOG_DEBUG("Ignoring writes to isolated cache\n");
                break;
            }
            const int32_t offset = instruction.getOffset();
            const uint8_t rt_val = getR(instruction.getRT());
            const uint32_t base_addr = getR(instruction.getBase());
            store8(base_addr + offset, rt_val);
        }
        break;
    case 0x29:
        // SH - Store Halfword
        LOG_DEBUG("SH: base:{:#x}, rt:{:#x}, offset {:#x}\n", instruction.getBase(), instruction.getRT(), instruction.getOffset());
        {
            if(getCop0R(Cop0RegAlias::SR) & 0x10000) {
                LOG_DEBUG("Ignoring writes to isolated cache\n");
                break;
            }
            const int32_t offset = instruction.getOffset();
            const uint16_t rt_val = getR(instruction.getRT());
            const uint32_t base_addr = getR(instruction.getBase());
            store16(base_addr + offset, rt_val);
        }
        break;
    case 0x2b:
        // SW - Store Word
        LOG_DEBUG("SW: base:{:#x}, rt:{:#x}, offset {:#x}\n", instruction.getBase(), instruction.getRT(), instruction.getOffset());
        {
            if(getCop0R(Cop0RegAlias::SR) & 0x10000) {
                LOG_DEBUG("Ignoring writes to isolated cache\n");
                break;
            }
            const int32_t offset = instruction.getOffset();
            const uint32_t rt_val = getR(instruction.getRT());
            const uint32_t base_addr = getR(instruction.getBase());
            store32(base_addr + offset, rt_val);
        }
        break;
    default:
        LOG("Unhandled instruction: {:#x}, opcode:{:#x}\n", instruction.whole, instruction.getOpcode());
        running = false;
        break;
    }
}

void CPU::mainLoop() {
    auto current_instruction = next_instruction;
    next_instruction = load32(pc);

    pc += 4;

    setR(load.first, load.second);
    load = {0,0};

    decodeExecute(current_instruction);
    std::copy(outR.begin(), outR.end(), R.begin());
}
