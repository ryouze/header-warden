include(FetchContent)

function(fetch_and_link_external_dependencies target)
  if(NOT TARGET ${target})
    message(FATAL_ERROR "[ERROR] Target '${target}' does not exist. Cannot fetch and link dependencies.")
  endif()

  set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
  set(FETCHCONTENT_QUIET OFF)
  set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/deps")

  # SYSTEM is used to prevent applying compile flags to the dependencies
  FetchContent_Declare(
    argparse
    URL https://github.com/p-ranav/argparse/archive/refs/tags/v3.1.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    EXCLUDE_FROM_ALL
    SYSTEM
  )

  FetchContent_Declare(
    fmt
    URL https://github.com/fmtlib/fmt/releases/download/11.0.2/fmt-11.0.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    EXCLUDE_FROM_ALL
    SYSTEM
  )

  FetchContent_Declare(
    thread_pool
    URL https://github.com/bshoshany/thread-pool/archive/v4.1.0.zip
    EXCLUDE_FROM_ALL
    SYSTEM
  )

  # Make dependencies available
  FetchContent_MakeAvailable(argparse fmt thread_pool)

  # Create an INTERFACE library target for thread_pool
  add_library(thread_pool INTERFACE)
  target_include_directories(thread_pool INTERFACE ${thread_pool_SOURCE_DIR}/include)

  # Link dependencies to the target
  target_link_libraries(${target} PUBLIC argparse fmt::fmt thread_pool)

  message(STATUS "[INFO] Linked dependencies 'argparse', 'fmt', and 'thread_pool' to target '${target}'.")
endfunction()
