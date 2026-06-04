#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace ptaslearn {

enum class MesiState {
  Invalid,
  Shared,
  Exclusive,
  Modified
};

struct CacheEvent {
  std::size_t core{0};
  std::uint64_t line{0};
  std::string action;
  std::string note;
};

class MesiSimulator {
 public:
  explicit MesiSimulator(std::size_t core_count);

  CacheEvent read(std::size_t core, std::uint64_t address);
  CacheEvent write(std::size_t core, std::uint64_t address);
  CacheEvent evict(std::size_t core, std::uint64_t address);

  MesiState state(std::size_t core, std::uint64_t address) const;
  std::vector<MesiState> states(std::uint64_t address) const;
  std::string snapshot(std::uint64_t address) const;
  std::size_t core_count() const;

 private:
  std::size_t core_count_;
  std::unordered_map<std::uint64_t, std::vector<MesiState>> lines_;

  std::vector<MesiState>& ensure_line(std::uint64_t address);
  void validate_core(std::size_t core) const;
};

std::string to_string(MesiState state);

}  // namespace ptaslearn
