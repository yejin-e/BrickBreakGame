///*
#pragma warning(disable:4996)
#include <windows.h>
#include <math.h>
#include <gl/gl.h>
//#include <gl/glu.h>
#include <gl/glut.h> // (or others, depending on the system in use)
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
//#include <ctime>

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

#define		brickWidth	100
#define		brickHeight	50

#define		barW		200
#define		barH		15


float	radius = 5.0;
float	point_size = 3.0;
int		n = 0;

int		brick_num = 100;
int		ball_num = 1;

int		collide = 0;
int		ranking[5] = { 234, 197, 120, 88, 36 };
int		score = 0;
int		time_limit = 321;

float	barX;
float	barY;

float	delta_x, delta_y;
float	fix_radius;

int		level = 6;				// 블럭 층 개수 (2, 4, 6)

bool	introScreen = true;		// 게임 시작 화면
bool	pauseScreen = false;	// 일시정지 화면
int		setBallAngle = 90.0;		// 시작공 좌우 방향 지정
bool	startBall = false;		// 시작공 작동 여부

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
	int		width = 50;
	int		height = 30;
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
	for (int j = 1; j <= level; j++) {
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
			else { brick[n].count = rand() % 7 + 1; }
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


void Draw_Brick(int idx) {
	float	x, y;
	float	r = 0.0, g = 0.0, b = 0.0;
	int		width, height, count;

	x = brick[idx].point.x;
	y = brick[idx].point.y;
	width = brick[idx].width;
	height = brick[idx].height;
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
	glVertex3f(x + width, y, 0.0);
	glVertex3f(x + width, y + height, 0.0);
	glVertex3f(x, y + height, 0.0);
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

	if (by <= allHeight / 2) {
		if (by - br < bgBottom && by < 0.0) {		// Bottom Face
			ball.velocity.y *= -1;
		}
		else if ((3.2 * (bx - br) - 34) < by && ball.velocity.x < 0.0) {		// 3 Face
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
			h = brick[0].height;
			w = brick[0].width;


			// 없는 벽돌이라면
			if (k.count > 0) {

				// 벽돌 하단 면
				if (x <= bx && bx <= (x + w) && y <= (by + br) && vy > 0.0) {
					//cout << "1";
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score++;
					return true;
				}

				// 벽돌 우하단 면
				if (sqrt(pow(x + w - bx, 2) + pow(y - by, 2)) < br) {
					ball[i].velocity.x *= -1;
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score++;
					return true;
				}
				/*if (x == (bx - br) && y == (by + br) && vx < 0.0 && vy > 0.0) {
					ball[i].velocity.x *= -1;
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score++;
					return true;
				}*/

				// 벽돌 우단 면
				if ((bx - br) <= (x + h) && y <= by && by <= (y + h) && vx < 0.0) {
					//cout << "2";
					ball[i].velocity.x *= -1;
					brick[j].count--;
					score++;
					return true;
				}

				// 벽돌 우상단 면
				if (sqrt(pow(x + w - bx, 2) + pow(y + h - by, 2)) < br) {
					ball[i].velocity.x *= -1;
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score++;
					return true;
				}
				/*if (x == (bx - br) && y == (by - br) && vx < 0.0 && vy < 0.0) {
					ball[i].velocity.x *= -1;
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score++;
					return true;
				}*/

				// 벽돌 상단 면
				if (x <= bx && bx <= (x + h) && y < (by - br) && (by - br) <= (y + h) && vy < 0.0) {
					//cout << "3";
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score++;
					return true;
				}

				// 벽돌 좌상단 면
				if (sqrt(pow(x - bx, 2) + pow(y + h - by, 2)) < br) {
					ball[i].velocity.x *= -1;
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score++;
					return true;
				}
				/*if (x == (bx + br) && y == (by - br) && vx > 0.0 && vy < 0.0) {
					ball[i].velocity.x *= -1;
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score++;
					return true;
				}*/

				// 벽돌 좌단 면
				if (x <= (bx + br) && y <= by && by <= (y + h) && vy > 0.0) {
					//cout << "4";
					ball[i].velocity.x *= -1;
					brick[j].count--;
					score++;
					return true;
				}

				// 벽돌 좌하단 면
				if (sqrt(pow(x - bx, 2) + pow(y - by, 2)) < br) {
					ball[i].velocity.x *= -1;
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score++;
					return true;
				}
				/*if (x == (bx + br) && y == (by + br) && vx > 0.0 && vy > 0.0) {
					ball[i].velocity.x *= -1;
					ball[i].velocity.y *= -1;
					brick[j].count--;
					score++;
					return true;
				}*/
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


//void SpecialKey(int key, int x, int y) {
//	switch (key) {
//	case GLUT_KEY_LEFT:		
//		barX -= 50;
//		break;
//	case GLUT_KEY_RIGHT:	
//		barX += 50;
//		break;
//	//case GLUT_KEY_DOWN:		moving_sphere.y -= 0.1;
//	//	break;
//	//case GLUT_KEY_UP:		moving_sphere.y += 0.1;
//	//	break;
//	default: break;
//	}
//	if (barX < 42)				barX = 42;					// backgroundLeft : 20
//	if (barX > 1058 - barW)		barX = 1058 - barW;			// backgroundRight: 1080		barWidth: 70
//}





bool collides(const Ball& ball, const Brick& block) {
	// Check if ball is within brick's bounds

	if (block.point.x <= ball.center.x + ball.radius && ball.center.x + ball.radius < block.point.x + block.width
		&& block.point.y <= ball.center.y + ball.radius && ball.center.y + ball.radius < block.point.y + block.height
		&& ball.velocity.x >= 0.0 && ball.velocity.y >= 0.0) {
		cout << ball.center.x << " " << ball.center.y << "\n";
		cout << block.point.x << " " << block.point.y << "\n";
		cout << "1 (+, +)" << "\n";
		return true;
	}
	if (block.point.x <= ball.center.x + ball.radius && ball.center.x + ball.radius < block.point.x + block.width
		&& block.point.y < ball.center.y - ball.radius && ball.center.y - ball.radius <= block.point.y + block.height
		&& ball.velocity.x >= 0.0 && ball.velocity.y < 0.0) {
		cout << ball.center.x << " " << ball.center.y << "\n";
		cout << block.point.x << " " << block.point.y << "\n";
		cout << "2 (+, -)" << "\n";
		return true;
	}
	if (block.point.x <= ball.center.x - ball.radius && ball.center.x - ball.radius < block.point.x + block.width
		&& block.point.y < ball.center.y + ball.radius <= block.point.y + block.height
		&& ball.velocity.x < 0.0 && ball.velocity.y >= 0.0) {
		cout << ball.center.x << " " << ball.center.y << "\n";
		cout << block.point.x << " " << block.point.y << "\n";
		cout << "3 (-, +)" << "\n";
		return true;
	}
	if (block.point.x < ball.center.x - ball.radius && ball.center.x - ball.radius <= block.point.x + block.width
		&& block.point.y < ball.center.y - ball.radius && ball.center.y - ball.radius <= block.point.y + block.height
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

	char str_1[] = "1";
	char str_2[] = "2";
	char str_3[] = "3";
	char str_4[] = "4";
	char str_5[] = "5";

	char str_Rank[] = "Rank";
	drawBitmapText(str_Rank, scoreX + scoreW / 3, 600);

	drawBitmapText(str_1, scoreX + scoreW / 4, 550);
	drawBitmapText(str_2, scoreX + scoreW / 4, 520);
	drawBitmapText(str_3, scoreX + scoreW / 4, 490);
	drawBitmapText(str_4, scoreX + scoreW / 4, 460);
	drawBitmapText(str_5, scoreX + scoreW / 4, 430);

	for (int i = 0; i < 5; i++) {
		int str_r = ranking[i];
		drawBitmapText(itoa(str_r, str_Rank, 10), scoreX + scoreW / 2, 550 - i * 30);
	}

	char str_Score[] = "Score";
	drawBitmapText(str_Score, scoreX + scoreW * 2 / 5, 350);
	drawBitmapText(itoa(score, str_Score, 10), scoreX + scoreW * 2 / 5, 300);


	char str_Time[] = "Time";
	char colon[] = ":";
	drawBitmapText(str_Time, scoreX + scoreW * 2 / 5, 200);
	drawBitmapText(itoa(time_limit / 60, str_Time, 10), scoreX + scoreW / 3, 150);
	drawBitmapText(colon, scoreX + scoreW / 2, 150);
	drawBitmapText(itoa(time_limit % 60, str_Time, 10), scoreX + scoreW * 2 / 3, 150);

}


bool Brick_Count(void) {
	for (int i = 0; i < brick_num; i++) {
		if (brick[i].count != 0) {
			return false;
		}
	}
	return true;
}


void End_Screen(bool success) {
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

	if (success) {
		char str[] = "Success";
		drawBitmapText(str, x + w / 3, y + h / 2);
	}
	else {
		char str[] = "Fail";
		drawBitmapText(str, x + w / 3, y + h / 2);
	}
}


// 시작 화면
void Intro_Screen(void) {
	
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
	glVertex2f(0.0, 0.0);
	glVertex2f(allWidth, 0.0);
	glVertex2f(allWidth, allHeight);
	glVertex2f(0.0, allHeight);
	glEnd();

	char str1[] = "Break Bricks";
	char str2[] = ">>> Press the f1 button to start the game <<<";
	drawBitmapText(str1, 430, 450);
	drawBitmapText(str2, 330, 150);


}

//double deg2rad(double degree) {	// 선배코드
//	return degree * PI / 180;
//}
//
//void draw_arrow() {		// 선배코드
//	double deg = deg2rad(415);
//	double deg2 = deg - arrow.getStartAngle();
//	double correct_deg = deg2rad(-40);
//	if (!arrow.is_on_game()) {
//		arrow.drawArr(ball[0]);
//	}
//	else if (arrow.is_started()) {
//		m_ball->adjVelo(gm.get_speed() * cos(deg2 - correct_deg), gm.get_speed() * sin(deg2 - correct_deg), false);
//		arrow.mov_flag_down();
//	}
//
//}


void Ball_Direction(void) {
	//float vx = setBallAngle;
	//float vy = 1.0;
	float x = ball[0].center.x;
	float y = ball[0].center.y;

	float vx = ball[0].velocity.x;
	float vy = ball[0].velocity.y;
	
	int dW = 5;
	int dH = 50;

	float a = PI/2 - tan(vy / -vx);

	glPushMatrix();

	glTranslatef(x, y, 0.0);				// 공 중심으로 좌표 이동
	glRotatef(-90, 0.0, 0.0, 1.0);
	glRotatef(setBallAngle, 0.0, 0.0, 1.0);
	glTranslatef(0.0, 20.0, 0.0);

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex3f(-dW, 0, 0.0);
	glVertex3f(dW, 0, 0.0);
	glVertex3f(dW, dH, 0.0);
	glVertex3f(-dW, dH, 0.0);
	glEnd();

	glPopMatrix();
}

void MyKey(unsigned char key, int x, int y) {

	switch (key) {
	case 'a':										// 막대바 움직이기
		if (barX > 42)				barX -= 50;		break;		// backgroundLeft : 20
	case 'd':	
		if (barX < 1058 - barW)		barX += 50;		break;		// backgroundRight: 1080	barWidth: 70

	case 'z':	introScreen = false;	break;		// 게임 화면으로 넘어가기

	case 'p':										// 일시정지 화면
		if (pauseScreen)	pauseScreen = false;
		else				pauseScreen = true;		break;

		if (!startBall) {
	case 'm':										// 시작공 좌우 방향 지정
		if (setBallAngle > 30)	setBallAngle -= 1;	cout << setBallAngle << "\n";	break;
		//if (setBallAngle > -1.5)	setBallAngle -= 1;	cout << setBallAngle << "\n";	break;
	case 'n':
		if (setBallAngle < 150)	setBallAngle += 1;	cout << setBallAngle << "\n";	break;
		//if (setBallAngle < 1.5)	setBallAngle += 1;	cout << setBallAngle << "\n";	break;
	case 'b':										// 공을 움직여 게임 시작하기
		startBall = true;
		//ball[0].velocity.y = 1.0;
		//ball[0].velocity.x = setBallAngle;
		//ball[0].velocity.x = cos(setBallAngle * PI / 180.0);
		//ball[0].velocity.y = sin(setBallAngle * PI / 180.0);
		ball[0].velocity.x = cos(setBallAngle * PI / 180.0);
		ball[0].velocity.y = sin(setBallAngle * PI / 180.0) * 0.3;
		cout << "b";
		break;
		}


	default:	break;
	}	

	glutPostRedisplay();
}


void RenderScene(void) {
	int	i;

	glClearColor(0.8, 0.8, 0.8, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (introScreen) {
		Intro_Screen();
	}
	else if (pauseScreen) {

	}
	else {

		
		

		glColor3f(0.0, 0.0, 0.0);

		if (startBall) {

			for (i = 0; i < ball_num; i++) {
				Check_Collision_Ball_Wall(ball[i]);
			}

			Check_Collision_Ball_Brick();

			for (i = 0; i < ball_num; i++) {
				Check_Collision_Ball_Bar(ball[i]);
			}

			Update_Position();

		}




		//for (i = 0; i < ball_num; i++) {
		//	for (int j = i + 1; j < ball_num; j++) {
		//		if (i != j) {
		//			//Check_Collision_Ball_Ball(i, j);
		//		}			
		//	}
		//}

		//for (i = 0; i < ball_num; i++) {
		//	for (int j = 0; j < brick_num; j++) {
		//		//Check_Collision_Ball_Block(i, j);
		//		if (collides(ball[i], brick[j])) {
		//			updateBallDirection(ball[i], brick[j]);
		//		}
		//	}
		//}

		// cout << "Ball Update" << "\n";



		Draw_Background();
		Draw_Score();


		Draw_Bar();

		for (i = 0; i < brick_num; i++)
			Draw_Brick(i);

		if (startBall == false) {
			Ball_Direction();
		}

		for (i = 0; i < ball_num; i++)
			Draw_Ball(i);
		
		

		


		if (Brick_Count())
			End_Screen(true);
		else if (Brick_Count() == false && time_limit < 0)
			End_Screen(false);
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
	init();
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(MyReshape);
	//glutSpecialFunc(SpecialKey);
	glutKeyboardFunc(MyKey);
	glutIdleFunc(RenderScene);
	glutMainLoop();
}
//*/
