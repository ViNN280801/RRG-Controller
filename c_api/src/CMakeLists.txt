# c_api/src/CMakeLists.txt

set(RRG_INCLUDE_DIRS ../include)
set(RRG_SOURCES_LIST rrg.c)
set(LIB_RRG rrg)

add_library(${LIB_RRG} SHARED ${RRG_SOURCES_LIST})
target_link_libraries(${LIB_RRG} PRIVATE ${LIBMODBUS_LIBRARIES})
target_include_directories(${LIB_RRG} PRIVATE ${RRG_INCLUDE_DIRS} ${LIBMODBUS_INCLUDE_DIRS})
target_compile_definitions(${LIB_RRG} PRIVATE RRG_DLL_EXPORTS)

set_target_properties(${LIB_RRG} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
