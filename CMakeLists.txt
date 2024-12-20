cmake_minimum_required(VERSION 3.5)

project(freepbx_sysadmin)

FIND_PACKAGE( Boost COMPONENTS program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_executable(packet_capture packet_capture.cpp)
add_executable(powermgmt power_management.cpp)
add_executable(hostname hostname.cpp)
add_executable(change_dns change_dns.cpp)
add_executable(network network.cpp)
add_executable(set_timezone timezone.cpp)

target_link_libraries(packet_capture Boost::program_options mysqlclient)
target_link_libraries(hostname Boost::program_options mysqlclient)
target_link_libraries(change_dns Boost::program_options mysqlclient)
target_link_libraries(network Boost::program_options mysqlclient)
target_link_libraries(set_timezone Boost::program_options mysqlclient)

install(TARGETS packet_capture RUNTIME DESTINATION bin)
