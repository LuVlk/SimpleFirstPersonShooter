#include <algorithm>

#include "olcConsoleGameEngine.h"

using namespace std;

class UltimateFPS : public olcConsoleGameEngine
{
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

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) 
	{ 
		// Controls

		// Handle Rotation
		if (m_keys[L'Q'].bHeld)
			fPlayerA -= (1.0f) * fElapsedTime;					// fElapsedTime varies with framerate so player movement feels constant.

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

		for (int x = 0; x < ScreenWidth(); x++)
		{
			// For each column, calculate the projected ray angle into world space
			float fRayAngle = (fPlayerA - fFieldOfView / 2.0f) + ((float)x / (float)ScreenWidth()) * fFieldOfView;

			float fDistanceToWall = 0.0f;
			bool bHitWall = false;
			bool bBoundary = false;

			float fEyeX = sinf(fRayAngle); // Unit vecotr for ray in player space
			float fEyeY = cosf(fRayAngle);

			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += 0.1f;

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

						vector<pair<float, float>> p; // distance, dot

						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++)
							{
								float vy = (float)nTestY + ty - fPlayerY;
								float vx = (float)nTestX + tx - fPlayerX;
								float d = sqrt(vx * vx + vy * vy);
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(make_pair(d, dot));
							}

						// Sort pairs from closest to furthest
						sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float>& right) { return left.first < right.first; });

						float fBound = 0.0025;
						if (acos(p.at(0).second) < fBound) bBoundary = true;
						if (acos(p.at(1).second) < fBound) bBoundary = true;
					}
				}
			}

			// Calculate distance to ceiling and floor
			int nCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceToWall);
			int nFloor = ScreenHeight() - nCeiling;


			// Shade depending on distance
			short nShade = ' ';

			// Fill the screen column wise
			for (int y = 0; y < ScreenHeight(); y++)
			{
				if (y < nCeiling)
				{
					// Ceiling
					Draw(x, y, L' ');
				}
				else if (y > nCeiling && y <= nFloor)
				{
					// Wall
					if (fDistanceToWall < fDepth / 4.0f)		nShade = 0x2588;	// Very close
					else if (fDistanceToWall < fDepth / 3.0f)	nShade = 0x2593;
					else if (fDistanceToWall < fDepth / 2.0f)	nShade = 0x2592;
					else if (fDistanceToWall < fDepth / 1.0f)	nShade = 0x2591;
					else										nShade = ' ';		// Too far away

					if (bBoundary)	nShade = ' '; // Black it out
					Draw(x, y, nShade);
				}
				else
				{
					// Floor
					float b = 1.0f - (((float)y - ScreenHeight() / 2.0f) / ((float)ScreenHeight() / 2.0f));
					if (b < 0.25f)		nShade = '#';
					else if (b < 0.5f)	nShade = 'x';
					else if (b < 0.75f)	nShade = '.';
					else if (b < 0.9f)	nShade = '-';
					else				nShade = ' ';

					Draw(x, y, nShade);
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

private:
	float fPlayerX = 2.0f;
	float fPlayerY = 30.0f;
	float fPlayerA = 0.0f;

	int nMapHeight = 32;
	int nMapWidth = 32;

	float fFieldOfView = 3.14159 / 4.0;
	float fDepth = 16.0f;

	wstring map;
};

int main()
{
	UltimateFPS game;
	game.ConstructConsole(240, 120, 6, 6);
	game.Start();

	return 0;
}