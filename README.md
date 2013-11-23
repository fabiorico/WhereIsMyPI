WhereIsMyPI
===========

Quickly find the IP address of the Raspberry PI in a network with DHCP

Installing and Compilling
----------

This code is intended to be used in a Linux terminal, please follow the steps bellow:

* 1 Clone the git repo
```
cd /home/username/someFolder
git clone https://github.com/fabiorico/WhereIsMyPI
```
* 2 Compile the code
```
cd WhereIsMyPI
gcc WhereIsMyPI.c -o WhereIsMyPI

```
Searching for the Raspberry PI
----------
```
./WhereIsMyPI eth0 #eth0 is the network interface
``` 
