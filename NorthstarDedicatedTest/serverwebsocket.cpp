#include "pch.h"
#include "serverwebsocket.h"
#include "squirrel.h" // Squirrel
#include "miscserverscript.h" // GetPlayerByIndex

class ServerDings; // TODO: ServerDings

void SendWebSocketMessage() {

}

SQRESULT SQ_SendToWebsocket(void* sqvm)
{
	int someInteger = ServerSq_getinteger(sqvm, 1);
	const char* someText = ServerSq_getstring(sqvm, 2);
	bool someBool = ServerSq_getbool(sqvm, 3);

	// ChatSendMessage(someInteger, someText, someBool);
	printf("[*] SendWebSocketMessage %d %s %s", someInteger, someText, someBool ? "true" : "false");

	return SQRESULT_NULL;
}

void InitialiseServerWebSocket_Server(HMODULE baseAddress)
{
	// WebSocket sending functions
	g_ServerSquirrelManager->AddFuncRegistration(
		"void", "NSSendToWebSocket", "int someInteger, string someText, bool someBool", "", SQ_SendToWebsocket);
}
