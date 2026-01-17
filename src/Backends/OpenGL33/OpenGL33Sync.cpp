// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "OpenGL33Sync.hpp"

namespace VRHI {

OpenGL33Fence::OpenGL33Fence(bool signaled)
    : m_signaled(signaled)
{
}

bool OpenGL33Fence::Wait(uint64_t timeout) {
    // In OpenGL, fences are implemented via glFenceSync/glClientWaitSync
    // For now, we use a simple boolean flag
    return m_signaled;
}

void OpenGL33Fence::Reset() {
    m_signaled = false;
}

bool OpenGL33Fence::IsSignaled() const noexcept {
    return m_signaled;
}

} // namespace VRHI
