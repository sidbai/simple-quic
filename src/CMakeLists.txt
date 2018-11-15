cmake_minimum_required (VERSION 3.0)

# Set project name
project (simple_quic_src)

add_executable(simple_quic
    main.cpp
)

install(TARGETS simple_quic DESTINATION bin)
