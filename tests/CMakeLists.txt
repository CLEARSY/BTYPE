find_package(GTest REQUIRED)

add_executable(btype_tests
    btype_tests.cpp
)

target_include_directories(btype_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(btype_tests
    PRIVATE
        btype
        GTest::GTest
        GTest::Main
        Threads::Threads
        fmt::fmt
)

add_test(NAME btype_tests COMMAND btype_tests)

add_executable(btype_formatter_tests
    btype_formatter_tests.cpp
)

target_include_directories(btype_formatter_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(btype_formatter_tests
    PRIVATE
        btype
        GTest::GTest
        GTest::Main
        Threads::Threads
        fmt::fmt
)

add_test(NAME btype_formatter_tests COMMAND btype_formatter_tests)

add_executable(btype_xml_writer_tests
    btype_xml_writer_tests.cpp
)
target_include_directories(btype_xml_writer_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(btype_xml_writer_tests
    PRIVATE
        btype
        GTest::GTest
        GTest::Main
        Threads::Threads
        fmt::fmt
)
add_test(NAME btype_xml_writer_tests COMMAND btype_xml_writer_tests)

add_executable(btype_xml_reader_tests
    btype_xml_reader_tests.cpp
)
target_include_directories(btype_xml_reader_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(btype_xml_reader_tests
    PRIVATE
        btype
        GTest::GTest
        GTest::Main
        Threads::Threads
        fmt::fmt
)
add_test(NAME btype_xml_reader_tests COMMAND btype_xml_reader_tests)

add_executable(btype_index_tests
    btype_index_tests.cpp
)

target_include_directories(btype_index_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(btype_index_tests
    PRIVATE
        btype
        GTest::GTest
        GTest::Main
        Threads::Threads
        fmt::fmt
)

add_test(NAME btype_index_tests COMMAND btype_index_tests)
