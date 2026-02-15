cmake_minimum_required(VERSION 3.10)

project(TowerDefense VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuration Debug par défaut si non spécifiée
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choix du type de build" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -Wall -O0)
    message(STATUS "Mode Debug activé : flags -g -Wall appliqués")
else()
    add_compile_options(-O3 -Wall)
    message(STATUS "Mode Release activé : optimisations activées")
endif()



# Recherche de la librairie SDL2 (nécessaire pour l'affichage graphique)
find_package(SDL2 REQUIRED)

# Inclusion du dossier courant pour trouver les headers (.h)
include_directories(${CMAKE_SOURCE_DIR})

# Liste des fichiers sources
# Note : main.cpp est attendu selon le README
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(TowerDefense ${SRC_FILES})

target_include_directories(TowerDefense PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(TowerDefense PRIVATE ${SDL2_LIBRARIES})
