// Copyright (c) 2024 Lazy_V
// SPDX-License-Identifier: MIT

#include "BackendInit.hpp"

namespace VRHI {

// Forward declare backend registration functions
#ifdef VRHI_BACKEND_OPENGL
namespace detail {
    extern void RegisterOpenGL33Backend();
}
#endif

void InitializeBackends() {
    // Call backend registration functions
    // This ensures the backend objects are linked and their static initializers run
    
#ifdef VRHI_BACKEND_OPENGL
    detail::RegisterOpenGL33Backend();
#endif
}

} // namespace VRHI
