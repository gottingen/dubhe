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
//
//  This program demonstrates how to use dependent async tasks to create
//  dependent algorithm tasks.
//

 #include <dubhe/taskflow.h>
 #include <dubhe/algorithm/for_each.h>
 #include <dubhe/algorithm/transform.h>
 #include <dubhe/algorithm/reduce.h>

int main(){

  const size_t N = 65536;

  dubhe::Executor executor;
  
  int sum{1};
  std::vector<int> data(N);

  // for-each
  dubhe::AsyncTask A = executor.silent_dependent_async(dubhe::make_for_each_task(
    data.begin(), data.end(), [](int& i){ i = 1; }
  ));

  // transform
  dubhe::AsyncTask B = executor.silent_dependent_async(dubhe::make_transform_task(
    data.begin(), data.end(), data.begin(), [](int& i) { return i*2; }
  ), A);

  // reduce
  dubhe::AsyncTask C = executor.silent_dependent_async(dubhe::make_reduce_task(
    data.begin(), data.end(), sum, std::plus<int>{}
  ), B);
  
  // wait for all async task to complete
  executor.wait_for_all();
  
  // verify the result
  if(sum != N*2 + 1) {
    throw std::runtime_error("INCORRECT RESULT");
  }
  else {
    std::cout << "CORRECT RESULT\n";
  }
  
  return 0;
}




