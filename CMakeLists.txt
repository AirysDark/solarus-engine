
cmake_minimum_required(VERSION 3.10)
project(solarus)

add_executable(solarus main.cpp)

# --- install workaround for checkinstall ---
install(TARGETS solarus DESTINATION bin)
