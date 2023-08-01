cmake_minimum_required (VERSION 3.8)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(mapreduce VERSION ${PROJECT_VERSION})

if (UNIX)
    find_package (Threads)
endif()

file(GLOB source
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable (${PROJECT_NAME} ${source})
target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
        ${CMAKE_THREAD_LIBS_INIT}
)

if (UNIX)
    target_link_libraries (${PROJECT_NAME} Threads::Threads)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)


set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

include(CPack)