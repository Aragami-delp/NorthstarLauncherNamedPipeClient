#pragma once
#include "pch.h"
#include "namedpipeclient.h"
#include "squirrel.h" // Squirrel
#include "miscserverscript.h" // GetPlayerByIndex

// Named Pipe Stuff
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <atlstr.h>

using namespace std;

#define GENERAL_PIPE_NAME TEXT("\\\\.\\pipe\\GameDataPipe")
#define BUFF_SIZE 512
#define CLOSE_PIPE_ON_MATCHEND true

HANDLE hPipe;
bool isConnected = false;

HANDLE GetNewPipeInstance()
{
	HANDLE generalPipe = CreateFile(GENERAL_PIPE_NAME, GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	bool isGeneralConnected = generalPipe != INVALID_HANDLE_VALUE;
	HANDLE thisMatchPipe = INVALID_HANDLE_VALUE;
	if (isGeneralConnected)
	{
		TCHAR chBuf[BUFF_SIZE]; // TODO: Length of general pipe ids + terminating zero
		DWORD cbRead;
		if (ReadFile(
				generalPipe, // pipe handle
				chBuf, // buffer to receive reply
				BUFF_SIZE * sizeof(TCHAR), // size of buffer
				&cbRead, // number of bytes read
				NULL)) // not overlapped
		{
			do
			{
				printf("New Pipe connection");
				thisMatchPipe = CreateFile(chBuf, GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
			} while (thisMatchPipe == INVALID_HANDLE_VALUE);
		}
	}
	return thisMatchPipe;
}

SQRESULT SQ_SendToNamedPipe(void* sqvm)
{
	if (isConnected)
	{
		// if (!shouldUseNamedPipe)
		//	return SQRESULT_NULL;
		string someText = ServerSq_getstring(sqvm, 1);

		bool success = false;
		DWORD read;

		// Create buffer
		TCHAR chBuff[BUFF_SIZE];
		// Copy message to buffer
		_tcscpy_s(chBuff, CA2T(someText.c_str()));

		do
		{
			success = WriteFile(hPipe, chBuff, BUFF_SIZE * sizeof(TCHAR), &read, nullptr);
		} while (!success);
	}
	else
	{
		printf("NamedPipeClient: no pipe connected");
	}

	return SQRESULT_NULL;
}

SQRESULT SQ_StartNewMatch(void* sqvm)
{
	// if (!shouldUseNamedPipe)
	// return;
	// if (!isConnected || hPipe == INVALID_HANDLE_VALUE)
	try
	{
		if (isConnected && hPipe != INVALID_HANDLE_VALUE)
		{
			string someText = "|2"; // End match
			// someText.append("|").append(Cvar_ns_server_name->GetString());

			bool success = false;
			DWORD read;

			// Create buffer
			TCHAR chBuff[BUFF_SIZE];
			// Copy message to buffer
			_tcscpy_s(chBuff, CA2T(someText.c_str()));
		}
		else
		{
			//isConnected = false;
			hPipe = GetNewPipeInstance();
		}
	}
	catch (exception _e)
	{
		isConnected = false;
		printf(_e.what());
		hPipe = GetNewPipeInstance();
	}
	//hPipe = GetNewPipeInstance();
	isConnected = hPipe != INVALID_HANDLE_VALUE;

	if (isConnected)
	{
		// if (!shouldUseNamedPipe)
		//	return SQRESULT_NULL;
		string someText = ServerSq_getstring(sqvm, 1);
		someText.append("|").append(Cvar_ns_server_name->GetString());

		bool success = false;
		DWORD read;

		// Create buffer
		TCHAR chBuff[BUFF_SIZE];
		// Copy message to buffer
		_tcscpy_s(chBuff, CA2T(someText.c_str()));

		do
		{
			success = WriteFile(hPipe, chBuff, BUFF_SIZE * sizeof(TCHAR), &read, nullptr);
		} while (!success);
	}
	else
	{
		cout << "INVALID_HANDLE_VALUE" << GetLastError() << endl;
	}

	return SQRESULT_NULL;
}

SQRESULT SQ_EndMatch(void* sqvm)
{
	if (isConnected)
	{
		// if (!shouldUseNamedPipe)
		//	return SQRESULT_NULL;
		string someText = ServerSq_getstring(sqvm, 1);
		someText.append("|").append(Cvar_ns_server_name->GetString());

		bool success = false;
		DWORD read;

		// Create buffer
		TCHAR chBuff[BUFF_SIZE];
		// Copy message to buffer
		_tcscpy_s(chBuff, CA2T(someText.c_str()));

		do
		{
			success = WriteFile(hPipe, chBuff, BUFF_SIZE * sizeof(TCHAR), &read, nullptr);
		} while (!success);

		if (CLOSE_PIPE_ON_MATCHEND)
		{
			isConnected = false;
			CloseHandle(hPipe);
		}
	}

	return SQRESULT_NULL;
}

// void InitialiseNamedPipeClient()
void InitialiseNamedPipeClient(HMODULE baseAddress)
{
	// NamedPipe sending functions
	g_ServerSquirrelManager->AddFuncRegistration("void", "NSSendToNamedPipe", "string textToSend", "", SQ_SendToNamedPipe);
	// TODO: Check for commandId and send additional stuff based on that
	g_ServerSquirrelManager->AddFuncRegistration("void", "NSStartNewMatch", "string textToSend", "", SQ_StartNewMatch);
	// End pipe
	g_ServerSquirrelManager->AddFuncRegistration("void", "NSEndMatch", "string textToSend", "", SQ_EndMatch);
}
