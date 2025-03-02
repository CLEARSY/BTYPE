project(btype_library)

# Create library target
add_library(btype
    btype.cpp
    btype.h
    btype_factory.cpp
    btype_fmt.h
)

# Add threading support
target_link_libraries(btype 
    PRIVATE
        Threads::Threads
        fmt::fmt
)

