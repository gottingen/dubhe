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
// that describes a generalized token dependencies,
// propagates a series of integers, and adds one to the result at each
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

    dubhe::Taskflow taskflow("pipeline");
    dubhe::Executor executor;

    const size_t num_lines = 4;

    // define the pipe callable
    auto pipe_callable = [](dubhe::Pipeflow &pf) mutable {
        switch (pf.pipe()) {
            // first stage generates only 15 scheduling tokens
            // and describes the token dependencies
            case 0: {
                if (pf.token() == 15) {
                    pf.stop();
                } else {
                    if (pf.token() == 5) {
                        switch (pf.num_deferrals()) {
                            case 0:
                                pf.defer(2);
                                printf("1st-time: Token %zu is deferred by 2\n", pf.token());
                                pf.defer(7);
                                printf("1st-time: Token %zu is deferred by 7\n", pf.token());
                                break;

                            case 1:
                                pf.defer(9);
                                printf("snd-time: Token %zu is deferred by 9\n", pf.token());
                                break;

                            case 2:
                                printf("3rd-time: Tokens 2, 7 and 9 resolved dependencies for token %zu\n", pf.token());
                                break;
                        }
                    } else if (pf.token() == 2) {
                        switch (pf.num_deferrals()) {
                            case 0:
                                pf.defer(8);
                                printf("1st-time: Token %zu is deferred by 8\n", pf.token());
                                break;

                            case 1:
                                printf("2nd-time: Token 8 resolved dependencies for token %zu\n", pf.token());
                                break;
                        }
                    } else {
                        printf("stage 1: Non-deferred token %zu\n", pf.token());
                    }
                }
            }
                break;

                // other stages prints input token
            default: {
                printf("stage %zu : input token %zu (deferrals=%zu)\n",
                       pf.pipe() + 1, pf.token(), pf.num_deferrals());
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



