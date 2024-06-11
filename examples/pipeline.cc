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
// that propagates a series of integers and adds one to the result at each
// stage.
//
// The pipeline has the following structure:
//
// o -> o -> o
// |         |
// v         v
// o -> o -> o
// |         |
// v         v
// o -> o -> o
// |         |
// v         v
// o -> o -> o

#include <dubhe/taskflow.h>
#include <dubhe/algorithm/pipeline.h>

int main() {

    dubhe::Taskflow taskflow("pipeline");
    dubhe::Executor executor;

    const size_t num_lines = 4;

    // custom data storage
    std::array<size_t, num_lines> buffer;

    // the pipeline consists of three pipes (serial-parallel-serial)
    // and up to four concurrent scheduling tokens
    dubhe::Pipeline pl(num_lines,
                       dubhe::Pipe{dubhe::PipeType::SERIAL, [&buffer](dubhe::Pipeflow &pf) {
                           // generate only 5 scheduling tokens
                           if (pf.token() == 5) {
                               pf.stop();
                           }
                               // save the result of this pipe into the buffer
                           else {
                               printf("stage 1: input token = %zu\n", pf.token());
                               buffer[pf.line()] = pf.token();
                           }
                       }},

                       dubhe::Pipe{dubhe::PipeType::PARALLEL, [&buffer](dubhe::Pipeflow &pf) {
                           printf(
                                   "stage 2: input buffer[%zu] = %zu\n", pf.line(), buffer[pf.line()]
                           );
                           // propagate the previous result to this pipe and increment
                           // it by one
                           buffer[pf.line()] = buffer[pf.line()] + 1;
                       }},

                       dubhe::Pipe{dubhe::PipeType::SERIAL, [&buffer](dubhe::Pipeflow &pf) {
                           printf(
                                   "stage 3: input buffer[%zu] = %zu\n", pf.line(), buffer[pf.line()]
                           );
                           // propagate the previous result to this pipe and increment
                           // it by one
                           buffer[pf.line()] = buffer[pf.line()] + 1;
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
