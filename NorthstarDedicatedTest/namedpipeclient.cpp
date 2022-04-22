#include "pch.h"
#include "namedpipeclient.h"
#include "squirrel.h" // Squirrel
#include "miscserverscript.h" // GetPlayerByIndex

#pragma once
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <atlstr.h>

using namespace std;

#define PIPE_NAME L"\\\\.\\pipe\\TestPipe"
#define BUFF_SIZE 512

HANDLE hPipe;

SQRESULT SQ_SendToWebsocket(void* sqvm)
{
	int someInteger = ServerSq_getinteger(sqvm, 1);
	string someText = ServerSq_getstring(sqvm, 2);
	bool someBool = ServerSq_getbool(sqvm, 3);

	// ChatSendMessage(someInteger, someText, someBool);
	//printf("[*] SendWebSocketMessage %d %s %s", someInteger, someText, someBool ? "true" : "false");

	bool success = false;
	DWORD read;

	TCHAR chBuff[BUFF_SIZE];
	_tcscpy_s(chBuff, CA2T(someText.c_str()));
	do
	{
		success = WriteFile(hPipe, chBuff, BUFF_SIZE * sizeof(TCHAR), &read, nullptr);
		// success = ReadFile(hPipe, chBuff, BUFF_SIZE * sizeof(TCHAR), &read, nullptr);
	} while (!success);

	return SQRESULT_NULL;
}

void InitialiseNamedPipeClient(HMODULE baseAddress)
{
	// WebSocket sending functions
	g_ServerSquirrelManager->AddFuncRegistration(
		"void", "NSSendToWebSocket", "int someInteger, string someText, bool someBool", "", SQ_SendToWebsocket);

	hPipe = CreateFile(PIPE_NAME, GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		cout << "INVALID_HANDLE_VALUE" << GetLastError() << endl;
		cin.get();
		return;
	}

	cout << hPipe << endl;
}
