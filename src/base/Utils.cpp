#include "Utils.h"

#include <iomanip>
#include <sstream>

const char Utils::digits[] = "9876543210123456789";
const char* Utils::zero = digits + 9;
const char Utils::digitsHex[] = "0123456789ABCDEF";

bool  Utils::SplitCmdline(const std::string &cmd, std::vector<std::string> &args)
{
     args.clear();

    size_t start_pos = 0;
    size_t end_pos = 0;

    while (true) {
        start_pos = cmd.find_first_not_of(" \t", end_pos);
        if (start_pos == std::string::npos)
            break;

        char start_char = cmd.at(start_pos);
        if (start_char == '\'' || start_char == '\"') {
            end_pos = cmd.find_first_of(start_char, start_pos + 1);
            if (end_pos == std::string::npos)
                return false;

            args.push_back(cmd.substr(start_pos + 1, end_pos - start_pos - 1));
            ++end_pos;
        } else {
            end_pos = cmd.find_first_of(" \t", start_pos);

            //! think of:
            //! --key="hello world"and'I am here'
            //! as whole string
            auto quot_start_pos = cmd.find_first_of("\'\"", start_pos);
            while (quot_start_pos != std::string::npos && (quot_start_pos < end_pos || end_pos == std::string::npos)) {
                char quot_char = cmd.at(quot_start_pos);
                auto quot_end_pos = cmd.find_first_of(quot_char, quot_start_pos + 1);
                end_pos = cmd.find_first_of(" \t", quot_end_pos + 1);
                quot_start_pos = cmd.find_first_of("\'\"", quot_end_pos + 1);
            }

            args.push_back(cmd.substr(start_pos, end_pos - start_pos));
            if (end_pos == std::string::npos)
                break;
        }
    }
    return true;
}

size_t Utils::convertHex(char buf[], uintptr_t value) {
  uintptr_t i = value;
  char* p = buf;
  do {
    int lsd = static_cast<int>(i % 16);
    i /= 16;
    *p++ = digitsHex[lsd];
  } while (i != 0);
  *p = '\0';
  std::reverse(buf, p);
  return p - buf;
}

size_t Utils::Split(const std::string &src_str, const std::string sep, std::vector<std::string> &str_vec)
{
    size_t start_pos = 0;
    size_t end_pos = 0;
    str_vec.clear();
    while (true) {
        end_pos = src_str.find(sep, start_pos);
        const std::string &str_chip = src_str.substr(start_pos, end_pos - start_pos);
        str_vec.push_back(str_chip);
        if (end_pos == std::string::npos)
            break;
        start_pos = end_pos + sep.size();
    }
    return str_vec.size();
}

size_t Utils::SplitBySpace(const std::string &src_str, std::vector<std::string> &str_vec)
{
    size_t start_pos = 0;
    size_t end_pos = 0;
    str_vec.clear();
    while (true) {
        start_pos = src_str.find_first_not_of(" \t", end_pos);
        if (start_pos == std::string::npos)
            break;
        end_pos = src_str.find_first_of(" \t", start_pos);
        const std::string &str_chip = src_str.substr(start_pos, end_pos - start_pos);
        str_vec.push_back(str_chip);
        if (end_pos == std::string::npos)
            break;
    }
    return str_vec.size();
}

std::string Utils::StripLeft(const std::string &orig_str)
{
    size_t start_pos = orig_str.find_first_not_of(' ', 0);
    if (start_pos == std::string::npos)
        return std::string();
    return orig_str.substr(start_pos);
}

std::string Utils::StripRight(const std::string &orig_str)
{
    size_t end_pos = orig_str.find_last_not_of(' ', orig_str.size() - 1);
    if (end_pos == std::string::npos)
        return std::string();
    return orig_str.substr(0, end_pos + 1);
}

std::string Utils::Strip(const std::string &orig_str)
{
    size_t start_pos = orig_str.find_first_not_of(' ');
    if (start_pos == std::string::npos)
        return std::string();

    size_t end_pos = orig_str.find_last_not_of(' ');
    if (end_pos == std::string::npos)
        return std::string();

    return orig_str.substr(start_pos, end_pos - start_pos + 1);
}

std::string Utils::StripQuot(const std::string &orig_str)
{
    auto first_char = orig_str.front();
    auto last_char = orig_str.back();
    if (first_char == last_char && (first_char == '\'' || first_char == '\"')) {
        return orig_str.substr(1, orig_str.length() - 2);
    } else {
        return orig_str;
    }
}

std::string Utils::RawDataToHexStr(const void *data_ptr, uint16_t data_len, bool uppercase, const std::string &delimiter)
{
    if ((data_ptr == NULL) || (data_len == 0))
        return std::string();

    using namespace std;
    ostringstream oss;
    oss << hex << setfill('0');
    if (uppercase)
        oss << std::uppercase;

    const uint8_t *ptr = static_cast<const uint8_t*>(data_ptr);
    for (uint16_t i = 0; i < data_len; ++i) {
        oss << setw(2) << (int)ptr[i];
        if (i < (data_len - 1))
            oss << delimiter;
    }
    return oss.str();
}

namespace {
static uint8_t hexCharToValue(char hex_char)
{
    if (('0' <= hex_char) && (hex_char <= '9'))
        return hex_char - '0';
    else if (('A' <= hex_char) && (hex_char <= 'F'))
        return hex_char - 'A' + 10;
    else if (('a' <= hex_char) && (hex_char <= 'f'))
        return hex_char - 'a' + 10;
    else 
        throw Utils::NotAZaz09Exception();
}
}

size_t Utils::HexStrToRawData(const std::string &hex_str, void *out_ptr, uint16_t out_len)
{
    if ((out_ptr == NULL) || (out_len == 0))
        return 0;

    uint8_t *p_data = (uint8_t*)out_ptr;
    size_t data_len = 0;
    for (size_t i = 0; (i < out_len) && ((i * 2 + 1) < hex_str.size()); ++i) {
        char h_char = hex_str[2 * i];
        char l_char = hex_str[2 * i + 1];
        p_data[i] = (hexCharToValue(h_char) << 4) | (hexCharToValue(l_char) & 0x0f);
        ++data_len;
    }
    return data_len;
}

namespace {
static void _HexStrToRawDataWithDelimiter(const std::string &hex_str, std::vector<uint8_t> &out, const std::string &delimiter)
{
    auto start_pos = hex_str.find_first_not_of(delimiter);
    while (start_pos != std::string::npos) {
        auto end_pos = hex_str.find_first_of(delimiter, start_pos);
        if (end_pos == std::string::npos)
            end_pos = hex_str.size();

        size_t len = end_pos - start_pos;
        uint8_t value = 0;
        if (len == 1) {
            value = hexCharToValue(hex_str.at(start_pos));
        } else if (len == 2) {
            value = hexCharToValue(hex_str.at(start_pos)) << 4;
            value |= hexCharToValue(hex_str.at(start_pos + 1));
        } else
        {
           throw Utils::MoreThan2CharException();
        }
        out.push_back(value);
        start_pos = hex_str.find_first_not_of(delimiter, end_pos);
    }
}

static void _HexStrToRawDataWithoutDelimiter(const std::string &hex_str, std::vector<uint8_t> &out)
{
    auto start_pos = hex_str.find_first_not_of(" \t");
    auto end_pos = hex_str.find_last_not_of(" \t") + 1;
    for (size_t i = 0; ((i * 2) < (end_pos - start_pos)); ++i) {
        char h_char = hex_str.at(start_pos + 2 * i);
        char l_char = hex_str.at(start_pos + 2 * i + 1);
        uint8_t value = (hexCharToValue(h_char) << 4) | (hexCharToValue(l_char) & 0x0f);
        out.push_back(value);
    }
}
}

size_t Utils::HexStrToRawData(const std::string &hex_str, std::vector<uint8_t> &out, const std::string &delimiter)
{
    out.clear();

    if (delimiter.empty())
        _HexStrToRawDataWithoutDelimiter(hex_str, out);
    else
        _HexStrToRawDataWithDelimiter(hex_str, out, delimiter);

    return out.size();
}

void Utils::Replace(std::string &target_str, const std::string &pattern_str, const std::string &replace_str,
             std::string::size_type start, std::string::size_type count)
{
    if (count == 0)
        count = UINT32_MAX;

    std::string::size_type pos = start;
    std::string::size_type pattern_len = pattern_str.size();
    std::string::size_type replace_str_len = replace_str.size();

    while (count > 0 && (pos = target_str.find(pattern_str, pos)) != std::string::npos) {
        target_str.replace(pos, pattern_len, replace_str);
        pos += replace_str_len;
        --count;
    }
}

/***********************************************************************************************/
