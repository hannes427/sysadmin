#include <iostream>
#include <fstream>
#include <regex>
#include <filesystem>
#include <boost/program_options.hpp>


int main ( int argc, char **argv) {
    std::string oldhostname;
    std::string olddomainname;
    std::string hostname;
    std::string domainname;
    int current_uid = getuid();
    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
        ("help,h", "Help screen")
        ("oldhostname", boost::program_options::value<std::string>(), "Old Hostname")
        ("olddomainname", boost::program_options::value<std::string>(), "Old Domainname")
        ("hostname", boost::program_options::value<std::string>(), "New Hostname")
        ("domainname", boost::program_options::value<std::string>(), "New Domainname");

        boost::program_options::variables_map vm;
        boost::program_options::command_line_style::allow_long_disguise;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';

        }
        if (vm.count("oldhostname")) {
            oldhostname = vm["oldhostname"].as<std::string>();
        }
        if (vm.count("olddomainname")) {
            olddomainname = vm["olddomainname"].as<std::string>();
        }
        if (vm.count("hostname")) {
            hostname = vm["hostname"].as<std::string>();
        }
        if (vm.count("domainname")) {
            domainname = vm["domainname"].as<std::string>();
        }
    }
    catch (const boost::program_options::error &ex) {
        std::cerr << ex.what() << '\n';
    }
    std::regex oldhostnamePattern(oldhostname);
    std::regex olddomainnamePattern(olddomainname);
    std::regex hostnameRegex(R"(^[a-zA-Z0-9][a-zA-Z0-9\-]*$)");
    std::regex domainnameRegex(R"(^[a-zA-Z0-9][a-zA-Z0-9\-.]*[a-zA-Z0-9]$)");

    if (!std::regex_match(hostname, hostnameRegex)) {
        std::cerr<<std::endl<<"Error! Invalid Hostname! Valid characters are a-z, numbers and the hyphen-minus ('-'). Hostname must not start with a hyphen."<<std::endl<<std::endl;
        return 1;
    }
    if (domainname != "" && !std::regex_match(domainname, domainnameRegex)) {
        std::cerr<<std::endl<<"Error! Invalid Domainname! Valid characters are a-z, numbers, the period and the hyphen-minus ('-'). Domainname must not start with a hyphen nor start or end with a period."<<std::endl<<std::endl;
        return 1;
    }
    std::string hostnamectl = "/usr/bin/hostnamectl --static hostname "+hostname;
    const char* command = hostnamectl.c_str();
    if (setuid(0)) { //I am now root!
        perror("setuid");
        return 1;
    }
    if (hostname != "") {
        std::system(command);
    }

    try {
        std::string line;
        std::ifstream hosts;
        std::ofstream hosts_temp;
        hosts.open("/etc/hosts");
        hosts_temp.open("/tmp/hosts");
        while(getline(hosts, line)) {
            if (hostname != oldhostname && hostname != "") {
                line = std::regex_replace(line, oldhostnamePattern, hostname);
            }
            if (domainname != olddomainname && domainname != "") {
                line = std::regex_replace(line, olddomainnamePattern, domainname);
            }
            hosts_temp << line <<std::endl;;
        }
        hosts.close();
        hosts_temp.close();
        std::filesystem::copy("/tmp/hosts", "/etc/hosts", std::filesystem::copy_options::update_existing);
    } catch (std::filesystem::filesystem_error& e) {
        std::cout << e.what() << '\n';
    }
    setuid(current_uid); //return to previous user
    return 0;
}


