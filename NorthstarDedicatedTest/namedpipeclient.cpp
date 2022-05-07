#pragma once
#include "pch.h"
#include "namedpipeclient.h"
#include "squirrel.h" // Squirrel
#include "miscserverscript.h" // GetPlayerByIndex

//Named Pipe Stuff
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <atlstr.h>

using namespace std;

#define PIPE_NAME L"\\\\.\\pipe\\GameDataPipe"
#define BUFF_SIZE 512

HANDLE hPipe;

SQRESULT SQ_SendToNamedPipe(void* sqvm)
{
	//if (!shouldUseNamedPipe)
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

	return SQRESULT_NULL;
}

SQRESULT SQ_StartNewMatch(void* sqvm)
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

	return SQRESULT_NULL;
}

//void InitialiseNamedPipeClient()
void InitialiseNamedPipeClient(HMODULE baseAddress)
{
	// NamedPipe sending functions
	g_ServerSquirrelManager->AddFuncRegistration(
		"void", "NSSendToNamedPipe", "string textToSend", "", SQ_SendToNamedPipe);
	// TODO: Check for commandId and send additional stuff based on that
	g_ServerSquirrelManager->AddFuncRegistration(
		"void", "NSStartNewMatch", "string textToSend", "", SQ_StartNewMatch);

	//if (!shouldUseNamedPipe)
	//	return;
	hPipe = CreateFile(PIPE_NAME, GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		cout << "INVALID_HANDLE_VALUE" << GetLastError() << endl;
		return;
	}

	cout << hPipe << endl;
}
