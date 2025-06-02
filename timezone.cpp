#include <iostream>
#include <regex>
#include <boost/program_options.hpp>

int main (int argc, char **argv) {
    std::string time_zone;
    int current_uid = getuid();
    std::regex tzRegex (R"(^UTC$|^[A-Za-z]+\/[A-Za-z_\-]+$)");
    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
        ("help,h", "Help screen")
        ("time-zone", boost::program_options::value<std::string>()->required(), "New Time Zone");

        boost::program_options::variables_map vm;
        boost::program_options::command_line_style::allow_long_disguise;
        store(parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';
            return 0;
        }

        notify(vm);

        if (vm.count("time-zone")) {
            time_zone = vm["time-zone"].as<std::string>();
        }
    }
    catch (const boost::program_options::error &ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
    if (!std::regex_match(time_zone, tzRegex)) {
        std::cerr<<std::endl<<"Error! Invalid time zone! "<<time_zone<<std::endl<<std::endl;
        return 1;
    }
    std::string tz_cmd = "/usr/bin/timedatectl set-timezone "+time_zone;
    if (setuid(0)) { //I am now root!
        perror("setuid");
        return 1;
    }
    std::system(tz_cmd.c_str());
    setuid(current_uid); //return to previous user
    return 0;
}
