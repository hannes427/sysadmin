#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP
#include <boost/program_options.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

std::vector<std::string> split(std::string s, std::string delimiter);
std::string exec(const char* cmd);
std::string get_nameserver (std::string file, std::string regex, std::string delimiter = "false");
std::string get_dns_domains (std::string file, std::string regex, std::string delimiter = "false");
bool check_auth (const std::string module);
bool validate_ip_address (std::string addresses, std::string delimiter);
bool is_ipv4_address (std::string address);
bool is_ipv6_address (std::string address);
void replaceAll (std::string& str, const std::string& from, const std::string& to);
#endif // FUNCTIONS_HPP


