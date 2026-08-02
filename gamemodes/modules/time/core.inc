#if defined _INC_core
	#endinput
#endif
#define _INC_time_core

#include <pp-hooks>

#define TIMER_INTERVAL      1000
#define MINUTES_PER_TICK    1

static s_WorldHour = 12;   
static s_WorldMinute = 0;     
 
static Text:txtTimeDisp = Text:INVALID_TEXT_DRAW;

hook OnGameModeInit()
{
    SetCityWeather(E_CITY_LS, E_WEATHER_EXTRASUNNY_SMOG);
    SetCityWeather(E_CITY_LV, E_WEATHER_EXTRASUNNY_MILD);   // Las Venturas com clima extra ensolarado ameno
    SetCityWeather(E_CITY_SF, E_WEATHER_SANDSTORM);         // San Fierro com tempestade de areia

    txtTimeDisp = TextDrawCreate(605.0, 25.0, "00:00");
    TextDrawUseBox(txtTimeDisp, false);
    TextDrawFont(txtTimeDisp, TEXT_DRAW_FONT_PRICEDOWN);
    TextDrawSetShadow(txtTimeDisp, false);
    TextDrawSetOutline(txtTimeDisp, 2);
    TextDrawBackgroundColour(txtTimeDisp, 0x000000FF);
    TextDrawColour(txtTimeDisp, 0xFFFFFFFF);
    TextDrawAlignment(txtTimeDisp, TEXT_DRAW_ALIGN_RIGHT);
    TextDrawLetterSize(txtTimeDisp, 0.5, 1.5);

    SetTimer("UpdateWorldTime", TIMER_INTERVAL, true);
    
    UpdateWorldTime();
}

hook OnPlayerConnect(playerid)
{
    SetPlayerTime(playerid, s_WorldHour, s_WorldMinute);

    if (txtTimeDisp != Text:INVALID_TEXT_DRAW) {
        TextDrawShowForPlayer(playerid, txtTimeDisp);
    }
}

hook OnPlayerSpawn(playerid)
{
    new cidade = GetPlayerCity(playerid);
    new Clima  = GetCityWeather(cidade);
    
    printf("Seu clima e %s sua cidade e %s", GetClimaName(Clima), GetCityName(cidade));
    return 1;
}

stock GetServerTime(&hour, &minute = 0)
{
    hour = s_WorldHour;
    minute = s_WorldMinute;
    return 1;
}

stock SetServerTime(hour, minute)
{
    s_WorldHour = hour;
    s_WorldMinute = minute;
    return 1;
}

forward UpdateWorldTime();
public UpdateWorldTime()
{
    s_WorldMinute += MINUTES_PER_TICK;

    if (s_WorldMinute >= 60) {
        s_WorldMinute -= 60;
        s_WorldHour++;

        if (s_WorldHour >= 24) {
            s_WorldHour = 0;
        }
    }

    foreach (new i : Player) {
        SetPlayerTime(i, s_WorldHour, s_WorldMinute);
    }

    new timeString[6];
    format(timeString, sizeof(timeString), "%02d:%02d", s_WorldHour, s_WorldMinute);
    TextDrawSetString(txtTimeDisp, timeString);
    return 1;
}
