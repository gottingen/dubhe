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
// This program demonstrates how to create a pipeline scheduling framework
// that defers the exection of current scheuling token to the future.  
//
// The pipeline has the following structure:
//
// o -> o -> o
// |    |    |
// v    v    v
// o -> o -> o
// |    |    |
// v    v    v
// o -> o -> o
// |    |    |
// v    v    v
// o -> o -> o

// The scheduling token has the following dependencies:
//    ___________
//   |           |
//   V _____     |
//   |     |     | 
//   |     V     | 
// 1 2 3 4 5 6 7 8 9 10 
//         ^   |   |
//         |___|   |
//         ^       | 
//         |_______|
//
// 2 is deferred by 8
// 5 is dieferred by 2, 7, and 9


#include <dubhe/taskflow.h>
#include <dubhe/algorithm/pipeline.h>

int main() {

    dubhe::Taskflow taskflow("deferred_pipeline");
    dubhe::Executor executor;

    const size_t num_lines = 4;

    // the pipeline consists of three pipes (serial-parallel-serial)
    // and up to four concurrent scheduling tokens
    dubhe::Pipeline pl(num_lines,
                       dubhe::Pipe{dubhe::PipeType::SERIAL, [](dubhe::Pipeflow &pf) {
                           // generate only 15 scheduling tokens
                           if (pf.token() == 15) {
                               pf.stop();
                           } else {
                               // Token 5 is deferred
                               if (pf.token() == 5) {
                                   switch (pf.num_deferrals()) {
                                       case 0:
                                           pf.defer(2);
                                           printf("1st-time: Token %zu is deferred by 2\n", pf.token());
                                           pf.defer(7);
                                           printf("1st-time: Token %zu is deferred by 7\n", pf.token());
                                           return;
                                           break;

                                       case 1:
                                           pf.defer(9);
                                           printf("2nd-time: Token %zu is deferred by 9\n", pf.token());
                                           return;
                                           break;

                                       case 2:
                                           printf("3rd-time: Tokens 2, 7 and 9 resolved dependencies for token %zu\n",
                                                  pf.token());
                                           break;
                                   }
                               } else if (pf.token() == 2) {
                                   switch (pf.num_deferrals()) {
                                       case 0:
                                           pf.defer(8);
                                           printf("1st-time: Token %zu is deferred by 8\n", pf.token());
                                           break;
                                       case 1:
                                           printf("2nd-time: Token 8 resolved dependencies for token %zu\n",
                                                  pf.token());
                                           break;
                                   }
                               } else {
                                   printf("stage 1: Non-deferred token %zu\n", pf.token());
                               }
                           }
                       }},

                       dubhe::Pipe{dubhe::PipeType::SERIAL, [](dubhe::Pipeflow &pf) {
                           printf("stage 2: input token %zu (deferrals=%zu)\n", pf.token(), pf.num_deferrals());
                       }},

                       dubhe::Pipe{dubhe::PipeType::SERIAL, [](dubhe::Pipeflow &pf) {
                           printf("stage 3: input token %zu\n", pf.token());
                       }}
    );

    // build the pipeline graph using composition
    dubhe::Task init = taskflow.emplace([]() { std::cout << "ready\n"; })
            .name("starting pipeline");
    dubhe::Task task = taskflow.composed_of(pl)
            .name("deferred_pipeline");
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


