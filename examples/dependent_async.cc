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
#include <dubhe/taskflow.h>  // the only include you need

int main() {

    dubhe::Executor executor;

    // demonstration of dependent async (with future)
    printf("Dependent Async\n");
    auto [A, fuA] = executor.dependent_async([]() { printf("A\n"); });
    auto [B, fuB] = executor.dependent_async([]() { printf("B\n"); }, A);
    auto [C, fuC] = executor.dependent_async([]() { printf("C\n"); }, A);
    auto [D, fuD] = executor.dependent_async([]() { printf("D\n"); }, B, C);

    fuD.get();

    // demonstration of silent dependent async (without future)
    printf("Silent Dependent Async\n");
    A = executor.silent_dependent_async([]() { printf("A\n"); });
    B = executor.silent_dependent_async([]() { printf("B\n"); }, A);
    C = executor.silent_dependent_async([]() { printf("C\n"); }, A);
    D = executor.silent_dependent_async([]() { printf("D\n"); }, B, C);

    executor.wait_for_all();

    return 0;
}




