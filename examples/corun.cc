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
// This example demonstrates how to use the corun
// method in the executor.
 #include <dubhe/taskflow.h>

int main(){
  
  const size_t N = 100;
  const size_t T = 1000;
  
  // create an executor and a taskflow
  dubhe::Executor executor(2);
  dubhe::Taskflow taskflow;

  std::array<dubhe::Taskflow, N> taskflows;

  std::atomic<size_t> counter{0};
  
  for(size_t n=0; n<N; n++) {
    for(size_t i=0; i<T; i++) {
      taskflows[n].emplace([&](){ counter++; });
    }
    taskflow.emplace([&executor, &tf=taskflows[n]](){
      executor.corun(tf);
      //executor.run(tf).wait();  <-- can result in deadlock
    });
  }

  executor.run(taskflow).wait();

  return 0;
}
