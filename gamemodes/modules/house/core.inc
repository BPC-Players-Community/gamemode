/*
    FilterScript Sistema de casas C-Casas.
    Criado por Cleyson Stein - StNN
    Não retire os créditos.
    Todos os direitos reservados. 02/05/2018*
    Link: https://sampforum.blast.hk/showthread.php?tid=653371
*/
#if defined _INC_house_core
	#endinput
#endif
#define _INC_house_core

#include <pp-hooks>

#define MAX_HOUSES (100)

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
new c[MAX_HOUSES][cInfo], TOTALCASA;

// interiores casas
enum inter
{
    Float:iX,
    Float:iY,
    Float:iZ,
    Float:iA,
    iID
}

new interiors[][inter] =
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
    return 1;
}

hook OnGameModeExit()
{
    DB_Close(Database);
    for (new i = 0; i < MAX_HOUSES; ++i) {
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
    if (p[playerid][TemCasa] > 0) {
        SalvarDadosPlayer(playerid);
    }
    return 1;
}

hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys)
{
    if (newkeys == KEY_SECONDARY_ATTACK) {
        for (new i = 0; i < MAX_HOUSES; i++) {
            if (IsPlayerInRangeOfPoint(playerid, 1, c[i][cX], c[i][cY], c[i][cZ])) {
                if(c[i][cTemDono] == 0) {
                    SetPlayerPos(playerid, interiors[c[i][cInt]][iX], interiors[c[i][cInt]][iY], interiors[c[i][cInt]][iZ]);
                    SetPlayerFacingAngle(playerid, interiors[c[i][cInt]][iA]);
                    SetPlayerInterior(playerid, interiors[c[i][cInt]][iID]);
                    p[playerid][EntrouCasa] = i;
                    
                    SendClientMessage(
                        playerid,
                        COLOR_ORANGE,
                        "Casa a venda! Para comprar use /comprarcasa no pickup de fora. Use 'N' para sair."
                    );
                    break;   
                }
                if (c[i][cTemDono] == 1) {
                    if (c[i][cTrancada] == 1 && p[playerid][TemCasa] != i) {
                        SendClientMessage(playerid, COLOR_RED, "Casa trancada.");
                        return 1;
                    }
                    
                    if (p[playerid][TemCasa] == i) {
                        SetPlayerPos(playerid, interiors[c[i][cInt]][iX], interiors[c[i][cInt]][iY], interiors[c[i][cInt]][iZ]);
                        SetPlayerFacingAngle(playerid, interiors[c[i][cInt]][iA]);
                        SetPlayerInterior(playerid, interiors[c[i][cInt]][iID]);
                        p[playerid][EntrouCasa] = i;
                        
                        SendClientMessage(
                            playerid,
                            COLOR_ORANGE,
                            "Bem vindo a sua casa. Use 'N' para sair."
                        );
                        break;
                    } else {
                        SendClientMessage(playerid, COLOR_RED, "Essa casa nao eh sua.");
                        return 1;
                    }
                }
            }
        }
    }

    if (newkeys == KEY_NO) {
        if (p[playerid][EntrouCasa] > 0) {
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
    for(new i = 0; i < MAX_HOUSES; i++)
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
        SendClientMessage(playerid, COLOR_RED, "Ocorreu um erro. Esse ID esta em uso.");
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
        SendClientMessage(playerid, COLOR_ORANGE, "Casa criada com sucesso.");
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
        SendClientMessage(playerid, COLOR_ORANGE, string);    
        c[id][cPreco] = preco;
        c[id][cInt] = interi;
        DestroyDynamicPickup(c[id][cPickup]);
        DestroyDynamicMapIcon(c[id][cIcone]);
        DestroyDynamic3DTextLabel(c[id][cText]);
        CriarCasa(id);
        } else {
        SendClientMessage(playerid, COLOR_RED, "Ocorreu um erro. Esse ID nгo existe.");
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
        SendClientMessage(playerid, COLOR_ORANGE, string);    
        SetPlayerPos(playerid, c[id][cX], c[id][cY], c[id][cZ]);
        SetPlayerInterior(playerid, 0);
        } else {
        SendClientMessage(playerid, COLOR_RED, "Ocorreu um erro. Esse ID nгo existe.");
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
        SendClientMessage(playerid, COLOR_ORANGE, string);    
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
        SendClientMessage(playerid, COLOR_RED, "Ocorreu um erro. Esse ID nгo existe.");
    }
    DB_FreeResultSet(Result);
    return 1;
}
