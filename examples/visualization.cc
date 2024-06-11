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
// This example demonstrates how to use 'dump' method to visualize
// a taskflow graph in DOT format.

#include <dubhe/taskflow.h>

int main() {

    dubhe::Taskflow taskflow("Visualization Demo");

    // ------------------------------------------------------
    // Static Tasking
    // ------------------------------------------------------
    auto A = taskflow.emplace([]() { std::cout << "Task A\n"; });
    auto B = taskflow.emplace([]() { std::cout << "Task B\n"; });
    auto C = taskflow.emplace([]() { std::cout << "Task C\n"; });
    auto D = taskflow.emplace([]() { std::cout << "Task D\n"; });
    auto E = taskflow.emplace([]() { std::cout << "Task E\n"; });

    A.precede(B, C, E);
    C.precede(D);
    B.precede(D, E);

    std::cout << "[dump without name assignment]\n";
    taskflow.dump(std::cout);

    std::cout << "[dump with name assignment]\n";
    A.name("A");
    B.name("B");
    C.name("C");
    D.name("D");
    E.name("E");

    // if the graph contains solely static tasks, you can simpley dump them
    // without running the graph
    taskflow.dump(std::cout);

    // ------------------------------------------------------
    // Dynamic Tasking
    // ------------------------------------------------------
    taskflow.emplace([](dubhe::Subflow &sf) {
        sf.emplace([]() { std::cout << "subflow task1"; }).name("s1");
        sf.emplace([]() { std::cout << "subflow task2"; }).name("s2");
        sf.emplace([]() { std::cout << "subflow task3"; }).name("s3");
    });

    // in order to visualize subflow tasks, you need to run the taskflow
    // to spawn the dynamic tasks first
    dubhe::Executor executor;
    executor.run(taskflow).wait();

    taskflow.dump(std::cout);


    return 0;
}


