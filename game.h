#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "user.h"

////////////////////////////////////////////////////////////////////////
struct Particle 
{
    float x, y, vx, vy, life, maxLife;
    Uint8 r, g, b;
};
struct DroppingCoin 
{
    int col, row, player;
    float startY, endY, t;
    bool done;
};
////////////////////////////////////////////////////////////////////////

// Helper
////////////////////////////////////////////////////////////////////////
static float easeSmoothstep(float t) 
{
    if (t <= 0.f)
    {
        return 0.f;
    }
    if (t >= 1.f)
    {
        return 1.f;
    }
    return t * t * (3.f - 2.f * t);
}
////////////////////////////////////////////////////////////////////////

// Game class
////////////////////////////////////////////////////////////////////////
class Game 
{
    private:
        // SDL
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        bool          sdlReady = false;
        int screenW = 0, screenH = 0;

        // State
        GameState state = GameState::MAIN_MENU;

        // Timing
        Uint32 prevTicks = 0;
        float  gameTime = 0.f;
        Uint32 matchStartTicks = 0;

        // Managers
        AccountManager  acctMgr;
        HistoryManager  histMgr;

        // Session
        SessionPlayer p1, p2;

        // Text input
        enum class InputField { USERNAME, PASSWORD, GUEST_NAME };
        string     inputUsername;
        string     inputPassword;
        string     inputGuestName;
        InputField activeField = InputField::USERNAME;
        string     statusMsg;
        float      statusTimer = 0.f;

        // Which player is logging in (1 or 2)
        int loginFor = 1;   // 1 = setting up P1, 2 = setting up P2

        // Settings
        int settingsPlayer = 1;

        // Scroll for history
        int historyScroll = 0;

        // Board / gameplay
        Board board;
        int   offsetX = 0, offsetY = 0;
        int   currentPlayer = 1;
        int   hoverCol = -1;
        int   winner = 0;
        float bobPhase = 0.f;
        float hoverAlpha = 0.f;
        float announceT = 0.f;

        vector<Particle>     particles;
        vector<DroppingCoin> dropping;

        // Stars
        struct Star 
        { 
            int x, y;
            float phase;
        };
        vector<Star> stars;

        // Pixel-font glyphs (5Ã—7) â€” identical to original
        ////////////////////////////////////////////////////////////////////////
        struct Glyph 
        { 
            int d[7][5];
        };
        static const int GLYPH_COUNT = 55;
        Glyph glyphs[GLYPH_COUNT];

        void cleanupSdl() 
        {
            if (renderer) 
            { 
                SDL_DestroyRenderer(renderer);
                renderer = nullptr; 
            }
            if (window) 
            { 
                SDL_DestroyWindow(window);
                window = nullptr; 
            }
            if (sdlReady) 
            { 
                SDL_Quit();
                sdlReady = false; 
            }
        }

        void initGlyphs() 
        {
            int P[7][5]      = { {1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0} };
            int L[7][5]      = { {1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,1,1,1,1} };
            int A[7][5]      = { {0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1} };
            int Y[7][5]      = { {1,0,0,0,1},{1,0,0,0,1},{0,1,0,1,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0} };
            int E[7][5]      = { {1,1,1,1,1},{1,0,0,0,0},{1,0,0,0,0},{1,1,1,1,0},{1,0,0,0,0},{1,0,0,0,0},{1,1,1,1,1} };
            int R[7][5]      = { {1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,0},{1,0,1,0,0},{1,0,0,1,0},{1,0,0,0,1} };
            int ONE[7][5]    = { {0,0,1,0,0},{0,1,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,1,1,1,0} };
            int TWO[7][5]    = { {0,1,1,1,0},{1,0,0,0,1},{0,0,0,0,1},{0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0},{1,1,1,1,1} };
            int W[7][5]      = { {1,0,0,0,1},{1,0,0,0,1},{1,0,1,0,1},{1,0,1,0,1},{1,0,1,0,1},{1,1,0,1,1},{1,0,0,0,1} };
            int I[7][5]      = { {0,1,1,1,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,1,1,1,0} };
            int N[7][5]      = { {1,0,0,0,1},{1,1,0,0,1},{1,0,1,0,1},{1,0,0,1,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1} };
            int S[7][5]      = { {0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,0},{0,1,1,1,0},{0,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0} };
            int D[7][5]      = { {1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,0} };
            int EXCL[7][5]   = { {0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,0,0,0},{0,0,1,0,0} };
            int SPC[7][5]    = { {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0} };
            int T[7][5]      = { {1,1,1,1,1},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0} };
            int O[7][5]      = { {0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0} };
            int G[7][5]      = { {0,1,1,1,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,1,1,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0} };
            int M[7][5]      = { {1,0,0,0,1},{1,1,0,1,1},{1,0,1,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1} };
            int U[7][5]      = { {1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0} };
            int C[7][5]      = { {0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,1},{0,1,1,1,0} };
            int F[7][5]      = { {1,1,1,1,1},{1,0,0,0,0},{1,0,0,0,0},{1,1,1,1,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0} };
            int H[7][5]      = { {1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1} };
            int K[7][5]      = { {1,0,0,1,0},{1,0,1,0,0},{1,1,0,0,0},{1,0,1,0,0},{1,0,0,1,0},{1,0,0,0,1},{1,0,0,0,1} };
            int B[7][5]      = { {1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,0} };
            int V[7][5]      = { {1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,0,1,0},{0,1,0,1,0},{0,0,1,0,0} };
            int X[7][5]      = { {1,0,0,0,1},{0,1,0,1,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,1,0,1,0},{1,0,0,0,1} };
            int Z[7][5]      = { {1,1,1,1,1},{0,0,0,0,1},{0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0},{1,0,0,0,0},{1,1,1,1,1} };
            int Q[7][5]      = { {0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,1,0,1},{1,0,0,1,0},{1,0,0,0,1},{0,1,1,1,1} };
            int SLASH[7][5]  = { {0,0,0,0,1},{0,0,0,1,0},{0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0},{0,1,0,0,0},{1,0,0,0,0} };
            int BSLASH[7][5] = { {1,0,0,0,0},{0,1,0,0,0},{0,1,0,0,0},{0,0,1,0,0},{0,0,0,1,0},{0,0,0,1,0},{0,0,0,0,1} };
            int DOT[7][5]    = { {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,1,0,0} };
            int COMMA[7][5]  = { {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,1,0,0,0} };
            int COLON[7][5]  = { {0,0,0,0,0},{0,0,1,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,1,0,0},{0,0,0,0,0} };
            int QMARK[7][5]  = { {0,1,1,1,0},{1,0,0,0,1},{0,0,0,1,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,0,0,0},{0,0,1,0,0} };
            int THREE[7][5]  = { {1,1,1,1,0},{0,0,0,0,1},{0,0,0,0,1},{0,1,1,1,0},{0,0,0,0,1},{0,0,0,0,1},{1,1,1,1,0} };
            int FOUR[7][5]   = { {1,0,0,1,0},{1,0,0,1,0},{1,0,0,1,0},{1,1,1,1,1},{0,0,0,1,0},{0,0,0,1,0},{0,0,0,1,0} };
            int FIVE[7][5]   = { {1,1,1,1,1},{1,0,0,0,0},{1,0,0,0,0},{1,1,1,1,0},{0,0,0,0,1},{0,0,0,0,1},{1,1,1,1,0} };
            int SIX[7][5]    = { {0,1,1,1,0},{1,0,0,0,0},{1,0,0,0,0},{1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0} };
            int SEVEN[7][5]  = { {1,1,1,1,1},{0,0,0,0,1},{0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0},{0,1,0,0,0},{0,1,0,0,0} };
            int EIGHT[7][5]  = { {0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0} };
            int NINE[7][5]   = { {0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,1},{0,0,0,0,1},{0,0,0,0,1},{0,1,1,1,0} };
            int ZERO[7][5]   = { {0,1,1,1,0},{1,0,0,0,1},{1,0,0,1,1},{1,0,1,0,1},{1,1,0,0,1},{1,0,0,0,1},{0,1,1,1,0} };
            int J[7][5]      = { {0,0,1,1,1},{0,0,0,1,0},{0,0,0,1,0},{0,0,0,1,0},{1,0,0,1,0},{1,0,0,1,0},{0,1,1,0,0} };
            int LARR[7][5]   = { {0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0},{1,0,0,0,0},{0,1,0,0,0},{0,0,1,0,0},{0,0,0,1,0} };
            int RARR[7][5]   = { {0,1,0,0,0},{0,0,1,0,0},{0,0,0,1,0},{0,0,0,0,1},{0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0} };
            int UARR[7][5]   = { {0,0,1,0,0},{0,1,0,1,0},{1,0,0,0,1},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0} };
            int DARR[7][5]   = { {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{1,0,0,0,1},{0,1,0,1,0},{0,0,1,0,0},{0,0,0,0,0} };
            int PLUS[7][5]   = { {0,0,0,0,0},{0,0,1,0,0},{0,0,1,0,0},{1,1,1,1,1},{0,0,1,0,0},{0,0,1,0,0},{0,0,0,0,0} };
            int MINUS[7][5]  = { {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{1,1,1,1,1},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0} };
            int EQ[7][5]     = { {0,0,0,0,0},{1,1,1,1,1},{0,0,0,0,0},{0,0,0,0,0},{1,1,1,1,1},{0,0,0,0,0},{0,0,0,0,0} };
            int UND[7][5]    = { {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{1,1,1,1,1} };
            int LPAR[7][5]   = { {0,0,1,1,0},{0,1,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{0,1,0,0,0},{0,0,1,1,0} };
            int RPAR[7][5]   = { {0,1,1,0,0},{0,0,0,1,0},{0,0,0,0,1},{0,0,0,0,1},{0,0,0,0,1},{0,0,0,1,0},{0,1,1,0,0} };

            int rows7[GLYPH_COUNT][7][5];

            memcpy(rows7[0],    P,      sizeof(P));         memcpy(rows7[1],    L,      sizeof(L));
            memcpy(rows7[2],    A,      sizeof(A));         memcpy(rows7[3],    Y,      sizeof(Y));
            memcpy(rows7[4],    E,      sizeof(E));         memcpy(rows7[5],    R,      sizeof(R));
            memcpy(rows7[6],    ONE,    sizeof(ONE));       memcpy(rows7[7],    TWO,    sizeof(TWO));
            memcpy(rows7[8],    W,      sizeof(W));         memcpy(rows7[9],    I,      sizeof(I));
            memcpy(rows7[10],   N,      sizeof(N));         memcpy(rows7[11],   S,      sizeof(S));
            memcpy(rows7[12],   D,      sizeof(D));         memcpy(rows7[13],   R,      sizeof(R));
            memcpy(rows7[14],   EXCL,   sizeof(EXCL));      memcpy(rows7[15],   SPC,    sizeof(SPC));
            memcpy(rows7[16],   T,      sizeof(T));         memcpy(rows7[17],   O,      sizeof(O));
            memcpy(rows7[18],   G,      sizeof(G));         memcpy(rows7[19],   M,      sizeof(M));
            memcpy(rows7[20],   U,      sizeof(U));         memcpy(rows7[21],   C,      sizeof(C));
            memcpy(rows7[22],   F,      sizeof(F));         memcpy(rows7[23],   H,      sizeof(H));
            memcpy(rows7[24],   K,      sizeof(K));         memcpy(rows7[25],   B,      sizeof(B));
            memcpy(rows7[26],   V,      sizeof(V));         memcpy(rows7[27],   X,      sizeof(X));
            memcpy(rows7[28],   Z,      sizeof(Z));         memcpy(rows7[29],   Q,      sizeof(Q));
            memcpy(rows7[30],   SLASH,  sizeof(SLASH));     memcpy(rows7[31],   BSLASH, sizeof(BSLASH));
            memcpy(rows7[32],   DOT,    sizeof(DOT));       memcpy(rows7[33],   COMMA,  sizeof(COMMA));
            memcpy(rows7[34],   COLON,  sizeof(COLON));     memcpy(rows7[35],   QMARK,  sizeof(QMARK));
            memcpy(rows7[36],   THREE,  sizeof(THREE));     memcpy(rows7[37],   FOUR,   sizeof(FOUR));
            memcpy(rows7[38],   FIVE,   sizeof(FIVE));      memcpy(rows7[39],   SIX,    sizeof(SIX));
            memcpy(rows7[40],   SEVEN,  sizeof(SEVEN));     memcpy(rows7[41],   EIGHT,  sizeof(EIGHT));
            memcpy(rows7[42],   NINE,   sizeof(NINE));      memcpy(rows7[43],   ZERO,   sizeof(ZERO));
            memcpy(rows7[44],   J,      sizeof(J));         memcpy(rows7[45],   LARR,   sizeof(LARR));
            memcpy(rows7[46],   RARR,   sizeof(RARR));      memcpy(rows7[47],   UARR,   sizeof(UARR));
            memcpy(rows7[48],   DARR,   sizeof(DARR));      memcpy(rows7[49],   PLUS,   sizeof(PLUS));
            memcpy(rows7[50],   MINUS,  sizeof(MINUS));     memcpy(rows7[51],   EQ,     sizeof(EQ));
            memcpy(rows7[52],   UND,    sizeof(UND));       memcpy(rows7[53],   LPAR,   sizeof(LPAR));
            memcpy(rows7[54],   RPAR,   sizeof(RPAR));

            for (int g = 0;g < GLYPH_COUNT;g++)
            {
                memcpy(glyphs[g].d, rows7[g], sizeof(glyphs[g].d));
            }
        }

        int charToGlyph(char c) const 
        {
            switch (toupper(c)) 
            {
                case 'P':   return 0;       case 'L':   return 1;
                case 'A':   return 2;       case 'Y':   return 3;
                case 'E':   return 4;       case 'R':   return 5;
                case '1':   return 6;       case '2':   return 7; 
                case 'W':   return 8;       case 'I':   return 9;
                case 'N':   return 10;      case 'S':   return 11;
                case 'D':   return 12;      case '!':   return 14;
                case ' ':   return 15;      case 'T':   return 16;
                case 'O':   return 17;      case 'G':   return 18;
                case 'M':   return 19;      case 'U':   return 20;
                case 'C':   return 21;      case 'F':   return 22;
                case 'H':   return 23;      case 'K':   return 24;
                case 'B':   return 25;      case 'V':   return 26;
                case 'X':   return 27;      case 'Z':   return 28;
                case 'Q':   return 29;      case '/':   return 30;
                case '.':   return 32;      case ',':   return 33;
                case ':':   return 34;      case '?':   return 35;
                case '3':   return 36;      case '4':   return 37;
                case '5':   return 38;      case '6':   return 39;
                case '7':   return 40;      case '8':   return 41;
                case '9':   return 42;      case '0':   return 43;
                case 'J':   return 44;      case '<':   return 45;
                case '>':   return 46;      case '+':   return 49;
                case '-':   return 50;      case '=':   return 51;
                case '_':   return 52;      case '(':   return 53;
                case ')':   return 54;      default:    return 15;
            }
        }
        ////////////////////////////////////////////////////////////////////////

        // Low-level Drawings Helpers
        ////////////////////////////////////////////////////////////////////////
        void drawFilledCircle(int cx, int cy, int r) 
        {
            for (int y = -r;y <= r;y++) 
            {
                int dx = (int)sqrt((double)(r * r - y * y));
                SDL_RenderDrawLine(renderer, cx - dx, cy + y, cx + dx, cy + y);
            }
        }
        void drawRing(int cx, int cy, int radius) 
        {
            int x = radius, y = 0, err = 0;
            while (x >= y) 
            {
                SDL_RenderDrawPoint(renderer, cx + x, cy + y);SDL_RenderDrawPoint(renderer, cx + y, cy + x);
                SDL_RenderDrawPoint(renderer, cx - y, cy + x);SDL_RenderDrawPoint(renderer, cx - x, cy + y);
                SDL_RenderDrawPoint(renderer, cx - x, cy - y);SDL_RenderDrawPoint(renderer, cx - y, cy - x);
                SDL_RenderDrawPoint(renderer, cx + y, cy - x);SDL_RenderDrawPoint(renderer, cx + x, cy - y);
                if(err <= 0) 
                { 
                    y++;
                    err += 2 * y + 1;
                }
                else 
                { 
                    x--;
                    err -= 2 * x + 1; 
                }
            }
        }
        ////////////////////////////////////////////////////////////////////////
    
        // Glyphs / Text Helpers
        ////////////////////////////////////////////////////////////////////////
        void drawGlyph(int gi, int px, int py, Uint8 r, Uint8 g, Uint8 b, int scale = 4, Uint8 alpha = 255) 
        {
            if (gi < 0 || gi >= GLYPH_COUNT)
            {
                gi = 15;
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
            for (int row = 0;row < 7;row++)
            {
                for (int col = 0;col < 5;col++)
                {
                    if (glyphs[gi].d[row][col])
                    {
                        SDL_Rect dot = { px + col * scale,py + row * scale,scale - 1,scale - 1 };
                        SDL_RenderFillRect(renderer, &dot);
                    }
                }
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
        int textWidth(const string& s, int scale) const 
        { 
            return (int)s.size() * (5 * scale + 2) - 2;
        }
        int textHeight(int scale) const 
        {
            return 7 * scale; 
        }

        void drawTextCX(const string& s, int cx, int y, Uint8 r, Uint8 g, Uint8 b, int scale = 4, Uint8 alpha = 255) 
        {
            int tw = textWidth(s, scale);
            int px = cx - tw / 2;
            int cw = 5 * scale + 2;
            for (size_t i = 0;i < s.size();i++)
            {
                drawGlyph(charToGlyph(s[i]), px + (int)i * cw, y, r, g, b, scale, alpha);
            }
        }
        void drawText(const string& s, int x, int y, Uint8 r, Uint8 g, Uint8 b, int scale = 4, Uint8 alpha = 255) 
        {
            int cw = 5 * scale + 2;
            for (size_t i = 0;i < s.size();i++)
            {
                drawGlyph(charToGlyph(s[i]), x + (int)i * cw, y, r, g, b, scale, alpha);
            }
        }
        void drawTextFit(const string& s, int cx, int y, int maxW, Uint8 r, Uint8 g, Uint8 b, int preferScale = 4, Uint8 alpha = 255) 
        {
            int scale = preferScale;
            while (scale > 1 && textWidth(s, scale) > maxW)
            {
                scale--;
            }
            drawTextCX(s, cx, y, r, g, b, scale, alpha);
        }
        ////////////////////////////////////////////////////////////////////////

        // Coins
        ////////////////////////////////////////////////////////////////////////
        void getPlayerColor(int playerSlot, Uint8& r, Uint8& g, Uint8& b) const 
        {
            int idx = (playerSlot == 1) ? p1.colorIdx : p2.colorIdx;
            idx = max(0, min(idx, PALETTE_SIZE - 1));
            r = PALETTE[idx].r; g = PALETTE[idx].g; b = PALETTE[idx].b;
        }

        void drawCoin(int cx, int cy, int player, int radius) 
        {
            Uint8 r, g, b;
            getPlayerColor(player, r, g, b);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            for (int i = 3;i >= 1;i--) 
            {
                Uint8 a = (Uint8)(50 * (3 - i) / 3);
                SDL_SetRenderDrawColor(renderer, r, g, b, a);
                drawRing(cx, cy, radius + i * 3);
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            drawFilledCircle(cx, cy, radius);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 90);
            drawRing(cx, cy, radius);
            drawRing(cx, cy, radius - 1);
            int hr = radius * 5 / 10, hx = cx - radius / 4, hy = cy - radius / 4;
            for (int dy = -hr;dy <= 0;dy++) 
            {
                int dx2 = (int)sqrt((double)(hr * hr - dy * dy));
                Uint8 a = (Uint8)(90 * (1.f - (float)abs(dy) / hr));
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, a);
                SDL_RenderDrawLine(renderer, hx - dx2 / 2, hy + dy, hx + dx2 / 2, hy + dy);
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
        ////////////////////////////////////////////////////////////////////////
    
        // Particles
        ////////////////////////////////////////////////////////////////////////
        void spawnParticles(int col, int row, int player) 
        {
            Uint8 r, g, b;
            getPlayerColor(player, r, g, b);
            int cx = offsetX + col * CELL_SIZE + CELL_SIZE / 2;
            int cy = offsetY + row * CELL_SIZE + CELL_SIZE / 2;
            for (int i = 0;i < 16;i++) 
            {
                float angle = ((float)(rand() % 360)) * 3.14159f / 180.f;
                float speed = 0.5f + (rand() % 100) / 40.f;
                Particle p;
                p.x = (float)(cx + rand() % 14 - 7); p.y = (float)cy;
                p.vx = cosf(angle) * speed * 0.6f; p.vy = sinf(angle) * speed - 1.5f;
                p.life = p.maxLife = 0.4f + (rand() % 25) / 100.f;
                p.r = r; p.g = g; p.b = b;
                particles.push_back(p);
            }
        }
        void updateParticles(float dt) 
        {
            float step = dt * 60.f;
            for (auto& p : particles) 
            { 
                p.x += p.vx * step;
                p.y += p.vy * step;
                p.vy += 0.18f * step;
                p.life -= dt; 
            }
            size_t w = 0;
            for (size_t i = 0;i < particles.size();i++)
            {
                if (particles[i].life > 0.f)
                {
                    particles[w++] = particles[i];
                }
            }
            particles.resize(w);
        }
        void renderParticles() 
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            for (auto& p : particles) 
            {
                float frac = p.life / p.maxLife;
                Uint8 a = (Uint8)(255.f * frac * frac);
                int sz = (frac > 0.5f) ? 4 : 3;
                SDL_SetRenderDrawColor(renderer, p.r, p.g, p.b, a);
                SDL_Rect rect = { (int)p.x - sz / 2,(int)p.y - sz / 2,sz,sz };
                SDL_RenderFillRect(renderer, &rect);
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
        ////////////////////////////////////////////////////////////////////////

        // Dropping Animation
        ////////////////////////////////////////////////////////////////////////
        void updateDropping(float dt) 
        {
            float speed = dt / 0.22f;
            for (auto& d : dropping) 
            { 
                if (d.done)
                {
                    continue;
                }
                d.t += speed;
                if (d.t >= 1.f) 
                { 
                    d.t = 1.f;
                    d.done = true;
                }
            }
            size_t w = 0;
            for (size_t i = 0;i < dropping.size();i++)
            {
                if (!dropping[i].done)
                {
                    dropping[w++] = dropping[i];
                }
            }
            dropping.resize(w);
        }
        void renderDropping(int hole) 
        {
            for (auto& d : dropping) 
            {
                float et = d.t * d.t;
                float cy = std::lerp(d.startY, d.endY, et);
                int   cx = offsetX + d.col * CELL_SIZE + CELL_SIZE / 2;
                drawCoin(cx, (int)cy, d.player, hole);
            }
        }
        ////////////////////////////////////////////////////////////////////////

        // Background / Panel / Button
        ////////////////////////////////////////////////////////////////////////
        void renderBackground() 
        {
            SDL_SetRenderDrawColor(renderer, 6, 6, 18, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            for (auto& s : stars) 
            {
                float tw = 0.4f + 0.6f * sinf(gameTime * 2.1f + s.phase);
                Uint8 a = (Uint8)(160 * tw);
                SDL_SetRenderDrawColor(renderer, 200, 210, 255, a);
                SDL_RenderDrawPoint(renderer, s.x, s.y);
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }

        void drawPanel(int x, int y, int w, int h, Uint8 borderR = 80, Uint8 borderG = 140, Uint8 borderB = 255) 
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            float pulse = 0.5f + 0.5f * sinf(gameTime * 2.f);
            for (int i = 5;i >= 1;i--) 
            {
                Uint8 a = (Uint8)(55 * pulse * (5 - i) / 5);
                SDL_SetRenderDrawColor(renderer, borderR, borderG, borderB, a);
                SDL_Rect gr = { x - i * 2,y - i * 2,w + i * 4,h + i * 4 };
                SDL_RenderDrawRect(renderer, &gr);
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(renderer, 8, 10, 28, 255);
            SDL_Rect panel = { x,y,w,h }; SDL_RenderFillRect(renderer, &panel);
            SDL_SetRenderDrawColor(renderer, borderR, borderG, borderB, 255);
            for (int t = 0;t < 2;t++) 
            { 
                SDL_Rect border = { x - t,y - t,w + 2 * t,h + 2 * t };
                SDL_RenderDrawRect(renderer, &border); 
            }
            SDL_Rect bar = { x,y,w,4 }; SDL_RenderFillRect(renderer, &bar);
        }

        bool drawButton(const string& label, int cx, int y, int btnW, int btnH,
            int mouseX, int mouseY, Uint8 br = 80, Uint8 bg_ = 140, Uint8 bb = 255) 
        {
            int x = cx - btnW / 2;
            bool hover = (mouseX >= x && mouseX < x + btnW && mouseY >= y && mouseY < y + btnH);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, br, bg_, bb, hover ? 80 : 30);
            SDL_Rect r = { x,y,btnW,btnH }; SDL_RenderFillRect(renderer, &r);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(renderer, br, bg_, bb, 255);
            SDL_RenderDrawRect(renderer, &r);
            if (hover) 
            {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, br, bg_, bb, 40);
                SDL_Rect inner = { x + 1,y + 1,btnW - 2,btnH - 2 }; SDL_RenderFillRect(renderer, &inner);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            }
            int innerW = btnW - 16, scale = 3;
            while (scale > 1 && textWidth(label, scale) > innerW) scale--;
            int tw = textWidth(label, scale);
            int tx = cx - tw / 2, ty = y + (btnH - textHeight(scale)) / 2;
            drawText(label, tx, ty, br, bg_, bb, scale, hover ? 255 : 200);
            return hover;
        }

        bool buttonHit(int mx, int my, int cx, int y, int btnW, int btnH) 
        {
            return mx >= cx - btnW / 2 && mx < cx + btnW / 2 && my >= y && my < y + btnH;
        }

        void drawInputField(const string& label, const string& value,
            bool active, bool masked, int x, int y, int w, int h) 
        {
            Uint8 borderR = active ? 100 : 50, borderG = active ? 180 : 80, borderB = active ? 255 : 140;
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, borderR, borderG, borderB, active ? 25 : 10);
            SDL_Rect fill = { x,y,w,h }; SDL_RenderFillRect(renderer, &fill);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(renderer, borderR, borderG, borderB, 255);
            SDL_RenderDrawRect(renderer, &fill);
            drawText(label, x, y - 22, 160, 185, 225, 3, 180);
            string display = masked ? string(value.size(), '*') : value;
            if (active)
            {
                bool blink = ((int)(gameTime * 2.f) % 2 == 0);
                if (blink)
                {
                    display += "_";
                }
            }
            int innerW = w - 20, scale = 3;
            while (scale > 1 && textWidth(display, scale) > innerW)
            {
                scale--;
            }
            drawText(display, x + 10, y + (h - textHeight(scale)) / 2, 220, 230, 255, scale, 255);
        }
        ////////////////////////////////////////////////////////////////////////

        // Status Message Helper
        ////////////////////////////////////////////////////////////////////////
        void setStatus(const string& msg) 
        { 
            statusMsg = msg;
            statusTimer = 3.f;
        }
        void clearInputFields() 
        {
            inputUsername.clear();
            inputPassword.clear();
            inputGuestName.clear();
            statusMsg.clear();
            statusTimer = 0.f;
            activeField = InputField::USERNAME;
        }
        ////////////////////////////////////////////////////////////////////////

        // MAIN MENU (Before Any Login â€“ Only Login & Exit)
        ////////////////////////////////////////////////////////////////////////
        void renderMainMenu(int mx, int my) 
        {
            renderBackground();
            int cx = screenW / 2;

            float titleBob = 4.f * sinf(gameTime * 1.8f);
            int titleY = max(40, screenH / 8);
            int tscale = 6;
            while (tscale > 2 && textWidth("CONNECT FOUR", tscale) > screenW - 80) tscale--;
            drawTextCX("CONNECT FOUR", cx + 2, (int)(titleY + 2 + titleBob), 20, 50, 120, tscale, 120);
            drawTextCX("CONNECT FOUR", cx, (int)(titleY + titleBob), 80, 140, 255, tscale, 255);

            int coinR = 22;
            int titleW = textWidth("CONNECT FOUR", tscale);
            int leftCoin = cx - titleW / 2 - coinR - 16, rightCoin = cx + titleW / 2 + coinR + 16;
            if (leftCoin > coinR + 5 && rightCoin < screenW - coinR - 5) 
            {
                drawCoin(leftCoin, (int)(titleY + tscale * 4 + titleBob), 1, coinR);
                drawCoin(rightCoin, (int)(titleY + tscale * 4 + titleBob), 2, coinR);
            }

            int btnW = min(300, screenW - 80), btnH = 50, btnSp = 64;
            int panW = btnW + 80, panH = 60 + 2 * btnSp + 10;
            int panX = cx - panW / 2, panY = screenH / 2 - panH / 2 + 20;
            drawPanel(panX, panY, panW, panH);

            int y = panY + 38;
            drawButton("LOGIN / SIGNUP", cx, y, btnW, btnH, mx, my, 80, 140, 255); y += btnSp;
            drawButton("EXIT", cx, y, btnW, btnH, mx, my, 200, 80, 80);

            drawTextCX("LOGIN REQUIRED TO PLAY", cx, screenH - 30, 80, 100, 160, 2, 100);
        }

        bool handleMainMenuClick(int mx, int my) 
        {
            int btnW = min(300, screenW - 80), btnH = 50, btnSp = 64;
            int panH = 60 + 2 * btnSp + 10;
            int panY = screenH / 2 - panH / 2 + 20;
            int y = panY + 38;

            if (buttonHit(mx, my, screenW / 2, y, btnW, btnH)) 
            {
                loginFor = 1; clearInputFields();
                state = GameState::LOGIN_P1;                        // Start P1 Login
                return true;
            }
            y += btnSp;
            if (buttonHit(mx, my, screenW / 2, y, btnW, btnH))
            {
                return false;                                       // Exit
            }
            return true;
        }
        ////////////////////////////////////////////////////////////////////////

        // LOGIN Screen (Shared For P1 & P2)
        ////////////////////////////////////////////////////////////////////////
        void renderLoginScreen(int mx, int my)
        {
            renderBackground();
            int cx = screenW / 2;
            string title = (loginFor == 1) ? "PLAYER 1 LOGIN" : "PLAYER 2 LOGIN";

            int panW = min(560, screenW - 40), panH = (loginFor == 2) ? 450 : 380;
            int panX = cx - panW / 2, panY = (screenH - panH) / 2 + 28;

            int tscale = 5;
            while (tscale > 2 && textWidth(title, tscale) > panW - 30) tscale--;
            int titleY = max(22, panY - textHeight(tscale) - 20);
            drawTextCX(title, cx, titleY, 80, 150, 255, tscale, 255);

            drawPanel(panX, panY, panW, panH);

            int fieldW = panW - 60, fieldH = 44, fieldX = cx - fieldW / 2;
            int fieldY1 = panY + 76, fieldY2 = fieldY1 + 90;

            drawInputField("USERNAME", inputUsername, activeField == InputField::USERNAME, false, fieldX, fieldY1, fieldW, fieldH);
            drawInputField("PASSWORD", inputPassword, activeField == InputField::PASSWORD, true, fieldX, fieldY2, fieldW, fieldH);

            if (statusTimer > 0.f) 
            {
                Uint8 a = (Uint8)(255 * min(1.f, statusTimer));
                bool isErr = (statusMsg.find("!") != string::npos ||
                    statusMsg.find("WRONG") != string::npos ||
                    statusMsg.find("TAKEN") != string::npos ||
                    statusMsg.find("EMPTY") < statusMsg.size());
                drawTextFit(statusMsg, cx, fieldY2 + fieldH + 14, panW - 24,
                    isErr ? 230 : 80, isErr ? 80 : 200, isErr ? 80 : 80, 3, a);
            }

            int btnW = fieldW / 2 - 8, btnH = 44;
            int btnY = (loginFor == 2) ? panY + panH - 120 : panY + panH - 88;
            int leftX = cx - btnW - 6, rightX = cx + 6;

            drawButton("LOGIN", leftX + btnW / 2, btnY, btnW, btnH, mx, my, 80, 180, 80);                   // Login / Register Buttons
            drawButton("REGISTER", rightX + btnW / 2, btnY, btnW, btnH, mx, my, 80, 140, 220);

            if (loginFor == 2) 
            {
                int guestY = btnY + btnH + 12;                                                              // For P2 only: Offer Guest Play
                drawButton("PLAY AS GUEST", cx, guestY, btnW + 16, btnH, mx, my, 160, 100, 220);
            }

            drawTextCX("ESC = BACK", cx, panY + panH - 18, 150, 205, 255, 2, 240);
        }

        void handleLoginClick(int mx, int my) 
        {
            int cx = screenW / 2;
            int panW = min(560, screenW - 40), panH = (loginFor == 2) ? 450 : 380;
            int panY = (screenH - panH) / 2 + 28;
            int fieldW = panW - 60, fieldH = 44, fieldX = cx - fieldW / 2;
            int fieldY1 = panY + 76, fieldY2 = fieldY1 + 90;

            if (mx >= fieldX && mx < fieldX + fieldW && my >= fieldY1 && my < fieldY1 + fieldH) 
            {
                activeField = InputField::USERNAME;
                return;                                                                                     //////////////////////////////
            }                                                                                               //                          //
            if (mx >= fieldX && mx < fieldX + fieldW && my >= fieldY2 && my < fieldY2 + fieldH)             //       Field  Focus       //
            {                                                                                               //                          //
                activeField = InputField::PASSWORD;                                                         //////////////////////////////
                return; 
            }

            int btnW = fieldW / 2 - 8, btnH = 44;
            int btnY = (loginFor == 2) ? panY + panH - 120 : panY + panH - 88;
            int leftX = cx - btnW - 6, rightX = cx + 6;

            if (mx >= leftX && mx < leftX + btnW && my >= btnY && my < btnY + btnH) 
            { 
                doLoginAction(false);
                return;
            }
            if (mx >= rightX && mx < rightX + btnW && my >= btnY && my < btnY + btnH) 
            {
                doLoginAction(true);
                return; 
            }

            if (loginFor == 2) 
            {
                int guestY = btnY + btnH + 12;
                if (buttonHit(mx, my, cx, guestY, btnW + 16, btnH)) 
                {
                    clearInputFields();
                    state = GameState::P2_GUEST_NAME;                                               // Go To Guest Name Entry
                    return;
                }
            }
        }

        void doLoginAction(bool isRegister) 
        {
            if (inputUsername.empty() || inputPassword.empty()) 
            { 
                setStatus("FIELDS CANNOT BE EMPTY!");
                return; 
            }
            if (isRegister) 
            {
                if (inputUsername.find(',') != string::npos || inputPassword.find(',') != string::npos)
                {
                    setStatus("INVALID CHARACTERS!");
                    return;
                }
                if (!acctMgr.registerUser(inputUsername, inputPassword))
                {
                    setStatus("USERNAME ALREADY TAKEN!");
                    return;
                }
                setStatus("ACCOUNT CREATED!");
            }
            UserAccount* acc = acctMgr.login(inputUsername, inputPassword);
            if (!acc) 
            { 
                setStatus("WRONG USERNAME OR PASSWORD!");
                return; 
            }

            if (loginFor == 1) 
            {
                p1.isGuest = false; p1.name = acc->username; p1.account = acc;
                p1.colorIdx = acc->p1ColorIdx;
                setStatus("WELCOME " + acc->username + "!");
                clearInputFields();
                loginFor = 2;
                state = GameState::LOGIN_P1; // Reuse Screen, loginFor = 2 Changes Title
            }
            else 
            {
                p2.isGuest = false; p2.name = acc->username; p2.account = acc;
                p2.colorIdx = acc->p2ColorIdx;
                setStatus("P2 LOGGED IN!");
                clearInputFields();
                state = GameState::POST_LOGIN_MENU;
            }
        }
        ////////////////////////////////////////////////////////////////////////

        //  P2 Guest Name Entry
        ////////////////////////////////////////////////////////////////////////
        void renderGuestName(int mx, int my) 
        {
            renderBackground();
            int cx = screenW / 2;
            string title = "PLAYER 2 GUEST";

            int panW = min(480, screenW - 40), panH = 300;
            int panX = cx - panW / 2, panY = (screenH - panH) / 2 + 28;

            int tscale = 5;
            while (tscale > 2 && textWidth(title, tscale) > panW - 30)
            {
                tscale--;
            }
            int titleY = max(22, panY - textHeight(tscale) - 20);
            drawTextCX(title, cx, titleY, 80, 150, 255, tscale, 255);

            drawPanel(panX, panY, panW, panH);

            int fieldW = panW - 60, fieldH = 44, fieldX = cx - fieldW / 2;
            int fieldY = panY + panH / 2 - 18;
            drawTextCX("ENTER YOUR NAME", cx, panY + 44, 180, 215, 255, 3, 230);
            drawInputField("NAME", inputGuestName, true, false, fieldX, fieldY, fieldW, fieldH);

            if (statusTimer > 0.f) 
            {
                Uint8 a = (Uint8)(255 * min(1.f, statusTimer));
                drawTextFit(statusMsg, cx, fieldY + fieldH + 14, panW - 24, 230, 80, 80, 3, a);
            }

            int btnW = 180, btnH = 44, btnY = panY + panH - 76;
            drawButton("CONTINUE", cx, btnY, btnW, btnH, mx, my, 80, 180, 80);
            drawTextCX("ESC = BACK", cx, panY + panH - 18, 150, 205, 255, 2, 240);
        }

        void handleGuestNameClick(int mx, int my) 
        {
            int cx = screenW / 2;
            int panH = 300;
            int panY = (screenH - panH) / 2 + 28;
            int btnW = 180, btnH = 44, btnY = panY + panH - 76;
            if (buttonHit(mx, my, cx, btnY, btnW, btnH)) 
            {
                if (inputGuestName.empty()) 
                {
                    setStatus("ENTER A NAME!");
                    return;
                }
                p2.isGuest = true;
                p2.name = inputGuestName;
                p2.account = nullptr;
                p2.colorIdx = 1;
                clearInputFields();
                state = GameState::POST_LOGIN_MENU;
            }
        }
        ////////////////////////////////////////////////////////////////////////

        // POST LOGIN MENU (Start / History / High Scores / Settings / Logout)
        ////////////////////////////////////////////////////////////////////////
        void renderPostLoginMenu(int mx, int my) 
        {
            renderBackground();
            int cx = screenW / 2;

            float titleBob = 3.f * sinf(gameTime * 1.8f);
            int tscale = 6;
            while (tscale > 2 && textWidth("CONNECT FOUR", tscale) > screenW - 80)
            {
                tscale--;
            }
            drawTextCX("CONNECT FOUR", cx + 2, (int)(42 + titleBob), 20, 50, 120, tscale, 120);
            drawTextCX("CONNECT FOUR", cx, (int)(40 + titleBob), 80, 140, 255, tscale, 255);

            Uint8 p1r, p1g, p1b, p2r, p2g, p2b;
            getPlayerColor(1, p1r, p1g, p1b);
            getPlayerColor(2, p2r, p2g, p2b);
            string p1tag = "P1: " + p1.name + (p1.isGuest ? " (GUEST)" : "");
            string p2tag = "P2: " + p2.name + (p2.isGuest ? " (GUEST)" : "");                                   ////////////////////////////
            int tagY = tscale * 7 + 50;                                                                         //                        //
            drawTextFit(p1tag, cx - screenW / 4, tagY, screenW / 2 - 20, p1r, p1g, p1b, 2, 200);                //      Player  Tags      //
            drawTextFit(p2tag, cx + screenW / 4, tagY, screenW / 2 - 20, p2r, p2g, p2b, 2, 200);                //                        //
            drawCoin(cx - screenW / 4 - textWidth(p1tag, 2) / 2 - 16, tagY + 7, 1, 10);                         ////////////////////////////
            drawCoin(cx + screenW / 4 + textWidth(p2tag, 2) / 2 + 16, tagY + 7, 2, 10);

            int btnW = min(300, screenW - 80), btnH = 50, btnSp = 62;
            int numBtns = 5;
            int panW = btnW + 80, panH = 50 + numBtns * btnSp + 10;
            int panX = cx - panW / 2, panY = tagY + 30;
            drawPanel(panX, panY, panW, panH);

            int y = panY + 34;
            drawButton("START GAME", cx, y, btnW, btnH, mx, my, 80, 180, 80);       y += btnSp;
            drawButton("GAME HISTORY", cx, y, btnW, btnH, mx, my, 160, 140, 80);    y += btnSp;
            drawButton("HIGH SCORES", cx, y, btnW, btnH, mx, my, 140, 80, 200);     y += btnSp;
            drawButton("SETTINGS", cx, y, btnW, btnH, mx, my, 80, 140, 255);        y += btnSp;
            drawButton("LOGOUT", cx, y, btnW, btnH, mx, my, 200, 80, 80);
        }

        bool handlePostLoginMenuClick(int mx, int my) 
        {
            int cx = screenW / 2;
            int btnW = min(300, screenW - 80), btnH = 50, btnSp = 62;
            Uint8 p1r, p1g, p1b; getPlayerColor(1, p1r, p1g, p1b);
            string p1tag = "P1: " + p1.name + (p1.isGuest ? " (GUEST)" : "");
            int tscale = 6;
            while (tscale > 2 && textWidth("CONNECT FOUR", tscale) > screenW - 80)
            {
                tscale--;
            }
            int tagY = tscale * 7 + 50;
            int panY = tagY + 30;
            int y = panY + 34;

            if (buttonHit(mx, my, cx, y, btnW, btnH)) 
            { 
                startGame();
                return true;
            } 
            y += btnSp;
            if (buttonHit(mx, my, cx, y, btnW, btnH)) 
            { 
                historyScroll = 0;
                state = GameState::HISTORY;
                return true; 
            } 
            y += btnSp;
            if (buttonHit(mx, my, cx, y, btnW, btnH)) 
            { state = GameState::HIGH_SCORES;
            return true;
            } 
            y += btnSp;
            if (buttonHit(mx, my, cx, y, btnW, btnH)) 
            { 
                settingsPlayer = 1;
                state = GameState::SETTINGS;
                return true;
            } 
            y += btnSp;
            if (buttonHit(mx, my, cx, y, btnW, btnH)) 
            { 
                doLogout();
                return true; 
            }
            return true;
        }

        void doLogout() 
        {
            p1 = SessionPlayer(); p2 = SessionPlayer();
            acctMgr.load();                                  // Refresh
            clearInputFields();
            state = GameState::MAIN_MENU;
        }
        ////////////////////////////////////////////////////////////////////////

        // GAME HISTORY Screen
        ////////////////////////////////////////////////////////////////////////
        void renderHistory(int mx, int my)
        {
            renderBackground();
            int cx = screenW / 2;
            int tscale = 5;
            while (tscale > 2 && textWidth("GAME HISTORY", tscale) > screenW - 80)
            {
                tscale--;
            }
            drawTextCX("GAME HISTORY", cx, 30, 80, 140, 255, tscale, 255);

            int panW = min(640, screenW - 40);
            int panH = screenH - 160;
            int panX = cx - panW / 2, panY = 90;
            drawPanel(panX, panY, panW, panH);

            string p1Lower = toLower(p1.name);
            string p2Lower = toLower(p2.name);

            vector<MatchRecord*> relevant;
            for (int i = (int)histMgr.records.size() - 1; i >= 0; i--)
            {
                MatchRecord& r = histMgr.records[i];
                if (r.p1Name == p1Lower || r.p2Name == p1Lower ||
                    (!p2.isGuest && (r.p1Name == p2Lower || r.p2Name == p2Lower)))
                {
                    relevant.push_back(&r);
                }
            }

            int rowH = 46, visRows = (panH - 60) / rowH;
            int maxScroll = max(0, (int)relevant.size() - visRows);
            historyScroll = max(0, min(historyScroll, maxScroll));

            if (relevant.empty())
            {
                drawTextCX("NO HISTORY YET", cx, panY + panH / 2 - 20, 100, 120, 160, 3, 160);
            }
            else
            {
                for (int i = 0; i < visRows && i + historyScroll < (int)relevant.size(); i++)
                {
                    MatchRecord* rec = relevant[i + historyScroll];
                    int ry = panY + 50 + i * rowH;

                    // Row Background
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(renderer, 80, 140, 255, (i % 2 == 0) ? 8 : 15);
                    SDL_Rect bg = { panX + 12, ry, panW - 24, rowH - 4 };
                    SDL_RenderFillRect(renderer, &bg);
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

                    string line = rec->p1Name + " VS " + rec->p2Name;
                    string resultStr;
                    Uint8 rr = 160, rg = 185, rb = 225;
                    if (rec->result == "P1")
                    {
                        resultStr = rec->p1Name + " WON";
                        rr = 80; rg = 220; rb = 80;
                    }
                    else if (rec->result == "P2")
                    {
                        resultStr = rec->p2Name + " WON";
                        rr = 80; rg = 160; rb = 255;
                    }
                    else
                    {
                        resultStr = "DRAW";
                        rr = 200; rg = 200; rb = 80;
                    }

                    int totalW = panW - 24 - 16;
                    int col1W = totalW * 45 / 100;
                    int col2W = totalW * 35 / 100;
                    int col3W = totalW * 20 / 100;

                    drawTextFit(line, panX + 20 + col1W / 2, ry + 8, col1W, 160, 185, 225, 2, 200);
                    drawTextFit(resultStr, panX + 20 + col1W + col2W / 2, ry + 8, col2W, rr, rg, rb, 2, 240);
                    drawTextFit(rec->timeTaken, panX + 20 + col1W + col2W + col3W / 2, ry + 8, col3W, 80, 100, 130, 2, 180);
                }
            }

            if ((int)relevant.size() > visRows)
            {
                drawTextCX("SCROLL: UP/DOWN KEYS", cx, panY + panH - 22, 80, 100, 140, 2, 120);
            }
            drawButton("< BACK", cx, panY + panH + 16, 200, 44, mx, my, 80, 140, 255);
        }
        ////////////////////////////////////////////////////////////////////////

        // HIGH SCORES Screen
        ////////////////////////////////////////////////////////////////////////
        void renderHighScores(int mx, int my)
        {
            renderBackground();
            int cx = screenW / 2;
            int tscale = 5;
            while (tscale > 2 && textWidth("HIGH SCORES", tscale) > screenW - 80)
            {
                tscale--;
            }
            drawTextCX("HIGH SCORES", cx, 30, 80, 140, 255, tscale, 255);

            // Sort Accounts By Wins Descending
            vector<UserAccount> sorted(acctMgr.accounts.begin(), acctMgr.accounts.end());
            sort(sorted.begin(), sorted.end(), [](const UserAccount& a, const UserAccount& b)
                {
                    return a.stats.wins > b.stats.wins;
                });

            int panW = min(580, screenW - 40), panH = screenH - 160;
            int panX = cx - panW / 2, panY = 90;
            drawPanel(panX, panY, panW, panH);

            // Header
            drawTextFit("RANK", panX + 30, panY + 20, 60, 160, 185, 225, 2, 180);
            drawTextFit("PLAYER", panX + 120, panY + 20, 160, 160, 185, 225, 2, 180);
            drawTextFit("WINS", panX + panW - 170, panY + 20, 80, 160, 185, 225, 2, 180);
            drawTextFit("GAMES", panX + panW - 60, panY + 20, 80, 160, 185, 225, 2, 180);

            int rowH = 44, visRows = (panH - 70) / rowH;
            for (int i = 0; i < visRows && i < (int)sorted.size(); i++)
            {
                auto& a = sorted[i];
                int ry = panY + 50 + i * rowH;

                // Highlight If It's A Logged-In Player
                bool isP1 = (a.username == p1.name);
                bool isP2 = (!p2.isGuest && a.username == p2.name);
                Uint8 hR = 80, hG = 80, hB = 80;
                if (isP1)       getPlayerColor(1, hR, hG, hB);
                else if (isP2)  getPlayerColor(2, hR, hG, hB);

                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, hR, hG, hB, (isP1 || isP2) ? 30 : 8);
                SDL_Rect bg = { panX + 12, ry, panW - 24, rowH - 4 };
                SDL_RenderFillRect(renderer, &bg);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

                Uint8 tr = (isP1 || isP2) ? 255 : 180;
                Uint8 tg = (isP1 || isP2) ? 255 : 200;
                Uint8 tb = (isP1 || isP2) ? 255 : 220;

                string rank = to_string(i + 1);
                if (i == 0) rank = "1ST";
                else if (i == 1) rank = "2ND";
                else if (i == 2) rank = "3RD";

                drawTextFit(rank, panX + 30, ry + 10, 60, hR + (isP1 || isP2 ? 0 : 60), hG + (isP1 || isP2 ? 0 : 60), hB + (isP1 || isP2 ? 0 : 60), 2, tr);
                drawTextFit(a.username, panX + 120, ry + 10, 180, tr, tg, tb, 2, tr);
                drawTextFit(to_string(a.stats.wins), panX + panW - 170, ry + 10, 80, 80, 220, 80, 3, tr);
                drawTextFit(to_string(a.stats.total), panX + panW - 60, ry + 10, 80, 160, 185, 225, 3, tr);
            }

            drawButton("< BACK", cx, panY + panH + 16, 200, 44, mx, my, 80, 140, 255);
        }
        ////////////////////////////////////////////////////////////////////////

        //  SETTINGS Screen (Colour Picker)
        ////////////////////////////////////////////////////////////////////////
        void renderSettings(int mx, int my) 
        {
            renderBackground();
            int cx = screenW / 2;
            int tscale = 6;
            while (tscale > 2 && textWidth("SETTINGS", tscale) > screenW - 80)
            {
                tscale--;
            }
            drawTextCX("SETTINGS", cx, 50, 80, 140, 255, tscale, 255);

            int panW = min(620, screenW - 40), panH = 450;
            int panX = cx - panW / 2, panY = screenH / 2 - panH / 2 - 10;
            drawPanel(panX, panY, panW, panH);

            int tabW = min(200, (panW - 60) / 2), tabH = 38, gap = 16;
            int totalTabW = tabW * 2 + gap, tabStartX = cx - totalTabW / 2;

            for (int p_ = 1;p_ <= 2;p_++) 
            {
                int tx = tabStartX + (p_ - 1) * (tabW + gap), ty = panY + 16;
                bool sel = (settingsPlayer == p_);
                Uint8 cr, cg, cb; getPlayerColor(p_, cr, cg, cb);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, cr, cg, cb, sel ? 60 : 20);
                SDL_Rect tr = { tx,ty,tabW,tabH }; SDL_RenderFillRect(renderer, &tr);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
                SDL_SetRenderDrawColor(renderer, cr, cg, cb, sel ? 255 : 100);
                SDL_RenderDrawRect(renderer, &tr);
                string label = "PLAYER "; label += (char)('0' + p_);
                drawCoin(tx + 24, ty + tabH / 2, p_, 11);
                drawTextFit(label, tx + tabW / 2 + 14, ty + (tabH - textHeight(3)) / 2, tabW - 58, cr, cg, cb, 3, sel ? 255 : 160);
            }

            int cols = 3, cellSz = min(84, (panW - 60) / cols - 12);
            int gridW = cols * cellSz + (cols - 1) * 12, gridX = cx - gridW / 2, gridY = panY + 76;

            for (int i = 0;i < PALETTE_SIZE;i++) 
            {
                int col = i % cols, row = i / cols;
                int bx = gridX + col * (cellSz + 12), by = gridY + row * (cellSz + 12);
                bool hover = (mx >= bx && mx < bx + cellSz && my >= by && my < by + cellSz);
                int curIdx = (settingsPlayer == 1) ? p1.colorIdx : p2.colorIdx;
                bool selected = (curIdx == i);

                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, PALETTE[i].r, PALETTE[i].g, PALETTE[i].b, selected ? 50 : (hover ? 30 : 15));
                SDL_Rect cell = { bx,by,cellSz,cellSz }; SDL_RenderFillRect(renderer, &cell);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
                SDL_SetRenderDrawColor(renderer, PALETTE[i].r, PALETTE[i].g, PALETTE[i].b, selected ? 255 : (hover ? 180 : 80));
                SDL_RenderDrawRect(renderer, &cell);
                if (selected) 
                { 
                    SDL_Rect inner = { bx + 1,by + 1,cellSz - 2,cellSz - 2 };
                    SDL_RenderDrawRect(renderer, &inner); 
                }

                int coinR = cellSz / 4;
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
                SDL_SetRenderDrawColor(renderer, PALETTE[i].r, PALETTE[i].g, PALETTE[i].b, 255);
                drawFilledCircle(bx + cellSz / 2, by + cellSz / 2 - 8, coinR);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 80);
                drawRing(bx + cellSz / 2, by + cellSz / 2 - 8, coinR);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

                drawTextFit(PALETTE[i].name, bx + cellSz / 2, by + cellSz - 22, cellSz - 8,
                    PALETTE[i].r, PALETTE[i].g, PALETTE[i].b, 2, selected ? 255 : 160);
            }

            drawButton("SAVE + BACK", cx, panY + panH - 52, 260, 44, mx, my, 80, 180, 80);
        }

        void handleSettingsClick(int mx, int my) 
        {
            int cx = screenW / 2;
            int panW = min(620, screenW - 40), panH = 450;
            int panY = screenH / 2 - panH / 2 - 10;
            int tabW = min(200, (panW - 60) / 2), tabH = 38, gap = 16;
            int totalTabW = tabW * 2 + gap, tabStartX = cx - totalTabW / 2;

            for (int p_ = 1;p_ <= 2;p_++) 
            {
                int tx = tabStartX + (p_ - 1) * (tabW + gap), ty = panY + 16;
                if (mx >= tx && mx < tx + tabW && my >= ty && my < ty + tabH) 
                { 
                    settingsPlayer = p_;
                    return;
                }
            }

            int cols = 3, cellSz = min(84, (panW - 60) / cols - 12);
            int gridW = cols * cellSz + (cols - 1) * 12, gridX = cx - gridW / 2, gridY = panY + 76;
            for (int i = 0;i < PALETTE_SIZE;i++) 
            {
                int col = i % cols, row = i / cols;
                int bx = gridX + col * (cellSz + 12), by = gridY + row * (cellSz + 12);
                if (mx >= bx && mx < bx + cellSz && my >= by && my < by + cellSz) {
                    if (settingsPlayer == 1) 
                    {
                        p1.colorIdx = i;
                        if (p1.account) 
                        { 
                            p1.account->p1ColorIdx = i;
                            acctMgr.saveAccount(*p1.account);
                        }
                    }
                    else 
                    {
                        p2.colorIdx = i;
                        if (p2.account) 
                        { 
                            p2.account->p2ColorIdx = i;
                            acctMgr.saveAccount(*p2.account);
                        }
                    }
                    return;
                }
            }

            if (buttonHit(mx, my, cx, panY + panH - 52, 260, 44))
            {
                state = GameState::POST_LOGIN_MENU;
            }
        }
        ///////////////////////////////////////////////////////////////////////

        // GAMEPLAY
        ////////////////////////////////////////////////////////////////////////
        void startGame() 
        {
            board.reset();
            particles.clear();
            dropping.clear();
            currentPlayer = 1;
            hoverCol = -1;
            winner = 0;
            bobPhase = 0.f;
            hoverAlpha = 0.f;
            announceT = 0.f;
            offsetX = screenW / 2 - (COLS * CELL_SIZE) / 2;
            offsetY = screenH / 2 - (ROWS * CELL_SIZE) / 2;
            matchStartTicks = SDL_GetTicks();
            state = GameState::PLAYING;
        }

        void recordResult(int win)
        {
            MatchRecord rec;
            rec.p1Name = toLower(p1.name);
            rec.p2Name = toLower(p2.name);

            if (win == 1)
            {
                rec.result = "P1";
            }
            else if (win == 2)
            {
                rec.result = "P2";
            }
            else
            {
                rec.result = "DRAW";
            }

            Uint32 elapsed = (SDL_GetTicks() - matchStartTicks) / 1000;
            Uint32 minutes = elapsed / 60;                                      ////////////////////////////////////
            Uint32 seconds = elapsed % 60;                                      //    Calculate Match Duration    //
            char buf[16];                                                       ////////////////////////////////////
            snprintf(buf, sizeof(buf), "%02u:%02u", minutes, seconds);
            rec.timeTaken = string(buf);

            histMgr.addRecord(rec);

            if (p1.account)                                                     // Update Persistent Stats For Logged-In Players
            {
                p1.account->stats.total++;
                if (win == 1)
                {
                    p1.account->stats.wins++;
                }
                else if (win == 2)
                {
                    p1.account->stats.losses++;
                }
                else
                {
                    p1.account->stats.draws++;
                }
                acctMgr.saveAccount(*p1.account);
            }
            if (p2.account)
            {
                p2.account->stats.total++;
                if (win == 2)
                {
                    p2.account->stats.wins++;
                }
                else if (win == 1)
                {
                    p2.account->stats.losses++;
                }
                else
                {
                    p2.account->stats.draws++;
                }
                acctMgr.saveAccount(*p2.account);
            }
        }

        void renderBoardGlow(int bW, int bH) 
        {
            float pulse = 0.5f + 0.5f * sinf(gameTime * 1.9f);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            for (int t = 5;t >= 0;t--) 
            {
                Uint8 a = (Uint8)(65 * pulse * (5 - t) / 5);
                SDL_SetRenderDrawColor(renderer, 80, 140, 255, a);
                SDL_Rect r = { offsetX - t * 2,offsetY - t * 2,bW + 4 * t,bH + 4 * t };
                SDL_RenderDrawRect(renderer, &r);
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }

        void renderHoverShimmer(int bH, float dt) 
        {
            float targetA = (hoverCol != -1) ? 1.f : 0.f;
            hoverAlpha = std::lerp(hoverAlpha, targetA, dt * 8.f);
            if (hoverAlpha < 0.01f || hoverCol == -1)
            {
                return;
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, (Uint8)(hoverAlpha * 28));
            SDL_Rect cr = { offsetX + hoverCol * CELL_SIZE,offsetY,CELL_SIZE,bH };
            SDL_RenderFillRect(renderer, &cr);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }

        void renderGameBoard(float dt) 
        {
            int boardW = COLS * CELL_SIZE, boardH = ROWS * CELL_SIZE;
            int hole = CELL_SIZE / 2 - 10;

            renderBackground();
            renderBoardGlow(boardW, boardH);
            renderHoverShimmer(boardH, dt);

            if (state == GameState::PLAYING) 
            {                                                                                     // Turn indicator with player name
                string pName = (currentPlayer == 1) ? p1.name : p2.name;
                string turnStr = pName + " TURN";
                Uint8 tr, tg, tb; getPlayerColor(currentPlayer, tr, tg, tb);
                int ts = 3; 
                while (ts > 1 && textWidth(turnStr, ts) > screenW / 2 - 20)
                {
                    ts--;
                }
                drawText(turnStr, 20, 20, tr, tg, tb, ts, 200);
                drawCoin(20 + textWidth(turnStr, ts) + 16, 20 + ts * 4, currentPlayer, 12);
            }

            if (hoverCol != -1 && state == GameState::PLAYING) 
            {
                int cx2 = offsetX + hoverCol * CELL_SIZE + CELL_SIZE / 2;
                int cy2 = offsetY - CELL_SIZE / 2;
                float bob = 4.f * sinf(bobPhase * 6.28f * 1.2f);
                drawCoin(cx2, (int)(cy2 + bob), currentPlayer, hole);
            }

            for (int r = 0;r < ROWS;r++)
            {
                for (int c = 0;c < COLS;c++)
                {
                    int cell = board.getCell(r, c);
                    if (!cell)
                    {
                        continue;
                    }
                    bool falling = false;
                    for (auto& d : dropping)
                    {
                        if (d.col == c && d.row == r)
                        {
                            falling = true;
                            break;
                        }
                        if (falling)
                        {
                            continue;
                        }
                    }
                    drawCoin(offsetX + c * CELL_SIZE + CELL_SIZE / 2, offsetY + r * CELL_SIZE + CELL_SIZE / 2, cell, hole);
                }
            }

            renderDropping(hole);

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(renderer, 20, 68, 188, 255);
            SDL_Rect boardRect = { offsetX,offsetY,boardW,boardH };
            SDL_RenderFillRect(renderer, &boardRect);

            SDL_SetRenderDrawColor(renderer, 55, 108, 228, 255);
            SDL_RenderDrawLine(renderer, offsetX, offsetY, offsetX + boardW, offsetY);
            SDL_RenderDrawLine(renderer, offsetX, offsetY + 1, offsetX + boardW, offsetY + 1);
            SDL_SetRenderDrawColor(renderer, 8, 36, 122, 255);
            SDL_RenderDrawLine(renderer, offsetX, offsetY + boardH, offsetX + boardW, offsetY + boardH);
            SDL_RenderDrawLine(renderer, offsetX, offsetY + boardH - 1, offsetX + boardW, offsetY + boardH - 1);

            for (int r = 0;r < ROWS;r++) for (int c = 0;c < COLS;c++) 
            {
                int cx2 = offsetX + c * CELL_SIZE + CELL_SIZE / 2;
                int cy2 = offsetY + r * CELL_SIZE + CELL_SIZE / 2;
                int cell = board.getCell(r, c);
                bool falling = false;
                for (auto& d : dropping)
                {
                    if (d.col == c && d.row == r)
                    {
                        falling = true;
                        break;
                    }
                }

                if (!cell || falling) 
                {
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
                    SDL_SetRenderDrawColor(renderer, 6, 6, 18, 255);
                    drawFilledCircle(cx2, cy2, hole);
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(renderer, 80, 130, 255, 30);
                    drawRing(cx2, cy2, hole - 2);
                }
                else 
                {
                    drawCoin(cx2, cy2, cell, hole);
                }
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
                SDL_SetRenderDrawColor(renderer, 10, 42, 140, 255);
                drawRing(cx2, cy2, hole + 1); drawRing(cx2, cy2, hole + 2);
                SDL_SetRenderDrawColor(renderer, 7, 30, 110, 255);
                drawRing(cx2, cy2, hole + 3);
            }

            renderParticles();
        }

        void renderAnnouncement(float dt) 
        {
            announceT = std::lerp(announceT, 1.f, 1.f - powf(0.0001f, dt));
            float et = easeSmoothstep(announceT);
            float pulse = 0.5f + 0.5f * sinf(gameTime * 4.4f);

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, (Uint8)(165 * et));
            SDL_Rect full = { 0,0,screenW,screenH }; SDL_RenderFillRect(renderer, &full);

            int panelW = min(540, screenW - 40), panelH = 290;
            int panelX = (screenW - panelW) / 2;
            int finalY = (screenH - panelH) / 2;
            int panelY = (int)std::lerp((float)(-panelH - 20), (float)finalY, et);

            Uint8 glowR, glowG, glowB;
            if (winner >= 1)
            {
                getPlayerColor(winner, glowR, glowG, glowB);
            }
            else 
            { 
                glowR = 100;
                glowG = 160;
                glowB = 255; 
            }

            for (int i = 6;i >= 1;i--) 
            {
                Uint8 a = (Uint8)(80 * pulse * (6 - i) / 6 * et);
                SDL_SetRenderDrawColor(renderer, glowR, glowG, glowB, a);
                SDL_Rect gr = { panelX - i * 2,panelY - i * 2,panelW + i * 4,panelH + i * 4 };
                SDL_RenderFillRect(renderer, &gr);
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_SetRenderDrawColor(renderer, 8, 10, 24, 255);
            SDL_Rect panel = { panelX,panelY,panelW,panelH }; SDL_RenderFillRect(renderer, &panel);
            SDL_SetRenderDrawColor(renderer, glowR, glowG, glowB, 255);
            for (int t = 0;t < 3;t++) 
            { 
                SDL_Rect border = { panelX - t,panelY - t,panelW + 2 * t,panelH + 2 * t };
                SDL_RenderDrawRect(renderer, &border); 
            }
            SDL_Rect bar = { panelX,panelY,panelW,5 };
            SDL_RenderFillRect(renderer, &bar);

            int pcx = panelX + panelW / 2, innerW = panelW - 40;

            if (winner >= 1) 
            {
                string wName = (winner == 1) ? p1.name : p2.name;
                int coinX = panelX + 52, coinY = panelY + 90;
                drawCoin(coinX, coinY, winner, 26);
                int textCX = coinX + 30 + (panelX + panelW - coinX - 30) / 2;
                drawTextFit(wName, textCX, panelY + 48, innerW - 70, glowR, glowG, glowB, 4, 255);
                Uint8 wa = (Uint8)(200 + 55 * pulse);
                drawTextFit("WINS!", textCX, panelY + 104, innerW - 70, 255, 255, 255, 4, wa);
            }
            else 
            {
                int ds = 6; 
                while (ds > 2 && textWidth("DRAW!", ds) > innerW)
                {
                    ds--;
                }
                Uint8 da = (Uint8)(190 + 65 * pulse);
                drawTextFit("DRAW!", pcx, panelY + 90, innerW, glowR, glowG, glowB, ds, da);
            }

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, glowR, glowG, glowB, 70);
            SDL_RenderDrawLine(renderer, panelX + 20, panelY + panelH - 60, panelX + panelW - 20, panelY + panelH - 60);
            Uint8 ha = (Uint8)((110 + 80 * sinf(gameTime * 3.1f)) * et);
            drawTextFit("R=RESTART  M=MENU", pcx, panelY + panelH - 46, panelW - 40, 160, 185, 225, 3, ha);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
        ////////////////////////////////////////////////////////////////////////

        // Text Input Handler
        ////////////////////////////////////////////////////////////////////////
        void handleTextInput(SDL_Keysym key) 
        {
            string* target = nullptr;
            if (state == GameState::P2_GUEST_NAME) 
            {
                target = &inputGuestName;
            }
            else 
            {
                target = (activeField == InputField::USERNAME) ? &inputUsername : &inputPassword;
            }

            if (key.sym == SDLK_BACKSPACE && !target->empty()) 
            { 
                target->pop_back();
                return; 
            }
            if (key.sym == SDLK_TAB && state != GameState::P2_GUEST_NAME) 
            {
                activeField = (activeField == InputField::USERNAME) ? InputField::PASSWORD : InputField::USERNAME;
                return;
            }
            if (key.sym == SDLK_RETURN) 
            {
                if (state == GameState::LOGIN_P1 || state == GameState::LOGIN_P2)
                {
                    doLoginAction(false);
                }
                if (state == GameState::P2_GUEST_NAME) 
                {
                    if (!inputGuestName.empty()) 
                    {
                        p2.isGuest = true;
                        p2.name = inputGuestName;
                        p2.account = nullptr; 
                        p2.colorIdx = 1;
                        clearInputFields();
                        state = GameState::POST_LOGIN_MENU;
                    }
                }
                return;
            }

            char c = 0;
            if (key.sym >= SDLK_a && key.sym <= SDLK_z) 
            {
                bool shift = (key.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) != 0;
                c = shift ? (char)toupper(key.sym) : (char)key.sym;
            }
            else if (key.sym >= SDLK_0 && key.sym <= SDLK_9)
            {
                c = (char)key.sym;
            }
            else if (key.sym == SDLK_UNDERSCORE || key.sym == SDLK_MINUS)
            {
                c = '_';
            }
            else if (key.sym == SDLK_PERIOD)
            {
                c = '.';
            }

            if (c && target->size() < 32)
            {
                *target += c;
            }
        }

    public:
        Game(const Game&) = delete;
        Game& operator=(const Game&) = delete;

        Game() 
        {
            if (SDL_Init(SDL_INIT_VIDEO) != 0)
            {
                throw runtime_error(string("SDL_Init failed: ") + SDL_GetError());
            }
            sdlReady = true;

            window = SDL_CreateWindow("Connect Four",
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
            if (!window) 
            {
                string err = SDL_GetError();
                cleanupSdl();
                throw runtime_error(string("SDL_CreateWindow failed: ") + err);
            }

            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (!renderer) 
            {
                string err = SDL_GetError();
                cleanupSdl();
                throw runtime_error(string("SDL_CreateRenderer failed: ") + err);
            }

            SDL_GetWindowSize(window, &screenW, &screenH);
            prevTicks = SDL_GetTicks();
            srand(42);
            initGlyphs();
            stars.reserve(120);
            for (int i = 0;i < 120;i++)
            {
                stars.push_back({ rand() % screenW,rand() % screenH,(float)i * 0.7f });
            }
            srand((unsigned)time(nullptr));

            // Default Session Players (Guest Placeholders So Colour Functions Don't Crash)
            p1.colorIdx = 0; p2.colorIdx = 1;

            acctMgr.load();
            histMgr.load();
        }

        ~Game() 
        {
            cleanupSdl();
        }

        void run() 
        {
            bool running = true;
            SDL_Event e;

            while (running) 
            {
                Uint32 now = SDL_GetTicks();
                float dt = (now - prevTicks) / 1000.f;
                if (dt > 0.05f) dt = 0.05f;
                prevTicks = now;
                gameTime += dt;
                bobPhase += dt;
                if (statusTimer > 0.f) statusTimer -= dt;

                int mx, my;
                SDL_GetMouseState(&mx, &my);

                while (SDL_PollEvent(&e)) 
                {
                    if (e.type == SDL_QUIT) 
                    { 
                        running = false;
                        break; 
                    }

                    if (e.type == SDL_KEYDOWN) 
                    {
                        auto sym = e.key.keysym.sym;

                        if (sym == SDLK_ESCAPE) 
                        {
                            switch (state)
                            {
                                case GameState::PLAYING:
                                case GameState::GAME_OVER:   state = GameState::POST_LOGIN_MENU;break;
                                case GameState::LOGIN_P1:
                                {
                                    if (loginFor == 2)
                                    {
                                        loginFor = 1; // Step Back
                                    }
                                    else
                                    {
                                        state = GameState::MAIN_MENU;
                                    }
                                    clearInputFields();
                                    break;
                                }

                                case GameState::P2_GUEST_NAME:
                                {
                                    state = GameState::LOGIN_P1;
                                    clearInputFields();
                                    break;
                                }
                                case GameState::HISTORY:
                                case GameState::HIGH_SCORES:
                                case GameState::SETTINGS:
                                {
                                    state = GameState::POST_LOGIN_MENU;
                                    break;
                                }
                                case GameState::POST_LOGIN_MENU:
                                {
                                    // No Operation â€“ Must Logout Explicitly
                                    break;
                                }
                                default:
                                {
                                    running = false;break;
                                }
                            }
                        }

                        // Scroll History
                        if (state == GameState::HISTORY) 
                        {
                            if (sym == SDLK_UP)
                            {
                                historyScroll = max(0, historyScroll - 1);
                            }
                            if (sym == SDLK_DOWN)
                            {
                                historyScroll++;
                            }
                        }

                        if (state == GameState::LOGIN_P1 || state == GameState::P2_GUEST_NAME)
                        {
                            handleTextInput(e.key.keysym);
                        }

                        if (state == GameState::GAME_OVER) 
                        {
                            if (sym == SDLK_r)
                            {
                                startGame();
                            }
                            if (sym == SDLK_m)
                            {
                                state = GameState::POST_LOGIN_MENU;
                            }
                        }
                    }

                    if (e.type == SDL_MOUSEMOTION && state == GameState::PLAYING) 
                    {
                        int mx2 = e.motion.x;
                        hoverCol = (mx2 >= offsetX && mx2 < offsetX + COLS * CELL_SIZE) ? (mx2 - offsetX) / CELL_SIZE : -1;
                    }

                    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) 
                    {
                        switch (state) 
                        {
                            case GameState::MAIN_MENU:
                            {
                                if (!handleMainMenuClick(mx, my))
                                {
                                    running = false;
                                }
                                break;
                            }
                            case GameState::LOGIN_P1:
                            case GameState::LOGIN_P2:
                            {
                                handleLoginClick(mx, my);
                                break;
                            }
                            case GameState::P2_GUEST_NAME:
                            {
                                handleGuestNameClick(mx, my);
                                break;
                            }
                            case GameState::POST_LOGIN_MENU:
                            {
                                if (!handlePostLoginMenuClick(mx, my))
                                {
                                    running = false;
                                }
                                break;
                            }
                            case GameState::HISTORY:
                            {
                                int panH = screenH - 160;
                                int panY = 90;
                                if (buttonHit(mx, my, screenW / 2, panY + panH + 16, 200, 44))
                                {
                                    state = GameState::POST_LOGIN_MENU;
                                }
                                break;
                            }
                            case GameState::HIGH_SCORES:
                            {
                                int panH = screenH - 160;
                                int panY = 90;
                                if (buttonHit(mx, my, screenW / 2, panY + panH + 16, 200, 44))
                                {
                                    state = GameState::POST_LOGIN_MENU;
                                }
                                break;
                            }
                            case GameState::SETTINGS:
                            {
                                handleSettingsClick(mx, my);
                                break;
                            }
                            case GameState::PLAYING:
                            {
                                if (hoverCol >= 0 && hoverCol < COLS) 
                                {
                                    int row = board.getDropRow(hoverCol);
                                    if (row != -1 && board.dropCoin(hoverCol, currentPlayer)) 
                                    {
                                        DroppingCoin dc;
                                        dc.col = hoverCol;dc.row = row;dc.player = currentPlayer;
                                        dc.startY = (float)(offsetY - CELL_SIZE / 2);
                                        dc.endY = (float)(offsetY + row * CELL_SIZE + CELL_SIZE / 2);
                                        dc.t = 0.f;dc.done = false;
                                        dropping.push_back(dc);
                                        spawnParticles(hoverCol, row, currentPlayer);

                                        if (board.wincheck(currentPlayer)) 
                                        {
                                            winner = currentPlayer;
                                            state = GameState::GAME_OVER;
                                            announceT = 0.f;
                                            recordResult(winner);
                                        }
                                        else if (board.isFull()) 
                                        {
                                            winner = 0;
                                            state = GameState::GAME_OVER;
                                            announceT = 0.f;
                                            recordResult(0);
                                        }
                                        else 
                                        {
                                            currentPlayer = 3 - currentPlayer;
                                        }
                                    }
                                }
                                break;
                            }
                            default:
                            {
                                break;
                            }
                        }
                    }

                    // Mousewheel Scrolling For History
                    if (e.type == SDL_MOUSEWHEEL && state == GameState::HISTORY)
                    {
                        historyScroll = max(0, historyScroll - (int)e.wheel.y);
                    }
                }

                if (state == GameState::PLAYING || state == GameState::GAME_OVER) 
                {
                    updateDropping(dt); updateParticles(dt);
                }

                switch (state) 
                {
                    case GameState::MAIN_MENU:
                    {
                        renderMainMenu(mx, my);
                        break;
                    }
                    case GameState::LOGIN_P1:
                    {
                        renderLoginScreen(mx, my);
                        break;
                    }
                    case GameState::LOGIN_P2:
                    {
                        renderLoginScreen(mx, my);
                        break;
                    }
                    case GameState::P2_GUEST_NAME:
                    {
                        renderGuestName(mx, my);
                        break;
                    }
                    case GameState::POST_LOGIN_MENU:
                    {
                        renderPostLoginMenu(mx, my);
                        break;
                    }
                    case GameState::HISTORY:
                    {
                        renderHistory(mx, my);
                        break;
                    }
                    case GameState::HIGH_SCORES:
                    {
                        renderHighScores(mx, my);
                        break;
                    }
                    case GameState::SETTINGS:
                    {
                        renderSettings(mx, my);
                        break;
                    }
                    case GameState::PLAYING:
                    {
                        renderGameBoard(dt);
                        break;
                    }
                    case GameState::GAME_OVER:
                    {
                        renderGameBoard(dt);
                        renderAnnouncement(dt);
                        break;
                    }
                }
                SDL_RenderPresent(renderer);
            }
        }
};


#endif
