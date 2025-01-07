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
add_executable(get_netplan_config get_netplan_config.cpp)
add_executable(notifications notifications.cpp)
add_executable(email_settings email.cpp)

target_link_libraries(packet_capture Boost::program_options mysqlclient)
target_link_libraries(hostname Boost::program_options mysqlclient)
target_link_libraries(change_dns Boost::program_options mysqlclient)
target_link_libraries(network Boost::program_options mysqlclient)
target_link_libraries(set_timezone Boost::program_options mysqlclient)
target_link_libraries(get_netplan_config Boost::program_options mysqlclient)
target_link_libraries(notifications Boost::program_options mysqlclient)
target_link_libraries(email_settings Boost::program_options mysqlclient)

install(TARGETS packet_capture RUNTIME DESTINATION bin)
