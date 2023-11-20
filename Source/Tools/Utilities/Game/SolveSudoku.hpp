#pragma once
#include <array>
#include <iostream>

using Board = std::array<int,81>;
using Position = int;
bool SolveSudoku(Board& aBoard);
bool SudokuSolver(Board& aBoard);


bool CheckCollumn(const Board& aBoard,const Position aPosition)
{
	int checkedValue = aBoard[aPosition];

	if(checkedValue == 0)
	{
		return true;
	}

	int positionInArray = aPosition + 1; //APosition is now the position in a array that starts at 1
	int Xposition = positionInArray % 9;

	if(Xposition == 0)
	{
		Xposition = 9;
	}

	for(int i = 1; i <= 9; i++)
	{
		int place = (i - 1) * 9 + Xposition;

		if(place != positionInArray)
		{
			if(aBoard[place - 1] == checkedValue)
			{
				return false;
			}
		}
	}
	return true;
}

bool CheckRow(const Board& aBoard,const Position aPosition)
{
	int checkedValue = aBoard[aPosition];

	if(checkedValue == 0)
	{
		return true;
	}

	int positionInArray = aPosition + 1; //APosition is now the position in a array that starts at 1
	int Yposition = static_cast<int>(ceil(positionInArray / 9.0f));

	for(int i = 1; i <= 9; i++)
	{
		int place = (Yposition - 1) * 9 + i;

		if((place) != positionInArray)
		{
			if(aBoard[place - 1] == checkedValue)
			{
				return false;
			}
		}
	}
	return true;
}

bool CheckBox(const Board& aBoard,const Position aPosition)
{
	int checkedValue = static_cast<int>(aBoard[aPosition]);

	if(checkedValue == 0)
	{
		return true;
	}

	int positionInArray = aPosition + 1; //APosition is now the position in a array that starts at 1

	int Xposition = positionInArray % 9;

	if(Xposition == 0)
	{
		Xposition = 9;
	}

	int Yposition = static_cast<int>(ceil(positionInArray / 9.0f));

	int XBox = static_cast<int>(ceil(Xposition / 3.0f));
	int YBox = static_cast<int>(ceil(Yposition / 3.0f));

	int Center = static_cast<int>((3 * XBox - 1.0f) + 9 + ((YBox - 1) * 3 * 9));

	int mdf[] =
	{
		-10, -9, -8,
		 -1,  0,  1,
		  8,  9, 10
	};

	for(auto mod : mdf)
	{
		int place = Center + mod;
		if(place != positionInArray)
		{
			if(aBoard[place - 1] == checkedValue)
			{
				return false;
			}
		}
	}
	return true;
}

bool SolveSudoku(Board& aBoard)
{

	for(int i = 0; i < aBoard.size(); i++)
	{
		if(!CheckCollumn(aBoard,i) || !CheckRow(aBoard,i) || !CheckBox(aBoard,i))
		{
			return false;
		}
	}

	return SudokuSolver(aBoard);
}

bool SudokuSolver(Board& aBoard)
{
	for(int i = 0; i < aBoard.size(); i++)
	{
		if(aBoard[i] == 0)
		{
			for(int j = 1; j <= 9; j++)
			{
				aBoard[i] = j;
				if(CheckCollumn(aBoard,i) && CheckRow(aBoard,i) && CheckBox(aBoard,i))
				{
					if(SolveSudoku(aBoard))
					{
						return true;
					}
					else
					{
						continue;
					}
				}
			}
			aBoard[i] = 0;
			return false;
		}
	}
	return true;
}