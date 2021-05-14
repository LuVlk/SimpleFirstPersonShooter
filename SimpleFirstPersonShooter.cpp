#include <algorithm>

#include "olcConsoleGameEngine.h"

using namespace std;
static const float PI = 3.14159f;

class UltimateFPS : public olcConsoleGameEngine
{
private:
	float fPlayerX = 2.0f;
	float fPlayerY = 30.0f;
	float fPlayerA = 3.14159;

	int nMapHeight = 32;
	int nMapWidth = 32;

	float fFieldOfView = 3.14159 / 4.0;
	float fDepth = 16.0f;

	wstring map;

	olcSprite* spriteWall;

public:
	UltimateFPS()
	{
		m_sAppName = L"Ultimate First Person Shooter";
	}

	virtual bool OnUserCreate() 
	{ 
		map += L"################################";
		map += L"#..............................#";
		map += L"###########...###.......########";
		map += L"#..............##..............#";
		map += L"#...########...##.........##...#";
		map += L"#.........#######.........##...#";
		map += L"#..............##.........##...#";
		map += L"#######...#######..............#";
		map += L"#.....#...#....##..............#";
		map += L"#..............##..............#";
		map += L"#...########...##..............#";
		map += L"#.......####...##..............#";
		map += L"#...########...##..............#";
		map += L"#..............##..............#";
		map += L"#..............##..............#";
		map += L"########..######################";
		map += L"########..######################";
		map += L"#..............##..............#";
		map += L"#..##############.......########";
		map += L"#..............##..............#";
		map += L"######....##...##.........##...#";
		map += L"#.........##...##.........##...#";
		map += L"#....#....##...##..............#";
		map += L"#....#.........##..............#";
		map += L"######...########..............#";
		map += L"#..............##..............#";
		map += L"#..............##..............#";
		map += L"#...#######....##..............#";
		map += L"#...##....#....##..............#";
		map += L"#...##....#....##..............#";
		map += L"#...##.........##..............#";
		map += L"################################";

		spriteWall = new olcSprite(L"./sprites/fps_wall1.spr");

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) 
	{ 
		// Controls

		// Handle Rotation
		if (m_keys[L'Q'].bHeld)
			fPlayerA -= (1.0f) * fElapsedTime;

		if (m_keys[L'E'].bHeld)
			fPlayerA += (1.0f) * fElapsedTime;

		// Handle Forwards 
		if (m_keys[L'W'].bHeld)
		{
			fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
			fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
			{
				fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
				fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
			}
		}

		// Handle Backwards
		if (m_keys[L'S'].bHeld)
		{
			fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
			fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
			{
				fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
				fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
			}
		}

		// Handle Left
		if (m_keys[L'A'].bHeld)
		{
			fPlayerX += sinf(fPlayerA - 3.14159 / 2.0) * 2.5f * fElapsedTime;
			fPlayerY += cosf(fPlayerA - 3.14159 / 2.0) * 2.5f * fElapsedTime;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
			{
				fPlayerX -= sinf(fPlayerA - 3.14159 / 2.0) * 2.5f * fElapsedTime;
				fPlayerY -= cosf(fPlayerA - 3.14159 / 2.0) * 2.5f * fElapsedTime;
			}
		}

		// Handle Right
		if (m_keys[L'D'].bHeld)
		{
			fPlayerX += sinf(fPlayerA + 3.14159 / 2.0) * 2.5f * fElapsedTime;
			fPlayerY += cosf(fPlayerA + 3.14159 / 2.0) * 2.5f * fElapsedTime;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#')
			{
				fPlayerX -= sinf(fPlayerA + 3.14159 / 2.0) * 2.5f * fElapsedTime;
				fPlayerY -= cosf(fPlayerA + 3.14159 / 2.0) * 2.5f * fElapsedTime;
			}
		}

		// build the frame column wise
		for (int x = 0; x < ScreenWidth(); x++)
		{
			// For each column, calculate the projected ray angle into world space
			float fRayAngle = (fPlayerA - fFieldOfView / 2.0f) + ((float)x / (float)ScreenWidth()) * fFieldOfView;

			float fStepSize = 0.01f;
			float fDistanceToWall = 0.0f;

			bool bHitWall = false;
			bool bBoundary = false;

			float fEyeX = sinf(fRayAngle); // Unit vecotr for ray in player space
			float fEyeY = cosf(fRayAngle);

			float fSampleX = 0.0f;

			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += fStepSize;

				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				// Test if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true;			// Just set Distance to maximum depth
					fDistanceToWall = fDepth;
				}
				else
				{
					// Ray is in bounds so test to see if the ray is wall block
					if (map[nTestY * nMapWidth + nTestX] == '#')
					{
						bHitWall = true;

						float fBlockMidX = (float)nTestX + 0.5f;
						float fBlockMidY = (float)nTestY + 0.5f;

						float fTestPointX = fPlayerX + fEyeX * fDistanceToWall;
						float fTestPointY = fPlayerY + fEyeY * fDistanceToWall;

						float fTestAngle = atan2f((fTestPointY - fBlockMidY), (fTestPointX - fBlockMidX));

						if (fTestAngle >= -PI * 0.25f && fTestAngle < PI * 0.25f)
							fSampleX = fTestPointY - (float)nTestY;
						if (fTestAngle >= PI * 0.25f && fTestAngle < PI * 0.75f)
							fSampleX = fTestPointX - (float)nTestX;
						if (fTestAngle >= -PI * 0.25f && fTestAngle < -PI * 0.75f)
							fSampleX = fTestPointX - (float)nTestX;
						if (fTestAngle >= PI * 0.75f && fTestAngle < -PI * 0.75f)
							fSampleX = fTestPointY - (float)nTestY;
					}
				}
			}

			// Calculate distance to ceiling and floor
			int nCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceToWall);
			int nFloor = ScreenHeight() - nCeiling;

			// Fill the screen row wise
			for (int y = 0; y < ScreenHeight(); y++)
			{
				if (y <= nCeiling)
				{
					// Ceiling
					Draw(x, y, L' ');
				}
				else if (y > nCeiling && y <= nFloor)
				{
					// Wall
					if (fDistanceToWall < fDepth)
					{
						float fSampleY = ((float)y - (float)nCeiling) / ((float)nFloor - (float)nCeiling);
						Draw(x, y, spriteWall->SampleGlyph(fSampleX, fSampleY), spriteWall->SampleColour(fSampleX, fSampleY));
					}
					else Draw(x, y, ' ');
				}
				else
				{
					// Floor
					Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
				}
			}
		}

		// Display map
		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapHeight; ny++)
				Draw(nx + 1, ny + 1, map[ny * nMapWidth + nx]);

		// Display player
		Draw((int)fPlayerX + 1, (int)fPlayerY + 1, 'P');

		return true; 
	}
};

int main()
{
	UltimateFPS game;
	game.ConstructConsole(240, 120, 6, 6);
	game.Start();

	return 0;
}