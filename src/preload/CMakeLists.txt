qt_add_plugin(MagicPocket-Preload
    SHARED
    mp_preload.cpp mp_preload.h
)

target_sources(MagicPocket-Preload
  PRIVATE
    model.cpp model.h
)

set_target_properties(MagicPocket-Preload
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/modules
)

target_link_libraries(MagicPocket-Preload PRIVATE
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

qt_add_resources(MagicPocket-Preload "qml"
    PREFIX
        "/CCMagicPocket"
    FILES
        ${qml_resource_files}
)

set(res_resource_files
    "res/icon.png"
)

qt_add_resources(MagicPocket-Preload "res"
    PREFIX
        "/CCMagicPocket"
    FILES
        ${res_resource_files}
)

install(TARGETS MagicPocket-Preload
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
