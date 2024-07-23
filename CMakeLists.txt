# CMakeList.txt: proyecto de CMake para Dijkstra, incluya el origen y defina
# la lógica específica del proyecto aquí.
#
cmake_minimum_required (VERSION 3.8)

project(Dijkstra)

set(SDL2_DIR  ${CMAKE_HOME_DIRECTORY}/thirdparty/SDL2-2.30.5/cmake)
find_package(SDL2 REQUIRED)

# Habilite Recarga activa para los compiladores de MSVC si se admiten.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Agregue un origen al ejecutable de este proyecto.
add_executable (Dijkstra "Dijkstra.cpp" "Dijkstra.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Dijkstra PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(Dijkstra SDL2::SDL2 SDL2::SDL2main)

add_custom_command(TARGET Dijkstra POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:Dijkstra> $<TARGET_RUNTIME_DLLS:Dijkstra>
  COMMAND_EXPAND_LISTS
)

# TODO: Agregue pruebas y destinos de instalación si es necesario.
