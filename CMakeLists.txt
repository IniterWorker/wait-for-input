cmake_minimum_required(VERSION 3.0)
project(wait_for_input C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS}  " -03 -Wall -Werror -pedantic")
set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG}  " -O0 -ggdb")
set(CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE} " -O3 -Wall -Werror -pedantic")

include_directories(include)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules/")

set(SRC_FILES src/main.c
  src/wfi_init.c 
  src/wfi_gpio.c 
  src/fork_exec.c 
  src/wfi_access.c 
  src/wfi_poll.c 
  src/wfi_config.c 
  src/wfi_core.c)

add_executable(wait_for_input ${SRC_FILES})

find_package(CJSON REQUIRED)
if (CJSON_FOUND)
  include_directories(${CJSON_INCLUDE_DIRS})
  target_link_libraries (wait_for_input ${CJSON_LIBRARIES})
endif (CJSON_FOUND)