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
// This program demonstrates how to use dubhe::DataPipeline to create
// a pipeline with in-pipe data automatically managed by the Taskflow
// library.

#include <dubhe/taskflow.h>
#include <dubhe/algorithm/data_pipeline.h>

int main() {

    // dataflow => void -> int -> std::string -> float -> void
    dubhe::Taskflow taskflow("pipeline");
    dubhe::Executor executor;

    const size_t num_lines = 3;

    // create a pipeline graph
    dubhe::DataPipeline pl(num_lines,
                           dubhe::make_data_pipe<void, int>(dubhe::PipeType::SERIAL, [&](dubhe::Pipeflow &pf) {
                               if (pf.token() == 5) {
                                   pf.stop();
                                   return 0;
                               } else {
                                   printf("first pipe returns %zu\n", pf.token());
                                   return static_cast<int>(pf.token());
                               }
                           }),

                           dubhe::make_data_pipe<int, std::string>(dubhe::PipeType::SERIAL, [](int &input) {
                               printf("second pipe returns a strong of %d\n", input + 100);
                               return std::to_string(input + 100);
                           }),

                           dubhe::make_data_pipe<std::string, void>(dubhe::PipeType::SERIAL, [](std::string &input) {
                               printf("third pipe receives the input string %s\n", input.c_str());
                           })
    );

    // build the pipeline graph using composition
    taskflow.composed_of(pl).name("pipeline");

    // dump the pipeline graph structure (with composition)
    taskflow.dump(std::cout);

    // run the pipeline
    executor.run(taskflow).wait();

    return 0;
}

