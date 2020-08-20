// storrentc.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00
#include <boost/asio.hpp>
#include <iostream>
#include <storrent/storrentlib.hpp>

int main(int argc, char** argv)
{
    std::string filename("");
     std::cout << "Hello World!\n";
     std::cout << "download: " << storrent::download_file(filename) << std::endl;
     // std::cout << "called function: " << storrent::download_file() << std::endl;
    /*torrent_download download(filename);
    download.start();*/
    /*torrent_session session;
    session.start(filename);
    auto progress = session.get_download_progress();*/


    /*boost::asio::io_context io_context(1);
    boost::asio::ip::udp::socket sock(io_context);
    boost::asio::ip::udp::resolver res(sock.get_executor());*/

    return 1;
}
