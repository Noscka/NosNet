# NosNet

This is the descendant of https://github.com/Noscka/Cpp-Networking

## What is NosNet?
NosNet is a communications platform framework (probably not the right term) with security and privacy in mind. The base idea is to give the user's all the power by not having a middle man server, the platform will allow for multiple ways for the users to communicate. The currently planned ways are:
 - Direct - The most simple yet most secure, you connect directly to the person you want to talk. Anything you send will only be seen by you and the other person, and this could allow for virtually unlimited file size sending
 - (Group Servers) User Hosted Server - this can be seen as a user hosted friend group chat. This will work by having 1 user host a server which others can connect to, this would allow for multiple users to connect to a private single server.
 - Dedicated Server - A server which is meant to be more permanent (kind of like discord servers), like for big communities. There will be both official and user-hosted servers. This server will allow for more indepth configuration then that User hosted servers, with options such as "displaying public", "invite only" and etc.

this would be done on the [DNICC](#namings) client, so the front face of the platform. The platform will also have a centralized "address book" type server ([DCHLS](#namings)). the centalized server will contain entries for joinable servers, I will have to look over and see speficially what this server should track, so for example if it should have entries of private servers even if they won't get displayed. 

## Role of DCHLS
this section will explain further what DCHLS does. DCHLS is meant to be an address book of the whole network, it will contain an entry for every server on the network (this I will need to look over, DHCLS contain every server or only server which have the "public" flag set to true), so every server will try to register itself with DCHLS upon creation. This is done so [DNICC](#role-of-dnicc), so the users, can more easily find server to join. DCHLS will only send servers with the "public" flag set to true to the [DNICC](#role-of-dnicc) requester. 

## Role of DNICC
this section will explain further what DNICC does. DNICC is meant to be the front face of the whole platform/network. DNICC allows for both joining severs, and hosting servers (only direct and group). upon starting, DNICC requests the server registry (an array of server entries). If the user decides to join a server, DNICC attempts to join the server directly. If the user inturn decides to host their own server (Only Direct and Group servers, Dedicated servers will require a seperate console program), DNICC will register itself with [DCHLS](#role-of-dchls) using the information the user has provided.

## Namings
DCHLS -> Dynamic Central Host Lookup Server  
DNICC -> Direct Non-Interfered Communications Client

## Planned Features
 - Encryption - The most important feature if this were to ever release, I have tried but couldn't figure out end-to-end encryption using boost.asio
 - actually implementing the communication methods, currently there is only the very basic Dedicated Server
 - Make sure the project is cross platform
 - Dedicated servers
 - Manual ip connection, so if the [DCHLS](#role-of-dchls) server goes down for whatever reason, the network is still usable
 - Custom community hosted [DCHLS](#role-of-dchls) servers, so the official [DCHLS](#role-of-dchls) servers goes down (project abandoned or whatever), the community can still use the whole network

## Libraries used
[NosLib](https://github.com/Noscka/NosLib)  
[Boost](https://www.boost.org/) (specifically asio)  
[QT](https://www.qt.io/download)  

Qt Build command
```
mkdir build && cd build
..\configure.bat -release -prefix "D:\Libraries\Qt\6.6.0\Build" && cmake --build . --parallel && cmake --install .
```
Add Boost Build command (need to find it)
