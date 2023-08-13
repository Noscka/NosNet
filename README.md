# NosNet

This is the descendant of https://github.com/Noscka/Cpp-Networking

## What is NosNet?
NosNet is a communications platform framework (probably not the right term) with security and privacy in mind. The base idea is to give the user's all the power by not having a middle man server, the platform will allow for multiple ways for the users to communicate. The currently planned ways are:
 - Direct - The most simple yet most secure, you connect directly to the person you want to talk. Anything you send will only be seen by you and the other person, and this could allow for virtually unlimited file size sending
 - User Hosted Server - this can be seen as a user hosted friend group chat. This will work by having 1 user host a server which others can connect to, this would allow for multiple users to connect to a private single server.
 - Dedicated Server - A server which is meant to be more permanent (kind of like discord servers), like for big communities. There will be both official and user-hosted servers. This server will allow for more indepth configuration then that User hosted servers, with options such as "displaying public", "invite only" and etc.

this would be done on the [DNICC](#namings) client, so the front face of the platform. The platform will also have a centralized "address book" type server ([DCHLS](#namings)). the centalized server will contain entries for joinable servers, I will have to look over and see speficially what this server should track, so for example if it should have entries of private servers even if they won't get displayed. 

## Namings
DCHLS -> Dynamic Central Host Lookup Server  
DNICC -> Direct Non-Interfered Communications Client

## Planned Features
 - Encryption - The most important feature if this were to ever release, I have tried but couldn't figure out end-to-end encryption using boost.asio
 - actually implementing the communication methods, currently there is only the very basic Dedicated Server

## Libraries used
[NosLib](https://github.com/Noscka/NosLib)  
[Boost](https://www.boost.org/) (specifically asio)  
[QT](https://www.qt.io/download)  

Qt Build command - JUST SO I REMEMBER HOW TO BUILD STATIC
```
configure.bat -release -opensource -confirm-license -prefix "D:\Libraries\Qt\6.6.0\Build" -nomake examples -nomake tests -skip qtwebengine & cmake --build . --parallel & cmake --install .
```
