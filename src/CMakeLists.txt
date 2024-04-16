# for each "src/x.cpp", generate target "x"
set(LOG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/log)
set(CORE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/core)

file(GLOB LOG_SRC_FILES ${LOG_SRC_DIR}/*.cpp)
file(GLOB CORE_SRC_FILES ${CORE_SRC_DIR}/*.cpp)

add_library(neolib ${LOG_SRC_FILES} ${CORE_SRC_FILES})