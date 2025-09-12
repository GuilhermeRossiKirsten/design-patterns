cmake_minimum_required(VERSION 3.16)  # versão mínima recomendada para C++20
project(CacheProject LANGUAGES CXX)

# Define o padrão de compilação
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # evita gnu++20, força c++20 puro

# Adiciona o executável
add_executable(cache_app 
    /workspaces/design-patterns/singleton/main.cpp
    /workspaces/design-patterns/singleton/src/cacheWithSingleton.cpp
)

# Inclui diretórios para headers
target_include_directories(cache_app PRIVATE
    "include"  # caminho onde estão os headers
)