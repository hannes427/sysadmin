#include <iostream>
#include <filesystem>
#include <regex>
#include <boost/program_options.hpp>

std::string device;
std::string raid_type; // Which program manages the raid (i.e. mdadm, lvm...)
std::string device_details;
int current_uid = getuid();

std::string fetch_md_raid (std::string device);

int main (int argc, char **argv) {
    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
        ("help,h", "Help screen")
        ("device", boost::program_options::value<std::string>()->required(), "Raid Device")
        ("raid_type", boost::program_options::value<std::string>()->required(), "Raid type (i.e. mdadm)");

        boost::program_options::variables_map vm;
        boost::program_options::command_line_style::allow_long_disguise;
        store(parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';
            return 0;
        }

        notify(vm);

        if (vm.count("device")) {
            device = vm["device"].as<std::string>();
        }
        if (vm.count("raid_type")) {
            raid_type = vm["raid_type"].as<std::string>();
        }
    }
    catch (const boost::program_options::error &ex) {
        std::cerr << ex.what() << '\n';
    }
    if (raid_type == "mdadm") {
        std::regex deviceRegex(R"(^\/dev\/md[0-9]+$)");
        if (!std::regex_match(device, deviceRegex) || !std::filesystem::exists(device)) {
            std::cerr<<std::endl<<"Error! Invalid Device "<<device<<"!"<<std::endl<<std::endl;
            return 1;
        }
        if (!std::filesystem::exists("/usr/sbin/mdadm")) {
            std::cerr<<std::endl<<"Error! mdadm executable not found. Please install mdadm!"<<std::endl<<std::endl;
            return 1;
        }
        if (!std::filesystem::exists("/usr/bin/jc")) {
            std::cerr<<std::endl<<"Error! /usr/bin/jc is rquired but was not found. Please install jc first!"<<std::endl<<std::endl;
            return 1;
        }
        if (setuid(0)) { //I am now root!
            perror("setuid");
            return 1;
        }
        device_details = fetch_md_raid (device);
        setuid(current_uid);
    }
    std::cout<<device_details<<std::endl;
    return 0;
}
