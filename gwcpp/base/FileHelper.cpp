#include "FileHelper.h"

#include <fcntl.h>
#include <glob.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <sstream>
using namespace gwcpp;
using std::istreambuf_iterator;
using std::string;
using std::vector;

/***********************************************************************************************/
bool FileHelper::isFileExists(const std::string fileName) {
  std::ifstream file(fileName);
  return file.good();
}
/***********************************************************************************************/
bool FileHelper::getContent(const std::string &file_name,
                            std::string *content) {
  std::ifstream fin(file_name);
  if (!fin) {
    return false;
  }
  std::stringstream str_stream;
  str_stream << fin.rdbuf();
  *content = str_stream.str();
  fin.close();
  return true;
}
/***********************************************************************************************/
std::string FileHelper::getAbsolutePath(const std::string &prefix,
                                        const std::string &relative_path) {
  if (relative_path.empty()) {
    return prefix;
  }
  if (prefix.empty() || relative_path.front() == '/') {
    return relative_path;
  }
  if (prefix.back() == '/') {
    return prefix + relative_path;
  }
  return prefix + "/" + relative_path;
}
/***********************************************************************************************/
bool FileHelper::pathExists(const std::string &path) {
  struct stat info;
  return stat(path.c_str(), &info) == 0;
}
/***********************************************************************************************/
bool FileHelper::directoryExists(const std::string &directory_path) {
  struct stat info;
  return stat(directory_path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
}
/***********************************************************************************************/
std::vector<std::string> FileHelper::globs(const std::string &pattern) {
  glob_t globs = {};
  std::vector<std::string> results;
  if (glob(pattern.c_str(), GLOB_TILDE, nullptr, &globs) == 0) {
    for (size_t i = 0; i < globs.gl_pathc; ++i) {
      results.emplace_back(globs.gl_pathv[i]);
    }
  }
  globfree(&globs);
  return results;
}
/***********************************************************************************************/
bool FileHelper::copyFile(const std::string &from, const std::string &to) {
  std::ifstream src(from, std::ios::binary);
  if (!src) {
    std::string command = "cp -r " + from + " " + to;
    const int ret = std::system(command.c_str());
    if (ret == 0) {
      return true;
    } else {
      return false;
    }
  }
  std::ofstream dst(to, std::ios::binary);
  if (!dst) {
    return false;
  }

  dst << src.rdbuf();
  return true;
}
/***********************************************************************************************/
bool FileHelper::copyDir(const std::string &from, const std::string &to) {
  DIR *directory = opendir(from.c_str());
  if (directory == nullptr) {
    return false;
  }
  bool ret = true;
  if (ensureDirectory(to)) {
    struct dirent *entry;
    while ((entry = readdir(directory)) != nullptr) {
      // skip directory_path/. and directory_path/..
      if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
        continue;
      }
      const std::string sub_path_from = from + "/" + entry->d_name;
      const std::string sub_path_to = to + "/" + entry->d_name;
      if (entry->d_type == DT_DIR) {
        ret &= copyDir(sub_path_from, sub_path_to);
      } else {
        ret &= copyFile(sub_path_from, sub_path_to);
      }
    }
  } else {
    ret = false;
  }
  closedir(directory);
  return ret;
}
/***********************************************************************************************/
bool FileHelper::copy(const std::string &from, const std::string &to) {
  return directoryExists(from) ? copyDir(from, to) : copyFile(from, to);
}
/***********************************************************************************************/
bool FileHelper::ensureDirectory(const std::string &directory_path) {
  std::string path = directory_path;
  for (size_t i = 1; i < directory_path.size(); ++i) {
    if (directory_path[i] == '/') {
      path[i] = 0;

      if (mkdir(path.c_str(), S_IRWXU) != 0) {
        if (errno != EEXIST) {
          return false;
        }
      }
      path[i] = '/';
    }
  }
  if (mkdir(path.c_str(), S_IRWXU) != 0) {
    if (errno != EEXIST) {
      return false;
    }
  }
  return true;
}
/***********************************************************************************************/
bool FileHelper::removeAllFiles(const std::string &directory_path) {
  DIR *directory = opendir(directory_path.c_str());
  if (directory == nullptr) {
    return false;
  }

  struct dirent *file;
  while ((file = readdir(directory)) != nullptr) {
    if (!strcmp(file->d_name, ".") || !strcmp(file->d_name, "..")) {
      continue;
    }
    std::string file_path = directory_path + "/" + file->d_name;
    if (unlink(file_path.c_str()) < 0) {
      closedir(directory);
      return false;
    }
  }
  closedir(directory);
  return true;
}
/***********************************************************************************************/
std::vector<std::string> FileHelper::listSubPaths(
    const std::string &directory_path, const unsigned char d_type) {
  std::vector<std::string> result;
  DIR *directory = opendir(directory_path.c_str());
  if (directory == nullptr) {
    return result;
  }

  struct dirent *entry;
  while ((entry = readdir(directory)) != nullptr) {
    if (entry->d_type == d_type && strcmp(entry->d_name, ".") != 0 &&
        strcmp(entry->d_name, "..") != 0) {
      result.emplace_back(entry->d_name);
    }
  }
  closedir(directory);
  return result;
}
/***********************************************************************************************/
std::string FileHelper::getFileName(const std::string &path,
                                    const bool remove_extension) {
  std::string::size_type start = path.rfind('/');
  if (start == std::string::npos) {
    start = 0;
  } else {
    ++start;
  }
  std::string::size_type end = std::string::npos;
  if (remove_extension) {
    end = path.rfind('.');
    if (end != std::string::npos && end < start) {
      end = std::string::npos;
    }
  }
  const auto len = (end != std::string::npos) ? end - start : end;
  return path.substr(start, len);
}
/***********************************************************************************************/
std::string FileHelper::getCurrentPath() {
  char tmp[PATH_MAX];
  return getcwd(tmp, sizeof(tmp)) ? std::string(tmp) : std::string("");
}
/***********************************************************************************************/
bool FileHelper::getType(const string &filename, FileType *type) {
  struct stat stat_buf;
  if (lstat(filename.c_str(), &stat_buf) != 0) {
    return false;
  }
  if (S_ISDIR(stat_buf.st_mode) != 0) {
    *type = TYPE_DIR;
  } else if (S_ISREG(stat_buf.st_mode) != 0) {
    *type = TYPE_FILE;
  } else {
    return false;
  }
  return true;
}
/***********************************************************************************************/
bool FileHelper::deleteFile(const string &filename) {
  if (!pathExists(filename)) {
    return true;
  }
  FileType type;
  if (!getType(filename, &type)) {
    return false;
  }
  if (type == TYPE_FILE) {
    if (remove(filename.c_str()) != 0) {
      return false;
    }
    return true;
  }
  DIR *dir = opendir(filename.c_str());
  if (dir == nullptr) {
    return false;
  }
  dirent *dir_info = nullptr;
  while ((dir_info = readdir(dir)) != nullptr) {
    if (strcmp(dir_info->d_name, ".") == 0 ||
        strcmp(dir_info->d_name, "..") == 0) {
      continue;
    }
    string temp_file = filename + "/" + string(dir_info->d_name);
    FileType temp_type;
    if (!getType(temp_file, &temp_type)) {
      closedir(dir);
      return false;
    }
    if (type == TYPE_DIR) {
      deleteFile(temp_file);
    }
    remove(temp_file.c_str());
  }
  closedir(dir);
  remove(filename.c_str());
  return true;
}
/***********************************************************************************************/
bool FileHelper::createDir(const string &dir) {
  int ret = mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
  return ret != 0 ? false : true;
}
/***********************************************************************************************/

