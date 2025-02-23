add_executable(SimpleUInt "SimpleUInt.cpp")

target_link_libraries(SimpleUInt PRIVATE HsBaGA)

target_link_directories(SimpleUInt PRIVATE ${CMAKE_SOURCE_DIR})