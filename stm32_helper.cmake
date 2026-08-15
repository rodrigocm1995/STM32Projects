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

# 4. Remoción automática de main.c de todos los posibles targets (Librería y Ejecutable)
# Evita compilar main.c si CubeMX lo añade a la biblioteca stm32cubemx o directamente al ejecutable
if (TARGET stm32cubemx)
    get_target_property(CUBEMX_SOURCES stm32cubemx SOURCES)
    if (CUBEMX_SOURCES)
        list(FILTER CUBEMX_SOURCES EXCLUDE REGEX ".*/main\\.c$")
        set_target_properties(stm32cubemx PROPERTIES SOURCES "${CUBEMX_SOURCES}")
    endif()
endif()

get_target_property(EXE_SOURCES ${CMAKE_PROJECT_NAME} SOURCES)
if (EXE_SOURCES)
    list(FILTER EXE_SOURCES EXCLUDE REGEX ".*/main\\.c$")
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES SOURCES "${EXE_SOURCES}")
endif()

# 5. Selector automático dinámico e ilimitado de archivos main
if (NOT DEFINED ACTIVE_MAIN)
    set(ACTIVE_MAIN "MAIN")
endif()

if (ACTIVE_MAIN STREQUAL "MAIN")
    set(MAIN_FILENAME "main.c")
else()
    # Si es del tipo "MAIN_X", extrae "X", convierte a minúsculas y crea "mainX.c"
    string(REPLACE "MAIN_" "main" MAIN_FILENAME_RAW ${ACTIVE_MAIN})
    string(TOLOWER ${MAIN_FILENAME_RAW} MAIN_FILENAME_LOWER)
    set(MAIN_FILENAME "${MAIN_FILENAME_LOWER}.c")
endif()

set(MAIN_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/${MAIN_FILENAME}")

# Validación de seguridad: Verifica si el archivo realmente existe en el disco
if (NOT EXISTS ${MAIN_SOURCE})
    message(FATAL_ERROR "ERROR EN LA COMPILACIÓN: El archivo '${MAIN_SOURCE}' no existe en el disco. Revisa la variable ACTIVE_MAIN en tu CMakeLists.txt")
endif()

# Agregar el archivo main seleccionado al ejecutable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${MAIN_SOURCE})
