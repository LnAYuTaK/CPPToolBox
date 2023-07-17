/**
 * @file Database.h
 * @author LnAYuTaK (807874484@qq.com)
 * @brief  数据库模块(sqlite3)
 * @version 0.1
 * @date 2023-06-05
 * @copyright Copyright (c) 2023
 */
#pragma once

#include <string.h>
#include <fstream>
#include <memory>
#include <vector>
#include "CLog.h"
#include "MacroDef.h"
#include "sqlite3.h"

using queryVector = std::vector<std::vector<std::string>>;
class Database {
  DISALLOW_COPY_AND_ASSIGN(Database)
 public:
  // if not Exits then creat
  Database(const char* apFilename,
           const int aFlags = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE,
           const int aBusyTimeoutMs = 0, const char* apVfs = nullptr);

  Database(const std::string& aFilename,
           const int aFlags = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE,
           const int aBusyTimeoutMs = 0, const std::string& aVfs = "")
      : Database(aFilename.c_str(), aFlags, aBusyTimeoutMs,
                 aVfs.empty() ? nullptr : aVfs.c_str()) {}
  ~Database();
  //设置超时连接时间
  void setBusyTimeout(const int aBusyTimeoutMs);
  //执行sql语句TUDO
  int exec(const char* apQueries);
  int exec(const std::string& aQueries) { return exec(aQueries.c_str()); }
  // sql查询---返回queryVector  行+列
  queryVector dbQuery(const std::string& query);
  //获取错误编码
  int getErrorCode() const noexcept;
  //获取错误信息
  const char* getErrorMsg() const noexcept;
  //获取当前数据库路径
  const std::string& getFilename() const noexcept;

 private:
  int tryExec(const char* apQueries) noexcept;
  int tryExec(const std::string& aQueries) noexcept {
    return tryExec(aQueries.c_str());
  }
  sqlite3* handle;
  std::string mFilename;
  //查询数据回调内部执行
  static int queryCallback(void* data, int argc, char** argv,
                           char** columnNames) {
    queryVector* results = static_cast<queryVector*>(data);
    std::vector<std::string> row;
    for (int i = 0; i < argc; ++i) {
      row.push_back(argv[i]);
    }
    results->push_back(row);
    return 0;
  }
};
