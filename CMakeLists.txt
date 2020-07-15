cmake_minimum_required(VERSION 3.6.0)

project(luafx)

# lua
file(GLOB LFX_LIB_LUA_SRCS
     ${CMAKE_SOURCE_DIR}/3rd/lua/src/*.h
     ${CMAKE_SOURCE_DIR}/3rd/lua/src/*.c
     )

if (${CMAKE_VERSION} VERSION_GREATER "3.8.0" )
    source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${LFX_LIB_LUA_SRCS})
endif ()

# json
file(GLOB LFX_LIB_CJSON_SRCS
     ${CMAKE_SOURCE_DIR}/3rd/cjson/*.h
     ${CMAKE_SOURCE_DIR}/3rd/cjson/*.c
     )

if (${CMAKE_VERSION} VERSION_GREATER "3.8.0" )
    source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${LFX_LIB_CJSON_SRCS})
endif ()

# get path
get_filename_component(LFX_LIB_SRC_DIR
                       ${CMAKE_SOURCE_DIR}/src/luafx
                       ABSOLUTE)

get_filename_component(LFX_EDITOR_SRC_DIR
                       ${CMAKE_SOURCE_DIR}/src/editor
                       ABSOLUTE)

get_filename_component(LFX_APP_SRC_DIR
                       ${CMAKE_SOURCE_DIR}/src/app
                       ABSOLUTE)

# luafx
file(GLOB LFX_LIB_SRCS
     ${CMAKE_SOURCE_DIR}/include/*.h
     ${LFX_LIB_SRC_DIR}/*.h
     ${LFX_LIB_SRC_DIR}/*.c
     ${LFX_LIB_SRC_DIR}/lua_bind/*.h
     ${LFX_LIB_SRC_DIR}/lua_bind/*.c
     )

# editor
file(GLOB LFX_EDITOR_SRCS
     ${LFX_EDITOR_SRC_DIR}/*.h
     ${LFX_EDITOR_SRC_DIR}/*.c
     )

# glew
if (${Target} MATCHES "Windows" OR ${Target} MATCHES "Mac")

    file(GLOB LFX_LIB_GLEW_SRCS
         ${CMAKE_SOURCE_DIR}/3rd/glew/src/glew.c
         )

    if (${CMAKE_VERSION} VERSION_GREATER "3.8.0" )
        source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${LFX_LIB_GLEW_SRCS})
    endif ()

    add_library(glew32 SHARED ${LFX_LIB_GLEW_SRCS})

    target_include_directories(glew32 PRIVATE
                               ${CMAKE_SOURCE_DIR}/3rd/glew/include
                               )

    set_property(TARGET glew32 PROPERTY COMPILE_DEFINITIONS "GLEW_BUILD")                       

endif ()

if (${Target} MATCHES "Android")
set(LFX_TARGET "luafx_static")
else ()
set(LFX_TARGET "luafx")
endif ()

# platforms
if (${Target} MATCHES "Windows")

    target_link_libraries(glew32
                          opengl32.lib
                          )

    add_library(${LFX_TARGET} SHARED
                ${LFX_LIB_SRCS}
                ${LFX_LIB_LUA_SRCS}
                ${LFX_LIB_CJSON_SRCS}
                )

    set_property(TARGET ${LFX_TARGET} PROPERTY COMPILE_DEFINITIONS "LFX_BUILD" "CJSON_HIDE_SYMBOLS" "CGLM_ALL_UNALIGNED")

    target_link_libraries(${LFX_TARGET}
                          glew32
                          )

elseif (${Target} MATCHES "Mac")

    add_library(${LFX_TARGET} SHARED
                ${LFX_LIB_SRCS}
                ${LFX_LIB_LUA_SRCS}
                ${LFX_LIB_CJSON_SRCS}
                )

    set_property(TARGET ${LFX_TARGET} PROPERTY COMPILE_DEFINITIONS "LFX_BUILD" "CJSON_HIDE_SYMBOLS" "CGLM_ALL_UNALIGNED")

    target_link_libraries(${LFX_TARGET}
                          glew32
                          "-framework OpenGL"
                          )

elseif (${Target} MATCHES "iOS")

    add_library(${LFX_TARGET} SHARED
                ${LFX_LIB_SRCS}
                ${LFX_LIB_LUA_SRCS}
                ${LFX_LIB_CJSON_SRCS}
                )

    set_property(TARGET ${LFX_TARGET} PROPERTY COMPILE_DEFINITIONS "LFX_BUILD" "CJSON_HIDE_SYMBOLS" "CGLM_ALL_UNALIGNED" "STBI_NO_THREAD_LOCALS=1")

    target_link_libraries(${LFX_TARGET}
                          "-framework OpenGLES"
                          )
                          
    set_target_properties(${LFX_TARGET} PROPERTIES
                          FRAMEWORK TRUE
                          XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
                          MACOSX_FRAMEWORK_INFO_PLIST ${LFX_APP_SRC_DIR}/platform/ios/luafx-Info.plist
                          )

elseif (${Target} MATCHES "Android")

    add_library(${LFX_TARGET} STATIC
                ${LFX_LIB_SRC_DIR}/platform/android/gl3stub.c
                ${LFX_LIB_SRCS}
                ${LFX_LIB_LUA_SRCS}
                ${LFX_LIB_CJSON_SRCS}
                )

    set_property(TARGET ${LFX_TARGET} PROPERTY COMPILE_DEFINITIONS "LFX_STATIC" "CJSON_HIDE_SYMBOLS" "CGLM_ALL_UNALIGNED")

elseif (${Target} MATCHES "WASM")
    
    add_library(${LFX_TARGET} STATIC
                ${LFX_LIB_SRCS}
                ${LFX_LIB_LUA_SRCS}
                ${LFX_LIB_CJSON_SRCS}
                )

    set_property(TARGET ${LFX_TARGET} PROPERTY COMPILE_DEFINITIONS "LFX_STATIC" "CJSON_HIDE_SYMBOLS" "CGLM_ALL_UNALIGNED")

endif ()

if (${CMAKE_VERSION} VERSION_GREATER "3.8.0" )
    source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${LFX_LIB_SRCS})
    source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${LFX_EDITOR_SRCS})
endif ()

target_include_directories(${LFX_TARGET} PRIVATE
                           ${CMAKE_SOURCE_DIR}/include
                           ${LFX_LIB_SRC_DIR}
                           ${CMAKE_SOURCE_DIR}/3rd/glew/include
                           ${CMAKE_SOURCE_DIR}/3rd/stb
                           ${CMAKE_SOURCE_DIR}/3rd/lua/src
                           ${CMAKE_SOURCE_DIR}/3rd/cglm/include
                           ${CMAKE_SOURCE_DIR}/3rd/cjson
                           )

if (${Target} MATCHES "Windows")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLFX_WINDOWS -W3 -D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS}")

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

    source_group(src FILES ${LFX_EDITOR_SRC_DIR}/platform/win/Main.cpp)

    add_executable(luafx_editor
                   ${LFX_EDITOR_SRCS}
                   ${LFX_EDITOR_SRC_DIR}/platform/win/Main.cpp
                   ${LFX_EDITOR_WEBCAMERA_SRCS}
                   )

    target_include_directories(luafx_editor PRIVATE
                               ${LFX_EDITOR_SRC_DIR}
                               ${CMAKE_SOURCE_DIR}/include
                               ${LFX_LIB_SRC_DIR}
                               ${CMAKE_SOURCE_DIR}/3rd/glew/include
                               )

    target_link_libraries(luafx_editor
                          glew32
                          ${LFX_TARGET}
                          winmm.lib
                          )

    set_property(TARGET luafx_editor PROPERTY LINK_FLAGS "/SUBSYSTEM:WINDOWS")

    string(REPLACE "/" "\\" BIN_DIR ${PROJECT_BINARY_DIR}/$(Configuration))
    string(REPLACE "/" "\\" ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)

    add_custom_command(TARGET luafx_editor
                       POST_BUILD
                       COMMAND xcopy ${ASSETS_DIR} ${BIN_DIR}\\assets\\ /s /d /y
                       )

    set_property(TARGET luafx_editor PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${BIN_DIR}")

elseif (${Target} MATCHES "Mac")

    add_definitions(-DLFX_MAC=1)
    add_compile_options(-fobjc-arc -Wall)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    
    set_source_files_properties(${LFX_EDITOR_SRC_DIR}/platform/mac/Assets.xcassets
                                ${CMAKE_SOURCE_DIR}/assets
                                PROPERTIES
                                MACOSX_PACKAGE_LOCATION Resources
                                )

    add_executable(luafx_editor
                   ${LFX_EDITOR_SRCS}
                   ${LFX_EDITOR_SRC_DIR}/platform/mac/Main.m
                   ${LFX_EDITOR_SRC_DIR}/platform/mac/AppDelegate.m
                   ${LFX_EDITOR_SRC_DIR}/platform/mac/ViewController.mm
                   ${LFX_EDITOR_SRC_DIR}/platform/mac/Assets.xcassets
                   ${CMAKE_SOURCE_DIR}/assets
                   )

    target_include_directories(luafx_editor PRIVATE
                               ${LFX_EDITOR_SRC_DIR}
                               ${CMAKE_SOURCE_DIR}/include
                               ${LFX_LIB_SRC_DIR}
                               ${CMAKE_SOURCE_DIR}/3rd/glew/include
                               )

    target_link_libraries(luafx_editor
                          glew32
                          ${LFX_TARGET}
                          "-framework AppKit"
                          )

    set_target_properties(luafx_editor PROPERTIES
                          MACOSX_BUNDLE TRUE
                          MACOSX_BUNDLE_INFO_PLIST ${LFX_EDITOR_SRC_DIR}/platform/mac/Info.plist
                          XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon
                          )

elseif (${Target} MATCHES "iOS")

    set(CMAKE_OSX_SYSROOT iphoneos)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 8.0)

    add_definitions(-DLFX_IOS=1)
    add_compile_options(-fobjc-arc -Wall)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

    set_source_files_properties(${LFX_APP_SRC_DIR}/platform/ios/Assets.xcassets
                                ${LFX_APP_SRC_DIR}/platform/ios/LaunchScreen.storyboard
                                ${CMAKE_SOURCE_DIR}/assets
                                PROPERTIES
                                MACOSX_PACKAGE_LOCATION Resources
                                )

    add_executable(luafx_camera
                   ${LFX_APP_SRC_DIR}/platform/ios/Main.m
                   ${LFX_APP_SRC_DIR}/platform/ios/AppDelegate.m
                   ${LFX_APP_SRC_DIR}/platform/ios/ViewController.mm
                   ${LFX_APP_SRC_DIR}/platform/ios/Assets.xcassets
                   ${LFX_APP_SRC_DIR}/platform/ios/LaunchScreen.storyboard
                   ${CMAKE_SOURCE_DIR}/assets
                   )

    target_include_directories(luafx_camera PRIVATE
                               ${CMAKE_SOURCE_DIR}/include
                               ${LFX_LIB_SRC_DIR}
                               )

    target_link_libraries(luafx_camera
                          ${LFX_TARGET}
                          "-framework UIKit"
                          "-framework QuartzCore"
                          )

    set_target_properties(luafx_camera PROPERTIES
                          MACOSX_BUNDLE TRUE
                          XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
                          MACOSX_BUNDLE_INFO_PLIST ${LFX_APP_SRC_DIR}/platform/ios/luafx_camera-Info.plist
                          XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon
                          XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path/Frameworks"
                          )
                          
    add_custom_command(TARGET luafx_camera POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory
                           $(BUILD_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)/luafx.framework
                           $(BUILD_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)/luafx_camera.app/Frameworks/luafx.framework
                       )

elseif (${Target} MATCHES "Android")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLFX_ANDROID -Wall")

    add_library(luafx SHARED
                ${CMAKE_SOURCE_DIR}/android/luafx/src/main/cpp/jni.c
                )

    target_include_directories(luafx PRIVATE
                               ${CMAKE_SOURCE_DIR}/include
                               ${LFX_LIB_SRC_DIR}
                               )

    target_link_libraries(luafx
                          ${LFX_TARGET}
                          GLESv2
                          EGL
                          log
                          android)

elseif (${Target} MATCHES "WASM")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLFX_WASM -Wall")
    add_definitions(-s WASM=1 -O3)

    add_library(luafx_wasm STATIC
                ${CMAKE_SOURCE_DIR}/src/app/platform/web/Main.c
                )

    target_include_directories(luafx_wasm PRIVATE
                               ${CMAKE_SOURCE_DIR}/include
                               ${LFX_LIB_SRC_DIR}
                               )

endif ()
