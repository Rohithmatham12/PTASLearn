#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace ptaslearn {

enum class PredictorState {
  StronglyNotTaken = 0,
  WeaklyNotTaken = 1,
  WeaklyTaken = 2,
  StronglyTaken = 3
};

class TwoBitBranchPredictor {
 public:
  bool predict(std::uint64_t branch_address) const;
  void update(std::uint64_t branch_address, bool taken);
  PredictorState state(std::uint64_t branch_address) const;

 private:
  std::unordered_map<std::uint64_t, PredictorState> table_;
};

std::string to_string(PredictorState state);

}  // namespace ptaslearn
