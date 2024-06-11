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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <dubhe/taskflow.h>
#include <dubhe/algorithm/pipeline.h>


// --------------------------------------------------------
// Testcase: Runtime.Schedule.ModuleTask
// --------------------------------------------------------

TEST_CASE("Runtime.Schedule.ModuleTask" * doctest::timeout(300)) {

  dubhe::Taskflow tf;
  int value {0};

  auto a = tf.emplace([&]() { value = -100; }).name("A");
  auto module_task = tf.placeholder().name("module");
  auto b = tf.emplace([&]() { value++; }).name("B");
  auto c = tf.emplace([&]() { value++; }).name("C");

  a.precede(module_task);
  module_task.precede(b);
  b.precede(c);

  dubhe::Taskflow module_flow;
  auto m1 = module_flow.emplace([&]() { value++; }).name("m1");
  auto m2 = module_flow.emplace([&]() { value++; }).name("m2");
  m1.precede(m2);

  module_task.composed_of(module_flow);

  auto entrypoint = tf.emplace([]() { return 0; }).name("entrypoint");
  auto schedule = tf.emplace([&](dubhe::Runtime& runtime) {
    value++;
    runtime.schedule(module_task);
  });
  entrypoint.precede(schedule, a);

  dubhe::Executor executor;
  executor.run(tf).wait();

  REQUIRE(value == 5);
}

// --------------------------------------------------------
// Testcase: Runtime.ExternalGraph.Simple
// --------------------------------------------------------

TEST_CASE("Runtime.ExternalGraph.Simple" * doctest::timeout(300)) {

  const size_t N = 100;

  dubhe::Executor executor;
  dubhe::Taskflow taskflow;
  
  std::vector<int> results(N, 0);
  std::vector<dubhe::Taskflow> graphs(N);

  for(size_t i=0; i<N; i++) {

    auto& fb = graphs[i];

    auto A = fb.emplace([&res=results[i]]()mutable{ ++res; });
    auto B = fb.emplace([&res=results[i]]()mutable{ ++res; });
    auto C = fb.emplace([&res=results[i]]()mutable{ ++res; });
    auto D = fb.emplace([&res=results[i]]()mutable{ ++res; });

    A.precede(B);
    B.precede(C);
    C.precede(D);

    taskflow.emplace([&res=results[i], &graph=graphs[i]](dubhe::Runtime& rt)mutable{
      rt.corun(graph);
    });
  }
  
  executor.run_n(taskflow, 100).wait();

  for(size_t i=0; i<N; i++) {
    REQUIRE(results[i] == 400);
  }

}

