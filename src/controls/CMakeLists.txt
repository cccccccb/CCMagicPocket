set(DEPENDES_SOURCE
    impl/edgetoedgemodehelper.cpp
    impl/edgetoedgemodehelper.h
    impl/framelessattached.cpp
    impl/framelessattached.h
    impl/iconimageprovider.cpp
    impl/iconimageprovider.h
    impl/mpqmlinstance.cpp
    impl/mpqmlinstance.h
    impl/mousetransparentitem.cpp
    impl/mousetransparentitem.h
    impl/activityinformation.cpp
    impl/activityinformation.h
    impl/activityitemmodel.cpp
    impl/activityitemmodel.h
    impl/activitymanager.cpp
    impl/activitymanager.h
    impl/activitymoduleinformation.h
    impl/modularlistmodel.h
    impl/modularlistmodel.cpp

    mp_plugin.cpp
    mp_plugin.h
)

if (WIN32)
    list(APPEND DEPENDES_SOURCE
        impl/frameless_win.h impl/frameless_win.cpp
    )
else()
    list(APPEND DEPENDES_SOURCE
        impl/frameless.h impl/frameless.cpp
        impl/framelessworker.cpp
        impl/framelessworker.h
        impl/framelessworkerevent.cpp
        impl/framelessworkerevent.h
    )
endif()

set(REGISTER_QML
    modules/Controls/CheckedAnimateButton.qml
    modules/Controls/CustomButton.qml
    modules/Controls/LineEdit.qml
    modules/Controls/SearchEdit.qml
    modules/Controls/MPIcon.qml
    modules/Controls/BlurBackground.qml
    modules/Controls/ClipAreaItem.qml
    modules/Controls/InnerShadow.qml
    modules/Controls/CustomToolTip.qml
    modules/Controls/OuterShadow.qml
    modules/Style/Style.qml
)

set(PRIVATE_QML
    modules/Style/desktop/CustomButtonStyle.qml
    modules/Style/desktop/DesktopStyle.qml
    modules/Style/desktop/SearchEditStyle.qml
    modules/Style/mobile/CustomButtonStyle.qml
    modules/Style/mobile/MobileStyle.qml
    modules/Style/mobile/SearchEditStyle.qml
    modules/Controls/ShadowBase.qml
    modules/Utils/functional.js
)

set(QML_DIR_FILE
    qmldir
)

set(PLUGIN_BINARY_PATH
    ${CMAKE_BINARY_DIR}/CCMagicPocket
)

qt_add_plugin(MPControlExtension
    SHARED
)

qt_add_resources(MPControlExtension "qml"
    PREFIX
        "/CCMagicPocket"
    FILES
        ${REGISTER_QML}
        ${PRIVATE_QML}
        ${QML_DIR_FILE}
)

file(COPY ${QML_DIR_FILE} DESTINATION ${PLUGIN_BINARY_PATH})

target_sources(MPControlExtension PRIVATE
    ${DEPENDES_SOURCE}
)

set_target_properties(MPControlExtension
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_BINARY_PATH}
)

target_link_libraries(MPControlExtension PRIVATE
    CCQuickStartup
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::QuickPrivate
    zip
    fast_blur
)

target_include_directories(MPControlExtension PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/impl
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

install(TARGETS MPControlExtension
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
