#include <iostream>
#include <Windows.h>
#include <chrono>
#include <random>

using namespace std;

int nScreenWidth = 32;
int nScreenHeight = 16;

string level;
int levelNums[32*16];


char nums[] = {'0', '1','2','3','4','5' };
char youWin[] = { 'Y', 'O', 'U',' ', 'W', 'I', 'N',' ' };

int main()
{
	bool win = false;
	int nSum = 0;

	int diff = 1;
	cout << "Enter diffuculy (1-5)\n";
	cin >> diff;

	random_device rdA;
	mt19937 gen(rdA());
	uniform_real_distribution<> dist(0, 1);

	random_device isNum;
	mt19937 genIsNum(isNum());
	uniform_int_distribution<> distIsNum(0, 6 - diff);

	random_device num;
	mt19937 randNum(num());
	uniform_int_distribution<> numDist(1, 5);



	for(int y = 0; y < nScreenHeight; y++)
		for (int x = 0; x < nScreenWidth; x++)
		{
			int rand = distIsNum(isNum);
			int randNum = numDist(num);

			if (rand == 1 && y < diff + 1  && (x != 0 && x != nScreenWidth - 1 && y != 0))
			{
				level += nums[randNum];
				levelNums[y * nScreenWidth + x] = randNum;
			}

			else if (x == 0 || y == 0 || x == nScreenWidth - 1)
			{
				level += '#';
				levelNums[y * nScreenWidth + x] = -1;
			}

			else
			{
				level += '.';
				levelNums[y * nScreenWidth + x] = -1;
			}
		}

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	int nPaddleWidth = 6 - diff;
	int nPaddleY = nScreenHeight - 1;

	float fPaddle = 16.0f;

	float fBallX = 16.0f;
	float fBallY = 6.0f;

	float fBallA = (dist(rdA)) * 3.14159 * 2.0f;

	while (fBallA > 3.14159f)
	{
		fBallA = (dist(rdA)) * 3.14159 * 2.0f;
	}

	float fBallDX = cosf(fBallA);
	float fBallDY = sinf(fBallA);


	// Create Console Screen
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	bool bDidWin = false;

	while (!win)
	{
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();

		// Controls

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
		{
			if((int)fPaddle > 1)
				fPaddle -= 15.0f * fElapsedTime;
			
		}
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
		{
			if ((int)fPaddle + nPaddleWidth + 1 < nScreenWidth - 1)
				fPaddle += 15.0f * fElapsedTime;
		}

		// Ball Movement
		
		float fBallOldX = fBallX;
		float fBallOldY = fBallY;

		fBallX += (diff + 2) * fElapsedTime * 1.5f * fBallDX;
		fBallY += (diff + 2) * fElapsedTime * 1.5f * fBallDY;

		int nBallOldCellX = (int)fBallOldX;
		int nBallOldCellY = (int)fBallOldY;

		int nBallNewCellX = (int)fBallX;
		int nBallNewCellY = (int)fBallY;

		char newcell = level[nBallNewCellY * nScreenWidth + nBallNewCellX];
		char oldcell = level[nBallOldCellY * nScreenWidth + nBallOldCellX];

		if (newcell == '#')
		{
			if (nBallOldCellX < nBallNewCellX || nBallOldCellX > nBallNewCellX)
				fBallDX *= -1.0f;
			if (nBallOldCellY > nBallNewCellY)
				fBallDY *= -1.0f;
		}

		if (newcell == '1' || newcell == '2' || newcell == '3' || newcell == '4' || newcell == '5')
		{
			if (nBallOldCellX < nBallNewCellX || nBallOldCellX > nBallNewCellX)
				fBallDX *= -1.0f;
			if (nBallOldCellY > nBallNewCellY || nBallOldCellY < nBallNewCellY)
				fBallDY *= -1.0f;

			levelNums[nBallNewCellY * nScreenWidth + nBallNewCellX] -= 1;
		}

		if (fBallY >= nPaddleY)
		{
			if (fBallX >= fPaddle && fBallX <= fPaddle + nPaddleWidth + 1)
				fBallDY *= -1.0f;
			else
			{
				fBallX = 16.0f;
				fBallY = 6.0f;
				fBallA = (dist(rdA)) * 3.14159 * 2.0f;
				while (fBallA > 3.14159f)
				{
					fBallA = (dist(rdA)) * 3.14159 * 2.0f;
				}
				fBallDX = cosf(fBallA);
				fBallDY = sinf(fBallA);
			}

		}
		
		for (int y = 0; y < nScreenHeight; y++)
			for (int x = 0; x < nScreenWidth; x++)
			{
				
				// Level Data Writer
				if (y == nPaddleY && x >= (int)fPaddle && x <= (int)fPaddle + nPaddleWidth)
					level[y * nScreenWidth + x] = '=';

				else if((y == nPaddleY && !(x >= (int)fPaddle && x <= (int)fPaddle + nPaddleWidth)) && !(x == 0 || x == nScreenWidth - 1))
					level[y * nScreenWidth + x] = '.';

				if (levelNums[y * nScreenWidth + x] >= 0)
				{
					level[y * nScreenWidth + x] = nums[levelNums[y * nScreenWidth + x]];
				}

				if (level[y * nScreenWidth + x] == '0')
				{
					level[y * nScreenWidth + x] = '.';
				}

				// Checks Win Condition

				nSum = 0;
				bDidWin = false;

				for (int y = 0; y < nScreenHeight; y++)
					for (int x = 0; x < nScreenWidth; x++)
					{
						if (levelNums[y * nScreenWidth + x] > 0)
						{
							nSum += 1;
							bDidWin = false;
						}
						else
							bDidWin = true;
					}

				// Level Data Displayers

				if (y == nBallNewCellY && x == nBallNewCellX)
				{
					screen[y * nScreenWidth + x] = 'o';
				}

				else if (level[y * nScreenWidth + x] == '.')
					screen[y * nScreenWidth + x] = ' ';

				else if (y == nPaddleY && x >= (int)fPaddle && x <= (int)fPaddle + nPaddleWidth)
					screen[y * nScreenWidth + x] = 0x2588;

				else if (level[y * nScreenWidth + x] == '#')
					screen[y * nScreenWidth + x] = 0x2588;

				else if (level[y * nScreenWidth + x] == '5' || level[y * nScreenWidth + x] == '4')
					screen[y * nScreenWidth + x] = 0x2593;

				else if (level[y * nScreenWidth + x] == '3' || level[y * nScreenWidth + x] == '2')
					screen[y * nScreenWidth + x] = 0x2592;

				else if (level[y * nScreenWidth + x] == '1')
					screen[y * nScreenWidth + x] = 0x2591;

				else 
					screen[y * nScreenWidth + x] = level[y * nScreenWidth + x];

			}

		if (bDidWin && nSum == 0)
			win = true;

		// Write To Screen
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

	}

	while (win)
	{

		for (int y = 0; y < nScreenHeight; y++)
			for (int x = 0; x < nScreenWidth; x++)
			{

				screen[y * nScreenWidth + x] = youWin[(y * nScreenWidth + x) % 8];

			}

		// Write To Screen
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth* nScreenHeight, { 0,0 }, & dwBytesWritten);
	}

	return 0;
}
