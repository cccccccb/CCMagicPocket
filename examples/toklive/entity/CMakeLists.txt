qt_add_plugin(TokLive-DesktopEntity
    SHARED
    toklive_entity.cpp toklive_entity.h
)

target_link_libraries(TokLive-DesktopEntity PRIVATE
    CCQuickStartup
    Qt::Core
    Qt::Gui
    Qt::Quick
)

# Resources:
set(qml_resource_files
    "qml/src/MainWindow.qml"
)

set(qml_shaders_files
    "qml/src/shaders/wobble.frag"
    "qml/src/shaders/wobble.frag.qsb"
)

qt_add_resources(TokLive-DesktopEntity "qml"
    PREFIX
        "/TokLive"
    BASE
        "qml"
    FILES
        ${qml_resource_files}
        ${qml_shaders_files}
)
set(res_resource_files
    "res/leftbackground.jpg"
    "res/live.png"
    "res/shaders/noise.png"
)

qt_add_resources(TokLive-DesktopEntity "res"
    PREFIX
        "/TokLive"
    BASE
        "res"
    FILES
        ${res_resource_files}
)
set(icons_resource_files
    "icons/TokLive-desktop/32x32/category.png"
    "icons/TokLive-desktop/32x32/category_checked.png"
    "icons/TokLive-desktop/32x32/close.png"
    "icons/TokLive-desktop/32x32/community.png"
    "icons/TokLive-desktop/32x32/community_checked.png"
    "icons/TokLive-desktop/32x32/favorite.png"
    "icons/TokLive-desktop/32x32/favorite_checked.png"
    "icons/TokLive-desktop/32x32/home.png"
    "icons/TokLive-desktop/32x32/home_checked.png"
    "icons/TokLive-desktop/32x32/label.png"
    "icons/TokLive-desktop/32x32/label_checked.png"
    "icons/TokLive-desktop/32x32/maximize.png"
    "icons/TokLive-desktop/32x32/menu.png"
    "icons/TokLive-desktop/32x32/minimize.png"
    "icons/TokLive-desktop/32x32/normalize.png"
    "icons/TokLive-desktop/32x32/search.png"
    "icons/TokLive-desktop/32x32@2/category.png"
    "icons/TokLive-desktop/32x32@2/category_checked.png"
    "icons/TokLive-desktop/32x32@2/close.png"
    "icons/TokLive-desktop/32x32@2/community.png"
    "icons/TokLive-desktop/32x32@2/community_checked.png"
    "icons/TokLive-desktop/32x32@2/favorite.png"
    "icons/TokLive-desktop/32x32@2/favorite_checked.png"
    "icons/TokLive-desktop/32x32@2/home.png"
    "icons/TokLive-desktop/32x32@2/home_checked.png"
    "icons/TokLive-desktop/32x32@2/label.png"
    "icons/TokLive-desktop/32x32@2/label_checked.png"
    "icons/TokLive-desktop/32x32@2/maximize.png"
    "icons/TokLive-desktop/32x32@2/menu.png"
    "icons/TokLive-desktop/32x32@2/minimize.png"
    "icons/TokLive-desktop/32x32@2/normalize.png"
    "icons/TokLive-desktop/32x32@2/search.png"
    "icons/TokLive-desktop/index.theme"
)

qt_add_resources(TokLive-DesktopEntity "icons"
    PREFIX
        "/TokLive"
    BASE
        "icons"
    FILES
        ${icons_resource_files}
)

install(TARGETS TokLive-DesktopEntity
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
