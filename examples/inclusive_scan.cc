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
// This program demonstrates how to perform parallel inclusive scan.

#include <dubhe/taskflow.h>
#include <dubhe/algorithm/scan.h>

int main(int argc, char *argv[]) {

    if (argc != 3) {
        std::cerr << "usage: ./inclusive_scan num_workers num_elements\n";
        std::exit(EXIT_FAILURE);
    }

    size_t W = std::atoi(argv[1]);
    size_t N = std::atoi(argv[2]);

    dubhe::Executor executor(W);
    dubhe::Taskflow taskflow;

    std::vector<int> elements(N), scan_seq(N), scan_par(N);
    for (size_t i = 0; i < N; i++) {
        elements[i] = static_cast<int>(i);
    }

    // sequential inclusive scan
    {
        std::cout << "sequential inclusive scan ... ";
        auto beg = std::chrono::steady_clock::now();
        std::inclusive_scan(
                elements.begin(), elements.end(), scan_seq.begin(), std::multiplies<int>{}
        );
        auto end = std::chrono::steady_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - beg).count()
                  << "ns\n";
    }

    // create a parallel inclusive scan task
    {
        std::cout << "parallel   inclusive scan ... ";
        auto beg = std::chrono::steady_clock::now();
        taskflow.inclusive_scan(
                elements.begin(), elements.end(), scan_par.begin(), std::multiplies<int>{}
        );
        executor.run(taskflow).wait();
        auto end = std::chrono::steady_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - beg).count()
                  << "ns\n";
    }

    // verify the result
    for (size_t i = 0; i < N; i++) {
        if (scan_seq[i] != scan_par[i]) {
            printf(
                    "scan_seq[%zu]=%d..., scan_par[%zu]=%d...\n",
                    i, scan_seq[i], i, scan_par[i]
            );
            throw std::runtime_error("incorrect result");
        }
    }

    printf("correct result\n");

    return 0;
}
