#include "BlockerManager.h"
BlockerManager::BlockerManager() { this->Observe(); }
BlockerManager::~BlockerManager() { blockers_.clear(); }
void BlockerManager::Observe() {
  std::lock_guard<std::mutex> lock(blocker_mutex_);
  for (auto& item : blockers_) {
    item.second->Observe();
  }
}

void BlockerManager::Reset() {
  std::lock_guard<std::mutex> lock(blocker_mutex_);
  for (auto& item : blockers_) {
    item.second->Reset();
  }
  blockers_.clear();
}
