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
// This program demonstrates how to set priority to a task.
//
// Currently, Taskflow supports only three priority levels:
//   + dubhe::TaskPriority::HIGH   (numerical value = 0)
//   + dubhe::TaskPriority::NORMAL (numerical value = 1)
//   + dubhe::TaskPriority::LOW    (numerical value = 2)
// 
// Priority-based execution is non-preemptive. Once a task 
// has started to execute, it will execute to completion,
// even if a higher priority task has been spawned or enqueued. 

#include <dubhe/taskflow.h>

int main() {

    // create an executor of only one worker to enable
    // deterministic behavior
    dubhe::Executor executor(1);

    dubhe::Taskflow taskflow;

    int counter{0};

    // Here we create five tasks and print thier execution
    // orders which should align with assigned priorities
    auto [A, B, C, D, E] = taskflow.emplace(
            []() {},
            [&]() {
                std::cout << "Task B: " << counter++ << '\n';  // 0
            },
            [&]() {
                std::cout << "Task C: " << counter++ << '\n';  // 2
            },
            [&]() {
                std::cout << "Task D: " << counter++ << '\n';  // 1
            },
            []() {}
    );

    A.precede(B, C, D);
    E.succeed(B, C, D);

    // By default, all tasks are of dubhe::TaskPriority::HIGH
    B.priority(dubhe::TaskPriority::HIGH);
    C.priority(dubhe::TaskPriority::LOW);
    D.priority(dubhe::TaskPriority::NORMAL);

    assert(B.priority() == dubhe::TaskPriority::HIGH);
    assert(C.priority() == dubhe::TaskPriority::LOW);
    assert(D.priority() == dubhe::TaskPriority::NORMAL);

    // we should see B, D, and C in their priority order
    executor.run(taskflow).wait();
}

