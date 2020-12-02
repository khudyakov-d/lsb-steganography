cmake_minimum_required(VERSION 3.12)
project(lsb C)

set(CMAKE_C_STANDARD 11)

add_executable(lsb main.c lsb.h lsb_embedding.c lsb_extraction.c utils.c utils.h)