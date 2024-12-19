// Demo_Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Demo_Client.h"
#include "afxsock.h"

#include <csignal>
#include <thread>

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#define  NUM_CONNECTIONS 4
#endif


// The one and only application object

CWinApp theApp;

using namespace std;



void signalHandler(int signum) {
	std::cout << "\nCtrl+C pressed. Program is exiting\n";
	exit(0); 
}






void download_part(int part_num, CSocket& client, int file_size) {

	/*
	int part_size = file_size / NUM_CONNECTIONS;

	// Đặt vị trí con trỏ file sao cho mỗi phần nhận được là khác nhau
	int start_byte = part_num * part_size;
	client.Send(&start_byte, sizeof(start_byte), 0); // Gửi vị trí bắt đầu cho server

	// Nhận phần dữ liệu từ server
	char* buffer = new char[part_size];
	int bytesReceived = client.Receive(buffer, part_size, 0);

	// Đảm bảo dữ liệu nhận được không bị lỗi
	if (bytesReceived > 0) {
		printf("Part %d received, %d bytes\n", part_num, bytesReceived);
		// Xử lý dữ liệu nhận được (ví dụ in ra màn hình)
		cout.write(buffer, bytesReceived);
	}
	else {
		printf("Error receiving part %d\n", part_num);
	}

	delete[] buffer;

	*/

	int num;
	client.Receive(&num, sizeof(num), 0);

	cout << num << endl;


}







int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	cout << "Program is running. Press Ctrl+C to stop...\n";
	signal(SIGINT, signalHandler);

	int nRetCode = 0;
	while (true)
	{
		HMODULE hModule = ::GetModuleHandle(NULL);

		if (hModule != NULL)
		{

			if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
			{

				_tprintf(_T("Fatal Error: MFC initialization failed\n"));
				nRetCode = 1;
			}
			else
			{

				AfxSocketInit(NULL);
				CSocket client;

				client.Create();
				client.Connect(_T("127.0.0.1"), 4567);



				int number_continue = 0;

				int count = 0;




				char* buffer = nullptr;

				int file_size = 0;
				client.Receive(&file_size, sizeof(file_size), 0);

				buffer = new char[file_size + 1];

				int bytesReceived;

				bytesReceived = client.Receive((char*)buffer, file_size, 0);

				buffer[file_size] = '\0';

				printf("Cac file co tren Server: \n\n%s\n", buffer);

				delete(buffer);

				client.Receive(&file_size, sizeof(file_size), 0); // Nhận kích thước file từ server

				// Mở 4 kết nối song song để tải các phần của file
				vector<thread> download_threads;

				// Tạo 4 thread để tải các phần của file
				for (int i = 0; i < NUM_CONNECTIONS; i++) {
					download_threads.push_back(thread(download_part, i, ref(client), file_size));
				}

				// Chờ cho tất cả các thread tải xong
				for (auto& t : download_threads) {
					t.join();
				}

			}



			cin.ignore();

		
		}
		else
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
			nRetCode = 1;
		}

	}
	return nRetCode;

}
