// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#pragma once

#include <VRHI/Sync.hpp>

namespace VRHI {

class OpenGL33Fence : public Fence {
public:
    OpenGL33Fence(bool signaled);
    ~OpenGL33Fence() override = default;
    
    bool Wait(uint64_t timeout = UINT64_MAX) override;
    void Reset() override;
    bool IsSignaled() const noexcept override;
    
private:
    bool m_signaled;
};

class OpenGL33Semaphore : public Semaphore {
public:
    OpenGL33Semaphore() = default;
    ~OpenGL33Semaphore() override = default;
};

} // namespace VRHI
