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
// A simple example with a semaphore constraint that only one task can
// execute at a time.

#include <dubhe/taskflow.h>

void sl() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main() {

    dubhe::Executor executor(4);
    dubhe::Taskflow taskflow;

    // define a critical region of 1 worker
    dubhe::Semaphore semaphore(1);

    // create give tasks in taskflow
    std::vector<dubhe::Task> tasks{
            taskflow.emplace([]() {
                sl();
                std::cout << "A" << std::endl;
            }),
            taskflow.emplace([]() {
                sl();
                std::cout << "B" << std::endl;
            }),
            taskflow.emplace([]() {
                sl();
                std::cout << "C" << std::endl;
            }),
            taskflow.emplace([]() {
                sl();
                std::cout << "D" << std::endl;
            }),
            taskflow.emplace([]() {
                sl();
                std::cout << "E" << std::endl;
            })
    };

    for (auto &task: tasks) {
        task.acquire(semaphore);
        task.release(semaphore);
    }

    executor.run(taskflow);
    executor.wait_for_all();

    return 0;
}

