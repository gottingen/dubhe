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
#include <dubhe/taskflow.h>

int spawn(int n, dubhe::Subflow &sbf) {
    if (n < 2) return n;
    int res1, res2;

    // compute f(n-1)
    sbf.emplace([&res1, n](dubhe::Subflow &sbf_n_1) { res1 = spawn(n - 1, sbf_n_1); })
            .name(std::to_string(n - 1));

    // compute f(n-2)
    sbf.emplace([&res2, n](dubhe::Subflow &sbf_n_2) { res2 = spawn(n - 2, sbf_n_2); })
            .name(std::to_string(n - 2));

    sbf.join();
    return res1 + res2;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cerr << "usage: ./fibonacci N\n";
        std::exit(EXIT_FAILURE);
    }

    int N = std::atoi(argv[1]);

    if (N < 0) {
        throw std::runtime_error("N must be non-negative");
    }

    int res;  // result

    dubhe::Executor executor;
    dubhe::Taskflow taskflow("fibonacci");

    taskflow.emplace([&res, N](dubhe::Subflow &sbf) {
        res = spawn(N, sbf);
    }).name(std::to_string(N));

    executor.run(taskflow).wait();

    //taskflow.dump(std::cout);

    std::cout << "Fib[" << N << "]: " << res << std::endl;

    return 0;
}









