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

#include <dubhe/core/task.h>

/**
@file critical.hpp
@brief critical include file
*/

namespace dubhe {

    // ----------------------------------------------------------------------------
    // CriticalSection
    // ----------------------------------------------------------------------------

    /**
    @class CriticalSection

    @brief class to create a critical region of limited workers to run tasks

    dubhe::CriticalSection is a wrapper over dubhe::Semaphore and is specialized for
    limiting the maximum concurrency over a set of tasks.
    A critical section starts with an initial count representing that limit.
    When a task is added to the critical section,
    the task acquires and releases the semaphore internal to the critical section.
    This design avoids explicit call of dubhe::Task::acquire and dubhe::Task::release.
    The following example creates a critical section of one worker and adds
    the five tasks to the critical section.

    @code{.cpp}
    dubhe::Executor executor(8);   // create an executor of 8 workers
    dubhe::Taskflow taskflow;

    // create a critical section of 1 worker
    dubhe::CriticalSection critical_section(1);

    dubhe::Task A = taskflow.emplace([](){ std::cout << "A" << std::endl; });
    dubhe::Task B = taskflow.emplace([](){ std::cout << "B" << std::endl; });
    dubhe::Task C = taskflow.emplace([](){ std::cout << "C" << std::endl; });
    dubhe::Task D = taskflow.emplace([](){ std::cout << "D" << std::endl; });
    dubhe::Task E = taskflow.emplace([](){ std::cout << "E" << std::endl; });

    critical_section.add(A, B, C, D, E);

    executor.run(taskflow).wait();
    @endcode

    */
    class CriticalSection : public Semaphore {

      public:

        /**
        @brief constructs a critical region of a limited number of workers
        */
        explicit CriticalSection(size_t max_workers = 1);

        /**
        @brief adds a task into the critical region
        */
        template <typename... Tasks>
        void add(Tasks...tasks);
    };

    inline CriticalSection::CriticalSection(size_t max_workers) :
      Semaphore {max_workers} {
    }

    template <typename... Tasks>
    void CriticalSection::add(Tasks... tasks) {
      (tasks.acquire(*this), ...);
      (tasks.release(*this), ...);
    }


}  // namespace dubhe


