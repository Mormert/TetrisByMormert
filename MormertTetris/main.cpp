#include <raylib.h>
#include <iostream>
#include <list>

#define COLUMNS 10
#define ROWS 20

class Square {
public:
	int x;
	int y;
	Color color;

	Square(int x, int y) : x(x), y(y)
	{
		color = Color{
			(unsigned char)GetRandomValue(0, 255),
			(unsigned char)GetRandomValue(0, 255),
			(unsigned char)GetRandomValue(0, 255)
			,255};
	}

	void Render() {
		DrawRectangle(2 + 30 * x, 2 + 30 * y, 28, 28, color);
	}
};

class FallingObject {
public:

	std::list<Square> squares;
	std::list<Square> *allSquaresRef;

	bool GenerateNewSquares() {		// Returns false if there is squares in the way

		switch (GetRandomValue(0,4))
		{
		case 0:
			squares.push_front(Square(5, 0));
			squares.push_front(Square(5, 1));
			squares.push_front(Square(5, 2));
			squares.push_front(Square(6, 2));
			break;
		case 1:
			squares.push_front(Square(5, 0));
			break;
		case 2:
			squares.push_front(Square(5, 0));
			squares.push_front(Square(6, 0));
			break;
		case 3:
			squares.push_front(Square(5, 0));
			squares.push_front(Square(5, 1));
			break;
		case 4:
			squares.push_front(Square(3, 0));
			squares.push_front(Square(4, 0));
			squares.push_front(Square(5, 0));
			squares.push_front(Square(6, 0));
			break;
		case 5:
			squares.push_front(Square(6, 0));
			squares.push_front(Square(6, 1));
			squares.push_front(Square(6, 2));
			squares.push_front(Square(6, 3));
			break;

		default:
			break;
		}
		
		return CollisionDetect(*allSquaresRef, 0, 0);
	}

	void MoveLeft() {
		if (CollisionDetect(*allSquaresRef, -1, 0)) {
			return;
		}

		std::list<Square>::iterator it;
		for (it = squares.begin(); it != squares.end(); it++)
		{
			if (it->x == 0) {
				it->x = COLUMNS-1;
			}
			else {
				it->x--;
			}
		}
	}

	void MoveRight() {
		if (CollisionDetect(*allSquaresRef, 1, 0)) {
			return;
		}

		std::list<Square>::iterator it;
		for (it = squares.begin(); it != squares.end(); it++)
		{
			if (it->x == COLUMNS-1){
				it->x = 0;
			}
			else {
				it->x++;
			}
			
		}
	}

	bool MoveDown() {
		std::list<Square>::iterator it;

		if (CollisionDetect(*allSquaresRef, 0, 1)) {
			return false;
		}

		for (it = squares.begin(); it != squares.end(); it++)
		{
			if (it->y + 1 == ROWS) {
				//std::cout << "Hit the ground" << std::endl;
				return false;
			}
		}

		for (it = squares.begin(); it != squares.end(); it++)
		{
			it->y++;
		}

		return true;
	}

private:

	bool CollisionDetect(std::list<Square> otherSquares, int dirX, int dirY) {

		std::list<Square>::iterator itI;
		std::list<Square>::iterator itJ;
		for (itI = otherSquares.begin(); itI != otherSquares.end(); itI++)
		{
			for (itJ = squares.begin(); itJ != squares.end(); itJ++)
			{
				if (itJ->x+dirX == itI->x && itJ->y+dirY == itI->y) {
					//std::cout << "Collided with other square" << std::endl;
					return true;
				}
			}
		}
		return false;
	}

};

int main(void)
{
	const int screenWidth = 30 * COLUMNS + 2;
	const int screenHeight = 30 * ROWS + 2;
	InitWindow(screenWidth, screenHeight, "Tetris by Mormert");
	SetTargetFPS(60);


	FallingObject *fallingObject;
	fallingObject = new FallingObject();

	std::list<Square> allSquares = { };

	fallingObject->allSquaresRef = &allSquares;

	int timeAtLastFall = 0;
	bool gameOver = false;
	int score = 0;

	fallingObject->GenerateNewSquares();

	// Game loop
	while (!WindowShouldClose())
	{
		// Update loop

		if (IsKeyPressed(KEY_A)) {
			fallingObject->MoveLeft();
		}

		if (IsKeyPressed(KEY_D)) {
			fallingObject->MoveRight();
		}

		if (IsKeyPressed(KEY_S)) {
			fallingObject->MoveDown();
		}

		if (IsKeyPressed(KEY_SPACE) && !gameOver) {
			while (fallingObject->MoveDown()); // Make the squares go all the way down

			std::copy(fallingObject->squares.begin(), fallingObject->squares.end(),
				std::back_insert_iterator<std::list<Square>>(allSquares));

			fallingObject->squares.clear();
			if (fallingObject->GenerateNewSquares()) {
				gameOver = true;
				score = allSquares.size();
				std::cout << score << std::endl;
			}
		}

		if (IsKeyPressed(KEY_R)) {
			// Todo: Rotate
		}

		if (IsKeyPressed(KEY_P)) {
			if (gameOver) {
				fallingObject->squares.clear();
				allSquares.clear();

				fallingObject->GenerateNewSquares();
				gameOver = false;
				score = 0;
			}
		}

		// Every 1 second move down
		if (GetTime() >= timeAtLastFall + 1 && !gameOver) {
			
			//std::cout << "moving down" << std::endl;
			if (!fallingObject->MoveDown()) { // Returns false when we hit ground or other squares
				std::copy(fallingObject->squares.begin(), fallingObject->squares.end(),
					std::back_insert_iterator<std::list<Square>>(allSquares));
				
				fallingObject->squares.clear();
				if (fallingObject->GenerateNewSquares()) {
					gameOver = true;
					score = allSquares.size();
					std::cout << score << std::endl;
				}
			}
			timeAtLastFall = (int)GetTime();
		}


		// End update

		// Draw loop
		BeginDrawing();


			// Draw static rectangles
			for (int i = 2; i < screenWidth; i += 30) {
				for (int j = 2; j < screenHeight; j += 30) {
					DrawRectangle(i, j, 28, 28, LIGHTGRAY);
				}
			}

			// Draw static squares
			std::list<Square>::iterator it;
			for (it = allSquares.begin(); it != allSquares.end(); it++)
			{
				it->Render();
			}

			// Draw squares in falling object
			for (it = fallingObject->squares.begin(); it != fallingObject->squares.end(); it++) {
				it->Render();
			}
			
			
			if (gameOver) {
				DrawRectangle(0, 0, screenWidth, screenHeight, RAYWHITE);
				DrawText("GAME OVER", 68, 250, 28, DARKGRAY);
				DrawText("SCORE:", 68, 280, 18, DARKGRAY);

				char s[11];
				sprintf_s(s, "%ld", score);

				DrawText(s, 68, 300, 18, DARKGRAY);
				DrawText("PRESS 'P' TO CONTINUE", 12, 350, 22, DARKGRAY);
			}
			
			ClearBackground(BLACK);


		EndDrawing();
		// End draw loop
	}

	CloseWindow();

	return 0;
}