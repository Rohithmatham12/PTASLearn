#include "ptaslearn/cache_mesi.hpp"

#include <sstream>
#include <stdexcept>

namespace ptaslearn {

MesiSimulator::MesiSimulator(std::size_t core_count) : core_count_(core_count) {
  if (core_count == 0) {
    throw std::invalid_argument("core_count must be greater than zero");
  }
}

CacheEvent MesiSimulator::read(std::size_t core, std::uint64_t address) {
  validate_core(core);
  auto& line = ensure_line(address);
  const bool other_has_copy = [&] {
    for (std::size_t i = 0; i < core_count_; ++i) {
      if (i != core && line[i] != MesiState::Invalid) {
        return true;
      }
    }
    return false;
  }();

  if (line[core] == MesiState::Invalid) {
    for (std::size_t i = 0; i < core_count_; ++i) {
      if (i != core && line[i] == MesiState::Modified) {
        line[i] = MesiState::Shared;
      }
      if (i != core && line[i] == MesiState::Exclusive) {
        line[i] = MesiState::Shared;
      }
    }
    line[core] = other_has_copy ? MesiState::Shared : MesiState::Exclusive;
  }

  return {core, address, "read", snapshot(address)};
}

CacheEvent MesiSimulator::write(std::size_t core, std::uint64_t address) {
  validate_core(core);
  auto& line = ensure_line(address);
  for (std::size_t i = 0; i < core_count_; ++i) {
    if (i != core) {
      line[i] = MesiState::Invalid;
    }
  }
  line[core] = MesiState::Modified;
  return {core, address, "write", snapshot(address)};
}

CacheEvent MesiSimulator::evict(std::size_t core, std::uint64_t address) {
  validate_core(core);
  auto& line = ensure_line(address);
  line[core] = MesiState::Invalid;
  return {core, address, "evict", snapshot(address)};
}

MesiState MesiSimulator::state(std::size_t core, std::uint64_t address) const {
  validate_core(core);
  const auto found = lines_.find(address);
  if (found == lines_.end()) {
    return MesiState::Invalid;
  }
  return found->second[core];
}

std::vector<MesiState> MesiSimulator::states(std::uint64_t address) const {
  const auto found = lines_.find(address);
  if (found == lines_.end()) {
    return std::vector<MesiState>(core_count_, MesiState::Invalid);
  }
  return found->second;
}

std::string MesiSimulator::snapshot(std::uint64_t address) const {
  std::ostringstream out;
  const auto current = states(address);
  for (std::size_t i = 0; i < current.size(); ++i) {
    if (i != 0) {
      out << " ";
    }
    out << "C" << i << "=" << to_string(current[i]);
  }
  return out.str();
}

std::size_t MesiSimulator::core_count() const {
  return core_count_;
}

std::vector<MesiState>& MesiSimulator::ensure_line(std::uint64_t address) {
  auto [it, inserted] = lines_.emplace(address, std::vector<MesiState>(core_count_, MesiState::Invalid));
  (void)inserted;
  return it->second;
}

void MesiSimulator::validate_core(std::size_t core) const {
  if (core >= core_count_) {
    throw std::out_of_range("core index out of range");
  }
}

std::string to_string(MesiState state) {
  switch (state) {
    case MesiState::Invalid:
      return "I";
    case MesiState::Shared:
      return "S";
    case MesiState::Exclusive:
      return "E";
    case MesiState::Modified:
      return "M";
  }
  return "?";
}

}  // namespace ptaslearn
