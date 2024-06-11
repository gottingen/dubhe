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

// The program demonstrates how to create asynchronous task
// from an executor and a subflow.
 #include <dubhe/taskflow.h>

int main() {

  dubhe::Executor executor;

  // create asynchronous tasks from the executor
  // (using executor as a thread pool)
  std::future<int> fu = executor.async([](){
    std::cout << "async task 1 returns 1\n";
    return 1;
  });

  executor.silent_async([](){  // silent async task doesn't return any future object
    std::cout << "async task 2 does not return (silent)\n";
  });

  executor.wait_for_all();  // wait for the two async tasks to finish

  // create asynchronous tasks from a subflow
  // all asynchronous tasks are guaranteed to finish when the subflow joins
  dubhe::Taskflow taskflow;

  std::atomic<int> counter {0};

  taskflow.emplace([&](dubhe::Subflow& sf){
    for(int i=0; i<100; i++) {
      sf.silent_async([&](){ counter.fetch_add(1, std::memory_order_relaxed); });
    }
    sf.join();

    // when subflow joins, all spawned tasks from the subflow will finish
    if(counter == 100) {
      std::cout << "async tasks spawned from the subflow all finish\n";
    }
    else {
      throw std::runtime_error("this should not happen");
    }
  });

  executor.run(taskflow).wait();

  return 0;
}








