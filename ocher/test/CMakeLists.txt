add_executable(BufferTest BufferTest.cpp)
target_link_libraries(BufferTest ocherbook)
add_test(BufferTest BufferTest)

add_executable(OcherTest OcherTest.cpp)
target_link_libraries(OcherTest ocherbook)
add_test(OcherTest OcherTest)

