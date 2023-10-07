#include "Database.h"
#include "CLog.h"

Database::Database(const char* apFilename,
                   const int aFlags /* = SQLite::OPEN_READONLY*/,
                   const int aBusyTimeoutMs /* = 0 */,
                   const char* apVfs /* = nullptr*/)
    : mFilename(apFilename), handle(NULL) {
  const int ret = sqlite3_open_v2(apFilename, &handle, aFlags, apVfs);
  if (SQLITE_OK == ret) {
  } else {
  }
  if (aBusyTimeoutMs > 0) {
    setBusyTimeout(aBusyTimeoutMs);
  }
}

Database::~Database() { sqlite3_close(handle); }

const std::string& Database::getFilename() const noexcept { return mFilename; }

void Database::setBusyTimeout(const int aBusyTimeoutMs) {
  sqlite3_busy_timeout(handle, aBusyTimeoutMs);
}

int Database::exec(const char* apQueries) {
  tryExec(apQueries);
  return sqlite3_changes(handle);
}

int Database::tryExec(const char* apQueries) noexcept {
  return sqlite3_exec(handle, apQueries, nullptr, nullptr, nullptr);
}

int Database::getErrorCode() const noexcept { return sqlite3_errcode(handle); }

const char* Database::getErrorMsg() const noexcept {
  return sqlite3_errmsg(handle);
}

queryVector Database::dbQuery(const std::string& query) {
  queryVector results;
  int result =
      sqlite3_exec(handle, query.c_str(), queryCallback, &results, NULL);
  return results;
}
