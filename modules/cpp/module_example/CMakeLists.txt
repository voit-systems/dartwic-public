cmake_minimum_required(VERSION 3.30)
project(module_example)

set(CMAKE_CXX_STANDARD 20)

############### MODULE DEPLOYMENT CONFIG ###############
# the name of the module (default set to the project name)
# must match the "name" parameter in the module_config.yaml
set(MODULE_NAME "${PROJECT_NAME}" CACHE STRING "Name of the module folder")
# the path to copy the module to dartwic registry
set(REGISTRY_PATH "<path-to-dartwic-project>/modules/registry" CACHE PATH "Path to DARTWIC modules registry folder")


############### SOURCE FILES ###############
file(GLOB_RECURSE SRC_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE SDK_SOURCES "${PROJECT_SOURCE_DIR}/sdk/*.cpp")

### CREATE MAIN LIBRARY ###
add_library(module_example SHARED ${SRC_SOURCES} ${SDK_SOURCES})

############### INCLUDE DIRECTORIES ###############
file(GLOB subdirs
        CONFIGURE_DEPENDS
        LIST_DIRECTORIES true
        "${PROJECT_SOURCE_DIR}/include/*")

target_include_directories(module_example PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        ${subdirs})

############### NON HEADER ONLY LIBRARY INCLUDES ###############
find_package(yaml-cpp CONFIG REQUIRED)

target_link_libraries(modbus_tcp_client PRIVATE
        yaml-cpp::yaml-cpp
)

############### DEBUGGING ###############
set(CMAKE_CXX_FLAGS_RELEASE "/Zi /Od")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/DEBUG")

############### OUTPUT DIRECTORIES ###############
# Default to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set_target_properties(labjack_t7_module PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/release/${CMAKE_BUILD_TYPE}/${MODULE_NAME}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/release/${CMAKE_BUILD_TYPE}/${MODULE_NAME}/bin"
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/release/${CMAKE_BUILD_TYPE}/${MODULE_NAME}/bin"
)

############### COPY FILES ###############
#module_config.yaml TO OUTPUT
set(CONFIG_FILE "${CMAKE_SOURCE_DIR}/src/module_config.yaml")

add_custom_command(TARGET labjack_t7_module POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CONFIG_FILE}"
        "${PROJECT_SOURCE_DIR}/release/${CMAKE_BUILD_TYPE}/${MODULE_NAME}/module_config.yaml"
        COMMENT "Copying module_config.yaml to output folder"
)

# module built files to registry folder
add_custom_command(TARGET labjack_t7_module POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "${REGISTRY_PATH}/${MODULE_NAME}"

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/release/${CMAKE_BUILD_TYPE}/${MODULE_NAME}"
        "${REGISTRY_PATH}/${MODULE_NAME}"

        COMMENT "Deploying module to registry: ${REGISTRY_PATH}"
)
