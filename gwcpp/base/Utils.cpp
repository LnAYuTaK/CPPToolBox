#include "Utils.h"

#include <iomanip>
#include <sstream>
using namespace gwcpp;
const char Utils::digits[] = "9876543210123456789";
const char *Utils::zero = digits + 9;
const char Utils::digitsHex[] = "0123456789ABCDEF";

bool Utils::SplitCmdline(const std::string &cmd,
                         std::vector<std::string> &args) {
  args.clear();

  size_t start_pos = 0;
  size_t end_pos = 0;

  while (true) {
    start_pos = cmd.find_first_not_of(" \t", end_pos);
    if (start_pos == std::string::npos) break;

    char start_char = cmd.at(start_pos);
    if (start_char == '\'' || start_char == '\"') {
      end_pos = cmd.find_first_of(start_char, start_pos + 1);
      if (end_pos == std::string::npos) return false;

      args.push_back(cmd.substr(start_pos + 1, end_pos - start_pos - 1));
      ++end_pos;
    } else {
      end_pos = cmd.find_first_of(" \t", start_pos);

      //! think of:
      //! --key="hello world"and'I am here'
      //! as whole string
      auto quot_start_pos = cmd.find_first_of("\'\"", start_pos);
      while (quot_start_pos != std::string::npos &&
             (quot_start_pos < end_pos || end_pos == std::string::npos)) {
        char quot_char = cmd.at(quot_start_pos);
        auto quot_end_pos = cmd.find_first_of(quot_char, quot_start_pos + 1);
        end_pos = cmd.find_first_of(" \t", quot_end_pos + 1);
        quot_start_pos = cmd.find_first_of("\'\"", quot_end_pos + 1);
      }

      args.push_back(cmd.substr(start_pos, end_pos - start_pos));
      if (end_pos == std::string::npos) break;
    }
  }
  return true;
}

size_t Utils::convertHex(char buf[], uintptr_t value) {
  uintptr_t i = value;
  char *p = buf;
  do {
    int lsd = static_cast<int>(i % 16);
    i /= 16;
    *p++ = digitsHex[lsd];
  } while (i != 0);
  *p = '\0';
  std::reverse(buf, p);
  return p - buf;
}

size_t Utils::Split(const std::string &src_str, const std::string sep,
                    std::vector<std::string> &str_vec) {
  size_t start_pos = 0;
  size_t end_pos = 0;
  str_vec.clear();
  while (true) {
    end_pos = src_str.find(sep, start_pos);
    const std::string &str_chip =
        src_str.substr(start_pos, end_pos - start_pos);
    str_vec.push_back(str_chip);
    if (end_pos == std::string::npos) break;
    start_pos = end_pos + sep.size();
  }
  return str_vec.size();
}

size_t Utils::SplitBySpace(const std::string &src_str,
                           std::vector<std::string> &str_vec) {
  size_t start_pos = 0;
  size_t end_pos = 0;
  str_vec.clear();
  while (true) {
    start_pos = src_str.find_first_not_of(" \t", end_pos);
    if (start_pos == std::string::npos) break;
    end_pos = src_str.find_first_of(" \t", start_pos);
    const std::string &str_chip =
        src_str.substr(start_pos, end_pos - start_pos);
    str_vec.push_back(str_chip);
    if (end_pos == std::string::npos) break;
  }
  return str_vec.size();
}

std::string Utils::StripLeft(const std::string &orig_str) {
  size_t start_pos = orig_str.find_first_not_of(' ', 0);
  if (start_pos == std::string::npos) return std::string();
  return orig_str.substr(start_pos);
}

std::string Utils::StripRight(const std::string &orig_str) {
  size_t end_pos = orig_str.find_last_not_of(' ', orig_str.size() - 1);
  if (end_pos == std::string::npos) return std::string();
  return orig_str.substr(0, end_pos + 1);
}

std::string Utils::Strip(const std::string &orig_str) {
  size_t start_pos = orig_str.find_first_not_of(' ');
  if (start_pos == std::string::npos) return std::string();

  size_t end_pos = orig_str.find_last_not_of(' ');
  if (end_pos == std::string::npos) return std::string();

  return orig_str.substr(start_pos, end_pos - start_pos + 1);
}

std::string Utils::StripQuot(const std::string &orig_str) {
  auto first_char = orig_str.front();
  auto last_char = orig_str.back();
  if (first_char == last_char && (first_char == '\'' || first_char == '\"')) {
    return orig_str.substr(1, orig_str.length() - 2);
  } else {
    return orig_str;
  }
}

