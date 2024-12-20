#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <regex>
#include <filesystem>
#include <boost/program_options.hpp>
#include <signal.h>
#include "functions.hpp"

const std::string MODULE = "packetcapture";

int startcapture (int argc, char **argv) {
    if(!check_auth(MODULE)) {
         std::cerr<<std::endl<<"Error! Authentication failed! Did you run this program from the command line?"<<std::endl<<std::endl;
         return 1;
    }
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss <<"dump-"<< std::put_time(&tm, "%d-%m-%Y-%H-%M-%S")<<".pcap";
    auto filename = oss.str();
    int current_uid = getuid();
    std::string tcpdump = "/usr/bin/tcpdump";
    std::string arguments = "-w /tmp/"+filename+" -U -n -e -tttt ";
    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
        ("help,h", "Help screen")
        ("count-bytes,c", boost::program_options::value<std::string>(), "Exit after receiving <arg> packets")
        ("maxsize,C", boost::program_options::value<std::string>(), "Maximum size of dump-file. If filesize is larger, a new file is created.")
        ("interface,i", boost::program_options::value<std::string>(), "Interface to listen on");

        boost::program_options::variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);

        notify(vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';
        }
        if (vm.count("count-bytes")) {
            arguments = arguments+"-c "+vm["count-bytes"].as<std::string>()+" ";
        }
        if (vm.count("maxsize")) {
            arguments = arguments+"-C "+vm["maxsize"].as<std::string>()+" ";
        }
        if (vm.count("interface")) {
            arguments = arguments+"-i "+vm["interface"].as<std::string>()+" ";
        }
    }
    catch (const boost::program_options::error &ex) {
        std::cerr << ex.what() << '\n';
    }
    tcpdump = tcpdump + " " + arguments + " > /dev/null 2>&1&";
    const char* command = tcpdump.c_str();
    if (setuid(0)) { //I am now root!
        perror("setuid");
        return 1;
    }
    std::system(command);
    setuid(current_uid); //return to previous user
    return 0;
}
int stopcapture (int pid) {
    std::regex processPattern(R"(^\/usr\/bin\/tcpdump)");
    std::string line;
    //Check if PID is a valid tcpdump process
     std::ifstream process;
     if (!std::filesystem::exists("/proc/"+std::to_string(pid)+"/cmdline")) {
          std::cerr<<std::endl<<"Error! Could not open process file! Is "<<pid<<" a valid PID?"<<std::endl<<std::endl;
          return 1;
     }
     process.open("/proc/"+std::to_string(pid)+"/cmdline");
     while(getline(process, line)) {
        std::smatch match;
        if (std::regex_search(line, match, processPattern)) { // PID is a valid tcpdump process
             kill(pid, SIGTERM);
        }
        else { //PID is not a tcpdump PID, possible misusage
            std::cerr<<std::endl<<"Error! PID " <<pid<<" is not a valid PID!"<<std::endl<<std::endl;
            return 1;
        }
     }
    return 0;
}
int main (int argc, char **argv) {
    if (argc < 2) {
        std::cerr<<std::endl<<"Error! Usage: "<<argv[0]<<" startcapture || stopcapture"<<std::endl<<std::endl;
        return 1;
    }
    else if(argv[1] == std::string("stopcapture") && argc < 3) {
        std::cerr<<std::endl<<"Error! Usage: "<<argv[0]<<" stopcapture PID"<<std::endl<<std::endl;
        return 1;
    }
    if (argv[1] == std::string("startcapture")) {
        startcapture(argc, argv);
    }
    else if (argv[1] == std::string("stopcapture")) {
        int pid = atoi(argv[2]);
        stopcapture(pid);
    }
    return 0;
}
