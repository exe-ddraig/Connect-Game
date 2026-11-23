#ifndef BOARD_H
#define BOARD_H

#include "common.h"

////////////////////////////////////////////////////////////////////////
class Board 
{
    private: 
        int board[ROWS][COLS];
    public:
        Board() 
        {
            reset();
        }
        void reset() 
        {
            for (int i = 0;i < ROWS;i++)
            {
                for (int j = 0;j < COLS;j++)
                {
                    board[i][j] = 0;
                }
            }
        }
        bool dropCoin(int col, int player) 
        {
            for (int r = ROWS - 1;r >= 0;r--)
            {
                if (board[r][col] == 0) 
                { 
                    board[r][col] = player;
                    return true; 
                }
            }
            return false;
        }
        int getDropRow(int col) const 
        {
            for (int r = ROWS - 1;r >= 0;r--)
            {
                if (board[r][col] == 0)
                {
                    return r;
                }
            }
            return -1;
        }
        int  getCell(int r, int c) const 
        { 
            return board[r][c];
        }
        bool isFull() const 
        {
            for (int c = 0;c < COLS;c++)
            {
                if (board[0][c] == 0)
                {
                    return false;
                }
            }
            return true;
        }
        bool checkDir(int p, int r, int c, int dr, int dc) const 
        {
            for (int i = 0;i < 4;i++) 
            {
                int nr = r + i * dr, nc = c + i * dc;
                if (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                {
                    return false;
                }
                if (board[nr][nc] != p)
                {
                    return false;
                }
            }
            return true;
        }
        bool wincheck(int p) const 
        {
            for (int r = 0;r < ROWS;r++)
            {
                for (int c = 0;c < COLS;c++)
                {
                    if (checkDir(p, r, c, 0, 1) || checkDir(p, r, c, 1, 0) ||
                        checkDir(p, r, c, 1, 1) || checkDir(p, r, c, 1, -1))
                    {
                        return true;
                    }
                }
            }
            return false;
        }
};
////////////////////////////////////////////////////////////////////////


#endif
