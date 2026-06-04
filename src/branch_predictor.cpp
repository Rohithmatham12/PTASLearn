#include "ptaslearn/branch_predictor.hpp"

namespace ptaslearn {

bool TwoBitBranchPredictor::predict(std::uint64_t branch_address) const {
  return static_cast<int>(state(branch_address)) >= static_cast<int>(PredictorState::WeaklyTaken);
}

void TwoBitBranchPredictor::update(std::uint64_t branch_address, bool taken) {
  auto current = static_cast<int>(state(branch_address));
  current += taken ? 1 : -1;
  if (current < 0) current = 0;
  if (current > 3) current = 3;
  table_[branch_address] = static_cast<PredictorState>(current);
}

PredictorState TwoBitBranchPredictor::state(std::uint64_t branch_address) const {
  const auto found = table_.find(branch_address);
  if (found == table_.end()) {
    return PredictorState::WeaklyNotTaken;
  }
  return found->second;
}

std::string to_string(PredictorState state) {
  switch (state) {
    case PredictorState::StronglyNotTaken:
      return "SNT";
    case PredictorState::WeaklyNotTaken:
      return "WNT";
    case PredictorState::WeaklyTaken:
      return "WT";
    case PredictorState::StronglyTaken:
      return "ST";
  }
  return "?";
}

}  // namespace ptaslearn
