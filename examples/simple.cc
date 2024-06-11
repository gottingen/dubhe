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
// A simple example to capture the following task dependencies.
//
//           +---+
//     +---->| B |-----+
//     |     +---+     |
//   +---+           +-v-+
//   | A |           | D |
//   +---+           +-^-+
//     |     +---+     |
//     +---->| C |-----+
//           +---+
//
#include <dubhe/taskflow.h>  // the only include you need

int main() {

    dubhe::Executor executor;
    dubhe::Taskflow taskflow("simple");

    auto [A, B, C, D] = taskflow.emplace(
            []() { std::cout << "TaskA\n"; },
            []() { std::cout << "TaskB\n"; },
            []() { std::cout << "TaskC\n"; },
            []() { std::cout << "TaskD\n"; }
    );

    A.precede(B, C);  // A runs before B and C
    D.succeed(B, C);  // D runs after  B and C

    executor.run(taskflow).wait();

    return 0;
}
