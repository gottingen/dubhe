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
// This program demonstrates how to implement while-loop control flow
// using condition tasks.
#include <dubhe/taskflow.h>

int main() {

    dubhe::Executor executor;
    dubhe::Taskflow taskflow;

    int i;

    auto [init, cond, body, back, done] = taskflow.emplace(
            [&]() {
                std::cout << "i=0\n";
                i = 0;
            },
            [&]() {
                std::cout << "while i<5\n";
                return i < 5 ? 0 : 1;
            },
            [&]() { std::cout << "i++=" << i++ << '\n'; },
            [&]() {
                std::cout << "back\n";
                return 0;
            },
            [&]() { std::cout << "done\n"; }
    );

    init.name("init");
    cond.name("while i<5");
    body.name("i++");
    back.name("back");
    done.name("done");

    init.precede(cond);
    cond.precede(body, done);
    body.precede(back);
    back.precede(cond);

    taskflow.dump(std::cout);

    executor.run(taskflow).wait();

    return 0;
}

