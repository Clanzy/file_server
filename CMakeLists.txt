cmake_minimum_required(VERSION 3.0.0)
project(file_server VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS_DEBUG_INIT "-g - Wall -Wextra -Wpedantic")

include(CTest)
enable_testing()

set(TOOLS_FILES tools/backend.h tools/backend.c tools/utilities.h  tools/request.c tools/request.h tools/constants.h)
set(CLIENT_FILES client/client-interface.c client/client-interface.h tools/backend-client.c)
set(SERVER_FILES tools/backend-server.c)

add_executable(server server/server.c ${TOOLS_FILES} ${SERVER_FILES})
add_executable(client client/client.c ${TOOLS_FILES} ${CLIENT_FILES})

target_include_directories(server PRIVATE tools)
target_include_directories(client PRIVATE tools)

target_link_libraries(server PRIVATE pthread)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

