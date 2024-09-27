qt_add_plugin(MagicPocket-DesktopEntity
    SHARED
    mp_entity.cpp mp_entity.h
)

set_target_properties(MagicPocket-DesktopEntity
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/modules
)

target_include_directories(MagicPocket-DesktopEntity PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../controls/impl
)

target_link_libraries(MagicPocket-DesktopEntity PRIVATE
    CCQuickStartup
    Qt::Core
    Qt::Gui
    Qt::Quick
)

set(qml_controls_files
    "qml/src/controls/PopupPane.qml"
    "qml/src/controls/PopupSearchEdit.qml"
    "qml/src/controls/RoundAnimateButton.qml"
)

set(qml_left_files
    "qml/src/left/LeftNavigationBar.qml"
    "qml/src/left/NavigationPopup.qml"
    "qml/src/left/WindowButtonPane.qml"
    "qml/src/left/SlidePopupButton.qml"
)

set(qml_title_files
    "qml/src/title/StatusTitleBar.qml"
    "qml/src/title/StatusBlock.qml"
)

set(qml_bottom_files
    "qml/src/bottom/ActivityDocker.qml"
    "qml/src/bottom/ActivityDockerModel.qml"
    "qml/src/bottom/ActivityDockerItem.qml"
    "qml/src/bottom/ActivityDockerView.qml"
)

set(qml_pane_files
    "qml/src/NavigationBarGroup.qml"
    "qml/src/MainPane.qml"
    "qml/src/MainWindow.qml"
    "qml/src/CenterPane.qml"
    "qml/src/ActivityTemplate.qml"
)

set(qml_resource_files
    ${qml_left_files}
    ${qml_title_files}
    ${qml_bottom_files}
    ${qml_controls_files}
    ${qml_pane_files}
)

qt_add_resources(MagicPocket-DesktopEntity "qml"
    PREFIX
        "/CCMagicPocket"
    FILES
        ${qml_resource_files}
)
set(res_image_files
    "res/leftbackground.jpg"
    "res/default_wallpaper_1.jpg"
    "res/default_wallpaper_2.jpg"
    "res/icon.png"
)

set(res_svg_files
    "res/svg/icon-circle.svg"
    "res/svg/firefox.svg"
    "res/svg/github.svg"
    "res/svg/gmail.svg"
    "res/svg/google-chrome.svg"
    "res/svg/safari.svg"
    "res/svg/tiktok.svg"
    "res/svg/twitter.svg"
    "res/svg/wechat.svg"
)

set(res_resource_files
    ${res_image_files}
    ${res_svg_files}
)

qt_add_resources(MagicPocket-DesktopEntity "res"
    PREFIX
        "/CCMagicPocket"
    FILES
        ${res_resource_files}
)
set(icons_resource_files
    "icons/MagicPocket-desktop/32x32/category.png"
    "icons/MagicPocket-desktop/32x32/category_checked.png"
    "icons/MagicPocket-desktop/32x32/close.png"
    "icons/MagicPocket-desktop/32x32/community.png"
    "icons/MagicPocket-desktop/32x32/community_checked.png"
    "icons/MagicPocket-desktop/32x32/favorite.png"
    "icons/MagicPocket-desktop/32x32/favorite_checked.png"
    "icons/MagicPocket-desktop/32x32/home.png"
    "icons/MagicPocket-desktop/32x32/home_checked.png"
    "icons/MagicPocket-desktop/32x32/label.png"
    "icons/MagicPocket-desktop/32x32/label_checked.png"
    "icons/MagicPocket-desktop/32x32/maximize.png"
    "icons/MagicPocket-desktop/32x32/menu.png"
    "icons/MagicPocket-desktop/32x32/minimize.png"
    "icons/MagicPocket-desktop/32x32/normalize.png"
    "icons/MagicPocket-desktop/32x32/search.png"
    "icons/MagicPocket-desktop/32x32/slide-out.png"
    "icons/MagicPocket-desktop/32x32/notification.png"
    "icons/MagicPocket-desktop/32x32/notification-off.png"
    "icons/MagicPocket-desktop/32x32/notification-on.png"
    "icons/MagicPocket-desktop/32x32/fullscreen_exit.png"
    "icons/MagicPocket-desktop/32x32/fullscreen.png"
    "icons/MagicPocket-desktop/32x32/minimize_activity.png"
    "icons/MagicPocket-desktop/32x32/close_activity.png"
    "icons/MagicPocket-desktop/32x32/layout_activity.png"
    "icons/MagicPocket-desktop/32x32@2/category.png"
    "icons/MagicPocket-desktop/32x32@2/category_checked.png"
    "icons/MagicPocket-desktop/32x32@2/close.png"
    "icons/MagicPocket-desktop/32x32@2/community.png"
    "icons/MagicPocket-desktop/32x32@2/community_checked.png"
    "icons/MagicPocket-desktop/32x32@2/favorite.png"
    "icons/MagicPocket-desktop/32x32@2/favorite_checked.png"
    "icons/MagicPocket-desktop/32x32@2/home.png"
    "icons/MagicPocket-desktop/32x32@2/home_checked.png"
    "icons/MagicPocket-desktop/32x32@2/label.png"
    "icons/MagicPocket-desktop/32x32@2/label_checked.png"
    "icons/MagicPocket-desktop/32x32@2/maximize.png"
    "icons/MagicPocket-desktop/32x32@2/menu.png"
    "icons/MagicPocket-desktop/32x32@2/minimize.png"
    "icons/MagicPocket-desktop/32x32@2/normalize.png"
    "icons/MagicPocket-desktop/32x32@2/search.png"
    "icons/MagicPocket-desktop/32x32@2/slide-out.png"
    "icons/MagicPocket-desktop/32x32@2/notification.png"
    "icons/MagicPocket-desktop/32x32@2/notification-off.png"
    "icons/MagicPocket-desktop/32x32@2/notification-on.png"
    "icons/MagicPocket-desktop/32x32@2/fullscreen_exit.png"
    "icons/MagicPocket-desktop/32x32@2/fullscreen.png"
    "icons/MagicPocket-desktop/32x32@2/minimize_activity.png"
    "icons/MagicPocket-desktop/32x32@2/close_activity.png"
    "icons/MagicPocket-desktop/32x32@2/layout_activity.png"
    "icons/MagicPocket-desktop/scalable/actions/all_application.svg"
    "icons/MagicPocket-desktop/index.theme"
)

qt_add_resources(MagicPocket-DesktopEntity "icons"
    FILES
        ${icons_resource_files}
)

install(TARGETS MagicPocket-DesktopEntity
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
