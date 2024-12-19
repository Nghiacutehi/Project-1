// Demo_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Demo_Server.h"
#include "afxsock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#define BUFFER_SIZE 1024
#endif



CWinApp theApp;

using namespace std;

DWORD WINAPI list_file(LPVOID arg)
{
	SOCKET* hConnected = (SOCKET*)arg;
	CSocket mysock;
	mysock.Attach(*hConnected);




	FILE* list_file;
	char* buffer = nullptr;
	long file_size;

	list_file = fopen("ListFile.txt", "rb");
	if (list_file == NULL) {
		perror("Unable to open file");
		return 1;
	}

	size_t bytesRead;

	fseek(list_file, 0, SEEK_END);
	file_size = ftell(list_file);
	rewind(list_file);

	buffer = (char*)malloc(file_size + 1);
	if (buffer == NULL) {
		perror("Unable to allocate memory for buffer");
		fclose(list_file);
		return 1;
	}

	size_t bytes_read = fread(buffer, 1, file_size, list_file);
	if (bytes_read != file_size) {
		printf("Error reading the file. Expected %ld bytes but read %zu bytes.\n", file_size, bytes_read);
		free(buffer);
		fclose(list_file);
		return 1;
	}

	mysock.Send(&bytes_read, sizeof(bytes_read), 0);

	int result = mysock.Send(buffer, bytes_read, 0);
	if (result == SOCKET_ERROR) {
		printf("Error sending data: %d\n", WSAGetLastError());
	}

	buffer[bytes_read] = '\0';
	printf("Cac file co tren Server: \n\n%s\n", buffer);
	


	free(buffer);
	fclose(list_file);


	delete hConnected;
	return 0;
}










DWORD WINAPI four_chunks(LPVOID arg)
{
	SOCKET* hConnected = (SOCKET*)arg;
	CSocket mysock;
	mysock.Attach(*hConnected);


	/*
	FILE* file;
	char* buffer = nullptr;
	long file_size;

	file = fopen("100MB.bin", "rb");
	if (file == NULL) {
		perror("Không thể mở file");
		return 1;
	}

	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	rewind(file);

	long chunk_size = file_size / 4;

	size_t bytesRead;
	buffer = (char*)malloc(BUFFER_SIZE);
	if (buffer == NULL) {
		perror("Không thể cấp phát bộ nhớ cho buffer");
		fclose(file);
		return 1;
	}

	long position = chunk_size * (intptr_t)arg;
	fseek(file, position, SEEK_SET);

	int num_parts = chunk_size / BUFFER_SIZE + (chunk_size % BUFFER_SIZE != 0);
	mysock.Send(&num_parts, sizeof(num_parts), 0);


	for (int part = 0; part < num_parts; part++) {
		bytesRead = fread(buffer, 1, BUFFER_SIZE, file);
		mysock.Send(&bytesRead, sizeof(bytesRead), 0);
		mysock.Send(buffer, bytesRead, 0);

		if (bytesRead != chunk_size) {
			printf("Lỗi khi đọc file. Dự kiến %ld byte nhưng đọc được %zu byte.\n", chunk_size, bytesRead);
			free(buffer);
			fclose(file);
			return 1;
		}
	}


	printf("Đã gửi phần %ld cho client\n", (intptr_t)arg + 1);

	
	

	free(buffer);
	fclose(file);

	*/

	mysock.Send(&(intptr_t)arg, sizeof((intptr_t)arg), 0);

	delete hConnected; // Giải phóng socket
	return 0;
}










int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			AfxSocketInit(NULL);
			CSocket server, s;
			DWORD threadID;
			HANDLE threadStatus;

			server.Create(4567);
			do {
				printf("Server lang nghe ket noi tu client\n");
				server.Listen();
				server.Accept(s);

				SOCKET* hConnected = new SOCKET();

				*hConnected	= s.Detach();
				
				threadStatus = CreateThread(NULL, 0, list_file, hConnected, 0, &threadID);
				for (int i = 0; i < 4; i++)
				{

					SOCKET* hConnected = new SOCKET();
					*hConnected = s.Detach();

					threadStatus = CreateThread(NULL, 0, four_chunks, (LPVOID)(intptr_t)i, 0, &threadID);
				}
			} while (1);
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}


