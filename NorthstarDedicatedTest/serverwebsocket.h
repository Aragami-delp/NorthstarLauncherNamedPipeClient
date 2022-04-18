#pragma once
#include "pch.h"

// Send a vanilla chat message as if it was from the player.
void SendWebSocketMessage();

void InitialiseServerWebSocket_Server(HMODULE baseAddress);
