#include "test_helpers.hpp"

#include "ptaslearn/cache_mesi.hpp"

TEST_CASE(mesi_read_read_write_transitions) {
  ptaslearn::MesiSimulator sim(2);
  constexpr auto address = 0x1000;

  sim.read(0, address);
  REQUIRE(sim.state(0, address) == ptaslearn::MesiState::Exclusive);
  REQUIRE(sim.state(1, address) == ptaslearn::MesiState::Invalid);

  sim.read(1, address);
  REQUIRE(sim.state(0, address) == ptaslearn::MesiState::Shared);
  REQUIRE(sim.state(1, address) == ptaslearn::MesiState::Shared);

  sim.write(0, address);
  REQUIRE(sim.state(0, address) == ptaslearn::MesiState::Modified);
  REQUIRE(sim.state(1, address) == ptaslearn::MesiState::Invalid);
}

TEST_CASE(mesi_evict_invalidates_line_for_core) {
  ptaslearn::MesiSimulator sim(2);
  constexpr auto address = 0x2000;
  sim.read(0, address);
  sim.evict(0, address);

  REQUIRE(sim.state(0, address) == ptaslearn::MesiState::Invalid);
}
