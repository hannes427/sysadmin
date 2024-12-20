cmake_minimum_required(VERSION 3.0)

project(freepbx_sysadmin)

add_executable(freepbx_sysadmin main.cpp)

install(TARGETS freepbx_sysadmin RUNTIME DESTINATION bin)
