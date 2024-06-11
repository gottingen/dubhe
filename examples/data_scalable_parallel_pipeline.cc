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

    //1. How can I put a placeholder in the first pipe, i.e. [] (void, dubhe::Pipeflow&) in order to match the pipe vector?
    auto pipe_callable1 = [] (dubhe::Pipeflow& pf) mutable -> int {
        if(pf.token() == 5) {
          pf.stop();
          return 0;
        }
        else {
          printf("stage 1: input token = %zu\n", pf.token());
          return pf.token();
        }
    };
    auto pipe_callable2 = [] (int input, dubhe::Pipeflow& pf) mutable -> float {
        return input + 1.0;
    };
    auto pipe_callable3 = [] (float input, dubhe::Pipeflow& pf) mutable -> int {
        return input + 1;
    };

  //2. Is this ok when the type in vector definition is different from the exact types of emplaced elements?
  std::vector< ScalableDataPipeBase* > pipes;

  pipes.emplace_back(dubhe::make_scalable_datapipe<void, int>(dubhe::PipeType::SERIAL, pipe_callable1));
  pipes.emplace_back(dubhe::make_scalable_datapipe<int, float>(dubhe::PipeType::SERIAL, pipe_callable2));
  pipes.emplace_back(dubhe::make_scalable_datapipe<float, int>(dubhe::PipeType::SERIAL, pipe_callable3));

  // create a pipeline of four parallel lines using the given vector of pipes
  dubhe::ScalablePipeline<decltype(pipes)::iterator> pl(num_lines, pipes.begin(), pipes.end());

  // build the pipeline graph using composition
  dubhe::Task init = taskflow.emplace([](){ std::cout << "ready\n"; })
                          .name("starting pipeline");
  dubhe::Task task = taskflow.composed_of(pl)
                          .name("pipeline");
  dubhe::Task stop = taskflow.emplace([](){ std::cout << "stopped\n"; })
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
  pipes.emplace_back(dubhe::make_scalable_datapipe<int, float>(dubhe::PipeType::SERIAL, pipe_callable1));
  pipes.emplace_back(dubhe::make_scalable_datapipe<float, int>(dubhe::PipeType::SERIAL, pipe_callable1));
  pl.reset(pipes.begin(), pipes.end());

  executor.run(taskflow).wait();

  return 0;
}



