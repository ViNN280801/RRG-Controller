# c_api/src/relay/CMakeLists.txt

set(RELAY_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/c_api/include/relay)
set(RELAY_SOURCES_LIST relay.c)
set(LIB_RELAY relay)

add_library(${LIB_RELAY} SHARED ${RELAY_SOURCES_LIST})
target_link_libraries(${LIB_RELAY} PRIVATE ${LIBMODBUS_LIBRARIES})
target_include_directories(${LIB_RELAY} PRIVATE ${RELAY_INCLUDE_DIRS} ${LIBMODBUS_INCLUDE_DIRS})
target_compile_definitions(${LIB_RELAY} PRIVATE RELAY_DLL_EXPORTS)
set_target_properties(${LIB_RELAY} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
