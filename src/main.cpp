#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

#include "ptaslearn/atpg.hpp"
#include "ptaslearn/cache_mesi.hpp"
#include "ptaslearn/scan_chain.hpp"
#include "ptaslearn/x86_decoder.hpp"

namespace {

void print_decoder_demo() {
  ptaslearn::X86Decoder decoder;
  const std::vector<std::uint8_t> bytes = {
      0x90,                         // nop
      0xB8, 0x34, 0x12, 0x00, 0x00,  // mov eax, 0x1234
      0x89, 0xD8,                    // mov eax, ebx
      0xEB, 0x02,                    // jmp +2
      0xC3                           // ret
  };

  std::cout << "x86 decode:\n";
  for (const auto& instruction : decoder.decode(bytes)) {
    std::cout << "  " << decoder.format(instruction) << "\n";
  }
}

void print_mesi_demo() {
  constexpr std::uint64_t address = 0x1000;
  ptaslearn::MesiSimulator mesi(2);
  std::cout << "\nMESI:\n";
  std::cout << "  core0 read  -> " << mesi.read(0, address).note << "\n";
  std::cout << "  core1 read  -> " << mesi.read(1, address).note << "\n";
  std::cout << "  core0 write -> " << mesi.write(0, address).note << "\n";
}

void print_scan_demo() {
  ptaslearn::AtpgGenerator generator(4);
  ptaslearn::ScanChainCoverageTracker tracker(4);
  for (const auto& vector : generator.generate(10)) {
    tracker.observe(vector);
  }

  const auto coverage = tracker.coverage();
  std::cout << "\nScan coverage:\n";
  std::cout << "  vectors=" << coverage.vector_count
            << " stuck-at=" << std::fixed << std::setprecision(1) << coverage.stuck_at_percent()
            << "% transition=" << coverage.transition_percent() << "%\n";
}

}  // namespace

int main() {
  print_decoder_demo();
  print_mesi_demo();
  print_scan_demo();
  return 0;
}
