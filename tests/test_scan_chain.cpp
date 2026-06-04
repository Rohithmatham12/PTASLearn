#include "test_helpers.hpp"

#include "ptaslearn/scan_chain.hpp"

TEST_CASE(scan_tracker_counts_stuck_at_and_transition_coverage) {
  ptaslearn::ScanChainCoverageTracker tracker(3);
  tracker.observe({false, false, false});
  tracker.observe({true, true, true});
  tracker.observe({false, false, false});

  const auto coverage = tracker.coverage();
  REQUIRE(coverage.vector_count == 3);
  REQUIRE(coverage.toggle_covered == 3);
  REQUIRE(coverage.stuck_at_covered == 6);
  REQUIRE(coverage.transition_covered == 6);
  REQUIRE(coverage.stuck_at_percent() == 100.0);
  REQUIRE(coverage.transition_percent() == 100.0);
}

TEST_CASE(scan_tracker_reports_uncovered_faults) {
  ptaslearn::ScanChainCoverageTracker tracker(2);
  tracker.observe({false, false});

  const auto faults = tracker.uncovered_faults();
  REQUIRE(!faults.empty());
}
