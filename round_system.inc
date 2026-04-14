/*

Dynamic Round System 

Credits
Special thanks to the following libraries and tools that made this system possible:

* Incognito Streamer Plugin
Used for dynamic objects and areas.
*BlueG MySQL Plugin
For MySQL database interaction.
*YSI Library
Especially y_hooks and y_iterate.
*sscanf Plugin
For command parameter parsing.
* Author
RoseeasT*/


#include <a_samp>
#include <streamer>
#include <a_mysql>
#include <YSI\y_hooks>
#include <YSI\y_iterate>
#include <sscanf2>

#define MAX_DYNAMIC_ROUND 300

enum e_rounddata
{
    RoundDBID,
    Float:RoundPos[3],
    STREAMER_TAG_OBJECT: RoundObject,
    STREAMER_TAG_AREA: RoundArea,
    RoundWorld,
    RoundInterior,
    RoundColor,
    Float:RoundSize,
    bool:RoundExists
};

new RoundData[MAX_DYNAMIC_ROUND][e_rounddata],
    Iterator:Round<MAX_DYNAMIC_ROUND>;

Round_Reset()
{
    for(new i; i < MAX_DYNAMIC_ROUND; i++)
    {
        RoundData[i][RoundExists] = false;
        RoundData[i][RoundDBID] = 0;
        RoundData[i][RoundObject] = STREAMER_TAG_OBJECT: INVALID_STREAMER_ID;
        RoundData[i][RoundArea] = STREAMER_TAG_AREA: INVALID_STREAMER_ID;
    }
    Iter_Clear(Round);
}

Round_Refresh(id)
{
    if(id == -1 || !RoundData[id][RoundExists]) return 0;

    if(IsValidDynamicObject(RoundData[id][RoundObject]))
        DestroyDynamicObject(RoundData[id][RoundObject]);
    if(IsValidDynamicArea(RoundData[id][RoundArea]))
        DestroyDynamicArea(RoundData[id][RoundArea]);

    RoundData[id][RoundObject] = CreateDynamicObject(
        1316,
        RoundData[id][RoundPos][0],
        RoundData[id][RoundPos][1],
        RoundData[id][RoundPos][2] - 0.8,
        0.0,0.0,0.0,
        RoundData[id][RoundWorld],
        RoundData[id][RoundInterior],
        -1,
        RoundData[id][RoundSize],
        RoundData[id][RoundSize]
    );
    SetDynamicObjectMaterial(
        RoundData[id][RoundObject],
        0,
        -1,
        "none",
        "none",
        RoundData[id][RoundColor]
    );

    RoundData[id][RoundArea] = CreateDynamicSphere(
        RoundData[id][RoundPos][0],
        RoundData[id][RoundPos][1],
        RoundData[id][RoundPos][2],
        2.0,
        RoundData[id][RoundWorld],
        RoundData[id][RoundInterior],
        -1
    );

    return 1;
}

ShowRoundForPlayer(playerid, id)
{
    if(!RoundData[id][RoundExists]) return 0;

    if(!IsValidDynamicObject(RoundData[id][RoundObject]) || !IsValidDynamicArea(RoundData[id][RoundArea]))
        Round_Refresh(id);

    return 1;
}

forward Load_Round();
public Load_Round()
{
    new rows = cache_num_rows();
    printf("[ROUND DEBUG] Load_Round called. Rows: %d", rows);
    if(!rows) return 1;

    for(new i; i < rows; i++)
    {
        new id = Iter_Free(Round);
        if(id == -1) break;

        cache_get_value_name_int(i, "id", RoundData[id][RoundDBID]);
        cache_get_value_name_float(i, "x", RoundData[id][RoundPos][0]);
        cache_get_value_name_float(i, "y", RoundData[id][RoundPos][1]);
        cache_get_value_name_float(i, "z", RoundData[id][RoundPos][2]);
        cache_get_value_name_int(i, "world", RoundData[id][RoundWorld]);
        cache_get_value_name_int(i, "interior", RoundData[id][RoundInterior]);
        cache_get_value_name_int(i, "color", RoundData[id][RoundColor]);
        cache_get_value_name_float(i, "size", RoundData[id][RoundSize]);

        RoundData[id][RoundExists] = true;
        RoundData[id][RoundObject] = STREAMER_TAG_OBJECT: INVALID_STREAMER_ID;
        RoundData[id][RoundArea] = STREAMER_TAG_AREA: INVALID_STREAMER_ID;

        Iter_Add(Round, id);
        Round_Refresh(id);

        printf("[ROUND DEBUG] Loaded ID=%d Pos=(%f,%f,%f) World=%d Int=%d Color=%d Size=%f",
            RoundData[id][RoundDBID],
            RoundData[id][RoundPos][0],
            RoundData[id][RoundPos][1],
            RoundData[id][RoundPos][2],
            RoundData[id][RoundWorld],
            RoundData[id][RoundInterior],
            RoundData[id][RoundColor],
            RoundData[id][RoundSize]
        );
    }

    printf("[ROUND]: Loaded %d rounds", rows);
    return 1;
}

Round_Save(id)
{
    if(!RoundData[id][RoundExists]) return 0;

    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "UPDATE rounds SET x=%f,y=%f,z=%f,world=%d,interior=%d,color=%d,size=%f WHERE id=%d",
        RoundData[id][RoundPos][0],
        RoundData[id][RoundPos][1],
        RoundData[id][RoundPos][2],
        RoundData[id][RoundWorld],
        RoundData[id][RoundInterior],
        RoundData[id][RoundColor],
        RoundData[id][RoundSize],
        RoundData[id][RoundDBID]
    );
    mysql_tquery(g_SQL, query);
    return 1;
}

stock GetColorByName(const name[])
{
    if(!strcmp(name, "merah", true)) return 0xF55555FF;
    if(!strcmp(name, "hijau", true)) return 0x4BFC4BFF;
    if(!strcmp(name, "biru", true)) return 0x4C4CFFFF;
    if(!strcmp(name, "kuning", true)) return 0xFFFF00FF;
    if(!strcmp(name, "pink", true)) return 0xF868F8FF;
    if(!strcmp(name, "putih", true)) return 0xFFFFFFFF;
    if(!strcmp(name, "hitam", true)) return 0x000000FF;
    return -1;
}

CMD:createround(playerid, params[])
{
    if(AccountData[playerid][pAdmin] < 7) return PermissionError(playerid);

    new colorName[32]; 
    new Float:size;
    if(sscanf(params, "s[32]f", colorName, size))
        return ShowTDN(playerid, NOTIFICATION_SYNTAX, "Usage: /createround [color] [size]");

    new color = GetColorByName(colorName);
    if(color == -1) return ShowTDN(playerid, NOTIFICATION_ERROR, "Warna tidak valid!");

    new id = Iter_Free(Round);
    if(id == -1) return ShowTDN(playerid, NOTIFICATION_ERROR, "Max round reached!");

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    RoundData[id][RoundExists] = true;
    RoundData[id][RoundPos][0] = x;
    RoundData[id][RoundPos][1] = y;
    RoundData[id][RoundPos][2] = z;
    RoundData[id][RoundWorld] = GetPlayerVirtualWorld(playerid);
    RoundData[id][RoundInterior] = GetPlayerInterior(playerid);
    RoundData[id][RoundColor] = color;
    RoundData[id][RoundSize] = size;
    RoundData[id][RoundObject] = STREAMER_TAG_OBJECT: INVALID_STREAMER_ID;
    RoundData[id][RoundArea] = STREAMER_TAG_AREA: INVALID_STREAMER_ID;

    Iter_Add(Round, id);
    Round_Refresh(id);

    new query[256];
    mysql_format(g_SQL, query, sizeof(query),
        "INSERT INTO rounds (x,y,z,world,interior,color,size) VALUES (%f,%f,%f,%d,%d,%d,%f)",
        x, y, z,
        RoundData[id][RoundWorld],
        RoundData[id][RoundInterior],
        color,
        size
    );

    mysql_tquery(g_SQL, query, "OnRoundCreated", "d", id);
    SendClientMessage(playerid, -1, sprintf("Round created ID: %d", id));
    return 1;
}

CMD:editround(playerid, params[])
{
    if(AccountData[playerid][pAdmin] < 7) return PermissionError(playerid);

    new id;
    new type[24], value[32];
    if(sscanf(params, "ds[24]s[32]", id, type, value))
        return ShowTDN(playerid, NOTIFICATION_SYNTAX, "Usage: /editround [id] [type] [value]");

    if(!Iter_Contains(Round, id))
        return ShowTDN(playerid, NOTIFICATION_ERROR, "Invalid ID");

    if(!strcmp(type, "delete", true))
    {
        if(IsValidDynamicObject(RoundData[id][RoundObject]))
            DestroyDynamicObject(RoundData[id][RoundObject]);
        if(IsValidDynamicArea(RoundData[id][RoundArea]))
            DestroyDynamicArea(RoundData[id][RoundArea]);

        new query[128];
        mysql_format(g_SQL, query, sizeof(query),
            "DELETE FROM rounds WHERE id=%d",
            RoundData[id][RoundDBID]
        );
        mysql_tquery(g_SQL, query);

        Iter_Remove(Round, id);
        RoundData[id][RoundExists] = false;

        return ShowTDN(playerid, NOTIFICATION_SUKSES, "Round deleted!");
    }
    else if(!strcmp(type, "pos", true))
    {
        GetPlayerPos(playerid,
            RoundData[id][RoundPos][0],
            RoundData[id][RoundPos][1],
            RoundData[id][RoundPos][2]
        );
    }
    else if(!strcmp(type, "vw", true))
    {
        RoundData[id][RoundWorld] = strval(value);
    }
    else if(!strcmp(type, "int", true))
    {
        RoundData[id][RoundInterior] = strval(value);
    }
    else if(!strcmp(type, "color", true))
    {
        new col = GetColorByName(value);
        if(col == -1) return ShowTDN(playerid, NOTIFICATION_ERROR, "Warna tidak valid!");
        RoundData[id][RoundColor] = col;
    }
    else if(!strcmp(type, "size", true))
    {
        RoundData[id][RoundSize] = floatstr(value);
    }

    Round_Refresh(id);
    Round_Save(id);

    SendClientMessage(playerid, -1, "Round updated!");
    return 1;
}

CMD:nearround(playerid, params[])
{
    if(AccountData[playerid][pAdmin] < 1) return PermissionError(playerid);

    foreach(new i : Round)
    {
        if(!RoundData[i][RoundExists]) continue;

        if(IsPlayerInRangeOfPoint(playerid, 2.0,
            RoundData[i][RoundPos][0],
            RoundData[i][RoundPos][1],
            RoundData[i][RoundPos][2]))
        {
            return Warning(playerid, "Dynamic Round on nearest you ID %d", i);
        }
    }

    return 1;
}

hook OnPlayerSpawn(playerid)
{
    foreach(new i : Round)
    {
        ShowRoundForPlayer(playerid, i);
    }
    return 1;
}

hook OnGameModeExit()
{
    foreach(new i : Round)
    {
        Round_Save(i);
    }
    return 1;
}

hook OnGameModeInit()
{
    Round_Reset();
    SetTimerEx("LoadRoundTimer", 2000, false, "");
    return 1;
}

public LoadRoundTimer()
{
    mysql_tquery(g_SQL, "SELECT * FROM rounds", "Load_Round");
}

forward OnRoundCreated(id);
public OnRoundCreated(id)
{
    RoundData[id][RoundDBID] = cache_insert_id();
    Round_Refresh(id);
    return 1;
}
