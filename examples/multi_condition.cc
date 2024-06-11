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
// This program demonstrates how to use multi-condition task
// to jump to multiple successor tasks
//
// A ----> B
//   |
//   |---> C
//   |
//   |---> D
//
#include <dubhe/taskflow.h>

int main() {

    dubhe::Executor executor;
    dubhe::Taskflow taskflow("Multi-Conditional Tasking Demo");

    auto A = taskflow.emplace([&]() -> dubhe::SmallVector<int> {
        std::cout << "A\n";
        return {0, 2};
    }).name("A");
    auto B = taskflow.emplace([&]() { std::cout << "B\n"; }).name("B");
    auto C = taskflow.emplace([&]() { std::cout << "C\n"; }).name("C");
    auto D = taskflow.emplace([&]() { std::cout << "D\n"; }).name("D");

    A.precede(B, C, D);

    // visualizes the taskflow
    taskflow.dump(std::cout);

    // executes the taskflow
    executor.run(taskflow).wait();

    return 0;
}

