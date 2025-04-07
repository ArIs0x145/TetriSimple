#include <iostream>
#include <thread>
#include <vector>
#include <math.h>
#include <Windows.h>

using namespace std;

int nFieldWidth = 12, nFieldHeight = 18;
unsigned char *pField = new unsigned char[nFieldWidth*nFieldHeight];
string tetromino[8];
int nCurrentPiece = rand() % 7, nCurrentRotation = 0, saveTetromino = 7;
int nCurrentX = nFieldWidth / 2 - 2, nCurrentY = 0;

int rotate(int px, int py, int r){
	switch (r % 4){
		case 0:		
			return py * 4 + px;					
		case 1:			
			return (3 - py) + px * 4;					
		case 2:			
			return 15 - px - py * 4;				
		case 3:			
			return py + (3 - px) * 4;
		default:
			return 0;					
	}					
}


bool doesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY){
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++){
			int pi = rotate(px, py, nRotation);
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight){
					if (tetromino[nTetromino][pi] != ' ' && pField[fi] != 0)
						return false;
				}
		}
	return true;
}


void initTetromino(){
	nCurrentX = nFieldWidth / 2 - 2;
	nCurrentY = 0;
	nCurrentRotation = 0;
}


int beep(size_t n){
	switch (n){
		case 1:		
			return Beep(200, 300);		
		case 2:			
			return Beep(400, 300);				
		case 3:			
			return Beep(600, 300);			
		case 4:			
			return Beep(800, 300);
		default:
			return 0;			
	}
}


int main(){
	tetromino[0] = "  X   X   X   X ";
	tetromino[1] = "  X  XX   X     ";
	tetromino[2] = "     XX  XX     ";
	tetromino[3] = "  X  XX  X      ";
	tetromino[4] = " X   XX   X     ";
	tetromino[5] = " X   X   XX     ";
	tetromino[6] = "  X   X  XX     ";
	tetromino[7] = "                ";
	int nScreenWidth = 120, nScreenHeight = 30;
	char *screen = new char[nScreenWidth * nScreenHeight];
	WORD *attributes = new WORD[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++){
		screen[i] = ' ';
		attributes[i] = 0;
	}
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	for (int x = 0; x < nFieldWidth; x++)
		for (int y = 0; y < nFieldHeight; y++)
			pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 8 : 0;


	bool bKey[7];
	int nSpeed = 20, nSpeedCount = 0;
	bool bGameOver = false, bForceDown = false;
	bool bZRotateHold = true, bXRotateHold = true, bUpHold = true, bSaveHold = true;
	int nPieceCount = 0, nScore = 0;
	WORD colorArr[10];
	colorArr[0] = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
    colorArr[1] = FOREGROUND_GREEN;
    colorArr[2] = FOREGROUND_RED;
    colorArr[3] = FOREGROUND_BLUE;
    colorArr[4] = FOREGROUND_BLUE | FOREGROUND_RED;
    colorArr[5] = FOREGROUND_RED | FOREGROUND_GREEN;
    colorArr[6] = FOREGROUND_GREEN | FOREGROUND_BLUE;
    colorArr[7] = FOREGROUND_RED | FOREGROUND_INTENSITY;
	colorArr[8] = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
	//colorArr[9] = BACKGROUND_BLUE |BACKGROUND_GREEN |BACKGROUND_RED;
	colorArr[9] = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
	vector<int> vLines;

	while (!bGameOver){
		this_thread::sleep_for(50ms);
		nSpeedCount++;
		bForceDown = (nSpeedCount == nSpeed);

		
		for (int k = 0; k < 7; k++)								// left, up, right, down, z, x, space
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x26\x27\x28\x5A\x58\x20"[k]))) != 0;

		nCurrentX -= (bKey[0] && doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
		nCurrentX += (bKey[2] && doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;		
		nCurrentY += (bKey[3] && doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;

		nCurrentRotation += (bKey[4] && bZRotateHold && doesPieceFit(nCurrentPiece, nCurrentRotation + 3, nCurrentX, nCurrentY)) ? 3 : 0;
		bZRotateHold = !bKey[4];

		nCurrentRotation += (bKey[5] && bXRotateHold && doesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
		bXRotateHold = !bKey[5];

		int nPreX = nCurrentX, nPreY = nCurrentY;
		while (doesPieceFit(nCurrentPiece, nCurrentRotation, nPreX, nPreY + 1) && ++nPreY);
		if (bKey[1] && bUpHold){
			nCurrentX = nPreX;
			nCurrentY = nPreY;
			bForceDown = true;
		}
		bUpHold = !bKey[1];
		
		if (bKey[6] && bSaveHold){
			swap(nCurrentPiece, saveTetromino);
			if (nCurrentPiece == 7)	nCurrentPiece =	rand() % 7;
			initTetromino();	
			bSaveHold = false;
		}


		if (bForceDown){
			nSpeedCount = 0;
			nPieceCount++;
			if (nPieceCount % 20 == 0)
				if (nSpeed >= 7) nSpeed--;
			
			if (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++;
			else{
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] != ' ')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

				for (int py = 0; py < 4; py++)
					if(nCurrentY + py < nFieldHeight - 1){
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++)
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

						if (bLine){
							for (int px = 1; px < nFieldWidth - 1; px++){
								attributes[(nCurrentY + py) * nFieldWidth + px] = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
								pField[(nCurrentY + py) * nFieldWidth + px] = 9;
							}
							vLines.push_back(nCurrentY + py);
						}						
					}

				nScore += 25;
				if(!vLines.empty())	nScore += pow(2, vLines.size() + 1) * 100;

				initTetromino();
				nCurrentPiece =	rand() % 7;
				bSaveHold = true;

				bGameOver = !doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}
		}
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++){
				screen[(y + 2)*nScreenWidth + (x + 2)] = " ABCDEFG#-"[pField[y*nFieldWidth + x]];
				attributes[(y + 2)*nScreenWidth + (x + 2)] = colorArr[pField[y*nFieldWidth + x]];
			}
		
		if (!vLines.empty()){
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			WriteConsoleOutputAttribute(hConsole, &attributes[0], nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			beep(vLines.size());
			for (int &v : vLines)
				for (int px = 1; px < nFieldWidth - 1; px++){
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					pField[px] = 0;
				}

			vLines.clear();
		}

		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] != ' '){
					screen[(nPreY + py + 2) * nScreenWidth + (nPreX + px + 2)] = '.';
					attributes[(nPreY + py + 2) * nScreenWidth + (nPreX + px + 2)] = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
					attributes[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = colorArr[nCurrentPiece + 1];
				}

		

		sprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 20, "SCORE: %d", nScore);
		for (int i = 0; i < 20; i++)
			attributes[2 * nScreenWidth + nFieldWidth + 6 + i] = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
		
		for (int y = 0; y < 4; y++)
			for (int x = 0; x < 4; x++){
					sprintf_s(&screen[(4 + y) * nScreenWidth + nFieldWidth + 7 + x], 2, "%c", tetromino[saveTetromino][x + y * 4]);
					attributes[(4 + y) * nScreenWidth + nFieldWidth + 7 + x] = colorArr[saveTetromino + 1];
			}

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		WriteConsoleOutputAttribute(hConsole, &attributes[0], nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}
	CloseHandle(hConsole);
	cout << "Game Over!! Score:" << nScore << endl;
	system("pause");
	return 0;
}
