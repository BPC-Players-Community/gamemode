#if defined _INC_company_core
	#endinput
#endif
#define _INC_company_core

/* Alerta de fofura
    Estou testando em hardcoded mesmo, 
    pretendo carregar tudo via bando de dados

*/

#include <pp-hooks>

#define MAX_COMPANIES          (1)

enum e_COMPANY_DATA
{
    /*
    E_COMPANY_SQL_ID,
    */

    E_COMPANY_NAME[32],

    bool:E_COMPANY_LOCKED,

    E_COMPANY_OPEN,
    E_COMPANY_CLOSE,

    Float:E_COMPANY_ENTER_X,
    Float:E_COMPANY_ENTER_Y,
    Float:E_COMPANY_ENTER_Z,
    E_COMPANY_ENTER_INTERIOR,

    Float:E_COMPANY_EXIT_X,
    Float:E_COMPANY_EXIT_Y,
    Float:E_COMPANY_EXIT_Z,
    E_COMPANY_EXIT_INTERIOR
};

new CompanyData[MAX_COMPANIES][e_COMPANY_DATA] = 
{
    {
        "Binco", false,
        8, 18, 
        2244.3909,-1665.5671,15.4766,
        0,
        207.5219,-109.7448,1005.1328,
        15
    }  
};

CMD:entrada(playerid)
{
    SetPlayerPos(playerid, 2244.3909, -1665.5671, 15.4766);
    SetPlayerInterior(playerid, 0);
    return 1;
}

CMD:saida(playerid)
{
    SetPlayerPos(playerid, 207.5219,-109.7448,1005.1328);
    SetPlayerInterior(playerid, 15);
    return 1;
}

enum e_ENTITY_TYPE
{
    ENTITY_NONE,

    ENTITY_COMPANY_ENTER_PICKUP,
    ENTITY_COMPANY_EXIT_PICKUP,

    ENTITY_COMPANY_ENTER_AREA,

    ENTITY_COMPANY_COUNTER_CP
};

#define MAX_DYNAMIC_PICKUPS 8192

enum E_PICKUP_ENTITY
{
    E_PICKUP_COMPANY,

    E_PICKUP_TYPE
};

new PickupEntity[MAX_DYNAMIC_PICKUPS][E_PICKUP_ENTITY];

new STREAMER_TAG_PICKUP:pickup;

hook OnGameModeInit()
{
    if (CreateCompany()) {
        printf("Empresa criada com sucesso");
    } else {
        printf("ERRO: Falha ao criar a empresa");
    }
}

stock GetCompanyBusinessHour(companyid, &open, &close)
{
    open = CompanyData[companyid][E_COMPANY_OPEN];
    close = CompanyData[companyid][E_COMPANY_CLOSE];
    return 1;
}

CMD:horario(playerid)
{
    new open, close;
    GetCompanyBusinessHour(0, open, close);
    SendClientMessage(playerid, -1, "%d fecha %d", open, close);
    return 1;
}

stock SetCompanyLocked(companyid, bool:locked)
{
    CompanyData[companyid][E_COMPANY_LOCKED] = locked;
    return 1;
}

stock IsCompanyLocked(companyid)
{
    new open, close;
    GetCompanyBusinessHour(companyid, open, close);

    new hour;
    GetServerTime(hour);

    if (hour >= open && hour <= close) return 1;
    else return 0;
    //return CompanyData[companyid][E_COMPANY_LOCKED];
}

stock CreateCompany(id = 0)
{
    pickup = CreateDynamicPickup(
        19135,
        1,
        CompanyData[id][E_COMPANY_ENTER_X],
        CompanyData[id][E_COMPANY_ENTER_Y],
        CompanyData[id][E_COMPANY_ENTER_Z]
    );
    if (!IsValidDynamicPickup(pickup)) {
        printf("ERRO: Falha ao criar o pickup de entrada");
    }
    else {
        printf("Pickup criado: %d", _:pickup);
    }

    PickupEntity[_:pickup][E_PICKUP_COMPANY] = id;
    PickupEntity[_:pickup][E_PICKUP_TYPE] = ENTITY_COMPANY_ENTER_PICKUP;

    pickup = CreateDynamicPickup(
        19135,
        1,
        CompanyData[id][E_COMPANY_EXIT_X],
        CompanyData[id][E_COMPANY_EXIT_Y],
        CompanyData[id][E_COMPANY_EXIT_Z]
    );
    if (!IsValidDynamicPickup(pickup)) {
        printf("ERRO: Falha ao criar o pickup de entrada");
    } else {
        printf("Pickup criado: %d", _:pickup);
    }
    PickupEntity[_:pickup][E_PICKUP_COMPANY] = id;
    PickupEntity[_:pickup][E_PICKUP_TYPE] = ENTITY_COMPANY_EXIT_PICKUP;
    return 1;
}

public OnPlayerPickUpDynamicPickup(playerid, STREAMER_TAG_PICKUP:pickupid)
{
    new companyid = PickupEntity[_:pickupid][E_PICKUP_COMPANY];

    switch (PickupEntity[_:pickupid][E_PICKUP_TYPE]) {
        case ENTITY_COMPANY_ENTER_PICKUP: {
            if (IsCompanyLocked(companyid)) {
                SendClientMessage(playerid, -1, "A empresa esta trancada.");
                return 1;
            }

            SetPlayerPos(
                playerid,
                CompanyData[companyid][E_COMPANY_EXIT_X],
                CompanyData[companyid][E_COMPANY_EXIT_Y] + 2,
                CompanyData[companyid][E_COMPANY_EXIT_Z]
            );
            SetPlayerInterior(playerid, CompanyData[companyid][E_COMPANY_EXIT_INTERIOR]);
        }

        case ENTITY_COMPANY_EXIT_PICKUP: {
            SetPlayerPos(
                playerid,
                CompanyData[companyid][E_COMPANY_ENTER_X],
                CompanyData[companyid][E_COMPANY_ENTER_Y] +2,
                CompanyData[companyid][E_COMPANY_ENTER_Z]
            );
            SetPlayerInterior(playerid, CompanyData[companyid][E_COMPANY_ENTER_INTERIOR]);
        }
    }
    return 1;
}

// public OnPlayerEnterDynamicCP(playerid, STREAMER_TAG_CP:checkpointid)
// {
//     new companyid = CPEntity[_:checkpointid][E_CP_COMPANY];

//     switch (CPEntity[_:checkpointid][E_CP_TYPE])
//     {
//         case ENTITY_COMPANY_COUNTER_CP:
//         {
//             ShowCompanyStore(playerid, companyid);
//         }
//     }

//     return 1;
// }
