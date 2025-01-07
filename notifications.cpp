#include <iostream>
#include <filesystem>
#include <fstream>
#include "functions.hpp"
#include <boost/program_options.hpp>

std::string mdadm_path;
std::string from_address;
std::string storage_address;
int current_uid = getuid();

int create_mdadm_config (std::string from_address, std::string storage_address, std::string mdadm_path) {
    std::string line;
    bool found_from = false;
    bool found_storage = false;
    std::ifstream mdadm_input_file;
    std::ofstream mdadm_output_temp;
    if (setuid(0)) { //I am now root!
        perror("setuid");
        return 1;
    }
    try {
        mdadm_input_file.open(mdadm_path);
        mdadm_output_temp.open("/tmp/mdadm.conf");
        while(getline(mdadm_input_file, line)) {
            if (line.find("MAILFROM") != std::string::npos) {
                line = "MAILFROM " + from_address;
                found_from = true;
            }
            else if (line.find("MAILADDR") != std::string::npos) {
                line = "MAILADDR " + storage_address;
                found_storage = true;
            }
            mdadm_output_temp << line <<std::endl;
        }
        if(!found_from) {
            mdadm_output_temp << "MAILFROM " << from_address << std::endl;
        }
        if(!found_storage) {
            mdadm_output_temp << "MAILADDR " << storage_address << std::endl;
        }
        mdadm_input_file.close();
        mdadm_output_temp.close();
        std::filesystem::copy("/tmp/mdadm.conf", mdadm_path, std::filesystem::copy_options::update_existing);
        std::remove("/tmp/mdadm.conf");
    }
    catch (std::filesystem::filesystem_error& e) {
        std::cout << e.what() << '\n';
    }
    setuid(current_uid); //return to previous user
    return true;
}

int main (int argc, char **argv) {
    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
        ("help,h", "Help screen")
        ("from-address", boost::program_options::value<std::string>(), "Sender e-mail address")
        ("storage-address", boost::program_options::value<std::string>(), "Storage e-mail address");

        boost::program_options::variables_map vm;
        boost::program_options::command_line_style::allow_long_disguise;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';

        }
        if (vm.count("from-address")) {
            from_address = vm["from-address"].as<std::string>();
        }
        if (vm.count("storage-address")) {
            storage_address = vm["storage-address"].as<std::string>();
        }
    }
    catch (const boost::program_options::error &ex) {
        std::cerr << ex.what() << '\n';
    }
    std::string mdadm_command = "/usr/bin/dpkg -L mdadm >> /dev/null 2>&1";
    int check_mdadm = system(mdadm_command.c_str());
    if (check_mdadm == 0) {
        if (std::filesystem::exists("/etc/mdadm") && std::filesystem::is_directory("/etc/mdadm")) {
            mdadm_path = "/etc/mdadm/mdadm.conf";
        }
        else {
            mdadm_path = "/etc/mdadm.conf";
        }
     }
     create_mdadm_config(from_address, storage_address, mdadm_path);
    return 0;
}
