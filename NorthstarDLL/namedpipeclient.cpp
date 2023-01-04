#pragma once
#include "pch.h"

#include "core/convar/convar.h"
#include "squirrel/squirrel.h" // Squirrel

using std::string;
using std::exception;

#define GENERAL_PIPE_NAME TEXT("\\\\.\\pipe\\GameDataPipe")
#define BUFF_SIZE 512

HANDLE hPipe;
bool isConnected = false;

ConVar* Cvar_ns_enable_named_pipe;

HANDLE GetNewPipeInstance()
{
	HANDLE generalPipe = CreateFile(GENERAL_PIPE_NAME, GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	bool isGeneralConnected = generalPipe != INVALID_HANDLE_VALUE;
	HANDLE thisMatchPipe = INVALID_HANDLE_VALUE;
	if (isGeneralConnected)
	{
		TCHAR chBuf[BUFF_SIZE]; // Server message should either be BUFF_SIZE or Length of general pipe ids + terminating zero
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
				spdlog::info("New Pipe connection");
				thisMatchPipe = CreateFile(chBuf, GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
			} while (thisMatchPipe == INVALID_HANDLE_VALUE);
		}
	}
	return thisMatchPipe;
}

void SendMessageToPipe(const string message)
{
	DWORD read;
	WriteFile(hPipe, message.c_str(), message.length(), &read, nullptr);
}

SQRESULT SQ_SendToNamedPipe(HSquirrelVM* sqvm)
{
	try
	{
		if (Cvar_ns_enable_named_pipe->GetInt() && isConnected)
		{
			SendMessageToPipe(g_pSquirrel<ScriptContext::SERVER>->getstring(sqvm, 1));
		}
	}
	catch (const exception _e)
	{
		spdlog::error("Internal error while sending message through named pipe");
		spdlog::error(_e.what());
	}

	return SQRESULT_NULL;
}

SQRESULT SQ_OpenNamedPipe(HSquirrelVM* sqvm)
{
	try
	{
		if (Cvar_ns_enable_named_pipe->GetInt())
		{
			if (!isConnected || hPipe == INVALID_HANDLE_VALUE)
			{
				isConnected = false;
				hPipe = GetNewPipeInstance();
			}
			else
			{
				SendMessageToPipe(g_pSquirrel<ScriptContext::SERVER>->getstring(sqvm, 2));
			}

			isConnected = hPipe != INVALID_HANDLE_VALUE;

			if (isConnected)
			{
				SendMessageToPipe(g_pSquirrel<ScriptContext::SERVER>->getstring(sqvm, 1));
			}
			else
			{
				spdlog::error("INVALID_HANDLE_VALUE while opening named pipe");
				spdlog::error(GetLastError());
			}
		}
	}
	catch (const exception _e)
	{
		spdlog::error("Internal error while opening named pipe");
		spdlog::error(_e.what());
	}

	return SQRESULT_NULL;
}

SQRESULT SQ_ClosePipe(HSquirrelVM* sqvm)
{
	try
	{
		if (Cvar_ns_enable_named_pipe->GetInt() && isConnected)
		{
			SendMessageToPipe(g_pSquirrel<ScriptContext::SERVER>->getstring(sqvm, 1));

			CloseHandle(hPipe);
			isConnected = false;
		}
	}
	catch (const exception _e)
	{
		spdlog::error("Internal error while closing named pipe");
		spdlog::error(_e.what());
	}

	return SQRESULT_NULL;
}


ON_DLL_LOAD_RELIESON("server.dll", NamedPipeClientCallBack, (ServerSquirrel, ClientSquirrel), (CModule module))
{
	g_pSquirrel<ScriptContext::SERVER>->AddFuncRegistration("void", "NSSendToNamedPipe", "string textToSend", "", SQ_SendToNamedPipe);
	g_pSquirrel<ScriptContext::SERVER>->AddFuncRegistration("void", "NSOpenNamedPipe", "string openingMessage, string closingMessageIfOpen", "", SQ_OpenNamedPipe);
	g_pSquirrel<ScriptContext::SERVER>->AddFuncRegistration("void", "NSCloseNamedPipe", "string closingMessage", "", SQ_ClosePipe);

}

ON_DLL_LOAD_RELIESON("server.dll", NamedPipeClientConvVar, ConVar, (CModule module))
{
	Cvar_ns_enable_named_pipe = new ConVar("ns_enable_named_pipe", "0", FCVAR_GAMEDLL, "Whether to start up a named pipe server on request from squirrel");
}
