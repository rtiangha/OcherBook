add_executable(JsonTest JsonTest.cpp)
target_link_libraries(JsonTest catch json)
add_test(JsonTest JsonTest)

add_executable(BufferTest BufferTest.cpp)
target_link_libraries(BufferTest catch ocherbook)
add_test(BufferTest BufferTest)

add_executable(SettingsTest SettingsTest.cpp)
target_link_libraries(SettingsTest catch ocherbook)
add_test(SettingsTest SettingsTest)

add_executable(OcherTest OcherTest.cpp)
target_link_libraries(OcherTest catch ocherbook)
add_test(OcherTest OcherTest)
