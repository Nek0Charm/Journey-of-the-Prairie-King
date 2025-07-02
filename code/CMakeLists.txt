# 明确指定源文件，避免包含CMake生成的文件
set(SOURCES
    "main.cpp"
    "app/application.cpp"
    "common/playerservice.cpp"
    "view/Animation.cpp"
    "view/AudioEventListener.cpp"
    "view/AudioManager.cpp"
    "view/EndWidget.cpp"
    "view/Entity.cpp"
    "view/GameMap.cpp"
    "view/GameWidget.cpp"
    "view/MainWindow.cpp"
    "view/playerview.cpp"
    "view/SpriteManager.cpp"
    "view/StartWidget.cpp"
    "viewmodel/BulletViewModel.cpp"
    "viewmodel/CollisionSystem.cpp"
    "viewmodel/EnemyManager.cpp"
    "viewmodel/GameViewModel.cpp"
    "viewmodel/ItemEffectManager.cpp"
    "viewmodel/ItemViewModel.cpp"
    "viewmodel/PlayerViewModel.cpp"
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
    Qt6::Multimedia
)

set_target_properties(MaodieAdventure PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)