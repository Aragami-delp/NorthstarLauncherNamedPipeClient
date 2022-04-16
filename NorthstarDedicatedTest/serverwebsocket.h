#pragma once
#include "pch.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

// Send a vanilla chat message as if it was from the player.
void SendWebSocketMessage();

void InitialiseServerWebSocket_Server(HMODULE baseAddress);
