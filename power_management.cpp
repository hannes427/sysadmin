#include <iostream>
#include <unistd.h>
#include <sys/reboot.h>

int current_uid = getuid();

int reboot () {
    sync();
    if (setuid(0)) { //I am now root!
        perror("setuid");
        return 1;
    }
  reboot(RB_AUTOBOOT);
  setuid(current_uid);
  return(0);
}
int shutdown () {
     sync();
     if (setuid(0)) { //I am now root!
        perror("setuid");
        return 1;
    }
     reboot(RB_POWER_OFF);
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
