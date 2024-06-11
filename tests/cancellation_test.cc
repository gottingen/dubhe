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

// EmptyFuture
TEST_CASE("EmptyFuture" * doctest::timeout(300)) {
  dubhe::Future<void> fu;
  REQUIRE(fu.valid() == false);
  REQUIRE(fu.cancel() == false);
}

// Future
TEST_CASE("Future" * doctest::timeout(300)) {

  dubhe::Taskflow taskflow;
  dubhe::Executor executor(4);

  std::atomic<int> counter{0};

  for(int i=0; i<100; i++) {
    taskflow.emplace([&](){
      counter.fetch_add(1, std::memory_order_relaxed);
    });
  }

  auto fu = executor.run(taskflow);

  fu.get();

  REQUIRE(counter == 100);
}

// Cancel
TEST_CASE("BasicCancellation" * doctest::timeout(300)) {

  dubhe::Taskflow taskflow;
  dubhe::Executor executor(4);

  std::atomic<int> counter{0};

  // artificially long (possible larger than 300 seconds)
  for(int i=0; i<10000; i++) {
    taskflow.emplace([&](){
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      counter.fetch_add(1, std::memory_order_relaxed);
    });
  }

  // a new round
  counter = 0;
  auto fu = executor.run(taskflow);
  REQUIRE(fu.cancel() == true);
  fu.get();
  REQUIRE(counter < 10000);

  // a new round
  counter = 0;
  fu = executor.run_n(taskflow, 100);
  REQUIRE(fu.cancel() == true);
  fu.get();
  REQUIRE(counter < 10000);
}

// multiple cnacels
TEST_CASE("MultipleCancellations" * doctest::timeout(300)) {

  dubhe::Taskflow taskflow1, taskflow2, taskflow3, taskflow4;
  dubhe::Executor executor(4);

  std::atomic<int> counter{0};

  // artificially long (possible larger than 300 seconds)
  for(int i=0; i<10000; i++) {
    taskflow1.emplace([&](){
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      counter.fetch_add(1, std::memory_order_relaxed);
    });
    taskflow2.emplace([&](){
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      counter.fetch_add(1, std::memory_order_relaxed);
    });
    taskflow3.emplace([&](){
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      counter.fetch_add(1, std::memory_order_relaxed);
    });
    taskflow4.emplace([&](){
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      counter.fetch_add(1, std::memory_order_relaxed);
    });
  }

  // a new round
  counter = 0;
  auto fu1 = executor.run(taskflow1);
  auto fu2 = executor.run(taskflow2);
  auto fu3 = executor.run(taskflow3);
  auto fu4 = executor.run(taskflow4);
  REQUIRE(fu1.cancel() == true);
  REQUIRE(fu2.cancel() == true);
  REQUIRE(fu3.cancel() == true);
  REQUIRE(fu4.cancel() == true);
  executor.wait_for_all();
  REQUIRE(counter < 10000);
  REQUIRE(fu1.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready);
  REQUIRE(fu2.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready);
  REQUIRE(fu3.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready);
  REQUIRE(fu4.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready);
}

// Cancel linear chain
//TEST_CASE("CancelLinearChain" * doctest::timeout(300)) {
//
//  dubhe::Taskflow taskflow;
//  dubhe::Executor executor(4);
//  dubhe::Future<void>* future;
//
//  std::atomic<int> counter{0};
//  dubhe::Task prev, curr;
//
//  for(int i=0; i<10000; i++) {
//    curr = taskflow.emplace([&, i](){
//      counter.fetch_add(1, std::memory_order_relaxed);
//      if(i == 5000) {
//        future->cancel();
//      }
//    });
//    if(i) {
//      prev.precede(curr);
//    }
//    prev = curr;
//  }
//  
//  future = executor.run(taskflow);
//  future->wait();
//}

// cancel subflow
TEST_CASE("CancelSubflow" * doctest::timeout(300)) {

  dubhe::Taskflow taskflow;
  dubhe::Executor executor(4);

  std::atomic<int> counter{0};

  // artificially long (possible larger than 300 seconds)
  for(int i=0; i<100; i++) {
    taskflow.emplace([&, i](dubhe::Subflow& sf){
      for(int j=0; j<100; j++) {
        sf.emplace([&](){
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          counter.fetch_add(1, std::memory_order_relaxed);
        });
      }
      if(i % 2) {
        sf.join();
      }
      else {
        sf.detach();
      }
    });
  }

  // a new round
  counter = 0;
  auto fu = executor.run(taskflow);
  REQUIRE(fu.cancel() == true);
  fu.get();
  REQUIRE(counter < 10000);

  // a new round
  counter = 0;
  auto fu1 = executor.run(taskflow);
  auto fu2 = executor.run(taskflow);
  auto fu3 = executor.run(taskflow);
  REQUIRE(fu1.cancel() == true);
  REQUIRE(fu2.cancel() == true);
  REQUIRE(fu3.cancel() == true);
  fu1.get();
  fu2.get();
  fu3.get();
  REQUIRE(counter < 10000);
}

// cancel infinite loop
TEST_CASE("CancelInfiniteLoop" * doctest::timeout(300)) {

  dubhe::Taskflow taskflow;
  dubhe::Executor executor(4);

  for(int i=0; i<100; i++) {
    auto a = taskflow.emplace([](){});
    auto b = taskflow.emplace([](){ return 0; });
    a.precede(b);
    b.precede(b);
  }

  auto fu = executor.run(taskflow);
  REQUIRE(fu.cancel() == true);
  fu.get();
}

// cancel from another
TEST_CASE("CancelFromAnother" * doctest::timeout(300)) {

  dubhe::Taskflow taskflow, another;
  dubhe::Executor executor(4);

  // create a single inifnite loop
  auto a = taskflow.emplace([](){});
  auto b = taskflow.emplace([](){ return 0; });
  a.precede(b);
  b.precede(b);

  auto fu = executor.run(taskflow);

  REQUIRE(fu.wait_for(
    std::chrono::milliseconds(100)) == std::future_status::timeout
  );

  // create a task to cancel another flow
  another.emplace([&]() { REQUIRE(fu.cancel() == true); });

  executor.run(another).wait();
}

// cancel from async task
TEST_CASE("CancelFromAsync" * doctest::timeout(300)) {

  dubhe::Taskflow taskflow;
  dubhe::Executor executor(4);

  // create a single inifnite loop
  auto a = taskflow.emplace([](){});
  auto b = taskflow.emplace([&](){ return 0; });
  a.precede(b);
  b.precede(b);

  executor.async([&](){
    auto fu = executor.run_n(taskflow, 100);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    REQUIRE(fu.cancel() == true);
  });

  executor.wait_for_all();
}

// cancel composition tasks
TEST_CASE("CancelComposition") {

  dubhe::Executor executor(4);

  // f1 has two independent tasks
  dubhe::Taskflow f1("F1");
  auto f1A = f1.emplace([&](){ });
  auto f1B = f1.emplace([&](){ });
  f1A.name("f1A");
  f1B.name("f1B");

  //  f2A ---
  //         |----> f2C
  //  f2B ---
  //
  //  f1_module_task
  dubhe::Taskflow f2("F2");
  auto f2A = f2.emplace([&](){ });
  auto f2B = f2.emplace([&](){ });
  auto f2C = f2.emplace([&](){ });
  f2A.name("f2A");
  f2B.name("f2B");
  f2C.name("f2C");

  f2A.precede(f2C);
  f2B.precede(f2C);
  f2.composed_of(f1).name("module_of_f1");

  // f3 has a module task (f2) and a regular task
  dubhe::Taskflow f3("F3");
  f3.composed_of(f2).name("module_of_f2");
  f3.emplace([](){ }).name("f3A");

  // f4: f3_module_task -> f2_module_task
  dubhe::Taskflow f4;
  f4.name("F4");
  auto f3_module_task = f4.composed_of(f3).name("module_of_f3");
  auto f2_module_task = f4.composed_of(f2).name("module_of_f2");
  f3_module_task.precede(f2_module_task);

  for(int r=0; r<100; r++) {

    size_t N = 100;
    size_t success = 0;

    std::vector<dubhe::Future<void>> futures;

    for(int i=0; i<100; i++) {
      futures.emplace_back(executor.run(f4));
    }

    for(auto& fu: futures) {
      success += (fu.cancel() ? 1 : 0);
    }

    executor.wait_for_all();

    REQUIRE(success <= N);
  }
}

