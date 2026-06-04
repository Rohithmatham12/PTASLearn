#include "ptaslearn/atpg.hpp"

#include <stdexcept>

namespace ptaslearn {

AtpgGenerator::AtpgGenerator(std::size_t flip_flop_count) : flip_flop_count_(flip_flop_count) {
  if (flip_flop_count == 0) {
    throw std::invalid_argument("flip_flop_count must be greater than zero");
  }
}

std::vector<ScanVector> AtpgGenerator::generate(std::size_t max_vectors) const {
  std::vector<ScanVector> generated;
  if (max_vectors == 0) {
    return generated;
  }

  ScanVector zeros(flip_flop_count_, false);
  ScanVector ones(flip_flop_count_, true);
  generated.push_back(zeros);
  if (generated.size() < max_vectors) {
    generated.push_back(ones);
  }

  for (std::size_t bit = 0; bit < flip_flop_count_ && generated.size() + 1 < max_vectors; ++bit) {
    generated.push_back(transition_pair_source(bit, true));
    generated.push_back(transition_pair_target(bit, true));
  }

  return generated;
}

ScanVector AtpgGenerator::seed_for_stuck_at(std::size_t bit, bool stuck_at_one) const {
  if (bit >= flip_flop_count_) {
    throw std::out_of_range("bit index out of range");
  }
  ScanVector vector(flip_flop_count_, false);
  vector[bit] = !stuck_at_one;
  return vector;
}

ScanVector AtpgGenerator::transition_pair_source(std::size_t bit, bool rising) const {
  if (bit >= flip_flop_count_) {
    throw std::out_of_range("bit index out of range");
  }
  ScanVector vector(flip_flop_count_, false);
  vector[bit] = !rising;
  return vector;
}

ScanVector AtpgGenerator::transition_pair_target(std::size_t bit, bool rising) const {
  if (bit >= flip_flop_count_) {
    throw std::out_of_range("bit index out of range");
  }
  ScanVector vector(flip_flop_count_, false);
  vector[bit] = rising;
  return vector;
}

}  // namespace ptaslearn
