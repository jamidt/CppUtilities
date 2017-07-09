set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(CATCH_MAIN OBJECT "testmain.cpp")

include_directories("3rdparty/Catch/include")

set(CPPUTIL_TEST_TARGET_NAME "cpputil_test")

set_target_properties(CATCH_MAIN PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(${CPPUTIL_TEST_TARGET_NAME}
    $<TARGET_OBJECTS:CATCH_MAIN>
    "test_tuple.cpp"
    "test_iterator.cpp"
    "test_type_traits.cpp"
    "test_algorithm.cpp"
)

set_target_properties(${CPPUTIL_TEST_TARGET_NAME} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(${CPPUTIL_TEST_TARGET_NAME} ${CPPUTIL_TARGET_NAME})
add_test(NAME "${CPPUTIL_TEST_TARGET_NAME}_default"
    COMMAND ${CPPUTIL_TEST_TARGET_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
