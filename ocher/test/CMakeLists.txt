add_executable(JsonTest JsonTest.cpp)
add_test(JsonTest JsonTest)

add_executable(BufferTest BufferTest.cpp)
target_link_libraries(BufferTest ocherbook)
add_test(BufferTest BufferTest)

add_executable(SettingsTest SettingsTest.cpp)
target_link_libraries(SettingsTest ocherbook)
add_test(SettingsTest SettingsTest)

add_executable(OcherTest OcherTest.cpp)
target_link_libraries(OcherTest ocherbook)
add_test(OcherTest OcherTest)
