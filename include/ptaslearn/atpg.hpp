#pragma once

#include <cstddef>
#include <vector>

#include "ptaslearn/scan_chain.hpp"

namespace ptaslearn {

class AtpgGenerator {
 public:
  explicit AtpgGenerator(std::size_t flip_flop_count);

  std::vector<ScanVector> generate(std::size_t max_vectors) const;
  ScanVector seed_for_stuck_at(std::size_t bit, bool stuck_at_one) const;
  ScanVector transition_pair_source(std::size_t bit, bool rising) const;
  ScanVector transition_pair_target(std::size_t bit, bool rising) const;

 private:
  std::size_t flip_flop_count_;
};

}  // namespace ptaslearn
