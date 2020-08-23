 //storrentc.cpp : This file contains the 'main' function. Program execution begins and ends there.
 #include <storrent/storrentlib.hpp>
#include <iostream>


int main(int argc, char** argv)
{
    std::string filename("C:\\Users\\JoeVidunas\\source\\repos\\stoked-torrent\\data\\Ghost in the Shell (2017) "
                         "[BluRay] [720p] [YTS.AM] - Copy.torrent");
     std::cout << "Hello World!\n";
     std::cout << "download: " << storrent::start_session(filename) << std::endl;
     // std::cout << "called function: " << storrent::start_session() << std::endl;
    ///*torrent_download download(filename);
    //download.start();*/
    ///*torrent_session session;
    //session.start(filename);
    //auto progress = session.get_download_progress();*/


    ///*boost::asio::io_context io_context(1);
    //boost::asio::ip::udp::socket sock(io_context);
    //boost::asio::ip::udp::resolver res(sock.get_executor());*/
    //boost::asio::io_context io_context(1);
    //boost::asio::ip::udp::resolver res(io_context);
    //boost::system::error_code err;
    //auto results = res.resolve("tracker.coppersurfer.tk",
    //                           "6969",
    //                           boost::asio::ip::udp::resolver::all_matching,
    //                           err);
    //if (!err)
    //{
    //    for (auto& i : results)
    //    {
    //        std::cout << "ep:" << i.endpoint() << std::endl;
    //    }
    //}
    //else
    //{
    //    std::cout << "error: " << err.message() << std::endl;
    //}
    //

    //return 1;
}
//
//#include <sdkddkver.h>
//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//#include <iostream>
//#include <queue>
//#include <string>
//#include <thread>
//#include <functional>
//#include <stdexcept>
// 
//class TcpConnection
//{
//public:
//    TcpConnection(const std::string& host, const std::string& port)
//        : m_host(host), m_port(port), m_socket(m_io_service), m_work(m_io_service)
//    {
//    }
//
//    void connect()
//    {
//        try
//        {
//            boost::asio::ip::tcp::resolver resolver(m_io_service);
//            boost::asio::ip::tcp::resolver::query query(m_host, m_port);
//            boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
//
//            boost::asio::connect(m_socket, endpoint_iterator);
//
//            boost::asio::async_read_until(m_socket,
//                                          m_buffer,
//                                          "\r\n",
//                                          std::bind(&TcpConnection::read_handler,
//                                                    this,
//                                                    std::placeholders::_1,
//                                                    std::placeholders::_2));
//
//            run_thread = std::thread([this]() { this->m_io_service.run(); });
//        }
//        catch (std::exception& e)
//        {
//            throw std::runtime_error(e.what());
//        }
//    }
//
//    void disconnect()
//    {
//        m_socket.close();
//        m_io_service.stop();
//    }
//
//    void read_handler(const boost::system::error_code& error, std::size_t bytes_transferred)
//    {
//        // Need to handle errors...
//
//        // Prepare m_buffer for output
//        std::istream is(&m_buffer);
//        std::string line;
//        std::getline(is, line);
//        line += "\n";
//        m_recv_queue.push(line);
//
//        if (error.value() != 0)
//        {
//            std::cout << error.message();
//            return;
//        }
//
//        // Read until next \r\n
//        boost::asio::async_read_until(m_socket,
//                                      m_buffer,
//                                      "\r\n",
//                                      std::bind(&TcpConnection::read_handler,
//                                                this,
//                                                std::placeholders::_1,
//                                                std::placeholders::_2));
//    }
//
//    void write_handler(const boost::system::error_code& error, std::size_t bytes_transferred) {}
//
//    void send(const std::string& message)
//    {
//        boost::asio::async_write(m_socket,
//                                 boost::asio::buffer(message),
//                                 std::bind(&TcpConnection::write_handler,
//                                           this,
//                                           std::placeholders::_1,
//                                           std::placeholders::_2));
//    }
//
//    const std::string& get_next_message()
//    {
//        message = m_recv_queue.front();
//        m_recv_queue.pop();
//        return message;
//    }
//
//    bool has_message() { return !m_recv_queue.empty(); }
//
//private:
//    std::string m_host;
//    std::string m_port;
//    std::queue<std::string> m_recv_queue;
//    std::thread run_thread;
//    std::string message;
//
//    boost::asio::streambuf m_buffer;
//    boost::asio::io_context m_io_service;
//    boost::asio::io_service::work m_work;
//    boost::asio::ip::tcp::socket m_socket;
//};
//
//int main()
//{
//    TcpConnection connection("irc.quakenet.org", "6667");
//    connection.connect();
//    bool joined = false;
//
//    while (true)
//    {
//        while (connection.has_message())
//        {
//            std::string message = connection.get_next_message();
//            if (message.find("NOTICE AUTH :*** Found your hostname") != std::string::npos)
//            {
//                connection.send("NICK ResidentBot\r\n");
//                connection.send("USER RBot 0 * :RBot\r\n");
//            }
//
//            if (message.find("PING") == 0)
//            {
//                // Reply to server PING requests
//                std::string pong = "PONG" + message.substr(4, std::string::npos);
//                connection.send(pong);
//                if (!joined)
//                {
//                    connection.send("JOIN #cplusplus\r\n");
//                    joined = true;
//                }
//            }
//
//            if (message != "")
//                std::cout << message;
//        }
//    }
//
//    return 0;
//}