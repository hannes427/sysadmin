#include <iostream>
#include <unistd.h>

int current_uid = getuid();

int reboot () {
    if (setuid(0)) { //I am now root!
        perror("setuid");
        return 1;
    }
    std::string reboot_cmd = "/usr/bin/systemctl reboot";
    system(reboot_cmd.c_str());
    setuid(current_uid);
    return(0);
}
int shutdown () {
     if (setuid(0)) { //I am now root!
        perror("setuid");
        return 1;
    }
    std::string reboot_cmd = "/usr/bin/systemctl poweroff";
    system(reboot_cmd.c_str());
    setuid(current_uid);
    return(0);
}
int main (int argc, char **argv) {
    if (argc < 2) {
        std::cerr<<std::endl<<"Error! Usage: "<<argv[0]<<" reboot || shutdown"<<std::endl<<std::endl;
        return 1;
    }
    if (argv[1] == std::string("reboot")) {
        reboot();
    }
    else if (argv[1] == std::string("shutdown")) {
        shutdown();
    }
    else {
         std::cerr<<std::endl<<"Error! Usage: "<<argv[0]<<" reboot || shutdown"<<std::endl<<std::endl;
        return 1;
    }
    return 0;
}
