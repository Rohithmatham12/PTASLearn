#include "test_helpers.hpp"

#include "ptaslearn/cpu_model.hpp"

TEST_CASE(golden_cpu_executes_mov_add_sub_and_flags) {
  ptaslearn::GoldenCpuModel cpu;
  cpu.execute({
      0xB8, 0x03, 0x00, 0x00, 0x00,  // mov eax, 3
      0xBB, 0x02, 0x00, 0x00, 0x00,  // mov ebx, 2
      0x01, 0xD8,                    // add eax, ebx
      0x29, 0xD8                     // sub eax, ebx
  });

  REQUIRE(cpu.reg(ptaslearn::Register32::Eax) == 3);
  REQUIRE(!cpu.state().flags.zero);
  REQUIRE(!cpu.state().flags.sign);
}

TEST_CASE(golden_cpu_sets_zero_and_carry_on_subtract) {
  ptaslearn::GoldenCpuModel cpu;
  cpu.set_reg(ptaslearn::Register32::Eax, 1);
  cpu.set_reg(ptaslearn::Register32::Ebx, 1);
  ptaslearn::X86Decoder decoder;
  cpu.execute(decoder.decode({0x29, 0xD8}).front());

  REQUIRE(cpu.reg(ptaslearn::Register32::Eax) == 0);
  REQUIRE(cpu.state().flags.zero);
  REQUIRE(!cpu.state().flags.carry);
}
