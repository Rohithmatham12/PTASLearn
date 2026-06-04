#include "ptaslearn/cpu_model.hpp"

#include <stdexcept>

namespace ptaslearn {
namespace {

Register32 parse_reg(const std::string& name) {
  if (name == "eax") return Register32::Eax;
  if (name == "ecx") return Register32::Ecx;
  if (name == "edx") return Register32::Edx;
  if (name == "ebx") return Register32::Ebx;
  if (name == "esp") return Register32::Esp;
  if (name == "ebp") return Register32::Ebp;
  if (name == "esi") return Register32::Esi;
  if (name == "edi") return Register32::Edi;
  throw std::runtime_error("unknown register: " + name);
}

}  // namespace

GoldenCpuModel::GoldenCpuModel() = default;

const CpuState& GoldenCpuModel::state() const {
  return state_;
}

std::uint32_t GoldenCpuModel::reg(Register32 reg) const {
  return state_.registers[static_cast<std::size_t>(reg)];
}

void GoldenCpuModel::set_reg(Register32 reg, std::uint32_t value) {
  state_.registers[static_cast<std::size_t>(reg)] = value;
}

void GoldenCpuModel::execute(const std::vector<std::uint8_t>& bytes) {
  for (const auto& instruction : decoder_.decode(bytes)) {
    execute(instruction);
  }
}

void GoldenCpuModel::execute(const Instruction& instruction) {
  if (instruction.mnemonic == "nop") {
    state_.eip += static_cast<std::uint32_t>(instruction.bytes.size());
    return;
  }
  if (instruction.mnemonic == "ret") {
    state_.eip += static_cast<std::uint32_t>(instruction.bytes.size());
    return;
  }
  if (instruction.mnemonic == "jmp") {
    state_.eip = static_cast<std::uint32_t>(instruction.operands.at(0).value);
    return;
  }

  const auto dst = parse_reg(instruction.operands.at(0).text);
  if (instruction.mnemonic == "mov") {
    if (instruction.operands.at(1).kind == OperandKind::Immediate) {
      set_reg(dst, static_cast<std::uint32_t>(instruction.operands.at(1).value));
    } else {
      set_reg(dst, reg(parse_reg(instruction.operands.at(1).text)));
    }
  } else if (instruction.mnemonic == "add") {
    const auto lhs = reg(dst);
    const auto rhs = reg(parse_reg(instruction.operands.at(1).text));
    const auto wide = static_cast<std::uint64_t>(lhs) + rhs;
    const auto result = static_cast<std::uint32_t>(wide);
    set_reg(dst, result);
    state_.flags.carry = wide > 0xFFFFFFFFULL;
    update_zero_sign(result);
  } else if (instruction.mnemonic == "sub") {
    const auto lhs = reg(dst);
    const auto rhs = reg(parse_reg(instruction.operands.at(1).text));
    const auto result = lhs - rhs;
    set_reg(dst, result);
    state_.flags.carry = lhs < rhs;
    update_zero_sign(result);
  } else {
    throw std::runtime_error("unsupported instruction in golden model: " + instruction.mnemonic);
  }

  state_.eip += static_cast<std::uint32_t>(instruction.bytes.size());
}

void GoldenCpuModel::update_zero_sign(std::uint32_t value) {
  state_.flags.zero = value == 0;
  state_.flags.sign = (value & 0x80000000U) != 0;
}

std::string register_name(Register32 reg) {
  switch (reg) {
    case Register32::Eax:
      return "eax";
    case Register32::Ecx:
      return "ecx";
    case Register32::Edx:
      return "edx";
    case Register32::Ebx:
      return "ebx";
    case Register32::Esp:
      return "esp";
    case Register32::Ebp:
      return "ebp";
    case Register32::Esi:
      return "esi";
    case Register32::Edi:
      return "edi";
  }
  return "?";
}

}  // namespace ptaslearn
