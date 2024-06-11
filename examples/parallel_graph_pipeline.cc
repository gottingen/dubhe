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
// This program demonstrates how to pipeline a sequence of linearly dependent
// tasks (stage function) over a directed acyclic graph.

#include <dubhe/taskflow.h>
#include <dubhe/algorithm/pipeline.h>

// 1st-stage function
void f1(const std::string &node) {
    printf("f1(%s)\n", node.c_str());
}

// 2nd-stage function
void f2(const std::string &node) {
    printf("f2(%s)\n", node.c_str());
}

// 3rd-stage function
void f3(const std::string &node) {
    printf("f3(%s)\n", node.c_str());
}

int main() {

    dubhe::Taskflow taskflow("graph processing pipeline");
    dubhe::Executor executor;

    const size_t num_lines = 2;

    // a topological order of the graph
    //    |-> B
    // A--|
    //    |-> C
    const std::vector<std::string> nodes = {"A", "B", "C"};

    // the pipeline consists of three serial pipes
    // and up to two concurrent scheduling tokens
    dubhe::Pipeline pl(num_lines,

            // first pipe calls f1
                       dubhe::Pipe{dubhe::PipeType::SERIAL, [&](dubhe::Pipeflow &pf) {
                           if (pf.token() == nodes.size()) {
                               pf.stop();
                           } else {
                               f1(nodes[pf.token()]);
                           }
                       }},

            // second pipe calls f2
                       dubhe::Pipe{dubhe::PipeType::SERIAL, [&](dubhe::Pipeflow &pf) {
                           f2(nodes[pf.token()]);
                       }},

            // third pipe calls f3
                       dubhe::Pipe{dubhe::PipeType::SERIAL, [&](dubhe::Pipeflow &pf) {
                           f3(nodes[pf.token()]);
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
