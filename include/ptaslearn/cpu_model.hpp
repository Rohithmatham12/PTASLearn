#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "ptaslearn/x86_decoder.hpp"

namespace ptaslearn {

enum class Register32 {
  Eax = 0,
  Ecx = 1,
  Edx = 2,
  Ebx = 3,
  Esp = 4,
  Ebp = 5,
  Esi = 6,
  Edi = 7
};

struct Flags {
  bool zero{false};
  bool sign{false};
  bool carry{false};
  bool overflow{false};
};

struct CpuState {
  std::array<std::uint32_t, 8> registers{};
  std::uint32_t eip{0};
  Flags flags;
};

class GoldenCpuModel {
 public:
  GoldenCpuModel();

  const CpuState& state() const;
  std::uint32_t reg(Register32 reg) const;
  void set_reg(Register32 reg, std::uint32_t value);
  void execute(const std::vector<std::uint8_t>& bytes);
  void execute(const Instruction& instruction);

 private:
  CpuState state_;
  X86Decoder decoder_;

  void update_zero_sign(std::uint32_t value);
};

std::string register_name(Register32 reg);

}  // namespace ptaslearn
