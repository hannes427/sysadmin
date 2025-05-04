# Systemadmin module
---
## What is the Systemadmin module?

Warning: The module is still in an early alpha version. Please do not install it on production machines, but only on development machines for testing/debugging.

Systemadmin is a free open source module for any FreePBX® 17 based systems (e.g. FreePBX, TangoPBX and IncrediblePBX). With this module you can change system settings of the PBX including:

* Network settings (IP assignment method, IP address etc)
* DNS settings
* Hostname/Domainname
* eMail server settings (wheter sending emails through the local or through a remote MTA).

These are the binaries needed for the actual modification of the system settings.

## Module License

This module is published under GNU AGPL 3.0

## Module Requirements

Systemadmin requires FreePBX, IncrediblePBX or TangoPBX 17. After the binaries are installed, the module for the web interface must also be installed. Installation instructions for the module can be found [Here](https://github.com/hannes427/systemadmin_php "Systemadmin module").

## How Do I install the binary files?

There are two methods to install the binaries: you can either install the deb package for the respective operating system (currently available for Debian 12 and Ubuntu 22.04) or you can compile the binaries yourself. If you want to compile it, do not compile the packages directly on the PBX system, but please use another computer (e.g., a local virtual machine). However, you will need the same operating system that is installed on your PBX system.


**Debian** (e.g. FreePBX® 17 Distro, IncrediblePBX)

To install the package, plese run the following commands in a shell

```
cd /tmp
wget https://github.com/hannes427/sysadmin/systemadmin-1.0.0-Debian12.deb
su -c 'apt install ./systemadmin-1.0.0-Debian12.deb'

```

To compile the packages yourself, run the following commands in a terminal:

```
cd /tmp
git clone https://github.com/hannes427/sysadmin
cd sysadmin
cmake CMakeLists.txt -DCMAKE_INSTALL_PREFIX=/usr/local/freepbx
cmake --build .
su -c 'mkdir /usr/local/freepbx && cmake --install . && chmod u+s /usr/local/freepbx/bin/* && cp -rp php /usr/local/freepbx && chmod +x /usr/local/freepbx/php/*.php'

```

**Ubuntu** (e.g. TangoPBX Distro)

To install the package, plese run the following commands in a shell

```
cd /tmp
wget https://github.com/hannes427/sysadmin/raw/refs/heads/master/install/systemadmin-1.0.0-Ubuntu22-04.deb
sudo apt install ./systemadmin-1.0.0-Ubuntu22-04.deb

```

To compile the packages yourself, run the following commands in a terminal:

```
sudo apt install git cmake libboost-program-options-dev g++ libodb-mysql-dev wget sed
cd /tmp
git clone https://github.com/hannes427/sysadmin
cd sysadmin
sed -i 's/mariadb/mysql/' functions.cpp
cmake CMakeLists.txt -DCMAKE_INSTALL_PREFIX=/usr/local/freepbx
cmake --build .
sudo mkdir /usr/local/freepbx && sudo cmake --install . && sudo chmod u+s /usr/local/freepbx/bin/* && sudo cp -rp php /usr/local/freepbx && sudo chmod +x /usr/local/freepbx/php/*.php

```

**All Versions**

Please remember to install the module for the web interface (link for the install instructions above) after these steps!


