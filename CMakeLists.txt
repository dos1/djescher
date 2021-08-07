cmake_minimum_required(VERSION 3.6)

project(djescher C)

SET(LIBSUPERDERPY_GAMENAME "djescher" CACHE INTERNAL "")
SET(LIBSUPERDERPY_GAMENAME_PRETTY "DJ Escher" CACHE INTERNAL "")

SET(LIBSUPERDERPY_APPID "net.dosowisko.DJEscher")
SET(LIBSUPERDERPY_ORIENTATION "LANDSCAPE")
SET(LIBSUPERDERPY_VERSION "1.0")
SET(LIBSUPERDERPY_VITA_TITLEID "DOS1ESCHE" CACHE INTERNAL "")

set(EMSCRIPTEN_TOTAL_MEMORY "128" CACHE INTERNAL "")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/libsuperderpy/cmake")

include(libsuperderpy)

add_subdirectory(libsuperderpy)
add_subdirectory(src)
add_subdirectory(data)
