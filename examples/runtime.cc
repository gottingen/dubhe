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
// This program demonstrates how to use a runtime task to forcefully
// schedule an active task that would never be scheduled.

#include <dubhe/taskflow.h>

int main() {

    dubhe::Taskflow taskflow("Runtime Tasking");
    dubhe::Executor executor;

    dubhe::Task A, B, C, D;

    std::tie(A, B, C, D) = taskflow.emplace(
            []() { return 0; },
            [&C](dubhe::Runtime &rt) {  // C must be captured by reference
                std::cout << "B\n";
                rt.schedule(C);
            },
            []() { std::cout << "C\n"; },
            []() { std::cout << "D\n"; }
    );

    // name tasks
    A.name("A");
    B.name("B");
    C.name("C");
    D.name("D");

    // create conditional dependencies
    A.precede(B, C, D);

    // dump the graph structure
    taskflow.dump(std::cout);

    // we will see both B and C in the output
    executor.run(taskflow).wait();

    return 0;
}
