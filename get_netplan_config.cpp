#include <iostream>
#include <vector>
#include "functions.hpp"
#include <boost/program_options.hpp>

int main (int argc, char **argv) {
    std::string interface;
    int current_uid = getuid();
    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
        ("help,h", "Help screen")
        ("interface", boost::program_options::value<std::string>()->required(), "Name of the interface (e.g. eth0)");

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
    }
    catch (const boost::program_options::error &ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }

    return 0;
}
