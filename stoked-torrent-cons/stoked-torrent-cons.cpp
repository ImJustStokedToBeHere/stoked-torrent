
#include <utils.h>
#include <sys.h>
#include <stoked_torrent.h>
#include <torrent.h>
#include <session.h>
#include <iostream>
#include <string>
#include <vector>


#ifdef UNICODE
int wmain(int argc, wchar_t* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    /*std::cout << TEXT("parameters=================================") << std::endl;
    int i = 0;
    std::vector<std::wstring> params {argv, argv + argc - 1};
    for ( const auto &param : params) {
        std::wcout << i++ << L") " << param << std::endl;
    }
    std::cout << TEXT("===========================================") << std::endl << std::endl;
*/
    int res = stoked::setup_stoked_torrent();
    std::vector<std::string> params{argv, argv + argc - 1};
    stoked::session_settings settings{
        params[1],
        params[2],
        params[3] == "true" || params[3] == "1" ? true : false,
        0,
        false,
        false,
        0,
        0
    };

    auto sess = stoked::session{settings};
    sess.init_trackers();
    sess.gather_peers();
    // sess.start();

    // auto v =  stoked::tracker::create_tracker("udp://tracker.coppersurfer.tk:6969/announce");
    // std::cout << "interval: " << static_cast<stoked::udp_tracker *>(v.get())->announce(&sess, stoked::announce_started).interval << std::endl;

    std::string in;
    std::getline(std::cin, in);
    stoked::teardown_stoked_torrent();
}


// auto v =  stoked::tracker::create_tracker("udp://stokeddevelopment.com:50184/udp");
/*auto v =  stoked::tracker::create_tracker("udp://tracker.coppersurfer.tk:6969/announce");
std::cout << "connection_id: " << static_cast<stoked::udp_tracker *>(v.get())->get_connection_id() << std::endl;*/

// std::string s = stoked::utils::module_info::get_file_version_info_value("D:\\saucecode\\storrent\\bin\\Win32\\Debug\\console_app.exe", stoked::utils::file_version_info_value::original_filename);
//stoked::utils::module_info mi {};
//std::cout << mi.company_name() << std::endl;
//std::cout << mi.file_description() << std::endl;
//std::cout << mi.file_version() << std::endl;
//std::cout << mi.internal_name() << std::endl;
//std::cout << mi.legal_copyright() << std::endl;
//std::cout << mi.product_name() << std::endl;
//std::cout << mi.product_version() << std::endl;

//std::array<char, 20> _peer_id {};

//if (_peer_id.at(0) == '\0') {
//	stoked::utils::module_info mod {};
//	std::string ver {"-S_"};
//	ver += mod.product_version() + "-";
//	stoked::utils::replace_substr(ver, ".", "", false, false);
//	std::copy(ver.begin(), ver.end(), _peer_id.data());
//	stoked::utils::generate_random_byte_str(_peer_id.data(), ver.length(), 20 - ver.length());
//}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file



//int main(int argc, wchar_t *argv[]) {
//	using namespace storrent;
//	std::wstring in;
//	bool val = utils::get_ip_from_hostname_w(argv[1], in);
//	std::wcout << "success: " << val << " ip: " << in << std::endl;
//	std::getline(std::wcin, in);
//}


//std::array<char, 20> _peer_id {};
//
//std::array<char, 20> get_peer_id() {
//	if (_peer_id.empty()) {
//		utils::module_info mod {};
//		std::string ver {"-S_"};
//		ver += mod.original_filename() + "-";
//		utils::replace_substr(ver, ".", "-", false, false);
//		std::copy(ver.begin(), ver.end(), _peer_id.data());
//		utils::generate_random_byte_str(_peer_id.data(), ver.length(), 20 - ver.length());
//	}
//
//	return _peer_id;
//}



///******************************************************************************
//
//							  Online C++ Compiler.
//			   Code, Compile, Run and Debug C++ program online.
//Write your code in this editor and press "Run" button to compile and execute it.
//
//*******************************************************************************/
//
//#include <stdio.h>
//
//#ifdef _WIN32
//#  include "winsock.h"
//#else
//#  include <netdb.h>
//#  include <arpa/inet.h>
//#endif
//
//static void initialise(void) {
//	#ifdef _WIN32
//	WSADATA data;
//	if (WSAStartup(MAKEWORD(1, 1), &data) != 0) {
//		fputs("Could not initialise Winsock.\n", stderr);
//		exit(1);
//	}
//	#endif
//}
//
//static void uninitialise(void) {
//	#ifdef _WIN32
//	WSACleanup();
//	#endif
//}
//
//int main(int argc, char *argv[]) {
//	struct hostent *he;
//
//	if (argc == 1)
//		return -1;
//
//	initialise();
//
//	he = gethostbyname(argv[1]);
//	if (he == NULL) {
//		switch (h_errno) {
//			case HOST_NOT_FOUND:
//				fputs("The host was not found.\n", stderr);
//				break;
//			case NO_ADDRESS:
//				fputs("The name is valid but it has no address.\n", stderr);
//				break;
//			case NO_RECOVERY:
//				fputs("A non-recoverable name server error occurred.\n", stderr);
//				break;
//			case TRY_AGAIN:
//				fputs("The name server is temporarily unavailable.", stderr);
//				break;
//		}
//	} else {
//		puts(utils::ip_str(*((struct in_addr *) he->h_addr_list[0])));
//	}
//
//	uninitialise();
//
//	return he != NULL;
//}
