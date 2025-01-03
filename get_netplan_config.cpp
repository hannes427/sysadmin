#include <iostream>
#include <vector>
#include "functions.hpp"
#include <boost/program_options.hpp>

std::string interface;
std::string dhcp4;
std::string dhcp6;
std::string ip_address;
std::string routes;
std::string accept_ra;
bool check_interface = false;
int current_uid = getuid();

void replaceAll (std::string& str, const std::string& from, const std::string& to) {
    if(from.empty()) {
        return;
    }
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

int check_configured (std::string interface) {
    int is_configured = 0;
    std::string check_cmd = "/usr/sbin/netplan get network.ethernets."+interface;
    std::string check = exec(check_cmd.c_str());
    check.erase(std::remove(check.begin(), check.end(), '\n'), check.end());
    if (check == "null") {
        is_configured = 1;
    }

    return is_configured;
}

int main (int argc, char **argv) {
    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
        ("help,h", "Help screen")
        ("interface", boost::program_options::value<std::string>()->required(), "Name of the interface (e.g. eth0)")
        ("check_configured", "Check if configuration for this interface exists)");

        boost::program_options::variables_map vm;
        boost::program_options::command_line_style::allow_long_disguise;
        store(parse_command_line(argc, argv, desc), vm);

        notify(vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';
        }
        if (vm.count("interface")) {
            interface = vm["interface"].as<std::string>();
        }
        if (vm.count("check_configured")) {
            check_interface = true;
            int result = check_configured (interface);
            std::cout<<result<<std::endl;
        }
    }
    catch (const boost::program_options::error &ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
     if (setuid(0)) { //I am now root!
        perror("setuid");
        return 1;
    }
    if (!check_interface) {
    std::string dhcp4_cmd = "/usr/sbin/netplan get network.ethernets."+interface+".dhcp4";
    dhcp4 = exec(dhcp4_cmd.c_str());
    //remove unwanted chars from string (last \n, -, spaces and '"')
    dhcp4.pop_back();
    dhcp4.erase(std::remove(dhcp4.begin(), dhcp4.end(), '-'), dhcp4.end());
    dhcp4.erase(std::remove(dhcp4.begin(), dhcp4.end(), ' '), dhcp4.end());
    dhcp4.erase(std::remove(dhcp4.begin(), dhcp4.end(), '"'), dhcp4.end());
    replaceAll(dhcp4, "\n", ", ");
    std::string dhcp6_cmd = "/usr/sbin/netplan get network.ethernets."+interface+".dhcp6";
    dhcp6 = exec(dhcp6_cmd.c_str());
   //remove unwanted chars from string (last \n, -, spaces and '"')
    dhcp6.pop_back();
    dhcp6.erase(std::remove(dhcp6.begin(), dhcp6.end(), '-'), dhcp6.end());
    dhcp6.erase(std::remove(dhcp6.begin(), dhcp6.end(), ' '), dhcp6.end());
    dhcp6.erase(std::remove(dhcp6.begin(), dhcp6.end(), '"'), dhcp6.end());
    replaceAll(dhcp6, "\n", ", ");
    std::string ip_address_cmd = "/usr/sbin/netplan get network.ethernets."+interface+".addresses";
    ip_address = exec(ip_address_cmd.c_str());
    //remove unwanted chars from string (last \n, -, spaces and '"')
    ip_address.pop_back();
    ip_address.erase(std::remove(ip_address.begin(), ip_address.end(), '-'), ip_address.end());
    ip_address.erase(std::remove(ip_address.begin(), ip_address.end(), ' '), ip_address.end());
    ip_address.erase(std::remove(ip_address.begin(), ip_address.end(), '"'), ip_address.end());
    replaceAll(ip_address, "\n", ", ");
    std::string routes_cmd = "/usr/sbin/netplan get network.ethernets."+interface+".routes";
    routes = exec(routes_cmd.c_str());
    //remove unwanted chars from string (last \n, -, spaces and '"')
    routes.pop_back();
    routes.erase(std::remove(routes.begin(), routes.end(), '-'), routes.end());
    routes.erase(std::remove(routes.begin(), routes.end(), ' '), routes.end());
    routes.erase(std::remove(routes.begin(), routes.end(), '"'), routes.end());
    replaceAll(routes, "\n", ", ");
    std::string accept_ra_cmd = "/usr/sbin/netplan get network.ethernets."+interface+".accept-ra";
    accept_ra = exec(accept_ra_cmd.c_str());
    //remove unwanted chars from string (last \n, -, spaces and '"')
    accept_ra.pop_back();
    accept_ra.erase(std::remove(accept_ra.begin(), accept_ra.end(), '-'), accept_ra.end());
    accept_ra.erase(std::remove(accept_ra.begin(), accept_ra.end(), ' '), accept_ra.end());
    accept_ra.erase(std::remove(accept_ra.begin(), accept_ra.end(), '"'), accept_ra.end());
    replaceAll(accept_ra, "\n", ", ");
    setuid(current_uid); //return to previous user
    std::cout<<"dhcp4: "<<dhcp4<<std::endl<<"dhcp6: "<<dhcp6<<std::endl<<"ip_address: "<<ip_address<<std::endl<<"routes: "<<routes<<std::endl<<"accept_ra: "<<accept_ra<<std::endl;
    }
    return 0;
}
