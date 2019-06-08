/*

	Created by Globixcon
	Last update: June 8, 2019

	Simple Atari Breakout in the console

	Ball New and old pos / cells code attributed to javidx9 on his version of Breakout made in his olcConsoleGameEngine.

*/

#include <iostream>
#include <Windows.h>
#include <chrono>
#include <random> 

using namespace std;


// Screen Dimensions
int nScreenWidth = 32;
int nScreenHeight = 16;

// Level Data
string level;
int levelNums[32 * 16]; // Brick Values

char nums[] = {'0', '1','2','3','4','5' }; // Digits for brick values to display
char youWin[] = { 'Y', 'O', 'U',' ', 'W', 'I', 'N',' ' }; // 'You Win' text screen letters

int main()
{
	bool win = false; // Are all of the bricks brocken?
	int nSum = 0; // Sum of the brick values

	// Input paddle width
	int padWid = 5;
	cout << "Enter paddle width (0-32)\n";
	cin >> padWid;
	cout << "\n";

	// Input brick rows
	int numLines = 5;
	cout << "Enter number of rows of bricks (1-8)\n";
	cin >> numLines;
	cout << "\n";

	// Input chance for cell to be a brick
	int brickChance = 3;
	cout << "Enter chance of cell to be a brick (1-5, 5 being most) \n";
	cin >> brickChance;
	cout << "\n";

	// Input ball speed
	int ballSpd = 3;
	cout << "Enter Ball Speed (1-20) \n";
	cin >> ballSpd;
	cout << "\n";

	// Random device for Ball Angle
	random_device rdA;
	mt19937 gen(rdA());
	uniform_real_distribution<> dist(0, 1);

	// Random device for brick generation chance
	random_device isNum;
	mt19937 genIsNum(isNum());
	uniform_int_distribution<> distIsNum(0, 6 - brickChance);

	// Random device for brick values
	random_device num;
	mt19937 randNum(num());
	uniform_int_distribution<> numDist(1, 5);

	// Initialize level string with char values
	
	// For each cell
	for(int y = 0; y < nScreenHeight; y++)
		for (int x = 0; x < nScreenWidth; x++)
		{
			int rand = distIsNum(isNum); // Rand gen (is the cell a brick?)
			int randNum = numDist(num); // Rand gen for brick values

			// If the cell is a brick
			if (rand == 1 && y < numLines + 1  && (x != 0 && x != nScreenWidth - 1 && y != 0)) 
			{
				level += nums[randNum]; // Digit char is added to level string
				levelNums[y * nScreenWidth + x] = randNum; // Brick value for that cell is set to the random number
			}

			// Border
			else if (x == 0 || y == 0 || x == nScreenWidth - 1)
			{
				level += '#';
				levelNums[y * nScreenWidth + x] = -1; // Filler Value for empty spaces
			}

			else // Empty Space
			{
				level += '.';
				levelNums[y * nScreenWidth + x] = -1; // Filler Value for empty spaces
			}
		}

	// Clocks to initialize fElapsedTime to regulate movement speed
	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	int nPaddleWidth = padWid - 1; // Paddle Width
	int nPaddleY = nScreenHeight - 1; // Paddle Y value on screen

	float fPaddle = 16.0f; // Paddle X value of screen (starting from the leftmost cell)

	// Ball coordinates
	float fBallX = 16.0f; 
	float fBallY = numLines + 2.0f;

	float fBallA = (dist(rdA)) * 3.14159 * 2.0f; // Initial ball angle

	// If the ball angle is not within a certain value (too shallow or going upwards), reassign the value until it is within the params
	while (!(fBallA > (1.0f/6.0f) * 3.14159f && fBallA < (5.0f/6.0f) * 3.14159f)) 
	{
		fBallA = (dist(rdA)) * 3.14159f * 2.0f;
	}
	
	// Ball direction based on angle (create unit vector)
	float fBallDX = cosf(fBallA);
	float fBallDY = sinf(fBallA);

	// Create Console Screen
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	bool bDidWin = false; // Seperate bool for win condition 

	while (!win) // Begin game loop
	{
		// Calculate fElapsedTime (regulating value for movement)
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();

		// Controls

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000) // Paddle Left
		{
			if((int)fPaddle > 1)
				fPaddle -= 16.0f * fElapsedTime;
			
		}
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000) // Paddle Right
		{
			if ((int)fPaddle + nPaddleWidth + 1 < nScreenWidth - 1)
				fPaddle += 16.0f * fElapsedTime;
		}

		// Ball Movement
		
		float fBallOldX = fBallX; // Previous ball X
		float fBallOldY = fBallY; // Previous ball Y

		// Add the unit vector to X and Y of ball based on ball speed
		fBallX += ((ballSpd + 5) *fElapsedTime * fBallDX);
		fBallY += ((ballSpd + 5) * fElapsedTime * fBallDY);

		// Integer value of old ball float positions
		int nBallOldCellX = (int)fBallOldX;
		int nBallOldCellY = (int)fBallOldY;

		// Integer value of new ball float positions
		int nBallNewCellX = (int)fBallX;
		int nBallNewCellY = (int)fBallY;

		// The character of new and old cell locations within the level string
		char newcell = level[nBallNewCellY * nScreenWidth + nBallNewCellX];
		char oldcell = level[nBallOldCellY * nScreenWidth + nBallOldCellX];


		// Collisions 
		
		if (newcell == '#') // Border
		{
			// Reverse the X direction of the ball collider
			if (nBallOldCellX < nBallNewCellX || nBallOldCellX > nBallNewCellX)
				fBallDX *= -1.0f;
			if (nBallOldCellY > nBallNewCellY)
				fBallDY *= -1.0f;
		}

		if (newcell == '1' || newcell == '2' || newcell == '3' || newcell == '4' || newcell == '5') // Bricks
		{
			// Reverse the DX or DY depending on what cell the ball is coming from
			if (nBallOldCellX < nBallNewCellX || nBallOldCellX > nBallNewCellX)
				fBallDX *= -1.0f;
			if (nBallOldCellY > nBallNewCellY || nBallOldCellY < nBallNewCellY)
				fBallDY *= -1.0f;

			levelNums[nBallNewCellY * nScreenWidth + nBallNewCellX] -= 1; // Decrease the brick value by 1
		}

		if (fBallY >= nPaddleY) // If the ball is on or below the paddly Y value
		{
			if (fBallX >= fPaddle - 1 && fBallX <= fPaddle + nPaddleWidth + 1) // If the ball hits the paddle
				fBallDY *= -1.0f; // Reverse Y direction of ball
			else // If the ball misses the paddle
			{
				// Reset ball position and random generate a new random angle
				fBallX = 16.0f;
				fBallY = numLines + 2.0f;
				fBallA = (dist(rdA)) * 3.14159 * 2.0f;
				while (!(fBallA > (1.0f / 6.0f) * 3.14159f && fBallA < (5.0f / 6.0f) * 3.14159f))
				{
					fBallA = (dist(rdA)) * 3.14159f * 2.0f;
				}

				// Create unit vector
				fBallDX = cosf(fBallA);
				fBallDY = sinf(fBallA);
			}

		}
		
		// Nested loop for x and y values on the screen
		for (int y = 0; y < nScreenHeight; y++)
			for (int x = 0; x < nScreenWidth; x++)
			{
				
				// Level Data Writers

				// Write the paddle pos data to the level 
				if (y == nPaddleY && x >= (int)fPaddle && x <= (int)fPaddle + nPaddleWidth)
					level[y * nScreenWidth + x] = '='; 

				// If the X value of cell is not within the paddle's range
				else if((y == nPaddleY && !(x >= (int)fPaddle && x <= (int)fPaddle + nPaddleWidth)) && !(x == 0 || x == nScreenWidth - 1))
					level[y * nScreenWidth + x] = '.'; // Make empty space

				if (levelNums[y * nScreenWidth + x] >= 0) // if the brick value is not empty space
				{
					level[y * nScreenWidth + x] = nums[levelNums[y * nScreenWidth + x]]; // Set cell char to the digit that the brick value is
				}

				if (level[y * nScreenWidth + x] == '0') // if the char is the number 0
				{
					level[y * nScreenWidth + x] = '.'; // Set cell as empty space
				}

				// Checks Win Conditions

				nSum = 0; // Reset sum of levelNums 
				bDidWin = false;

				// Check each cell
				for (int y = 0; y < nScreenHeight; y++)
					for (int x = 0; x < nScreenWidth; x++)
					{
						// If any cell has a positive levelNums value, the sum is positive and the player has not cleared the stage yet
						if (levelNums[y * nScreenWidth + x] > 0) 
						{
							nSum += 1;
							bDidWin = false;
						}
						else // Else the player wins
							bDidWin = true;
					}

				// Level Data Displayers

				if (y == nBallNewCellY && x == nBallNewCellX) // Display Ball at (BallX, BallY) integer values
				{
					screen[y * nScreenWidth + x] = 'o'; // Ball char
				}

				else if (level[y * nScreenWidth + x] == '.') // Empty space
					screen[y * nScreenWidth + x] = ' ';

				else if (y == nPaddleY && x >= (int)fPaddle && x <= (int)fPaddle + nPaddleWidth) // Paddle
					screen[y * nScreenWidth + x] = 0x2588; // Full block

				else if (level[y * nScreenWidth + x] == '#') // Border
					screen[y * nScreenWidth + x] = 0x2588; // Full block

				// Bricks
				else if (level[y * nScreenWidth + x] == '5' || level[y * nScreenWidth + x] == '4') 
					screen[y * nScreenWidth + x] = 0x2593; // Deep shaded block

				else if (level[y * nScreenWidth + x] == '3' || level[y * nScreenWidth + x] == '2')
					screen[y * nScreenWidth + x] = 0x2592; // Med shaded block

				else if (level[y * nScreenWidth + x] == '1')
					screen[y * nScreenWidth + x] = 0x2591; // Light shaded block

				else // Anything else displays itself
					screen[y * nScreenWidth + x] = level[y * nScreenWidth + x];

			}

		if (bDidWin && nSum == 0) // Check win condition
			win = true;

		// Write To Screen
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

	} // End Game Loop

	while (win) // Begin draw loop
	{

		// For each cell
		for (int y = 0; y < nScreenHeight; y++)
			for (int x = 0; x < nScreenWidth; x++)
			{
				// Display a char from (YOU WIN ) 
				screen[y * nScreenWidth + x] = youWin[(y * nScreenWidth + x) % 8];
			}

		// Write To Screen
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth* nScreenHeight, { 0,0 }, & dwBytesWritten);

	} // End draw loop

	return 0;
}
