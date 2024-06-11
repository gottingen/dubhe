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
// The program demonstrates how to cancel a submitted taskflow
// graph and wait until the cancellation completes.

 #include <dubhe/taskflow.h>

int main() {

  dubhe::Executor executor;
  dubhe::Taskflow taskflow("cancel");

  // We create a taskflow graph of 1000 tasks each of 1 second.
  // Ideally, the taskflow completes in 1000/P seconds, where P
  // is the number of workers.
  for(int i=0; i<1000; i++) {
    taskflow.emplace([](){
      std::this_thread::sleep_for(std::chrono::seconds(1));
    });
  }

  // submit the taskflow
  auto beg = std::chrono::steady_clock::now();
  dubhe::Future fu = executor.run(taskflow);

  // submit a cancel request to cancel all 1000 tasks.
  fu.cancel();

  // wait until the cancellation finishes
  fu.get();
  auto end = std::chrono::steady_clock::now();

  // the duration should be much less than 1000 seconds
  std::cout << "taskflow completes in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end-beg).count()
            << " milliseconds\n";

  return 0;
}


