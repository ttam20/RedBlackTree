cmake_minimum_required(VERSION 3.5)

project(RedBlackTree VERSION 1.0.0 LANGUAGES CXX)

set(Qt_DLLS_BIN "C:/Qt/6.8.0/mingw_64/bin/")
set(Qt_DIR "C:/Qt/6.8.0/mingw_64/")

set(CPACK_PACKAGE_NAME "RedBlackTree")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_GENERATOR "NSIS")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/license.txt")
set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut \$DESKTOP\\RedBlackTree.lnk \$INSTDIR\\bin\\rbt.exe")
set(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete \$DESKTOP\\RedBlackTree.lnk")

list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "SetOutPath \$INSTDIR")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "File ${CMAKE_BINARY_DIR}\\tests\\rbtree.json")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "SetOutPath \$INSTDIR\\bin")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "File ${Qt_DLLS_BIN}\\Qt6Core.dll")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "File ${Qt_DLLS_BIN}\\Qt6Gui.dll")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "File ${Qt_DLLS_BIN}\\Qt6Widgets.dll")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "File ${Qt_DLLS_BIN}\\Qt6Svg.dll")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "File ${Qt_DLLS_BIN}\\libwinpthread-1.dll")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "File ${Qt_DLLS_BIN}\\libgcc_s_seh-1.dll")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "File ${Qt_DLLS_BIN}\\libstdc++-6.dll")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "SetOutPath \$INSTDIR\\bin\\platforms")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "File ${Qt_DIR}\\plugins\\platforms\\qwindows.dll")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "SetOutPath \$INSTDIR\\bin\\styles")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "File ${Qt_DIR}\\plugins\\styles\\qmodernwindowsstyle.dll")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "SetOutPath \$INSTDIR\\bin\\iconengines")
list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "File /r ${Qt_DIR}\\plugins\\iconengines\\qsvgicon.dll")

list(APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "RMDir /r \$INSTDIR\\bin")
list(APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "Delete \$INSTDIR\\rbtree.json")

string (REPLACE ";" "\n" CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}")
string (REPLACE ";" "\n" CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/config.rc")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Test Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS Xml)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

add_library(RedBlackTreeLib SHARED redblacktree.h redblacktree.cpp node.h node.cpp)
target_link_libraries(RedBlackTreeLib Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(RedBlackTreeLib Qt${QT_VERSION_MAJOR}::Xml)
target_link_libraries(RedBlackTreeLib Qt${QT_VERSION_MAJOR}::Gui)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RedBlackTree
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc
        redblacktree.h

        redblacktree.cpp
        treenode.h treenode.cpp
        nilnode.h nilnode.cpp
        node.h
        node.cpp
        cgraphicsview.h
        ${CONFIG}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET RedBlackTree APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(RedBlackTree SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(RedBlackTree
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(RedBlackTree PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(RedBlackTree PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(RedBlackTree PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(RedBlackTree PRIVATE Qt${QT_VERSION_MAJOR}::Xml)
target_link_libraries(RedBlackTree PRIVATE RedBlackTreeLib)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.RedBlackTree)
endif()
set_target_properties(RedBlackTree PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "rbt"
)

include(GNUInstallDirs)
include(CPack)
install(TARGETS RedBlackTree
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RedBlackTree)
endif()

add_subdirectory(tests)
