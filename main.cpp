///*
#pragma warning(disable:4996)
#include <windows.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <algorithm>



using namespace std;


#define		PI			3.1415926
#define		epsilon		1.0
#define		polygon_num 36

#define		allWidth	1300
#define		allHeight	700

#define		bgLeft		20
#define		bgRight		1080
#define		bgBottom	30
#define		bgTop		670

#define		scoreX		1100
#define		scoreW		180

#define		brickWidth	50
#define		brickHeight	30

#define		barW		200
#define		barH		15


float	radius = 5.0;
float	point_size = 3.0;
int		n = 0;

int		brick_num = 100;
int		ball_num = 1;

int		collide = 0;
int		ranking[6] = { 0, 0, 0, 0, 0, 0 };
int		score = 0;
int		time_limit = 320;

float	barX;
float	barY;

float	delta_x, delta_y;
float	fix_radius;

int		brickFloor = 3;			// 블럭 층 개수 (1 ~ 6)
int		brickColor = 4;			// 블럭 색깔 다양성 (빨강 1 ~ 보라 7)
bool	selectBrickFloor = false;
bool	selectBrickColor = false;

bool	introScreen = true;		// 게임 시작 화면
bool	pauseScreen = false;	// 일시정지 화면
int		setBallAngle = 90.0;	// 시작공 좌우 방향 지정
bool	startBall = false;		// 시작공 작동 여부
bool	endScreen = false;		// 게임 결과 화면

int		ballColor[3][3] = { {0.0, 1.0, 1.0}, {1.0, 1.0, 0.0}, {1.0, 0.0, 1.0} };

struct Point {
	float	x;
	float	y;
};

struct Vector {
	float	x;
	float	y;
};

struct Color {
	float	r = 0.0;
	float	g = 0.0;
	float	b = 0.0;
};

struct Ball {
	Point	center;
	float	radius = 10.0;
	float	mass;
	Color	color;
	Vector	velocity;
};

struct Brick {
	Point	point;
	int		count = 0;
};

struct Ball* ball;
struct Brick* brick;
struct Check* check;
struct Point* polygon_vertex;


void init(void) {
	float	delta;
	float	distribution_delta;
	float	x, y;
	int		r, g, b;
	int		a, i = 0, k = 0;

	polygon_vertex = new Point[polygon_num];
	delta = 2.0 * PI / polygon_num;
	for (int i = 0; i < polygon_num; i++) {
		polygon_vertex[i].x = cos(delta * i);
		polygon_vertex[i].y = sin(delta * i);
	}

	distribution_delta = 2.0 * PI / ball_num;
	ball = new Ball[ball_num];

	for (int i = 0; i < ball_num; i++) {
		//ball[i].radius = radius * (1.5 + sin((double)(i)));
		ball[i].center.x = 500;
		ball[i].center.y = 110;
		ball[i].velocity.x = 0.0;
		ball[i].velocity.y = 0.0;
		ball[i].mass = 1;
		ball[i].color.r = ballColor[i][0];
		ball[i].color.g = ballColor[i][1];
		ball[i].color.b = ballColor[i][2];
	}

	int n = 0;
	float last = 0.0;
	brick = new Brick[100];
	for (int j = 1; j <= brickFloor; j++) {
		a = 20 + rand() % 30;
		k = i;
		for (int i = 0; i < 20; i++) {

			y = bgTop - 30 * 1.5 * j;
			//x = ((y - 746) * -0.3030) + a + last;
			x = ((y - 746) * -0.3030) + a + ((50 + 10.0) * (i - k));
			//last = a + 20;
			//cout << y << " " << x << "\n";

			if (((y + 2884) * 0.3031) < x + 50 + 5.0) { break; }

			brick[n].point.x = x;
			brick[n].point.y = y;

			// Set Brick Color
			if (n % 11 == 6) { brick[n].count = 0; }
			else { brick[n].count = rand() % brickColor + 1; }
			n++;
		}
	}

	brick_num = n;

	// bar X Y position
	barX = 400;
	barY = 100;
}


void Draw_Background() {
	int size = 100;

	// Game Background 
	glColor3f(255.0, 255.0, 255.0);
	glBegin(GL_POLYGON);
	glVertex2f(bgLeft, bgBottom);
	glVertex2f(bgLeft + size, allHeight / 2);
	glVertex2f(bgRight - size, allHeight / 2);
	glVertex2f(bgRight, bgBottom);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex2f(bgBottom, bgTop);
	glVertex2f(bgLeft + size, allHeight / 2);
	glVertex2f(bgRight - size, allHeight / 2);
	glVertex2f(bgRight, bgTop);
	glEnd();

	// Game Background Line
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(bgLeft, bgBottom);
	glVertex2f(bgLeft + size, allHeight / 2);
	glVertex2f(bgBottom, bgTop);
	glVertex2f(bgRight, bgTop);
	glVertex2f(bgRight - size, allHeight / 2);
	glVertex2f(bgRight, bgBottom);
	glEnd();

	// Score Background
	glColor3f(255.0, 255.0, 255.0);
	glBegin(GL_POLYGON);
	glVertex2f(scoreX, bgBottom);
	glVertex2f(scoreX, bgTop);
	glVertex2f(scoreX + scoreW, bgTop);
	glVertex2f(scoreX + scoreW, bgBottom);
	glEnd();
}


void Draw_Bar() {
	glColor3f(0.5, 0.0, 1.0);
	glBegin(GL_POLYGON);
	glVertex3f(barX, barY, 0.0);
	glVertex3f(barX + barW, barY, 0.0);
	glVertex3f(barX + barW, barY - barH, 0.0);
	glVertex3f(barX, barY - barH, 0.0);
	glEnd();
}


void Draw_Ball(int indexBall) {
	float	x, y, radius;

	x = ball[indexBall].center.x;
	y = ball[indexBall].center.y;
	radius = ball[indexBall].radius;

	glColor3f(ball[indexBall].color.r, ball[indexBall].color.g, ball[indexBall].color.b);

	glBegin(GL_POLYGON);
	for (int i = 0; i < polygon_num; i++)
		glVertex3f(x + radius * polygon_vertex[i].x, y + radius * polygon_vertex[i].y, 0.0);
	glEnd();
}


void Draw_Brick(Brick *brick, int idx) {
	float	x, y;
	float	r = 0.0, g = 0.0, b = 0.0;
	int		count;

	x = brick[idx].point.x;
	y = brick[idx].point.y;
	count = brick[idx].count;

	switch (count)
	{
	case 0:		r = 1.0;	g = 1.0;	b = 1.0;	break;
	case 1:		r = 1.0;							break;
	case 2:		r = 1.0;	g = 0.5;				break;
	case 3:		r = 1.0;	g = 1.0;				break;
	case 4:		g = 1.0;							break;
	case 5:		r = 0.5;	g = 0.8;	b = 1.0;	break;
	case 6:		b = 1.0;							break;
	case 7:		r = 0.6;	g = 0.1;	b = 0.9;	break;
	default:
		break;
	}

	glColor3f(r, g, b);

	glBegin(GL_POLYGON);
	glVertex3f(x, y, 0.0);
	glVertex3f(x + brickWidth, y, 0.0);
	glVertex3f(x + brickWidth, y + brickHeight, 0.0);
	glVertex3f(x, y + brickHeight, 0.0);
	glEnd();
}


void MyReshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, allWidth, 0, allHeight);
}


void Check_Collision_Ball_Wall(Ball& ball) {


	float bx = ball.center.x;
	float by = ball.center.y;
	float br = ball.radius;

	if (by < bgBottom) {
		ball_num--;
	}

	if (by <= allHeight / 2) {
		//if (by - br < bgBottom && by < 0.0) {		// Bottom Face
		//	ball.velocity.y *= -1;
		//}
		if ((3.2 * (bx - br) - 34) < by && ball.velocity.x < 0.0) {		// 3 Face
			ball.velocity.x *= -1;
		}
		else if ((-3.2 * (bx + br) + 3486) < by && ball.velocity.x > 0.0) {		// 4 Face
			ball.velocity.x *= -1;
		}
	}
	else if (by > allHeight / 2) {
		if (by + br > bgTop && by > 0.0) {		// Top Face
			ball.velocity.y *= -1;
		}
		else if ((-3.3 * (bx - br) + 746) > by && ball.velocity.x < 0.0) {		// 2 Face
			ball.velocity.x *= -1;
		}
		else if ((3.3 * (bx + br) - 2884) > by && ball.velocity.x > 0.0) {		// 1 Face
			ball.velocity.x *= -1;
		}
	}
}


bool Check_Collision_Ball_Brick() {
	int i, j;
	float bx, by, br, vx, vy;
	float x, y, h, w;

	for (i = 0; i < ball_num; i++) {
		Ball b = ball[i];
		bx = b.center.x;
		by = b.center.y;
		br = b.radius;
		vx = b.velocity.x;
		vy = b.velocity.y;

		for (j = 0; j < brick_num; j++) {
			Brick k = brick[j];
			x = k.point.x;		// 점 좌하단 기준 
			y = k.point.y;
			h = brickHeight;
			w = brickWidth;


			// 없는 벽돌이라면
			if (k.count > 0) {

				// 벽돌 하단 면
				if (x <= bx && bx <= (x + w) && y <= (by + br) && vy > 0.0) {
					//cout << "1";
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score += 10;
					return true;
				}

				// 벽돌 우하단 면
				if (sqrt(pow(x + w - bx, 2) + pow(y - by, 2)) < br) {
					ball[i].velocity.x *= -1;
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score += 10;
					return true;
				}

				// 벽돌 우단 면
				if (x + h - 1 < (bx + br) && (bx + br) < x + h + 1 && y <= by && by <= (y + h) && vx < 0.0) {
					//cout << "2";
					ball[i].velocity.x *= -1;
					brick[j].count--;
					score += 10;
					return true;
				}

				// 벽돌 우상단 면
				if (sqrt(pow(x + w - bx, 2) + pow(y + h - by, 2)) < br) {
					ball[i].velocity.x *= -1;
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score += 10;
					return true;
				}

				// 벽돌 상단 면
				if (x <= bx && bx <= (x + h) && y < (by - br) && (by - br) <= (y + h) && vy < 0.0) {
					//cout << "3";
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score += 10;
					return true;
				}

				// 벽돌 좌상단 면
				if (sqrt(pow(x - bx, 2) + pow(y + h - by, 2)) < br) {
					ball[i].velocity.x *= -1;
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score += 10;
					return true;
				}

				// 벽돌 좌단 면
				if (x - 1 < (bx + br) && (bx + br) < x + 1 && y <= by && by <= (y + h) && vy > 0.0) {
					//cout << "4";
					ball[i].velocity.x *= -1;
					brick[j].count--;
					score += 10;
					return true;
				}

				// 벽돌 좌하단 면
				if (sqrt(pow(x - bx, 2) + pow(y - by, 2)) < br) {
					ball[i].velocity.x *= -1;
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score += 10;
					return true;
				}
			}
		}
	}
	return false;
}

void Check_Collision_Ball_Bar(Ball& ball) {
	float bx = ball.center.x;
	float by = ball.center.y;
	float br = ball.radius;
	float vy = ball.velocity.y;

	if (barX < bx && bx < barX + barW && by + br < barY + barH && vy < 0.0) {
		ball.velocity.y *= -1;
	}

}


void Update_Position(void) {
	for (int i = 0; i < ball_num; i++) {
		ball[i].center.x += ball[i].velocity.x;
		ball[i].center.y += ball[i].velocity.y;
	}
}


bool collides(const Ball& ball, const Brick& block) {
	// Check if ball is within brick's bounds

	if (block.point.x <= ball.center.x + ball.radius && ball.center.x + ball.radius < block.point.x + brickWidth
		&& block.point.y <= ball.center.y + ball.radius && ball.center.y + ball.radius < block.point.y + brickHeight
		&& ball.velocity.x >= 0.0 && ball.velocity.y >= 0.0) {
		cout << ball.center.x << " " << ball.center.y << "\n";
		cout << block.point.x << " " << block.point.y << "\n";
		cout << "1 (+, +)" << "\n";
		return true;
	}
	if (block.point.x <= ball.center.x + ball.radius && ball.center.x + ball.radius < block.point.x + brickWidth
		&& block.point.y < ball.center.y - ball.radius && ball.center.y - ball.radius <= block.point.y + brickHeight
		&& ball.velocity.x >= 0.0 && ball.velocity.y < 0.0) {
		cout << ball.center.x << " " << ball.center.y << "\n";
		cout << block.point.x << " " << block.point.y << "\n";
		cout << "2 (+, -)" << "\n";
		return true;
	}
	if (block.point.x <= ball.center.x - ball.radius && ball.center.x - ball.radius < block.point.x + brickWidth
		&& block.point.y < ball.center.y + ball.radius <= block.point.y + brickHeight
		&& ball.velocity.x < 0.0 && ball.velocity.y >= 0.0) {
		cout << ball.center.x << " " << ball.center.y << "\n";
		cout << block.point.x << " " << block.point.y << "\n";
		cout << "3 (-, +)" << "\n";
		return true;
	}
	if (block.point.x < ball.center.x - ball.radius && ball.center.x - ball.radius <= block.point.x + brickWidth
		&& block.point.y < ball.center.y - ball.radius && ball.center.y - ball.radius <= block.point.y + brickHeight
		&& ball.velocity.x < 0.0 && ball.velocity.y < 0.0) {
		cout << ball.center.x << " " << ball.center.y << "\n";
		cout << block.point.x << " " << block.point.y << "\n";
		cout << "4 (-, -)" << "\n";
		return true;
	}


	return false;
}

// Calculate angle of incidence and angle of reflection after collision with brick
void updateBallDirection(Ball& ball, Brick& block) {
	// Calculate the normal vector of the brick's surface at the point of collision

	//float normX, normY;
	//if (ball.center.x < brick.point.x) {
	//	normX = -1;
	//}
	//else if (ball.center.x > brick.point.x + brick.width) {
	//	normX = 1;
	//}
	//else {
	//	normX = 0;
	//}
	//if (ball.center.y < brick.point.y) {
	//	normY = -1;
	//}
	//else if (ball.center.y > brick.point.y + brick.height) {
	//	normY = 1;
	//}
	//else {
	//	normY = 0;
	//}
	//// Calculate the dot product of the ball's velocity and the normal vector
	//float dotProduct = ball.velocity.x * normX + ball.velocity.y * normY;
	//// Calculate the angle of incidence
	//float angleOfIncidence = atan2(ball.velocity.y, ball.velocity.x);
	//// Calculate the angle of reflection
	//float angleOfReflection = angleOfIncidence - 2 * atan2(dotProduct * normY, dotProduct * normX);
	//// Update the direction of the ball based on the angle of reflection
	//ball.velocity.x = cos(angleOfReflection) * ball.velocity.x - sin(angleOfReflection) * ball.velocity.y;
	//ball.velocity.y = sin(angleOfReflection) * ball.velocity.x + cos(angleOfReflection) * ball.velocity.y;

}


void drawBitmapText(char* str, float x, float y) {
	int len = strlen(str);
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2f(x, y);
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	}
}


void Draw_Score(void) {
	string str;
	glColor3f(0.0, 0.0, 0.0);

	string strRanking = "Rank";
	string sequence = "12345";
	glColor3f(0.0, 0.0, 1.0);
	glRasterPos2f(scoreX + scoreW * 2 / 5.0, 630.0);
	for (int i = 0; i < strRanking.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, strRanking[i]);
	}
	glColor3f(0.0, 0.0, 0.0);
	for (int i = 0; i < sequence.size(); i++) {
		glRasterPos2f(scoreX + scoreW / 4.0, 590 - i * 30.0);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, sequence[i]);
		
		str = to_string(ranking[i]);
		if (str.size() == 2)		str = "0" + str;
		else if (str.size() < 2)	str = "00" + str;
		glRasterPos2f(scoreX + scoreW / 2.0, 590 - i * 30.0);
		for (int j = 0; j < str.size(); j++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[j]);
		}
	}

	glColor3f(0.0, 0.0, 1.0);
	string strScore = "Score";
	glRasterPos2f(scoreX + scoreW * 2 / 5.0, 400.0);
	for (int i = 0; i < strScore.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, strScore[i]);
	}
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2f(scoreX + scoreW * 2 / 5.0, 360.0);
	str = to_string(score);
	if (str.size() == 2)		str = "0" + str;
	else if (str.size() < 2)	str = "00" + str;
	for (int i = 0; i < str.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	}

	glColor3f(0.0, 0.0, 1.0);
	string strTime = "Time:";
	glRasterPos2f(scoreX + scoreW * 2 / 5.0, 290.0);
	for (int i = 0; i < strTime.size() - 1; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, strTime[i]);
	}
	glColor3f(0.0, 0.0, 0.0);
	str = to_string(time_limit / 60);
	if (str.size() == 1)	str = "0" + str;
	glRasterPos2f(scoreX + scoreW / 3.0, 250.0);
	for (int i = 0; i < str.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	}
	glRasterPos2f(scoreX + scoreW / 2.0, 250.0);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, strTime[4]);
	str = to_string(time_limit % 60);
	if (str.size() == 1)	str = "0" + str;
	glRasterPos2f(scoreX + scoreW * 3 / 5.0, 250.0);
	for (int i = 0; i < str.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	}

	glColor3f(0.0, 0.0, 1.0);
	string strHotKey = "HotKey";
	glRasterPos2f(scoreX + scoreW / 3.0, 180.0);
	for (int i = 0; i < strHotKey.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, strHotKey[i]);
	}
	glColor3f(0.0, 0.0, 0.0);
	string strBall = "N <-  Ball  -> M";
	glRasterPos2f(scoreX + scoreW / 7.0, 145.0);
	for (int i = 0; i < strBall.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, strBall[i]);
	}
	string strBar = "A <-  Bar  -> D";
	glRasterPos2f(scoreX + scoreW / 7.0, 115.0);
	for (int i = 0; i < strBar.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, strBar[i]);
	}
	string strStart = "Start    =>    B";
	glRasterPos2f(scoreX + scoreW / 6.0, 85.0);
	for (int i = 0; i < strStart.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, strStart[i]);
	}
	string strPause = "Pause   =>    P";
	glRasterPos2f(scoreX + scoreW / 6.0, 60.0);
	for (int i = 0; i < strPause.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, strPause[i]);
	}
}


bool Brick_Count(void) {
	for (int i = 0; i < brick_num; i++) {
		if (brick[i].count != 0) {
			return false;
		}
	}
	return true;
}


void Draw_miniScreen(string strResult) {
	int x = 450;
	int y = 350;
	int w = 300;
	int h = 200;

	glColor3f(0.960784f, 0.960784f, 0.862745f);
	glBegin(GL_POLYGON);
	glVertex3f(x, y, 0.0);
	glVertex3f(x + w, y, 0.0);
	glVertex3f(x + w, y + h, 0.0);
	glVertex3f(x, y + h, 0.0);
	glEnd();

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y, 0.0);
	glVertex3f(x + w, y, 0.0);
	glVertex3f(x + w, y + h, 0.0);
	glVertex3f(x, y + h, 0.0);
	glEnd();


	/*if (pauseScreen) {
		strResult = "Pause";

		glRasterPos2f(x + 120, y + h / 2);
	}
	else if (success) {
		strResult = "Success";
		glRasterPos2f(x + 120, y + h / 2);
	}
	else {
		strResult = "Fail";
		glRasterPos2f(x + 140, y + h / 2);
	}*/
	glColor3f(1.0, 0.0, 0.0);
	glRasterPos2f(x + 120, y + h / 2);
	for (int i = 0; i < strResult.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, strResult[i]);
	}
}


void Update_Ranking() {
	endScreen = true;
	startBall = false;
	ranking[5] = score;
	sort(ranking, ranking + 6);
}


void Intro_Screen(void) {
	string str;
	Brick *introBrick = new Brick[7];


	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
	glVertex2f(0.0, 0.0);
	glVertex2f(allWidth, 0.0);
	glVertex2f(allWidth, allHeight);
	glVertex2f(0.0, allHeight);
	glEnd();


	for (int i = 0; i < 7; i++) {
		introBrick[i].point.x = 300 + i * 100;
		introBrick[i].point.y = allHeight - 100;
		introBrick[i].count = i + 1;
		Draw_Brick(introBrick, i);
	}
	glColor3f(0.5, 0.5, 0.5);
	string subtitle = "break the rainbow";
	glRasterPos2f(allWidth * 2 / 5, allHeight * 8 / 10);
	for (int i = 0; i < subtitle.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, subtitle[i]);
	}
	

	glColor3f(0.0, 0.0, 0.0);
	string title = "Break Bricks";
	glRasterPos2f(allWidth * 2 / 5, allHeight * 7 / 10);
	for (int i = 0; i < title.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, title[i]);
	}

	glColor3f(0.2, 0.2, 0.2);
	string setting = "Setting";
	glRasterPos2f(allWidth / 3, allHeight * 5 / 10);
	for (int i = 0; i < setting.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, setting[i]);
	}

	if (selectBrickFloor)		glColor3f(0.2, 0.2, 0.2);
	else						glColor3f(0.5, 0.5, 0.5);
	string strBrickfloor = "Brick floors";
	glRasterPos2f(allWidth / 3, allHeight * 4 / 10);
	for (int i = 0; i < strBrickfloor.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, strBrickfloor[i]);
	}

	if (brickFloor == 1)		str = "       " + to_string(brickFloor) + "     ->";
	else if (brickFloor == 6)	str = "<-     " + to_string(brickFloor) + "       ";
	else						str = "<-     " + to_string(brickFloor) + "     ->";	
	if (selectBrickFloor)		glColor3f(1.0, 0.0, 0.0);
	else						glColor3f(0.5, 0.5, 0.5);
	glRasterPos2f(allWidth / 2, allHeight * 4 / 10);
	for (int i = 0; i < str.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	}
	glColor3f(0.5, 0.5, 0.5);

	string strBrickLevel = "Break count";
	if (selectBrickColor)		glColor3f(0.2, 0.2, 0.2);
	else						glColor3f(0.5, 0.5, 0.5);
	glRasterPos2f(allWidth / 3, allHeight * 3 / 10);
	for (int i = 0; i < strBrickLevel.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, strBrickLevel[i]);
	}

	if (brickColor == 1)		str = "       " + to_string(brickColor) + "     ->";
	else if (brickColor == 7)	str = "<-     " + to_string(brickColor) + "       ";
	else						str = "<-     " + to_string(brickColor) + "     ->";
	if (selectBrickColor)		glColor3f(1.0, 0.0, 0.0);
	else						glColor3f(0.5, 0.5, 0.5);
	glRasterPos2f(allWidth / 2, allHeight * 3 / 10);
	for (int i = 0; i < str.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	}
	

	glColor3f(1.0, 0.0, 0.0);
	string strStart = "Press the F1 button to start the game.";
	glRasterPos2f(allWidth / 3, allHeight / 10);
	for (int i = 0; i < strStart.size(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, strStart[i]);
	}
	
	glColor3f(0.5, 0.5, 0.5);
}


void Ball_Direction(void) {
	float x = ball[0].center.x;
	float y = ball[0].center.y;

	float vx = ball[0].velocity.x;
	float vy = ball[0].velocity.y;
	
	float dW = 3.0;
	float dH = 50.0;

	glPushMatrix();

	glTranslatef(x, y, 0.0);				// 공 중심으로 좌표 이동
	glRotatef(-90, 0.0, 0.0, 1.0);
	glRotatef(setBallAngle, 0.0, 0.0, 1.0);
	glTranslatef(0.0, 20.0, 0.0);

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POLYGON);

	glVertex3f(-dW, 0, 0.0);
	glVertex3f(-dW, dH, 0.0);
	glVertex3f(dW, dH, 0.0);
	glVertex3f(dW, 0.0, 0.0);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(-dW - 10, dH, 0.0);
	glVertex3f(0.0, dH + 10, 0.0);
	glVertex3f(dW + 10, dH, 0.0);
	glEnd();

	glPopMatrix();
}

void MyKey(unsigned char key, int x, int y) {

	switch (key) {	
		
	case 'p':										// 일시정지 화면
		cout << pauseScreen << "\n";
		if (pauseScreen) {
			pauseScreen = false;	break;
		}
		else {
			pauseScreen = true;		break;
		}


	case 'a':										// 시작공이 움직인 이후부터, 막대바 움직이기
		if (startBall && barX > 92) {
			barX -= 50;		break;		// backgroundLeft : 20
		}
	case 'd':
		if (startBall && barX < 1010 - barW) {
			barX += 50;		break;		// backgroundRight: 1080	barWidth: 70
		}


	case 'm':										// 시작공 좌우 방향 지정
		if (!startBall && !introScreen && setBallAngle > 30) {
			setBallAngle -= 1;	cout << setBallAngle << "\n";	break;
		}
	case 'n':
		if (!startBall && !introScreen && setBallAngle < 150) {
			setBallAngle += 1;	cout << setBallAngle << "\n";	break;
		}
	case 'b':										// 공을 움직여 게임 시작하기
		if (!startBall && !introScreen) {
			startBall = true;
			ball[0].velocity.x = cos(setBallAngle * PI / 180.0);
			ball[0].velocity.y = sin(setBallAngle * PI / 180.0) * 0.3;
			cout << "b";
			break;
		}
	default:	break;
	}	

	glutPostRedisplay();
}


void SpecialKey(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_DOWN:
		if (!selectBrickFloor) {
			selectBrickFloor = true;
		}
		if (selectBrickFloor) {
			selectBrickFloor = false;
			selectBrickColor = true;
		}
		break;
	case GLUT_KEY_UP:
		if (selectBrickFloor) {
			selectBrickFloor = false;
		}
		if (selectBrickColor) {
			selectBrickColor = false;
			selectBrickFloor = true;
		}
		break;
	case GLUT_KEY_LEFT:
		if (selectBrickFloor && brickFloor > 1)		brickFloor--;	// 1까지
		if (selectBrickColor && brickColor > 1)		brickColor--;	// 1까지
		break;
	case GLUT_KEY_RIGHT:
		if (selectBrickFloor && brickFloor < 6)		brickFloor++;	// 6까지
		if (selectBrickColor && brickColor < 7)		brickColor++;	// 7까지
		break;
	case GLUT_KEY_F1:
		introScreen = false;	init();		break;		// 게임 화면으로 넘어가기
	default: break;
	}

	glutPostRedisplay();
}

void MyTimer(int Value) {

	if (startBall && !pauseScreen) {
		time_limit--;
		score++;
	}
		
	glutPostRedisplay();
	glutTimerFunc(1000, MyTimer, time_limit);
}


void RenderScene(void) {
	int	i;

	glClearColor(0.8, 0.8, 0.8, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (introScreen) {
		Intro_Screen();
	}
	else {
		
		glColor3f(0.0, 0.0, 0.0);

		if (startBall && !pauseScreen) {

			for (i = 0; i < ball_num; i++) {
				Check_Collision_Ball_Wall(ball[i]);
			}

			Check_Collision_Ball_Brick();

			for (i = 0; i < ball_num; i++) {
				Check_Collision_Ball_Bar(ball[i]);
			}

			Update_Position();

		}


		Draw_Background();
		Draw_Score();

		Draw_Bar();

		for (i = 0; i < brick_num; i++)
			Draw_Brick(brick, i);

		if (!startBall && !endScreen) {
			Ball_Direction();
		}

		for (i = 0; i < ball_num; i++)
			Draw_Ball(i);
		
		if (pauseScreen) {
			Draw_miniScreen("Pause");
		}
		
		if (Brick_Count()) {
			Update_Ranking();
			Draw_miniScreen("Success");			
		}
		else if ((!Brick_Count() && time_limit < 1) || ball_num == 0) {
			Update_Ranking();
			Draw_miniScreen("Fail");			
		}
	}

	glFlush();
	glutSwapBuffers();
}


void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(allWidth, allHeight);
	glutCreateWindow("Project: Break Bricks");
	//init();
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(MyReshape);
	glutSpecialFunc(SpecialKey);
	glutKeyboardFunc(MyKey);
	glutIdleFunc(RenderScene);
	glutTimerFunc(1000, MyTimer, time_limit);		// 타이머 함수
	glutMainLoop();
}
//*/
