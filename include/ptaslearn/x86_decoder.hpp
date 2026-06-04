#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ptaslearn {

enum class OperandKind {
  None,
  Register,
  Immediate,
  Relative
};

struct Operand {
  OperandKind kind{OperandKind::None};
  std::string text;
  std::int64_t value{0};
};

struct Instruction {
  std::size_t offset{0};
  std::vector<std::uint8_t> bytes;
  std::string mnemonic;
  std::vector<Operand> operands;
};

class X86Decoder {
 public:
  std::vector<Instruction> decode(const std::vector<std::uint8_t>& bytes) const;
  std::string format(const Instruction& instruction) const;
};

}  // namespace ptaslearn
