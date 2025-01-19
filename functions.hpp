#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP
#include <boost/program_options.hpp>
#include <vector>
#include <fstream>

std::vector<std::string> split(std::string s, std::string delimiter);
std::string exec(const char* cmd);
std::string get_nameserver (std::string file, std::string regex, std::string delimiter = "false");
std::string get_dns_domains (std::string file, std::string regex, std::string delimiter = "false");
bool check_auth (const std::string module);
bool validate_ip_address (std::string addresses, std::string delimiter);
bool is_ipv4_address (std::string address);
bool is_ipv6_address (std::string address);
//bool check_ifupdown (const std::string interface);
//bool check_netplan (const std::string interface);
//bool check_networkManager (const std::string interface);
//bool check_systemd_networkd (const std::string interface);


#endif // FUNCTIONS_HPP


