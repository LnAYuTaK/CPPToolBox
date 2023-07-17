/**
 * @file BlockerManager.h
 * @author LnAYuTaK (807874484@qq.com)
 * @brief  事件注册发布管理器
 * @version 0.1
 * @date 2023-06-26
 * @copyright Copyright (c) 2023
 */
#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include "Blocker.h"
#include "MacroDef.h"

class BlockerManager {
  DECLARE_SINGLETON(BlockerManager)
  using BlockerMap =
      std::unordered_map<std::string, std::shared_ptr<BlockerBase>>;

 public:
  virtual ~BlockerManager();
  //发布消息
  template <typename T>
  bool Publish(const std::string& channel_name,
               const typename Blocker<T>::MessagePtr& msg);

  template <typename T>
  bool Publish(const std::string& channel_name,
               const typename Blocker<T>::MessageType& msg);
  //订阅消息
  template <typename T>
  bool Subscribe(const std::string& channel_name, size_t capacity,
                 const std::string& callback_id,
                 const typename Blocker<T>::Callback& callback);

  template <typename T>
  bool Unsubscribe(const std::string& channel_name,
                   const std::string& callback_id);

  template <typename T>
  std::shared_ptr<Blocker<T>> GetBlocker(const std::string& channel_name);

  template <typename T>
  std::shared_ptr<Blocker<T>> GetOrCreateBlocker(const BlockerAttr& attr);

  void Observe();
  void Reset();

 private:
  BlockerMap blockers_;
  std::mutex blocker_mutex_;
};

template <typename T>
bool BlockerManager::Publish(const std::string& channel_name,
                             const typename Blocker<T>::MessagePtr& msg) {
  auto blocker = GetOrCreateBlocker<T>(BlockerAttr(channel_name));
  if (blocker == nullptr) {
    return false;
  }
  blocker->Publish(msg);
  return true;
}

template <typename T>
bool BlockerManager::Publish(const std::string& channel_name,
                             const typename Blocker<T>::MessageType& msg) {
  auto blocker = GetOrCreateBlocker<T>(BlockerAttr(channel_name));
  if (blocker == nullptr) {
    return false;
  }
  blocker->Publish(msg);
  return true;
}

template <typename T>
bool BlockerManager::Subscribe(const std::string& channel_name, size_t capacity,
                               const std::string& callback_id,
                               const typename Blocker<T>::Callback& callback) {
  auto blocker = GetOrCreateBlocker<T>(BlockerAttr(capacity, channel_name));
  if (blocker == nullptr) {
    return false;
  }
  return blocker->Subscribe(callback_id, callback);
}

template <typename T>
bool BlockerManager::Unsubscribe(const std::string& channel_name,
                                 const std::string& callback_id) {
  auto blocker = GetBlocker<T>(channel_name);
  if (blocker == nullptr) {
    return false;
  }
  return blocker->Unsubscribe(callback_id);
}

template <typename T>
std::shared_ptr<Blocker<T>> BlockerManager::GetBlocker(
    const std::string& channel_name) {
  std::shared_ptr<Blocker<T>> blocker = nullptr;
  {
    std::lock_guard<std::mutex> lock(blocker_mutex_);
    auto search = blockers_.find(channel_name);
    if (search != blockers_.end()) {
      blocker = std::dynamic_pointer_cast<Blocker<T>>(search->second);
    }
  }
  return blocker;
}

template <typename T>
std::shared_ptr<Blocker<T>> BlockerManager::GetOrCreateBlocker(
    const BlockerAttr& attr) {
  std::shared_ptr<Blocker<T>> blocker = nullptr;
  {
    std::lock_guard<std::mutex> lock(blocker_mutex_);
    auto search = blockers_.find(attr.channel_name);
    if (search != blockers_.end()) {
      blocker = std::dynamic_pointer_cast<Blocker<T>>(search->second);
    } else {
      blocker = std::make_shared<Blocker<T>>(attr);
      blockers_[attr.channel_name] = blocker;
    }
  }
  return blocker;
}
