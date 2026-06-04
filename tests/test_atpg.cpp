#include "test_helpers.hpp"

#include "ptaslearn/atpg.hpp"
#include "ptaslearn/scan_chain.hpp"

TEST_CASE(atpg_generates_vectors_that_improve_coverage) {
  ptaslearn::AtpgGenerator generator(4);
  ptaslearn::ScanChainCoverageTracker tracker(4);

  for (const auto& vector : generator.generate(10)) {
    tracker.observe(vector);
  }

  const auto coverage = tracker.coverage();
  REQUIRE(coverage.stuck_at_percent() == 100.0);
  REQUIRE(coverage.transition_percent() == 100.0);
}

TEST_CASE(atpg_seed_targets_stuck_at_fault) {
  ptaslearn::AtpgGenerator generator(3);
  const auto detect_stuck_at_one = generator.seed_for_stuck_at(1, true);
  const auto detect_stuck_at_zero = generator.seed_for_stuck_at(1, false);

  REQUIRE(!detect_stuck_at_one[1]);
  REQUIRE(detect_stuck_at_zero[1]);
}
