/*
    FilterScript Sistema de casas C-Casas.
    Criado por Cleyson Stein - StNN
    Nгo retire os crйditos.
    Todos os direitos reservados. 02/05/2018*
    
*/

#include <YSI_Coding\y_hooks>

enum
{
    MAX_CASAS = 100, // Maximo de casas
    VERMELHO = 0xFF000FFF,
    CINZA = 0xCECECEFF,
    LARANJA = 0xF97804FF
};
enum pInfo
{
    Nome[24],
    TemCasa,
    EntrouCasa
};
new p[MAX_PLAYERS][pInfo];
enum cInfo
{
    cDono[24],
    Float:cX,
    Float:cY,
    Float:cZ,
    cTemDono,
    cTrancada,
    cIcone,
    cPickup,
    cInt,
    Text3D:cText,
    cPreco
};
new c[MAX_CASAS][cInfo], TOTALCASA;
// interiores casas
enum inter
{
    Float:iX,
    Float:iY,
    Float:iZ,
    Float:iA,
    iID
}
new interior[][inter] =
{
    {2196.84, -1204.36, 1049.02, 94.0010, 6},
    {2259.38, -1135.89, 1050.64, 275.3992, 10},
    {2282.99, -1140.28, 1050.89, 358.4660, 11},
    {2233.69, -1115.26, 1050.88, 358.4660, 5},
    {2218.39, -1076.21, 1050.48, 95.2635, 1},
    {2496.00, -1692.08, 1014.74, 177.8159, 3},
    {2365.25, -1135.58, 1050.88, 359.0367, 8},
    {2317.77, -1026.76, 1050.21, 359.0367, 9},
    {2324.41, -1149.54, 1050.71, 359.0367, 12},
    {1260.6603, -785.4005, 1091.9063, 270.9891, 5},
    {140.28, 1365.92, 1083.85, 9.6901, 5}
};
// sqlite
new DB:Database;
hook OnGameModeInit()
{
    print("\n-------------------------------------");
    print("Sistema C-casas carregado com sucesso.");
    print("   Criado por Cleyson Stein - StNN.");
    print("    Todos os direitos reservados.");
    print("-------------------------------------\n");
    ConectarDB();
    CarregarCasas();
    EnableStuntBonusForAll(false);
    DisableInteriorEnterExits();
    return 1;
}

hook OnGameModeExit()
{
    DB_Close(Database);
    for(new i = 0; i < MAX_CASAS; ++i)
    {
        DestroyDynamicPickup(c[i][cPickup]);
        DestroyDynamicMapIcon(c[i][cIcone]);
        DestroyDynamic3DTextLabel(c[i][cText]);
    }    
    return 1;
}
hook OnPlayerConnect(playerid)
{
    GetPlayerName(playerid, p[playerid][Nome], 24);
    CarregarDadosPlayer(playerid);
    return 1;
}
hook OnPlayerDisconnect(playerid, reason)
{
    if(p[playerid][TemCasa] > 0) SalvarDadosPlayer(playerid);
    return 1;
}
// ============= COMMANDS ADMIN
CMD:acasa(playerid)
{
    if(!IsPlayerAdmin(playerid)) return SendClientMessage(playerid, VERMELHO, "Comando restrito a administradores.");
    new String_COMANDOS[220]; //
    format(String_COMANDOS, sizeof String_COMANDOS, "");
    
    strcat(String_COMANDOS, "{009900}ADMIN C-CASAS\n\n"); 
    strcat(String_COMANDOS, "{CECECE}/criarcasa {FFFFFF}- Criar uma casa.\n");
    strcat(String_COMANDOS, "{CECECE}/editarcasa {FFFFFF}- Editar uma casa.\n");
    strcat(String_COMANDOS, "{CECECE}/deletarcasa {FFFFFF}- Deletar casa.\n");
    strcat(String_COMANDOS, "{CECECE}/ircasa {FFFFFF}- Ir a uma casa.\n");
    ShowPlayerDialog(playerid, 9545, DIALOG_STYLE_MSGBOX, "Comandos Admin C-Casas", String_COMANDOS, "Ok", "");
    return 1;
}
CMD:criarcasa(playerid, params[])
{
    if(!IsPlayerAdmin(playerid)) return SendClientMessage(playerid, VERMELHO, "Comando restrito a administradores.");
    if(sscanf(params, "ii", params[0], params[1])) return SendClientMessage(playerid, CINZA, "Use /criarcasa [preco] [int 0a10]");
    Criar_Casa(playerid, TOTALCASA+1, params[0], params[1]);
    return 1;
}
CMD:ircasa(playerid, params[])
{
    if(!IsPlayerAdmin(playerid)) return SendClientMessage(playerid, VERMELHO, "Comando restrito a administradores.");
    if(sscanf(params, "i", params[0])) return SendClientMessage(playerid, CINZA, "Use /ircasa [id]");
    Ir_Casa(playerid, params[0]);
    return 1;
}
CMD:editarcasa(playerid, params[])
{
    if(!IsPlayerAdmin(playerid)) return SendClientMessage(playerid, VERMELHO, "Comando restrito a administradores.");
    if(sscanf(params, "iii", params[0], params[1], params[2])) return SendClientMessage(playerid, CINZA, "Use /editarcasa [id] [preco] [int 0a10]");
    Editar_Casa(playerid, params[0], params[1], params[2]);
    return 1;
}
CMD:deletarcasa(playerid, params[])
{
    if(!IsPlayerAdmin(playerid)) return SendClientMessage(playerid, VERMELHO, "Comando restrito a administradores.");
    if(sscanf(params, "i", params[0])) return SendClientMessage(playerid, CINZA, "Use /deletarcasa [id]");
    Deletar_Casa(playerid, params[0]);
    return 1;
}
// ================= COMMANDS PLAYERS
/*CMD:dinheiro(playerid)
{
    GivePlayerMoney(playerid, 500000);
    return 1;
}
*/
CMD:comprarcasa(playerid)
{
    for(new i = 0; i < MAX_CASAS; i++)
    {
        if(IsPlayerInRangeOfPoint(playerid, 1, c[i][cX], c[i][cY], c[i][cZ]))
        {
            if(p[playerid][TemCasa] > 0) return SendClientMessage(playerid, VERMELHO, "Vocк ja possue uma casa.");
            if(c[i][cTemDono] == 1) return SendClientMessage(playerid, VERMELHO, "Essa casa ja possue dono.");
            if(GetPlayerMoney(playerid) < c[i][cPreco]) return SendClientMessage(playerid, VERMELHO, "Dinheiro insuficiente.");
            format(c[i][cDono], 24, p[playerid][Nome]);
            c[i][cTemDono] = 1;
            p[playerid][TemCasa] = i;
            SendClientMessage(playerid, LARANJA, "Casa adiquirida com sucesso.");
            GivePlayerMoney(playerid, -c[i][cPreco]);
            DestroyDynamicPickup(c[i][cPickup]);
            DestroyDynamicMapIcon(c[i][cIcone]);
            DestroyDynamic3DTextLabel(c[i][cText]);
            new texto[200];
            c[i][cIcone] = CreateDynamicMapIcon(c[i][cX], c[i][cY], c[i][cZ], 32, -1);
            c[i][cPickup] = CreateDynamicPickup(19522, 21, c[i][cX], c[i][cY], c[i][cZ]);//1272
            
            format(texto, sizeof(texto), "{B22222}CASA ID: {FFFFFF}%d\n{CD5C5C}Dono: {FFFFFF}%s\n{CD5C5C}Preзo: {FFFFFF}$ %d{CD5C5C}\n/ajudacasa", i, c[i][cDono], c[i][cPreco]);
            c[i][cText] = CreateDynamic3DTextLabel(texto, 0xFFFFFFAA, c[i][cX], c[i][cY], c[i][cZ]+1, 20);
            new Query[80];
            format(Query, sizeof(Query), "UPDATE CASAS SET DONO ='%s', TEMDONO = 1 WHERE ID =%d", c[i][cDono], i);
            DB_ExecuteQuery(Database, Query);
            break;
        }
    }
    return 1;
}
CMD:vendercasa(playerid)
{
    for(new i = 0; i < MAX_CASAS; i++)
    {
        if(IsPlayerInRangeOfPoint(playerid, 1, c[i][cX], c[i][cY], c[i][cZ]))
        {
            if(p[playerid][TemCasa] == 0) return SendClientMessage(playerid, VERMELHO, "Vocк nгo possue uma casa.");
            if(c[i][cTemDono] == 0) return SendClientMessage(playerid, VERMELHO, "Essa casa nгo possue dono.");
            if(p[playerid][TemCasa] == i)
            {
                SendClientMessage(playerid, LARANJA, "Casa vendida com sucesso.");
                GivePlayerMoney(playerid, c[i][cPreco]/2);
                format(c[i][cDono], 24, "Nenhum");
                c[i][cTemDono] = 0;
                p[playerid][TemCasa] = 0;
                DestroyDynamicPickup(c[i][cPickup]);
                DestroyDynamicMapIcon(c[i][cIcone]);
                DestroyDynamic3DTextLabel(c[i][cText]);
                new texto[200];
                c[i][cIcone] = CreateDynamicMapIcon(c[i][cX], c[i][cY], c[i][cZ], 31, -1);
                c[i][cPickup] = CreateDynamicPickup(1273, 21, c[i][cX], c[i][cY], c[i][cZ]);
                format(texto, sizeof(texto), "{2E8B57}CASA ID: {FFFFFF}%d\n{3CB371}Dono: {FFFFFF}%s\n{3CB371}Preзo: {FFFFFF}$ %d\n{3CB371}Aperte 'F' para entrar\n/ajudacasa", i, c[i][cDono], c[i][cPreco]);
                c[i][cText] = CreateDynamic3DTextLabel(texto, 0xFFFFFFAA, c[i][cX], c[i][cY], c[i][cZ]+1, 20);
                new Query[80];
                format(Query, sizeof(Query), "UPDATE CASAS SET DONO ='%s', TEMDONO = 0 WHERE ID =%d", c[i][cDono], i);
                DB_ExecuteQuery(Database, Query);
                break;
                } else {
                SendClientMessage(playerid, VERMELHO, "Essa casa nгo й sua.");
                return 1;
               }
        }
    }
    return 1;
}
CMD:trancar(playerid)
{
    for(new i = 0; i < MAX_CASAS; i++)
    {
        if(IsPlayerInRangeOfPoint(playerid, 1, c[i][cX], c[i][cY], c[i][cZ]))
        {
            if(p[playerid][TemCasa] == 0) return SendClientMessage(playerid, VERMELHO, "Vocк nгo possue uma casa.");
            if(c[i][cTemDono] == 0) return SendClientMessage(playerid, VERMELHO, "Essa casa nгo possue dono.");
            if(p[playerid][TemCasa] == i)
            {
                new Query[80];
                if(c[i][cTrancada] == 0)
                {
                    SendClientMessage(playerid, LARANJA, "Casa trancada.");
                    c[i][cTrancada] = 1;
                    
                    format(Query, sizeof(Query), "UPDATE CASAS SET TRANCADA = 1 WHERE ID =%d", i);
                    DB_ExecuteQuery(Database, Query);
                    break;
                    
                    } else {
                    
                    SendClientMessage(playerid, LARANJA, "Casa destrancada.");
                    c[i][cTrancada] = 0;
                    
                    format(Query, sizeof(Query), "UPDATE CASAS SET TRANCADA = 0 WHERE ID =%d", i);
                    DB_ExecuteQuery(Database, Query);
                    break;
                }
                } else {
                SendClientMessage(playerid, VERMELHO, "Essa casa nгo й sua.");
                return 1;
               }
        }
    }
    return 1;
}
CMD:ajudacasa(playerid)
{
    ShowPlayerDialog(playerid, 9595, DIALOG_STYLE_MSGBOX, "Ajuda C-Casas","{009900}CASAS\n\n{CECECE}/comprarcasa {FFFFFF}- Comprar uma casa.\n{CECECE}/vendercasa {FFFFFF}- Vender sua casa.\n{CECECE}/trancar {FFFFFF}- trancar | destrancar casa.\n", "Ok", "");
    return 1;
}
hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
    if(newkeys == KEY_SECONDARY_ATTACK) // TECLA F
    {
        for(new x = 0; x < MAX_CASAS; x++)
        {
            if(IsPlayerInRangeOfPoint(playerid, 1, c[x][cX], c[x][cY], c[x][cZ]))
            {
                if(c[x][cTemDono] == 0)
                {
                    SetPlayerPos(playerid, interior[c[x][cInt]][iX], interior[c[x][cInt]][iY], interior[c[x][cInt]][iZ]);
                    SetPlayerFacingAngle(playerid, interior[c[x][cInt]][iA]);
                    SetPlayerInterior(playerid, interior[c[x][cInt]][iID]);
                    p[playerid][EntrouCasa] = x;
                    
                    SendClientMessage(playerid, LARANJA, "Casa a venda! Para comprar use /comprarcasa no pickup de fora. Use 'N' para sair.");
                    break;
                    
                }  
                if(c[x][cTemDono] == 1)
                {
                    if(c[x][cTrancada] == 1 && p[playerid][TemCasa] != x) return SendClientMessage(playerid, VERMELHO, "Casa trancada.");
                    
                    if(p[playerid][TemCasa] == x)
                    {
                        SetPlayerPos(playerid, interior[c[x][cInt]][iX], interior[c[x][cInt]][iY], interior[c[x][cInt]][iZ]);
                        SetPlayerFacingAngle(playerid, interior[c[x][cInt]][iA]);
                        SetPlayerInterior(playerid, interior[c[x][cInt]][iID]);
                        p[playerid][EntrouCasa] = x;
                        
                        SendClientMessage(playerid, LARANJA, "Bem vindo a sua casa. Use 'N' para sair.");
                        break;
                        
                        } else {
                        SendClientMessage(playerid, VERMELHO, "Essa casa nгo й sua.");
                        return 1;
                    }
                }
            }
        }
    }
    if(newkeys == 131072) // TECLA N
    {
        if(p[playerid][EntrouCasa] > 0)
        {
            SetPlayerPos(playerid, c[p[playerid][EntrouCasa]][cX], c[p[playerid][EntrouCasa]][cY], c[p[playerid][EntrouCasa]][cZ]);
            SetPlayerInterior(playerid, 0);
            p[playerid][EntrouCasa] = 0;
        }
    }
    return 1;
}
// ============== DATA
ConectarDB()
{
    if((Database = DB_Open("C-Casas.db")) == DB:0)
    {
        SendRconCommand("exit");
        print("Falha ao se conectar a DataBase SQlite"); // DEBUG
        
        } else {
        
        print("Conectado com sucesso a DataBase SQlite."); // DEBUG
    }
    //criar a db esta em db.txt
    new Query[280];
    format(Query, sizeof(Query), "CREATE TABLE IF NOT EXISTS CASAS(ID INTEGER PRIMARY KEY AUTOINCREMENT,\
    DONO TEXT DEFAULT '',\
    X FLOAT DEFAULT 0.0,\
    Y FLOAT DEFAULT 0.0,\
    Z FLOAT DEFAULT 0.0,\
    INTERIOR INTEGER DEFAULT 0,\
    TRANCADA INTEGER DEFAULT 0,\
    TEMDONO INTEGER DEFAULT 0,\
    PRECO INTEGER DEFAULT 0)");    
    DB_ExecuteQuery(Database, Query);
    format(Query, sizeof(Query), "CREATE TABLE IF NOT EXISTS DONOS(ID INTEGER PRIMARY KEY AUTOINCREMENT, NOME TEXT DEFAULT '', CASAID INTEGER DEFAULT 0)");    
    DB_ExecuteQuery(Database, Query);
    return 1;
}
CarregarDadosPlayer(playerid)
{
    new DBResult:Result, Query[65];
    format(Query, sizeof(Query), "SELECT * FROM DONOS WHERE NOME = '%s'", p[playerid][Nome]);
    Result = DB_ExecuteQuery(Database, Query);
    if(DB_GetRowCount(Result))
    {
        p[playerid][TemCasa] = DB_GetFieldIntByName(Result, "CASAID");
        } else {
        new Query2[75];
        format(Query2, sizeof(Query2), "INSERT INTO DONOS (NOME, CASAID) VALUES ('%s', 0)", p[playerid][Nome]);
        DB_ExecuteQuery(Database, Query2);
        p[playerid][TemCasa] = 0;
    }
    DB_FreeResultSet(Result);
    return 1;
}
SalvarDadosPlayer(playerid)
{
    new Query[75];
    format(Query, sizeof(Query), "UPDATE DONOS SET CASAID =%d WHERE NOME ='%s'", p[playerid][TemCasa], p[playerid][Nome]);
    DB_ExecuteQuery(Database, Query); 
    return 1;
}
// ============== INIT
CarregarCasas()
{
    new DBResult:Result, Query[50];
    for(new i = 0; i < MAX_CASAS; i++)
    {
        format(Query, sizeof(Query), "SELECT * FROM CASAS WHERE ID = %d", i);
        Result = DB_ExecuteQuery(Database, Query);
        if(DB_GetRowCount(Result))
        {
            DB_GetFieldStringByName(Result, "DONO", c[i][cDono], 24);
            c[i][cX] = DB_GetFieldFloatByName(Result, "X");
            c[i][cY] = DB_GetFieldFloatByName(Result, "Y");
            c[i][cZ] = DB_GetFieldFloatByName(Result, "Z");
            c[i][cTemDono] = DB_GetFieldIntByName(Result, "TEMDONO");
            c[i][cPreco] = DB_GetFieldIntByName(Result, "PRECO");
            c[i][cInt] = DB_GetFieldIntByName(Result, "INTERIOR");
            c[i][cTrancada] = DB_GetFieldIntByName(Result, "TRANCADA");
            TOTALCASA ++;
            CriarCasa(i);
        }
        DB_FreeResultSet(Result);
    }
    printf("\nCasas carregadas: %d", TOTALCASA);
    return 1;
}
CriarCasa(i)
{
    new texto[200];
    if(c[i][cTemDono] == 0)
    {
        c[i][cIcone] = CreateDynamicMapIcon(c[i][cX], c[i][cY], c[i][cZ], 31, -1);
        c[i][cPickup] = CreateDynamicPickup(1273, 21, c[i][cX], c[i][cY], c[i][cZ]);
        format(texto, sizeof(texto), "{2E8B57}CASA ID: {FFFFFF}%d\n{3CB371}Dono: {FFFFFF}%s\n{3CB371}Preзo: {FFFFFF}$ %d\n{3CB371}Aperte 'F' para entrar\n/ajudacasa", i, c[i][cDono], c[i][cPreco]);
        c[i][cText] = CreateDynamic3DTextLabel(texto, 0xFFFFFFAA, c[i][cX], c[i][cY], c[i][cZ]+1, 20);
    }
    else if(c[i][cTemDono] == 1)
    {
        c[i][cIcone] = CreateDynamicMapIcon(c[i][cX], c[i][cY], c[i][cZ], 32, -1);
        c[i][cPickup] = CreateDynamicPickup(19522, 21, c[i][cX], c[i][cY], c[i][cZ]);//1272
        format(texto, sizeof(texto), "{B22222}CASA ID: {FFFFFF}%d\n{CD5C5C}Dono: {FFFFFF}%s\n{CD5C5C}Preзo: {FFFFFF}$ %d{CD5C5C}\n/ajudacasa", i, c[i][cDono], c[i][cPreco]);
        c[i][cText] = CreateDynamic3DTextLabel(texto, 0xFFFFFFAA, c[i][cX], c[i][cY], c[i][cZ]+1, 20);
    }
    return 1;
}
// ============== ADMIN
Criar_Casa(playerid, id, preco, inte)
{
    new DBResult:Result, Query[160];
    format(Query, sizeof(Query), "SELECT * FROM CASAS WHERE ID = %d", id);
    Result = DB_ExecuteQuery(Database, Query);
    if(DB_GetRowCount(Result))
    {
        SendClientMessage(playerid, VERMELHO, "Ocorreu um erro. Esse ID esta em uso.");
        } else {
        new Float:Pos[3];
        GetPlayerPos(playerid, Pos[0], Pos[1], Pos[2]);
        format(c[id][cDono], 24, "Nenhum");
        c[id][cX] = Pos[0];
        c[id][cY] = Pos[1];
        c[id][cZ] = Pos[2];
        c[id][cTemDono] = 0;
        c[id][cPreco] = preco;
        c[id][cInt] = inte;
        SendClientMessage(playerid, LARANJA, "Casa criada com sucesso.");
        CriarCasa(id);
        format(Query, sizeof(Query), "INSERT INTO CASAS(DONO, X, Y, Z, PRECO, INTERIOR) VALUES ('%s', %f, %f, %f, %d, %d)", c[id][cDono], c[id][cX], c[id][cY], c[id][cZ], c[id][cPreco], c[id][cInt]);
        DB_ExecuteQuery(Database, Query);
        TOTALCASA ++;
    }
    DB_FreeResultSet(Result);
    return 1;
}
Editar_Casa(playerid, id, preco, interi)
{
    new DBResult:Result, Query[50];
    format(Query, sizeof(Query), "SELECT * FROM CASAS WHERE ID = %d", id);
    Result = DB_ExecuteQuery(Database, Query);
    if(DB_GetRowCount(Result))
    {
        new string[60];
        format(string, sizeof(string), "Vocк editou a casa id %d com sucesso!", id);
        SendClientMessage(playerid, LARANJA, string);    
        c[id][cPreco] = preco;
        c[id][cInt] = interi;
        DestroyDynamicPickup(c[id][cPickup]);
        DestroyDynamicMapIcon(c[id][cIcone]);
        DestroyDynamic3DTextLabel(c[id][cText]);
        CriarCasa(id);
        } else {
        SendClientMessage(playerid, VERMELHO, "Ocorreu um erro. Esse ID nгo existe.");
    }
    DB_FreeResultSet(Result);
    return 1;
}
Ir_Casa(playerid, id)
{
    new DBResult:Result, Query[50];
    format(Query, sizeof(Query), "SELECT * FROM CASAS WHERE ID = %d", id);
    Result = DB_ExecuteQuery(Database, Query);
    if(DB_GetRowCount(Result))
    {
        new string[60];
        format(string, sizeof(string), "Vocк foi a casa id %d.", id);
        SendClientMessage(playerid, LARANJA, string);    
        SetPlayerPos(playerid, c[id][cX], c[id][cY], c[id][cZ]);
        SetPlayerInterior(playerid, 0);
        } else {
        SendClientMessage(playerid, VERMELHO, "Ocorreu um erro. Esse ID nгo existe.");
    }
    DB_FreeResultSet(Result);
    return 1;
}
Deletar_Casa(playerid, id)
{
    new DBResult:Result, Query[50];
    format(Query, sizeof(Query), "SELECT * FROM CASAS WHERE ID = %d", id);
    Result = DB_ExecuteQuery(Database, Query);
    if(DB_GetRowCount(Result))
    {
        format(Query, sizeof(Query), "DELETE FROM CASAS WHERE ID = %d", id);
        DB_ExecuteQuery(Database, Query);
        new string[60];
        format(string, sizeof(string), "Vocк excluiu a casa id %d com sucesso!", id);
        SendClientMessage(playerid, LARANJA, string);    
        format(c[id][cDono], 24, "Nenhum");
        c[id][cX] = -1;
        c[id][cY] = -1;
        c[id][cZ] = -1;
        c[id][cTemDono] = -1;
        c[id][cPreco] = -1;
        c[id][cInt] = -1;
        c[id][cTrancada] = -1;
        DestroyDynamicPickup(c[id][cPickup]);
        DestroyDynamicMapIcon(c[id][cIcone]);
        DestroyDynamic3DTextLabel(c[id][cText]);
        TOTALCASA --;
        } else {
        SendClientMessage(playerid, VERMELHO, "Ocorreu um erro. Esse ID nгo existe.");
    }
    DB_FreeResultSet(Result);
    return 1;
}
