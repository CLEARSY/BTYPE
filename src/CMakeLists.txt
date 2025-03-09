project(btype_library)

# Create library target
add_library(btype
    btype.cpp
    btype.h
    btype_factory.cpp
    btype_xml_writer.cpp
    btype_xml_reader.cpp
    btype_fmt.h
)

# Add threading support
target_link_libraries(btype 
    PRIVATE
        Threads::Threads
        fmt::fmt
        tinyxml2::tinyxml2
)

