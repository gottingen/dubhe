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
// The program demonstrates how to create asynchronous task
// from a running subflow.
#include <dubhe/taskflow.h>

int main() {

    dubhe::Taskflow taskflow("Subflow Async");
    dubhe::Executor executor;

    std::atomic<int> counter{0};

    taskflow.emplace([&](dubhe::Subflow &sf) {
        for (int i = 0; i < 10; i++) {
            // Here, we use "silent_async" instead of "async" because we do
            // not care the return value. The method "silent_async" gives us
            // less overhead compared to "async".
            // The 10 asynchronous tasks run concurrently.
            sf.silent_async([&]() {
                std::cout << "async task from the subflow\n";
                counter.fetch_add(1, std::memory_order_relaxed);
            });
        }
        sf.join();
        std::cout << counter << " = 10\n";
    });

    executor.run(taskflow).wait();

    return 0;
}

