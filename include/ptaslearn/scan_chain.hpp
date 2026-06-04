#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace ptaslearn {

using ScanVector = std::vector<bool>;

struct ScanCoverage {
  std::size_t vector_count{0};
  std::size_t flip_flop_count{0};
  std::size_t toggle_covered{0};
  std::size_t stuck_at_covered{0};
  std::size_t stuck_at_total{0};
  std::size_t transition_covered{0};
  std::size_t transition_total{0};

  double toggle_percent() const;
  double stuck_at_percent() const;
  double transition_percent() const;
};

class ScanChainCoverageTracker {
 public:
  explicit ScanChainCoverageTracker(std::size_t flip_flop_count);

  void observe(const ScanVector& vector);
  ScanCoverage coverage() const;
  std::vector<std::string> uncovered_faults() const;
  const std::vector<ScanVector>& vectors() const;
  std::size_t flip_flop_count() const;

 private:
  std::size_t flip_flop_count_;
  std::vector<ScanVector> vectors_;
  std::vector<bool> seen_zero_;
  std::vector<bool> seen_one_;
  std::vector<bool> rose_;
  std::vector<bool> fell_;

  void validate(const ScanVector& vector) const;
};

std::string format_vector(const ScanVector& vector);

}  // namespace ptaslearn
