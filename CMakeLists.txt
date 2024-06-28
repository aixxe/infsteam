cmake_minimum_required(VERSION 3.28)

project(infsteam LANGUAGES CXX VERSION 1.0.0.0)

include(cmake/get_cpm.cmake)

CPMAddPackage(
    NAME              safetyhook
    GIT_TAG           v0.4.1
    GITHUB_REPOSITORY cursey/safetyhook
    OPTIONS           "SAFETYHOOK_FETCH_ZYDIS ON"
)

add_library(Steamworks INTERFACE)
target_link_libraries(Steamworks INTERFACE steam_api64)
target_link_directories(Steamworks INTERFACE third_party/steam/lib)
target_include_directories(Steamworks INTERFACE third_party/steam/include)

add_library(${PROJECT_NAME} SHARED
    src/init.cc
    src/util.cc
    src/bm2dxi.cc
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.h)
target_link_libraries(${PROJECT_NAME} PRIVATE Steamworks safetyhook::safetyhook)