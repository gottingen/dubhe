#
# Copyright 2023 The titan-search Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
find_path(DOCTEST_INCLUDE_DIR doctest/doctest.h)
if (NOT DOCTEST_INCLUDE_DIR)
    message(FATAL_ERROR "doctest not found")
endif()
include_directories(${DOCTEST_INCLUDE_DIR})

list(APPEND UNITTESTS
        utility
        work_stealing
        priorities
        basics
        asyncs
        dependent_asyncs
        subflows
        control_flow
        semaphores
        movable
        cancellation
        for_each
        reduce
        transform
        sort
        scan
        find
        compositions
        traversals
        pipelines
        scalable_pipelines
        deferred_pipelines
        deferred_scalable_pipelines
        runtimes
        data_pipelines
)

foreach(unittest IN LISTS UNITTESTS)
    carbin_cc_test(
            NAME ${unittest}
            MODULE base
            SOURCES ${unittest}_test.cc
            CXXOPTS ${CARBIN_CXX_OPTIONS}
            LINKS ${CARBIN_DEPS_LINK}
    )
endforeach()

# include CUDA tests
if(DUBHE_BUILD_CUDA)
    add_subdirectory(${DUBHE_UTEST_DIR}/cuda)
endif()

# include SYCL tests
if(DUBHE_BUILD_SYCL)
    add_subdirectory(${DUBHE_UTEST_DIR}/sycl)
endif()



