/**
 * @file Utils.h
 * @author 刘宽 (807874484@qq.com)
 * @brief 工具类
 * @version 0.1
 * @date 2023-07-02
 *  
 * @copyright Copyright (c) 2023 国网中电研发部
 * 
 */
#pragma once

#include <assert.h>
// #include <google/protobuf/util/json_util.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <algorithm>
#include <chrono>
#include <limits>
#include <thread>
#include <type_traits>
#include <vector>

// using google::protobuf::util::JsonStringToMessage;
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

class Utils {
 public:
  static const char digits[];
  static const char* zero;
  static const char digitsHex[];

  static inline void memZero(void* p, size_t n) { memset(p, 0, n); }

  // Proto 类转换Json-string
  // static bool protoToJson(const google::protobuf::Message& message,
  //                         std::string& json) {
  //   google::protobuf::util::JsonPrintOptions options;
  //   options.add_whitespace = true;
  //   options.always_print_primitive_fields = true;
  //   options.preserve_proto_field_names = true;
  //   return MessageToJsonString(message, &json, options).ok();
  // }
  // // Json string 转换Proto
  // static bool jsonToProto(const std::string& json,
  //                         google::protobuf::Message& message) {
  //   return JsonStringToMessage(json, &message).ok();
  // }
  //
  static size_t convertHex(char buf[], uintptr_t value);

  template <typename T>
  static size_t convert(char buf[], T value) {
    T i = value;
    char* p = buf;
    do {
      int lsd = static_cast<int>(i % 10);
      i /= 10;
      *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0) {
      *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);
    return p - buf;
  }

  template <typename To, typename From>
  static inline To implicit_cast(From const& f) {
    return f;
  }

  // Get Function Run Time
  template <typename F>
  double calculateExecutionTime(F func) {
    // 获取当前时间点
    auto start = std::chrono::high_resolution_clock::now();
    // 执行函数
    func();
    // 获取当前时间点
    auto end = std::chrono::high_resolution_clock::now();
    // 计算执行时间（以毫秒为单位）
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    return duration;
  }
 static bool SplitCmdline(const std::string &cmd, std::vector<std::string> &args);
/**
 * \brief       分隔字符串
 * \param src_str   被分隔的字串
 * \param sep       分隔字串
 * \param str_vec   分隔后输出的字串数组
 * \return      分隔得到了子字串个数
 */
static size_t Split(const std::string &src_str, const std::string sep, std::vector<std::string> &str_vec);

/**
 * \brief       以空格分隔字串
 */
static size_t SplitBySpace(const std::string &src_str, std::vector<std::string> &str_vec);

/**
 * \brief       消除字串左边的空格符
 * \param orig_str  原始字串
 * \return      处理后的字串
 */
static std::string StripLeft(const std::string &orig_str);

/**
 * \brief       消除字串右边的空格符
 * \param orig_str  原始字串
 * \return      处理后的字串
 */
static std::string StripRight(const std::string &orig_str);

/**
 * \brief       消除字串双边的空格符
 * \param orig_str  原始字串
 * \return      处理后的字串
 */
static std::string Strip(const std::string &orig_str);

/**
 * \brief       消除字串两边的引号，包括单引号与双引号
 * \param orig_str  原始字串
 * \return      处理后的字串
 */
static std::string StripQuot(const std::string &orig_str);

/**
 * \brief       将原始数据转换成HEX字串
 *
 * \param data_ptr  原始数据内存地址
 * \param data_len  原始数据长度
 * \param uppercase HEX中a~f
 * \param delimiter 分隔字串
 *
 * \return      转换后的HEX字串
 */
static std::string RawDataToHexStr(const void *data_ptr, uint16_t data_len,
                            bool uppercase = false,
                            const std::string &delimiter = std::string(" "));

//! 字符不是A-Z a-z 0-9
class NotAZaz09Exception : public std::exception {
    virtual const char* what() const throw() override {
        return "character not a-z A-Z 0-9";
    }
};

//! 字串个体超过2个
class MoreThan2CharException : public std::exception {
    virtual const char* what() const throw() override {
        return "more than 2 characters";
    }
};

/**
 * \brief       将HEX字串转换成原始数据
 *
 * \param hex_str   HEX字串
 * \param out_ptr   输出到数据地址
 * \param out_len   输出到数据长度
 *
 * \return      实际转换的数据长度
 */
static size_t HexStrToRawData(const std::string &hex_str, void *out_ptr, uint16_t out_len);

/**
 * \brief       将HEX字串转换成原始数据
 *
 * \param hex_str   HEX字串
 * \param out       输出到数据vector
 * \param delimiter 分隔符，常用的分隔符有 " \t", ": ", ", "
 *
 * \return      实际转换的数据长度
 */
static size_t HexStrToRawData(const std::string &hex_str, std::vector<uint8_t> &out, const std::string &delimiter = "");

/**
 * \brief       替换字串中指定个数的字串
 *
 * \param target_str    被修改的字串
 * \param pattern_str   匹配字串
 * \param replace_str   用于替换的字串
 * \param count         替换次数
 */
static void Replace(std::string &target_str, const std::string &pattern_str, const std::string &replace_str,
             std::string::size_type start = 0, std::string::size_type count = 0);


 private:
  Utils(/* args */) {}
  ~Utils() {}
};
