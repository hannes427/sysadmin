#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include "functions.hpp"
#include <boost/program_options.hpp>

int main (int argc, char **argv) {
    std::string dns_server;
    std::string search;
    std::string connection;
    int current_uid = getuid();
    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
        ("help,h", "Help screen")
        ("dns-server", boost::program_options::value<std::string>()->required(), "List of dns-servers, seperated by comma")
        ("search", boost::program_options::value<std::string>(), "domain search list. Seperate entries by comma")
        ("connection", boost::program_options::value<std::string>(), "NetworkManager connection (if NetworkManager is used");

        boost::program_options::variables_map vm;
        boost::program_options::command_line_style::allow_long_disguise;
        store(parse_command_line(argc, argv, desc), vm);

        notify(vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';
        }
        if (vm.count("dns-server")) {
            dns_server = vm["dns-server"].as<std::string>();
        }
        if (vm.count("search")) {
            search = vm["search"].as<std::string>();
        }
         if (vm.count("connection")) {
            connection = vm["connection"].as<std::string>();
        }
    }
    catch (const boost::program_options::error &ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
    if (!validate_ip_address(dns_server, ",")) {
        std::cerr<<std::endl<<"Error! Invalid Nameservers! "<<dns_server<<std::endl<<std::endl;
        return 1;
    }
    std::string path = "/sys/class/net/";
    for (const auto & interface : std::filesystem::directory_iterator(path)) {
        if (interface.path().filename() == "lo") {
            continue;
        }
        const std::string interface_name = interface.path().filename();
         if (setuid(0)) { //I am now root!
             perror("setuid");
             return 1;
        }
        int check_dhcp_process = system("/usr/bin/ps -ef | grep dhclient >> /dev/null 2>&1");
        if (check_dhcp_process == 0) {
            std::string dhcphookfile = "/etc/dhcp/dhclient-enter-hooks.d/zzz_nodnsupdate";
            std::ofstream dhcphook;
            dhcphook.open(dhcphookfile);
            dhcphook<<"#!/bin/sh"<<std::endl<<"make_resolv_conf(){"<<std::endl<<"	:"<<std::endl<<"}"<<std::endl;
            dhcphook.close();
        }
        if (check_ifupdown(interface.path().filename())) { //Interface is managed by ifupdown
            if (std::filesystem::is_symlink("/etc/resolv.conf")) {
                if (std::filesystem::read_symlink("/etc/resolv.conf") == "../run/resolvconf/resolv.conf") { // /etc/resolv.conf is managed by resolvconf
                    std::string filepath = std::string("/etc/network/interfaces.d/") + interface_name;
                    std::string temp_filepath =  std::string("/tmp/") + interface_name;
                    if (std::filesystem::exists(filepath)) {
                        std::ifstream file;
                        std::ofstream temp_file;
                        file.open(filepath);
                        temp_file.open(temp_filepath);
                        std::string line;
                        std::regex delete_dns(R"(^dns-nameservers\s+(.*))");
                        std::regex delete_search(R"(^dns-domain|dns-search\s+(.*))");
                        replace(dns_server.begin(), dns_server.end(), ',', ' ');
                        replace(search.begin(), search.end(), ',', ' ');
                        while (std::getline(file, line)) {
                            line = std::regex_replace(line, delete_dns, "dns-nameservers "+dns_server);
                            line = std::regex_replace(line, delete_search, "dns-search "+search);
                            temp_file<<line<<std::endl;
                        }
                        file.close();
                        temp_file.close();
                        std::filesystem::copy(temp_filepath, filepath, std::filesystem::copy_options::update_existing);
                    }
                    else {
                        std::string filepath = std::string("/etc/network/interfaces");
                        std::string temp_filepath =  std::string("/tmp/interfaces");
                        std::ifstream file;
                        std::ofstream temp_file;
                        file.open(filepath);
                        temp_file.open(temp_filepath);
                        std::string line;
                        std::regex delete_dns(R"(^dns-nameservers\s+(.*))");
                        std::regex delete_search(R"(^dns-domain|dns-search\s+(.*))");
                        replace(dns_server.begin(), dns_server.end(), ',', ' ');
                        replace(search.begin(), search.end(), ',', ' ');
                        while (std::getline(file, line)) {
                            line = std::regex_replace(line, delete_dns, "dns-nameservers "+dns_server);
                            line = std::regex_replace(line, delete_search, "dns-search "+search);
                            temp_file<<line<<std::endl;
                        }
                        file.close();
                        temp_file.close();
                        std::filesystem::copy(temp_filepath, filepath, std::filesystem::copy_options::update_existing);
                    }
                }
            }
            else  { // /etc/resolv.conf is not managed by resolvconf
                std::vector<std::string> nameservers = split(dns_server, ",");
                std::vector<std::string> search_domains = split(search, ",");
                std::ofstream resolv_conf;
                resolv_conf.open("/etc/resolv.conf");
                for(int i = 0; i < nameservers.size(); i++) {
                    resolv_conf << "nameserver "<<nameservers[i]<<std::endl;
                }
                if (search != "") {
                    resolv_conf << "search";
                    for(int i = 0; i < search_domains.size(); i++) {
                        resolv_conf << " "<<search_domains[i];
                    }
                    resolv_conf<<std::endl;
                }
                resolv_conf.close();
            }
        }
        else if (check_networkManager(interface.path().filename())) {
            std::vector<std::string> nameservers = split(dns_server, ",");
            std::string ipv4_nameserver;
            std::string ipv6_nameserver;
            for(int i = 0; i < nameservers.size(); i++) {
                if(is_ipv4_address((nameservers[i]))) {
                    ipv4_nameserver += nameservers[i]+",";
                }
                else if (is_ipv6_address(nameservers[i])) {
                    ipv6_nameserver += nameservers[i]+",";
                }
            }
            if (ipv4_nameserver != "") {
                std::string ipv4_command = "/usr/bin/nmcli conn modify "+connection+" ipv4.dns \""+ipv4_nameserver+"\"";
                if (search != "") {
                    ipv4_command += " ipv4.dns-search \""+search+"\"";
                }
                const char* command = ipv4_command.c_str();
                std::system(command);
            }
            if (ipv6_nameserver != "") {
                std::string ipv6_command = "/usr/bin/nmcli conn modify "+connection+" ipv6.dns \""+ipv6_nameserver+"\"";
                if (search != "") {
                    ipv6_command += " ipv6.dns-search \""+search+"\"";
                }
                const char* command = ipv6_command.c_str();
                std::system(command);
            }

        }
        else if (check_systemd_networkd(interface.path().filename())) {
            if (std::filesystem::is_symlink("/etc/resolv.conf")) { //etc/resolv.conf is managed by resolvconf or systemd-resolved
                std::string temp_command = "/usr/bin/grep -l "+interface_name+" /etc/systemd/network/*.network";
                const char* command = temp_command.c_str();
                std::string dns_file = exec(command);
                replace(dns_file.begin(), dns_file.end(), '\n', ' ');
                dns_file.pop_back();
                std::string search_command = "/usr/bin/grep -q Domains= "+dns_file;
                int search_found = system(search_command.c_str());
                std::string filename = std::filesystem::path(dns_file).filename();
                std::string temp_filepath =  std::string("/tmp/") + filename;
                std::ifstream file;
                std::ofstream temp_file;
                file.open(dns_file);
                temp_file.open(temp_filepath);
                std::string line;
                int line_counter = 0;
                int first_match = 0;
                std::regex delete_dns(R"(^DNS=(.*))");
                std::regex delete_search(R"(^Domains=(.*))");
                std::vector<std::string> nameservers = split(dns_server, ",");
                replace(search.begin(), search.end(), ',', ' ');
                std::string dns_lines;
                int i = 0;
                for (auto it = nameservers.begin(); it != nameservers.end(); it++) {
                    dns_lines += "DNS="+it[i];
                    if (std::next(it) != nameservers.end()) {
                        dns_lines += "\n";
                    }
                }
                while (std::getline(file, line)) {
                    line_counter++;
                    if (std::regex_match(line, delete_dns)) {
                        if (first_match == 0) {
                            first_match = line_counter;
                            line = std::regex_replace(line, delete_dns, dns_lines);
                            if (search_found != 0 and search != "") {
                                line += "\nDomains="+search;
                            }
                        }
                        else {
                            continue;
                        }
                    }
                    if (search != "" and search_found == 0) {
                        line = std::regex_replace(line, delete_search, "Domains="+search);
                    }
                    temp_file<<line<<std::endl;
                }
                file.close();
                temp_file.close();
                std::filesystem::copy(temp_filepath, dns_file, std::filesystem::copy_options::update_existing);
            }
            else { // /etc/resolv.conf is not managed by resolvconf or systemd-resolved
                std::vector<std::string> nameservers = split(dns_server, ",");
                std::vector<std::string> search_domains = split(search, ",");
                std::ofstream resolv_conf;
                resolv_conf.open("/etc/resolv.conf");
                for(int i = 0; i < nameservers.size(); i++) {
                    resolv_conf << "nameserver "<<nameservers[i]<<std::endl;
                }
                if (search != "") {
                    resolv_conf << "search";
                    for(int i = 0; i < search_domains.size(); i++) {
                        resolv_conf << " "<<search_domains[i];
                    }
                    resolv_conf<<std::endl;
                }
                resolv_conf.close();
            }
        }
        else if (check_netplan(interface.path().filename())) {
            std::cout<<"Interface wird  von netplan verwaltet"<<std::endl;
            if (std::filesystem::is_symlink("/etc/resolv.conf")) { //etc/resolv.conf is managed by resolvconf or systemd-resolved
                std::string interface_command = "/usr/bin/grep -q "+interface_name+" /etc/netplan/*.yaml";
                int interface_found = std::system(interface_command.c_str());
                if (interface_found == 0) { //yaml-file for this interface exists
                    std::string netplan_delete_server = "/usr/sbin/netplan set \"network.ethernets."+interface_name+".nameservers.addresses=null\"";
                    std::system(netplan_delete_server.c_str());
                    std::string netplan_add_server = "/usr/sbin/netplan set \"network.ethernets."+interface_name+".nameservers.addresses=["+dns_server+"\"]";
                    std::system(netplan_add_server.c_str());

                    std::string netplan_delete_search = "/usr/sbin/netplan set \"network.ethernets."+interface_name+".nameservers.search=null\"";
                    std::system(netplan_delete_search.c_str());
                    std::string netplan_add_search = "/usr/sbin/netplan set \"network.ethernets."+interface_name+".nameservers.search=["+search+"\"]";
                    std::system(netplan_add_search.c_str());
                    std::system("/usr/sbin/netplan apply >> /dev/null 2>&1");
                }
            }
            else { // /etc/resolv.conf is not managed by resolvconf or systemd-resolved
                std::vector<std::string> nameservers = split(dns_server, ",");
                std::vector<std::string> search_domains = split(search, ",");
                std::ofstream resolv_conf;
                resolv_conf.open("/etc/resolv.conf");
                for(int i = 0; i < nameservers.size(); i++) {
                    resolv_conf << "nameserver "<<nameservers[i]<<std::endl;
                }
                if (search != "") {
                    resolv_conf << "search";
                    for(int i = 0; i < search_domains.size(); i++) {
                        resolv_conf << " "<<search_domains[i];
                    }
                    resolv_conf<<std::endl;
                }
                resolv_conf.close();
            }
        }
    }
    setuid(current_uid);
    return 0;
}

