#if defined _INC_commands_core
	#endinput
#endif
#define _INC_commands_core

static LastCommand[MAX_PLAYERS];

public OnPlayerCommandReceived(playerid, cmd[], params[], flags)
{
    if (!HasPlayerLogged(playerid)) {
		GameTextForPlayer(
			playerid,
			"~r~ERRO~n~~w~Voce NAO esta logado!",
			5000, 16
		);
        return 0;
    }

	if (GetTickCount() - LastCommand[playerid] < 500) {
		GameTextForPlayer(playerid, "Aguarde o comando atual terminar.", 3000, 3);
        return 0;
    }

    LastCommand[playerid] = GetTickCount();
    return 1;
}

public OnPlayerCommandPerformed(playerid, cmd[], params[], result, flags)
{
	if (result == -1) {
		GameTextForPlayer(playerid, "~r~Comando inexistente", 3000, 3);
	}
    return 1;
}

CMD:money(playerid, params[])
{
	GivePlayerMoney(playerid, 10000);
	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	PlayerPlaySound(playerid, 5405, x, y, z);
	return 1;
}
