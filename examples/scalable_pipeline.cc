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
// stage, using a range of pipes provided by the application.
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
//
// Then, the program resets the pipeline to a new range of five pipes.
//
// o -> o -> o -> o -> o
// |    |    |    |    |
// v    v    v    v    v
// o -> o -> o -> o -> o
// |    |    |    |    |
// v    v    v    v    v
// o -> o -> o -> o -> o
// |    |    |    |    |
// v    v    v    v    v
// o -> o -> o -> o -> o

#include <dubhe/taskflow.h>
#include <dubhe/algorithm/pipeline.h>

int main() {

    dubhe::Taskflow taskflow("pipeline");
    dubhe::Executor executor;

    const size_t num_lines = 4;

    // create data storage
    std::array<size_t, num_lines> buffer;

    // define the pipe callable
    auto pipe_callable = [&buffer](dubhe::Pipeflow &pf) mutable {
        switch (pf.pipe()) {
            // first stage generates only 5 scheduling tokens and saves the
            // token number into the buffer.
            case 0: {
                if (pf.token() == 5) {
                    pf.stop();
                } else {
                    printf("stage 1: input token = %zu\n", pf.token());
                    buffer[pf.line()] = pf.token();
                }
                return;
            }
                break;

                // other stages propagate the previous result to this pipe and
                // increment it by one
            default: {
                printf(
                        "stage %zu: input buffer[%zu] = %zu\n", pf.pipe(), pf.line(), buffer[pf.line()]
                );
                buffer[pf.line()] = buffer[pf.line()] + 1;
            }
                break;
        }
    };

    // create a vector of three pipes
    std::vector<dubhe::Pipe<std::function<void(dubhe::Pipeflow &)>>> pipes;

    for (size_t i = 0; i < 3; i++) {
        pipes.emplace_back(dubhe::PipeType::SERIAL, pipe_callable);
    }

    // create a pipeline of four parallel lines using the given vector of pipes
    dubhe::ScalablePipeline<decltype(pipes)::iterator> pl(num_lines, pipes.begin(), pipes.end());

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

    // reset the pipeline to a new range of five pipes and starts from
    // the initial state (i.e., token counts from zero)
    for (size_t i = 0; i < 2; i++) {
        pipes.emplace_back(dubhe::PipeType::SERIAL, pipe_callable);
    }
    pl.reset(pipes.begin(), pipes.end());

    executor.run(taskflow).wait();

    return 0;
}



