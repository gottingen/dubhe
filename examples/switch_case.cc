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
// This program demonstrates how to implement switch-case control flow
// using condition tasks.
#include <dubhe/taskflow.h>

int main() {

    dubhe::Executor executor;
    dubhe::Taskflow taskflow;

    auto [source, swcond, case1, case2, case3, target] = taskflow.emplace(
            []() { std::cout << "source\n"; },
            []() {
                std::cout << "switch\n";
                return rand() % 3;
            },
            []() {
                std::cout << "case 1\n";
                return 0;
            },
            []() {
                std::cout << "case 2\n";
                return 0;
            },
            []() {
                std::cout << "case 3\n";
                return 0;
            },
            []() { std::cout << "target\n"; }
    );

    source.precede(swcond);
    swcond.precede(case1, case2, case3);
    target.succeed(case1, case2, case3);

    executor.run(taskflow).wait();

    return 0;
}
