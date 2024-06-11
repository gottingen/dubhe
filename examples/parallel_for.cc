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
// This program demonstrates loop-based parallelism using:
//   + STL-styled iterators
//   + plain integral indices

#include <dubhe/taskflow.h>
#include <dubhe/algorithm/for_each.h>

// Procedure: for_each
void for_each(int N) {

    dubhe::Executor executor;
    dubhe::Taskflow taskflow;

    std::vector<int> range(N);
    std::iota(range.begin(), range.end(), 0);

    taskflow.for_each(range.begin(), range.end(), [&](int i) {
        printf("for_each on container item: %d\n", i);
    });

    executor.run(taskflow).get();

    taskflow.dump(std::cout);
}

// Procedure: for_each_index
void for_each_index(int N) {

    dubhe::Executor executor;
    dubhe::Taskflow taskflow;

    // [0, N) with step size 2
    taskflow.for_each_index(0, N, 2, [](int i) {
        printf("for_each_index on index: %d\n", i);
    });

    executor.run(taskflow).get();

    taskflow.dump(std::cout);
}

// ----------------------------------------------------------------------------

// Function: main
int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: ./parallel_for num_iterations" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    for_each(std::atoi(argv[1]));
    for_each_index(std::atoi(argv[1]));

    return 0;
}






