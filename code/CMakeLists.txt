set(SOURCES
    main.cpp
    view/MainWindow.cpp
    view/StartWidget.cpp
    view/GameWidget.cpp
    view/GameMap.cpp
    view/Animation.cpp
    view/SpriteManager.cpp
    view/Entity.cpp
)

set(HEADERS
    include/view/MainWindow.h
    include/view/StartWidget.h
    include/view/GameWidget.h
    include/view/GameMap.h
    include/view/Animation.h
    include/view/SpriteManager.h
    include/view/Entity.h
)

set(RESOURCES
    ../resource.qrc
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