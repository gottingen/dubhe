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

#[[
carbin_cc_binary(
        NAME hashes_example
        SOURCES hashes_example.cc
        DEPS tedis::tedis ${CARBIN_DEPS_LINK}
        COPTS ${USER_CXX_FLAGS}
)
]]
#[[
carbin_cc_binary(
        NAME foo_ex
        SOURCES foo_ex.cc
        DEPS changeme::foo ${CARBIN_DEPS_LINK}
        COPTS ${USER_CXX_FLAGS}
)
]]
list(APPEND DUBHE_EXAMPLES
        simple
        attach_data
        async
        subflow_async
        dependent_async
        dependent_async_algorithm
        observer
        subflow
        fibonacci
        condition
        multi_condition
        switch_case
        do_while_loop
        while_loop
        if_else
        nested_if_else
        priority
        visualization
        parallel_for
        parallel_sort
        reduce
        inclusive_scan
        exclusive_scan
        pipeline
        scalable_pipeline
        scalable_pipeline_with_deferred_tokens
        text_pipeline
        taskflow_pipeline
        parallel_graph_pipeline
        parallel_data_pipeline
        pipeline_with_deferred_tokens
        run
        corun
        runtime
        composition
        limited_concurrency
        cancel
        exception
)

foreach(example IN LISTS DUBHE_EXAMPLES)
    carbin_cc_binary(
            NAME ${example}_example
            SOURCES ${example}.cc
            LINKS  ${CARBIN_DEPS_LINK}
            CXXOPTS ${USER_CXX_FLAGS}
    )
endforeach()