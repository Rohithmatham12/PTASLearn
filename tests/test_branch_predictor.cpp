#include "test_helpers.hpp"

#include "ptaslearn/branch_predictor.hpp"

TEST_CASE(two_bit_predictor_saturates_toward_taken_and_not_taken) {
  ptaslearn::TwoBitBranchPredictor predictor;
  constexpr auto branch = 0x400100;

  REQUIRE(!predictor.predict(branch));
  predictor.update(branch, true);
  REQUIRE(predictor.predict(branch));
  predictor.update(branch, true);
  REQUIRE(predictor.state(branch) == ptaslearn::PredictorState::StronglyTaken);
  predictor.update(branch, false);
  predictor.update(branch, false);
  predictor.update(branch, false);
  REQUIRE(predictor.state(branch) == ptaslearn::PredictorState::StronglyNotTaken);
}
