ADD_PACS_EXECUTABLE(TARGET smallpt_thread_pool SOURCES smallpt_thread_pool.cpp)
ADD_PACS_EXECUTABLE(TARGET smallpt_overcommit SOURCES smallpt_overcommit.cpp)
ADD_PACS_EXECUTABLE(TARGET queueBench SOURCES queueBench.cpp)

target_include_directories(smallpt_thread_pool
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories(smallpt_overcommit
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories(queueBench
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")