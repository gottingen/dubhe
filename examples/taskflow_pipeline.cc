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
// This program demonstrates how to propagates a sequence of input tokens through
// linearly dependent taskflows to implement complex parallel algorithms.
// Parallelism exhibits both inside and outside these taskflows, combining
// both task graph parallelism and pipeline parallelism.

#include <dubhe/taskflow.h>
#include <dubhe/algorithm/pipeline.h>

// taskflow on the first pipe
void make_taskflow1(dubhe::Taskflow &tf) {
    auto [A1, B1, C1, D1] = tf.emplace(
            []() { printf("A1\n"); },
            []() { printf("B1\n"); },
            []() { printf("C1\n"); },
            []() { printf("D1\n"); }
    );
    A1.precede(B1, C1);
    D1.succeed(B1, C1);
}

// taskflow on the second pipe
void make_taskflow2(dubhe::Taskflow &tf) {
    auto [A2, B2, C2, D2] = tf.emplace(
            []() { printf("A2\n"); },
            []() { printf("B2\n"); },
            []() { printf("C2\n"); },
            []() { printf("D2\n"); }
    );
    tf.linearize({A2, B2, C2, D2});
}

// taskflow on the third pipe
void make_taskflow3(dubhe::Taskflow &tf) {
    auto [A3, B3, C3, D3] = tf.emplace(
            []() { printf("A3\n"); },
            []() { printf("B3\n"); },
            []() { printf("C3\n"); },
            []() { printf("D3\n"); }
    );
    A3.precede(B3, C3, D3);
}

int main() {

    dubhe::Taskflow taskflow("taskflow processing pipeline");
    dubhe::Executor executor;

    const size_t num_lines = 2;
    const size_t num_pipes = 3;

    // define the taskflow storage
    // we use the pipe dimension because we create three 'serial' pipes
    std::array<dubhe::Taskflow, num_pipes> taskflows;

    // create three different taskflows for the three pipes
    make_taskflow1(taskflows[0]);
    make_taskflow2(taskflows[1]);
    make_taskflow3(taskflows[2]);

    // the pipeline consists of three serial pipes
    // and up to two concurrent scheduling tokens
    dubhe::Pipeline pl(num_lines,

            // first pipe runs taskflow1
                       dubhe::Pipe{dubhe::PipeType::SERIAL, [&](dubhe::Pipeflow &pf) {
                           if (pf.token() == 5) {  // we only handle five scheduling tokens
                               pf.stop();
                               return;
                           }
                           printf("begin token %zu\n", pf.token());
                           executor.corun(taskflows[pf.pipe()]);
                       }},

            // second pipe runs taskflow2
                       dubhe::Pipe{dubhe::PipeType::SERIAL, [&](dubhe::Pipeflow &pf) {
                           executor.corun(taskflows[pf.pipe()]);
                       }},

            // third pipe calls taskflow3
                       dubhe::Pipe{dubhe::PipeType::SERIAL, [&](dubhe::Pipeflow &pf) {
                           executor.corun(taskflows[pf.pipe()]);
                       }}
    );

    // build the pipeline graph using composition
    dubhe::Task init = taskflow.emplace([]() { std::cout << "ready\n"; })
            .name("starting pipeline");
    dubhe::Task task = taskflow.composed_of(pl)
            .name("pipeline");
    dubhe::Task stop = taskflow.emplace([]() { std::cout << "stopped\n"; })
            .name("pipeline stopped");

    // create task dependency
    init.precede(task);
    task.precede(stop);

    // dump the pipeline graph structure (with composition)
    taskflow.dump(std::cout);

    // run the pipeline
    executor.run(taskflow).wait();

    return 0;
}
