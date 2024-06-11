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

TEST_CASE("SimplePriority.Sequential" * doctest::timeout(300)) {
  
  dubhe::Executor executor(1);
  dubhe::Taskflow taskflow;

  int counter = 0;

  auto [A, B, C, D, E] = taskflow.emplace(
    [&] () { counter = 0; },
    [&] () { REQUIRE(counter == 0); counter++; },
    [&] () { REQUIRE(counter == 2); counter++; },
    [&] () { REQUIRE(counter == 1); counter++; },
    [&] () { }
  );

  A.precede(B, C, D); 
  E.succeed(B, C, D);
  
  REQUIRE(B.priority() == dubhe::TaskPriority::HIGH);
  REQUIRE(C.priority() == dubhe::TaskPriority::HIGH);
  REQUIRE(D.priority() == dubhe::TaskPriority::HIGH);

  B.priority(dubhe::TaskPriority::HIGH);
  C.priority(dubhe::TaskPriority::LOW);
  D.priority(dubhe::TaskPriority::NORMAL);

  REQUIRE(B.priority() == dubhe::TaskPriority::HIGH);
  REQUIRE(C.priority() == dubhe::TaskPriority::LOW);
  REQUIRE(D.priority() == dubhe::TaskPriority::NORMAL);

  executor.run_n(taskflow, 100).wait();
}

TEST_CASE("RandomPriority.Sequential" * doctest::timeout(300)) {
  
  dubhe::Executor executor(1);
  dubhe::Taskflow taskflow;

  const auto MAX_P = static_cast<unsigned>(dubhe::TaskPriority::MAX);

  auto beg = taskflow.emplace([](){});
  auto end = taskflow.emplace([](){});

  size_t counters[MAX_P];
  size_t priorities[MAX_P];

  for(unsigned p=0; p<MAX_P; p++) {
    counters[p] = 0;
    priorities[p] = 0;
  }

  for(size_t i=0; i<10000; i++) {
    unsigned p = ::rand() % MAX_P;
    taskflow.emplace([p, &counters](){ counters[p]++; })
            .priority(static_cast<dubhe::TaskPriority>(p))
            .succeed(beg)
            .precede(end);
    priorities[p]++;
  }

  executor.run(taskflow).wait();

  for(unsigned p=0; p<MAX_P; p++) {
    REQUIRE(priorities[p] == counters[p]);
  }

}

TEST_CASE("RandomPriority.Parallel" * doctest::timeout(300)) {
  
  dubhe::Executor executor;
  dubhe::Taskflow taskflow;

  const auto MAX_P = static_cast<unsigned>(dubhe::TaskPriority::MAX);

  auto beg = taskflow.emplace([](){});
  auto end = taskflow.emplace([](){});

  std::atomic<size_t> counters[MAX_P];
  size_t priorities[MAX_P];

  for(unsigned p=0; p<MAX_P; p++) {
    counters[p] = 0;
    priorities[p] = 0;
  }

  for(size_t i=0; i<10000; i++) {
    unsigned p = ::rand() % MAX_P;
    taskflow.emplace([p, &counters](){ counters[p]++; })
            .priority(static_cast<dubhe::TaskPriority>(p))
            .succeed(beg)
            .precede(end);
    priorities[p]++;
  }

  executor.run_n(taskflow, 2).wait();

  for(unsigned p=0; p<MAX_P; p++) {
    REQUIRE(counters[p]!=0);
    //std::cout << priorities[p] << ' ' << counters[p] << '\n';
  }

}





