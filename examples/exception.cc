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
// The program demonstrate how to capture an exception thrown
// from a running taskflow
#include <dubhe/taskflow.h>

int main() {

    dubhe::Executor executor;
    dubhe::Taskflow taskflow("exception");

    auto [A, B, C, D] = taskflow.emplace(
            []() { std::cout << "TaskA\n"; },
            []() {
                std::cout << "TaskB\n";
                throw std::runtime_error("Exception on Task B");
            },
            []() {
                std::cout << "TaskC\n";
                throw std::runtime_error("Exception on Task C");
            },
            []() { std::cout << "TaskD will not be printed due to exception\n"; }
    );

    A.precede(B, C);  // A runs before B and C
    D.succeed(B, C);  // D runs after  B and C

    try {
        executor.run(taskflow).get();
    }
    catch (const std::runtime_error &e) {
        // catched either TaskB's or TaskC's exception
        std::cout << e.what() << std::endl;
    }

    return 0;
}


