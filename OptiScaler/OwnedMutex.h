#pragma once

#include "SysUtils.h"

#include <atomic>
#include <mutex>
#include <chrono>

class OwnedMutex
{
  private:
    std::timed_mutex mtx;
    std::atomic<uint32_t> owner { 0 }; // don't use 0

  public:
    bool lock(uint32_t _owner, uint32_t timeout_ms = 100)
    {
        if (mtx.try_lock_for(std::chrono::milliseconds(timeout_ms)))
        {
            owner.store(_owner, std::memory_order_release);
            return true;
        }
        LOG_WARN("lock timeout! current_owner: {}, _owner: {}", owner.load(), _owner);
        return false;
    }

    // Only unlocks if owner matches
    void unlockThis(uint32_t _owner)
    {
        uint32_t current_owner = owner.load(std::memory_order_acquire);

        if (current_owner == 0 || current_owner != _owner)
        {
            LOG_WARN("current_owner: {}, _owner: {}", current_owner, _owner);
            return;
        }

        owner.store(0, std::memory_order_release);
        mtx.unlock();
    }

    uint32_t getOwner() { return owner.load(std::memory_order_seq_cst); }
};

class OwnedLockGuard
{
  private:
    OwnedMutex& _mutex;
    uint32_t _owner_id;
    bool _locked = false;

  public:
    OwnedLockGuard(OwnedMutex& mutex, uint32_t owner_id, uint32_t timeout_ms = 100) : _mutex(mutex), _owner_id(owner_id)
    {
        _locked = _mutex.lock(_owner_id, timeout_ms);
    }

    ~OwnedLockGuard()
    {
        if (_locked)
            _mutex.unlockThis(_owner_id);
    }

    bool isLocked() const { return _locked; }
};
