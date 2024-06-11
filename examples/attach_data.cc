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
// This example demonstrates how to attach data to a task and run
// the task iteratively with changing data.

 #include <dubhe/taskflow.h>

int main(){

  dubhe::Executor executor;
  dubhe::Taskflow taskflow("attach data to a task");

  int data;

  // create a task and attach it the data
  auto A = taskflow.placeholder();
  A.data(&data).work([A](){
    auto d = *static_cast<int*>(A.data());
    std::cout << "data is " << d << std::endl;
  });

  // run the taskflow iteratively with changing data
  for(data = 0; data<10; data++){
    executor.run(taskflow).wait();
  }

  return 0;
}
