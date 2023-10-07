/**
 * @file FileHelper.h
 * @author 刘宽 (807874484@qq.com)
 * @brief  文件操作工具类
 * 内部是都是静态方法，无法实例化
 * @version 0.1
 * @date 2023-06-25
 *
 * @copyright Copyright (c) 2023 国网中电研发部
 *
 */
#pragma once

#include <dirent.h>
#include <iostream>
#include <string>
#include <vector>
namespace gwcpp
{
  class FileHelper {
 public:
  enum FileType { TYPE_FILE, TYPE_DIR };
  //文件是否存在
  static bool isFileExists(const std::string filename);
  //获取文件内容
  static bool getContent(const std::string &file_name, std::string *content);
  //拼接路径
  static std::string getAbsolutePath(const std::string &prefix,
                                     const std::string &relative_path);
  //判断路径是否存在
  static bool pathExists(const std::string &path);
  //判断文件夹是否存在
  static bool directoryExists(const std::string &directory_path);
  //通配符包含
  static std::vector<std::string> globs(const std::string &pattern);
  //拷贝文件
  static bool copyFile(const std::string &from, const std::string &to);
  //拷贝文件夹
  static bool copyDir(const std::string &from, const std::string &to);
  //拷贝 文件夹或者文件
  static bool copy(const std::string &from, const std::string &to);
  //确保目录
  static bool ensureDirectory(const std::string &directory_path);
  //删除所有文件
  static bool removeAllFiles(const std::string &directory_path);
  //列出所属路径
  static std::vector<std::string> listSubPaths(
      const std::string &directory_path, const unsigned char d_type = DT_DIR);
  //获取文件名
  static std::string getFileName(const std::string &path,
                                 const bool remove_extension = false);
  //获取当前路径
  static std::string getCurrentPath();
  //删除文件
  static bool deleteFile(const std::string &filename);
  //获取类型
  static bool getType(const std::string &filename, FileType *type);
  //创建文件夹
  static bool createDir(const std::string &dir);

 private:
  FileHelper() = delete;
  ~FileHelper() = delete;
};
} // namespace gwcpp


