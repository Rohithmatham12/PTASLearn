#include "test_helpers.hpp"

#include <cstdint>
#include <vector>

#include "ptaslearn/x86_decoder.hpp"

TEST_CASE(decodes_nop_mov_and_ret) {
  ptaslearn::X86Decoder decoder;
  const std::vector<std::uint8_t> bytes = {0x90, 0xB8, 0x34, 0x12, 0x00, 0x00, 0xC3};

  const auto instructions = decoder.decode(bytes);

  REQUIRE(instructions.size() == 3);
  REQUIRE(instructions[0].mnemonic == "nop");
  REQUIRE(instructions[1].mnemonic == "mov");
  REQUIRE(instructions[1].operands[0].text == "eax");
  REQUIRE(instructions[1].operands[1].value == 0x1234);
  REQUIRE(instructions[2].mnemonic == "ret");
}

TEST_CASE(decodes_register_to_register_mov) {
  ptaslearn::X86Decoder decoder;
  const auto instructions = decoder.decode({0x89, 0xD8});

  REQUIRE(instructions.size() == 1);
  REQUIRE(instructions[0].operands[0].text == "eax");
  REQUIRE(instructions[0].operands[1].text == "ebx");
}
