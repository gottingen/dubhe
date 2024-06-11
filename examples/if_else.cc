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
// This program demonstrates how to create if-else control flow
// using condition tasks.
#include <dubhe/taskflow.h>

int main() {

    dubhe::Executor executor;
    dubhe::Taskflow taskflow;

    // create three static tasks and one condition task
    auto [init, cond, yes, no] = taskflow.emplace(
            []() {},
            []() { return 0; },
            []() { std::cout << "yes\n"; },
            []() { std::cout << "no\n"; }
    );

    init.name("init");
    cond.name("cond");
    yes.name("yes");
    no.name("no");

    cond.succeed(init);

    // With this order, when cond returns 0, execution
    // moves on to yes. When cond returns 1, execution
    // moves on to no.
    cond.precede(yes, no);

    // dump the conditioned flow
    taskflow.dump(std::cout);

    executor.run(taskflow).wait();

    return 0;
}

