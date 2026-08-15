# stm32_helper.cmake - Configuración automatizada global de proyectos STM32

# 1. Búsqueda automática de archivos fuentes (.c y .cpp) en Core
file(GLOB_RECURSE USER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/**/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/**/*.cpp"
)
list(FILTER USER_SOURCES EXCLUDE REGEX ".*/Core/Src/[^/]+\\.c$")
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${USER_SOURCES})

# 2. Búsqueda automática de carpetas con archivos de cabecera (.h y .hpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/**/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/**/*.hpp"
)
set(USER_INCLUDE_DIRS "")
foreach(HEADER_FILE ${HEADER_FILES})
    get_filename_component(HEADER_DIR ${HEADER_FILE} DIRECTORY)
    list(APPEND USER_INCLUDE_DIRS ${HEADER_DIR})
endforeach()
if(USER_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES USER_INCLUDE_DIRS)
endif()
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${USER_INCLUDE_DIRS})

# 3. Soporte de flotantes en printf/sprintf
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    -u_printf_float
    -u_scanf_float
)