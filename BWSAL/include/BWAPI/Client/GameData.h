#pragma once

#include "UnitCommand.h"
#include "ForceData.h"
#include "PlayerData.h"
#include "UnitData.h"
#include "BulletData.h"
#include "Event.h"
#include "Command.h"
#include "Shape.h"

namespace BWAPIC
{
  struct GameData
  {
    GameData();
    //forces
    int forceCount;
    ForceData forces[4];

    //players
    int playerCount;
    PlayerData players[12];

    //units
    int initialUnitCount;
    UnitData units[10000];

    //unit table
    int unitArray[1700];

    //bullets
    BulletData bullets[100];

    int gameType;
    int latency;
    int frameCount;

    // user input
    int mouseX;
    int mouseY;
    bool mouseState[3];
    bool keyState[256];
    int screenX;
    int screenY;

    bool flags[2];

    // map
    int mapWidth;
    int mapHeight;
    char mapFilename[260];  //size based on broodwar memory
    char mapName[32];      //size based on broodwar memory
    int mapHash;

    //tile data
    int getGroundHeight[1024][1024];
    bool isWalkable[1024][1024]; 
    bool isBuildable[256][256];
    bool isVisible[256][256];
    bool isExplored[256][256];
    bool hasCreep[256][256];

    // start locations
    int startLocationCount;
    int startLocationsX[8];
    int startLocationsY[8];

    // match mode
    bool isInGame;
    bool isMultiplayer;
    bool isBattleNet;
    bool isPaused;
    bool isReplay;

    //selected units
    int selectedUnitCount;
    int selectedUnits[12];

    // players
    int self;

    //events from server to client
    int eventCount;
    Event events[1000];

    //strings (used in events, shapes, and commands)
    int stringCount;
    char strings[1000][256];

    //shapes, commands, unitCommands, from client to server
    int shapeCount;
    Shape shapes[10000];

    int commandCount;
    Command commands[10000];

    int unitCommandCount;
    UnitCommand unitCommands[10000];

  };
}