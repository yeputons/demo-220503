add_executable(server-cpp-sync main.cpp)

find_package(SQLite3)
target_include_directories(server-cpp-sync PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(server-cpp-sync ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${NETWORKING_LIBS} SQLite::SQLite3 SQLiteCpp)
