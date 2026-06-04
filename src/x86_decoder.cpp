#include "ptaslearn/x86_decoder.hpp"

#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace ptaslearn {
namespace {

const char* kRegister32[8] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};

std::uint32_t read_u32(const std::vector<std::uint8_t>& bytes, std::size_t offset) {
  if (offset + 4 > bytes.size()) {
    throw std::runtime_error("truncated 32-bit immediate");
  }
  return static_cast<std::uint32_t>(bytes[offset]) |
         (static_cast<std::uint32_t>(bytes[offset + 1]) << 8U) |
         (static_cast<std::uint32_t>(bytes[offset + 2]) << 16U) |
         (static_cast<std::uint32_t>(bytes[offset + 3]) << 24U);
}

std::string hex_value(std::uint64_t value) {
  std::ostringstream out;
  out << "0x" << std::hex << std::nouppercase << value;
  return out.str();
}

std::string byte_text(std::uint8_t value) {
  std::ostringstream out;
  out << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value);
  return out.str();
}

}  // namespace

std::vector<Instruction> X86Decoder::decode(const std::vector<std::uint8_t>& bytes) const {
  std::vector<Instruction> instructions;
  std::size_t pc = 0;

  while (pc < bytes.size()) {
    const std::size_t offset = pc;
    const std::uint8_t opcode = bytes[pc++];
    Instruction instruction;
    instruction.offset = offset;
    instruction.bytes.push_back(opcode);

    if (opcode == 0x90) {
      instruction.mnemonic = "nop";
    } else if (opcode == 0xC3) {
      instruction.mnemonic = "ret";
    } else if (opcode >= 0xB8 && opcode <= 0xBF) {
      const std::uint32_t immediate = read_u32(bytes, pc);
      for (std::size_t i = 0; i < 4; ++i) {
        instruction.bytes.push_back(bytes[pc + i]);
      }
      pc += 4;
      instruction.mnemonic = "mov";
      instruction.operands.push_back({OperandKind::Register, kRegister32[opcode - 0xB8], 0});
      instruction.operands.push_back(
          {OperandKind::Immediate, hex_value(immediate), static_cast<std::int64_t>(immediate)});
    } else if (opcode == 0x89 || opcode == 0x8B || opcode == 0x01 || opcode == 0x29) {
      if (pc >= bytes.size()) {
        throw std::runtime_error("truncated ModR/M byte");
      }
      const std::uint8_t modrm = bytes[pc++];
      instruction.bytes.push_back(modrm);
      const std::uint8_t mod = (modrm >> 6U) & 0x3U;
      const std::uint8_t reg = (modrm >> 3U) & 0x7U;
      const std::uint8_t rm = modrm & 0x7U;
      if (mod != 0x3U) {
        throw std::runtime_error("only register-to-register MOV is supported");
      }
      if (opcode == 0x01) {
        instruction.mnemonic = "add";
        instruction.operands.push_back({OperandKind::Register, kRegister32[rm], 0});
        instruction.operands.push_back({OperandKind::Register, kRegister32[reg], 0});
      } else if (opcode == 0x29) {
        instruction.mnemonic = "sub";
        instruction.operands.push_back({OperandKind::Register, kRegister32[rm], 0});
        instruction.operands.push_back({OperandKind::Register, kRegister32[reg], 0});
      } else if (opcode == 0x89) {
        instruction.mnemonic = "mov";
        instruction.operands.push_back({OperandKind::Register, kRegister32[rm], 0});
        instruction.operands.push_back({OperandKind::Register, kRegister32[reg], 0});
      } else {
        instruction.mnemonic = "mov";
        instruction.operands.push_back({OperandKind::Register, kRegister32[reg], 0});
        instruction.operands.push_back({OperandKind::Register, kRegister32[rm], 0});
      }
    } else if (opcode == 0xEB) {
      if (pc >= bytes.size()) {
        throw std::runtime_error("truncated short jump displacement");
      }
      const auto displacement = static_cast<std::int8_t>(bytes[pc++]);
      instruction.bytes.push_back(static_cast<std::uint8_t>(displacement));
      const auto target = static_cast<std::int64_t>(pc) + displacement;
      instruction.mnemonic = "jmp";
      instruction.operands.push_back({OperandKind::Relative, hex_value(target), target});
    } else {
      throw std::runtime_error("unsupported opcode " + byte_text(opcode));
    }

    instructions.push_back(instruction);
  }

  return instructions;
}

std::string X86Decoder::format(const Instruction& instruction) const {
  std::ostringstream out;
  out << std::setw(4) << std::setfill('0') << std::hex << instruction.offset << ": "
      << instruction.mnemonic;
  if (!instruction.operands.empty()) {
    out << " ";
    for (std::size_t i = 0; i < instruction.operands.size(); ++i) {
      if (i != 0) {
        out << ", ";
      }
      out << instruction.operands[i].text;
    }
  }
  return out.str();
}

}  // namespace ptaslearn
