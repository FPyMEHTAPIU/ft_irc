#include "irc.hpp"

std::string generateRandomId() {
  static std::random_device rd;
  static std::mt19937_64 gen(rd());
  static std::uniform_int_distribution<uint64_t> dis;
  
  uint64_t part1 = dis(gen);
  uint64_t part2 = dis(gen);

  std::stringstream ss;
  ss << std::hex << std::setw(16) << std::setfill('0') << part1;
  ss << std::hex << std::setw(16) << std::setfill('0') << part2;

  return ss.str();
}