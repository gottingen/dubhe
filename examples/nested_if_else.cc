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
// This program demonstrates how to create nested if-else control flow
// using condition tasks.
#include <dubhe/taskflow.h>

int main() {

    dubhe::Executor executor;
    dubhe::Taskflow taskflow;

    int i;

    // create three condition tasks for nested control flow
    auto initi = taskflow.emplace([&]() { i = 3; });
    auto cond1 = taskflow.emplace([&]() { return i > 1 ? 1 : 0; });
    auto cond2 = taskflow.emplace([&]() { return i > 2 ? 1 : 0; });
    auto cond3 = taskflow.emplace([&]() { return i > 3 ? 1 : 0; });
    auto equl1 = taskflow.emplace([&]() { std::cout << "i=1\n"; });
    auto equl2 = taskflow.emplace([&]() { std::cout << "i=2\n"; });
    auto equl3 = taskflow.emplace([&]() { std::cout << "i=3\n"; });
    auto grtr3 = taskflow.emplace([&]() { std::cout << "i>3\n"; });

    initi.precede(cond1);
    cond1.precede(equl1, cond2);
    cond2.precede(equl2, cond3);
    cond3.precede(equl3, grtr3);

    // dump the conditioned flow
    taskflow.dump(std::cout);

    executor.run(taskflow).wait();

    return 0;
}

