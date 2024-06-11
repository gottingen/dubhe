// Copyright (C) 2024 EA group inc.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

#if defined(_MSC_VER)
  #define DUBHE_FORCE_INLINE __forceinline
#elif defined(__GNUC__) && __GNUC__ > 3
  #define DUBHE_FORCE_INLINE __attribute__((__always_inline__)) inline
#else
  #define DUBHE_FORCE_INLINE inline
#endif

#if defined(_MSC_VER)
  #define DUBHE_NO_INLINE __declspec(noinline)
#elif defined(__GNUC__) && __GNUC__ > 3
  #define DUBHE_NO_INLINE __attribute__((__noinline__))
#else
  #define DUBHE_NO_INLINE
#endif

// ----------------------------------------------------------------------------

#ifdef DUBHE_DISABLE_EXCEPTION_HANDLING
  #define DUBHE_EXECUTOR_EXCEPTION_HANDLER(worker, node, code_block) \
    code_block;
#else
  #define DUBHE_EXECUTOR_EXCEPTION_HANDLER(worker, node, code_block)  \
    try {                                          \
      code_block;                                  \
    } catch(...) {                                 \
      _process_exception(worker, node);            \
    }
#endif

// ----------------------------------------------------------------------------    
