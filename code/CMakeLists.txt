file(GLOB_RECURSE SOURCES 
    "main.cpp"
    "./*.cpp"
)

# 收集头文件
file(GLOB_RECURSE HEADERS
    "include/*.h"
)
set(RESOURCES
    "../resource.qrc"
)



add_executable(MaodieAdventure 
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

target_include_directories(MaodieAdventure PRIVATE
    include
    # include/app
    include/view
    # include/viewmodel
)

target_precompile_headers(MaodieAdventure PRIVATE
    precomp.h
)

target_link_libraries(MaodieAdventure PRIVATE 
    Qt6::Core 
    Qt6::Widgets 
)

set_target_properties(MaodieAdventure PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)