qt_add_executable(MagicPocket WIN32 MACOSX_BUNDLE
    main.cpp
)

target_compile_definitions(MagicPocket PRIVATE
    QT_DEPRECATED_WARNINGS
)

target_link_libraries(MagicPocket PRIVATE
    CCQuickStartup
    Qt::Core
    Qt::Gui
    Qt::Quick
)

# Resources:
set(style_resource_files
    "qtquickcontrols2.conf"
)

qt_add_resources(MagicPocket "style"
    PREFIX
        "/"
    FILES
        ${style_resource_files}
)

install(TARGETS MagicPocket
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET MagicPocket
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
