message(CHECK_START "Boost.Context")

find_package(Boost CONFIG COMPONENTS context)

if(Boost_FOUND)
  message(CHECK_PASS "found")
  set(MAIN)
  set(main_headers)
  add_executable(MAIN)
  target_sources(
    MAIN
      PRIVATE
        FILE_SET main_headers
        TYPE HEADERS
        BASE_DIRS
          "${CMAKE_SOURCE_DIR}/task-scheduler/include"
        FILES
          "${CMAKE_SOURCE_DIR}/task-scheduler/include/engine/task_scheduler/task_scheduler.hpp"
      PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
  )
  target_compile_features(
    MAIN
      PRIVATE
        cxx_std_23
      PUBLIC
        cxx_std_20
  )
  target_link_libraries(
    MAIN
      PRIVATE
        Boost::context
  )
  set_target_properties(
    MAIN
      PROPERTIES
        OUTPUT_NAME
          "usage$<$<PLATFORM_ID:Windows>:.exe>"
        RUNTIME_OUTPUT_DIRECTORY
          "${CMAKE_CURRENT_BINARY_DIR}/bin"
  )
else()
  message(CHECK_FAIL "not found")
endif()
