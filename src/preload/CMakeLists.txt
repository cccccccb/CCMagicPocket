qt_add_plugin(MPDesktopPreload
    SHARED
    mp_preload.cpp mp_preload.h
)

set_target_properties(MPDesktopPreload
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)

target_link_libraries(MPDesktopPreload PRIVATE
    CCQuickStartup
    Qt::Core
    Qt::Gui
    Qt::Quick
)

# Resources:
set(qml_resource_files
    "qml/Countdown.qml"
    "qml/PreloadOverlay.qml"
    "qml/PreloadWindow.qml"
)

qt_add_resources(MPDesktopPreload "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)

set(res_resource_files
    "res/icon.png"
)

qt_add_resources(MPDesktopPreload "res"
    PREFIX
        "/"
    BASE
        "res"
    FILES
        ${res_resource_files}
)

install(TARGETS MPDesktopPreload
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
