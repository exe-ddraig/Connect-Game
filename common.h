#ifndef COMMON_H
#define COMMON_H

#include <SDL.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <deque>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <stdexcept>

using namespace std;

const int ROWS = 7;                             ////////////////////
const int COLS = 7;                             //    Constants   //
const int CELL_SIZE = 100;                      ////////////////////
static const char* DB_FILE = "users.txt";
static const char* HISTORY_FILE = "history.txt";


enum class GameState 
{
    MAIN_MENU,          // Before Any Login -> Only Login/Register/Exit                         /////////////////////////
    LOGIN_P1,           // P1 Login/Register                                                    //  Game State Machine //
    LOGIN_P2,           // P2 Login / Guest Choice                                              //                     //
    P2_GUEST_NAME,      // P2 Enters Guest Name                                                 //  Giving Meaningful  //
    POST_LOGIN_MENU,    // After Both Ready -> Start / History / HighScores / Logout            //      Names To       //
    HISTORY,            // Match History Screen                                                 //      Instances      //
    HIGH_SCORES,        // High Scores Screen                                                   //                     //
    SETTINGS,           // Colour Settings                                                      /////////////////////////
    PLAYING,            // Playing Game
    GAME_OVER           // Game Ending
};

struct CoinColor 
{                                                                       /////////////////////
    Uint8 r, g, b;                                                      //                 //
    const char* name;                                                   //                 //
};                                                                      //                 //
const CoinColor PALETTE[] = {                                           //                 //
    { 230,  60,  60, "RED"    },                                        // Colour  Palette //
    { 235, 200,  40, "YELLOW" },                                        //    For Coins'   //
    {  60, 160, 230, "BLUE"   },                                        //     Colours     //
    {  60, 210,  80, "GREEN"  },                                        //                 //
    { 210,  80, 210, "PURPLE" },                                        //                 //
    { 240, 140,  30, "ORANGE" },                                        /////////////////////
};
const int PALETTE_SIZE = 6;

struct MatchRecord 
{                                                                       /////////////////////////
    string p1Name;                                                      //                     //
    string p2Name;                                                      // Match History Entry //
    string result;   // Syntax -> "P1", "P2", "DRAW"                    //                     //
    string timeTaken;                                                   /////////////////////////
};

struct PlayerStats 
{                                                                       //////////////////////
    int wins = 0;                                                       //                  //
    int losses = 0;                                                     //   Player Stats   //
    int draws = 0;                                                      //                  //
    int total = 0;                                                      //////////////////////
};

struct UserAccount 
{
    string      username;                                               ///////////////////////
    string      password;                                               //  Default Account  //
    int         p1ColorIdx = 0;                                         //     Settings      //
    int         p2ColorIdx = 1;                                         ///////////////////////
    PlayerStats stats;
};

struct SessionPlayer 
{
    bool         isGuest = true;
    string       name;               // display name                    ///////////////////////////////////////
    UserAccount* account = nullptr;  // nullptr if guest                // Session Player's Default Settings //
    int          colorIdx = 0;                                          ///////////////////////////////////////
};


static string toLower(const string& s)
{
    string result = s;
    for (char& c : result) c = (char)tolower(c);
    return result;
}

#endif
