#include "ptaslearn/scan_chain.hpp"

#include <sstream>
#include <stdexcept>

namespace ptaslearn {
namespace {

double percent(std::size_t covered, std::size_t total) {
  if (total == 0) {
    return 100.0;
  }
  return 100.0 * static_cast<double>(covered) / static_cast<double>(total);
}

}  // namespace

double ScanCoverage::toggle_percent() const {
  return percent(toggle_covered, flip_flop_count);
}

double ScanCoverage::stuck_at_percent() const {
  return percent(stuck_at_covered, stuck_at_total);
}

double ScanCoverage::transition_percent() const {
  return percent(transition_covered, transition_total);
}

ScanChainCoverageTracker::ScanChainCoverageTracker(std::size_t flip_flop_count)
    : flip_flop_count_(flip_flop_count),
      seen_zero_(flip_flop_count, false),
      seen_one_(flip_flop_count, false),
      rose_(flip_flop_count, false),
      fell_(flip_flop_count, false) {
  if (flip_flop_count == 0) {
    throw std::invalid_argument("flip_flop_count must be greater than zero");
  }
}

void ScanChainCoverageTracker::observe(const ScanVector& vector) {
  validate(vector);
  if (!vectors_.empty()) {
    const auto& previous = vectors_.back();
    for (std::size_t i = 0; i < flip_flop_count_; ++i) {
      if (!previous[i] && vector[i]) {
        rose_[i] = true;
      }
      if (previous[i] && !vector[i]) {
        fell_[i] = true;
      }
    }
  }

  for (std::size_t i = 0; i < flip_flop_count_; ++i) {
    seen_zero_[i] = seen_zero_[i] || !vector[i];
    seen_one_[i] = seen_one_[i] || vector[i];
  }
  vectors_.push_back(vector);
}

ScanCoverage ScanChainCoverageTracker::coverage() const {
  ScanCoverage result;
  result.vector_count = vectors_.size();
  result.flip_flop_count = flip_flop_count_;
  result.stuck_at_total = flip_flop_count_ * 2;
  result.transition_total = flip_flop_count_ * 2;

  for (std::size_t i = 0; i < flip_flop_count_; ++i) {
    if (seen_zero_[i] && seen_one_[i]) {
      ++result.toggle_covered;
    }
    if (seen_zero_[i]) {
      ++result.stuck_at_covered;
    }
    if (seen_one_[i]) {
      ++result.stuck_at_covered;
    }
    if (rose_[i]) {
      ++result.transition_covered;
    }
    if (fell_[i]) {
      ++result.transition_covered;
    }
  }

  return result;
}

std::vector<std::string> ScanChainCoverageTracker::uncovered_faults() const {
  std::vector<std::string> faults;
  for (std::size_t i = 0; i < flip_flop_count_; ++i) {
    if (!seen_one_[i]) {
      faults.push_back("ff" + std::to_string(i) + " stuck-at-0 not detected");
    }
    if (!seen_zero_[i]) {
      faults.push_back("ff" + std::to_string(i) + " stuck-at-1 not detected");
    }
    if (!rose_[i]) {
      faults.push_back("ff" + std::to_string(i) + " rising transition not detected");
    }
    if (!fell_[i]) {
      faults.push_back("ff" + std::to_string(i) + " falling transition not detected");
    }
  }
  return faults;
}

const std::vector<ScanVector>& ScanChainCoverageTracker::vectors() const {
  return vectors_;
}

std::size_t ScanChainCoverageTracker::flip_flop_count() const {
  return flip_flop_count_;
}

void ScanChainCoverageTracker::validate(const ScanVector& vector) const {
  if (vector.size() != flip_flop_count_) {
    throw std::invalid_argument("scan vector length does not match scan chain length");
  }
}

std::string format_vector(const ScanVector& vector) {
  std::ostringstream out;
  for (bool bit : vector) {
    out << (bit ? '1' : '0');
  }
  return out.str();
}

}  // namespace ptaslearn
