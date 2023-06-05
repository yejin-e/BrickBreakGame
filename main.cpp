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

using namespace std;


#define		PI			3.1415926
#define		epsilon		1.0

// Size of Clipping Window

float	radius = 5.0;
int		width = 1300; 
int		height = 700;
float	point_size = 3.0;
int		n = 0;

int		backLeft = 20;
int		backRight = 1080;
int		backBottom = 30;
int		backTop = 670;

int		polygon_num = 36;
int		ball_num = 1;
int		block_num = 100;

int		scoreX = 1100;
int		scoreW = 180;

int		collide = 0;
int		ranking[5] = { 234, 197, 120, 88, 36 };
int		score = 12;
int		time_limit = 321;

float	barX;
float	barY;
int		barW = 200;
int		barH = 15;

int		brickWidth = 50;
int		brickHeight = 30;

float	delta_x, delta_y;
float	fix_radius;

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
		ball[i].center.x = 230;
		ball[i].center.y = 110;
		ball[i].velocity.x = 1.0;
		ball[i].velocity.y = 0.5;
		ball[i].mass = 1;
		ball[i].color.r = ballColor[i][0];
		ball[i].color.g = ballColor[i][1];
		ball[i].color.b = ballColor[i][2];
	}

	int n = 0;
	float last = 0.0;
	brick = new Brick[100];
	for (int j = 1; j <= 2; j++) {
		a = 20 + rand() % 30;
		k = i;
		for (int i = 0; i < 20; i++) {

			y = backTop - 30 * 1.5 * j;
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

	block_num = n;

	// bar X Y position
	barX = 200;
	barY = 100;
}


void Draw_Background() {
	int size = 100;

	// Game Background 
	glColor3f(255.0, 255.0, 255.0);
	glBegin(GL_POLYGON);
	glVertex2f(backLeft, backBottom);
	glVertex2f(backLeft + size, height / 2);
	glVertex2f(backRight - size, height / 2);
	glVertex2f(backRight, backBottom);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex2f(backBottom, backTop);
	glVertex2f(backLeft + size, height / 2);
	glVertex2f(backRight - size, height / 2);
	glVertex2f(backRight, backTop);
	glEnd();

	// Game Background Line
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(backLeft, backBottom);
	glVertex2f(backLeft + size, height / 2);
	glVertex2f(backBottom, backTop);
	glVertex2f(backRight, backTop);
	glVertex2f(backRight - size, height / 2);
	glVertex2f(backRight, backBottom);
	glEnd();

	// Score Background
	glColor3f(255.0, 255.0, 255.0);
	glBegin(GL_POLYGON);
	glVertex2f(scoreX, backBottom);
	glVertex2f(scoreX, backTop);
	glVertex2f(scoreX + scoreW, backTop);
	glVertex2f(scoreX + scoreW, backBottom);
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
	gluOrtho2D(0, width, 0, height);
}


void Check_Collision_Ball_Wall(Ball& ball) {

	float bx = ball.center.x;
	float by = ball.center.y;
	float br = ball.radius;

	if (by <= height / 2) {
		if (by - br < backBottom && by < 0.0) {		// Bottom Face
			ball.velocity.y *= -1;
		}
		else if ((3.2 * (bx - br) - 34) < by && ball.velocity.x < 0.0) {		// 3 Face
			ball.velocity.x *= -1;
		}
		else if ((-3.2 * (bx + br) + 3486) < by && ball.velocity.x > 0.0) {		// 4 Face
			ball.velocity.x *= -1;
		}
	}
	else if (by > height / 2) {
		if (by + br > backTop && by > 0.0) {		// Top Face
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

		for (j = 0; j < block_num; j++) {
			Brick k = brick[j];
			x = k.point.x;		// 점 좌하단 기준 
			y = k.point.y;
			h = brick[0].height;
			w = brick[0].width;


			// 없는 벽돌이라면
			if (k.count == 0) {
				continue;
			}


			// 벽돌 하단 면
			if (x <= bx && bx <= (x + w) && y <= (by + br) && vy > 0.0) {
				//cout << "1";
				ball[i].velocity.y *= -1;
				brick[j].count--;
				if (k.count > 0)	return true;
			}

			// 벽돌 우하단 면
			if (x == (bx - br) && y == (by + br)) {
				ball[i].velocity.x *= -1;
				ball[i].velocity.y *= -1;
				brick[j].count--;
				if (k.count > 0)	return true;
			}

			// 벽돌 우단 면
			if ((bx - br) <= (x + h) && y <= by && by <= (y + h) && vx < 0.0) {
				//cout << "2";
				ball[i].velocity.x *= -1;
				brick[j].count--;
				if (k.count > 0)	return true;
			}

			// 벽돌 우상단 면
			if (x == (bx - br) && y == (by - br)) {
				ball[i].velocity.x *= -1;
				ball[i].velocity.y *= -1;
				brick[j].count--;
				if (k.count > 0)	return true;
			}

			// 벽돌 상단 면
			if (x <= bx && bx <= (x + h) && y < (by - br) && (by - br) <= (y + h) && vy < 0.0) {
				//cout << "3";
				ball[i].velocity.y *= -1;
				brick[j].count--;
				if (k.count > 0)	return true;
			}

			// 벽돌 좌상단 면
			if (x == (bx + br) && y == (by - br)) {
				ball[i].velocity.x *= -1;
				ball[i].velocity.y *= -1;
				brick[j].count--;
				if (k.count > 0)	return true;
			}

			// 벽돌 좌단 면
			if (x <= (bx + br) && y <= by && by <= (y + h) && vy > 0.0) {
				//cout << "4";
				ball[i].velocity.x *= -1;
				brick[j].count--;
				if (k.count > 0)	return true;
			}

			// 벽돌 좌하단 면
			if (x == (bx + br) && y == (by + br)) {
				ball[i].velocity.x *= -1;
				ball[i].velocity.y *= -1;
				brick[j].count--;
				if (k.count > 0)	return true;
			}

		}
	}
	return false;
}

void Check_Collision_Ball_Bar(Ball& ball) {
	float bx = ball.center.x;
	float by = ball.center.y;
	float br = ball.radius;

	if (barX < bx && bx < barX + barW && by + br < barY + barH) {
		ball.velocity.y *= -1;
	}

}


void Update_Position(void) {
	for (int i = 0; i < ball_num; i++) {
		ball[i].center.x += ball[i].velocity.x;
		ball[i].center.y += ball[i].velocity.y;
	}
}


void SpecialKey(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:		
		barX -= 50;
		break;
	case GLUT_KEY_RIGHT:	
		barX += 50;
		break;
	//case GLUT_KEY_DOWN:		moving_sphere.y -= 0.1;
	//	break;
	//case GLUT_KEY_UP:		moving_sphere.y += 0.1;
	//	break;
	default: break;
	}
	if (barX < 42)				barX = 42;					// backgroundLeft : 20
	if (barX > 1058 - barW)		barX = 1058 - barW;			// backgroundRight: 1080		barWidth: 70
}


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


void RenderScene(void) {
	int	i;

	glClearColor(0.8, 0.8, 0.8, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.0, 0.0, 0.0);


	for (i = 0; i < ball_num; i++) {
		Check_Collision_Ball_Wall(ball[i]);
	}


	//for (i = 0; i < ball_num; i++) {
	//	for (int j = i + 1; j < ball_num; j++) {
	//		if (i != j) {
	//			//Check_Collision_Ball_Ball(i, j);
	//		}			
	//	}
	//}

	//for (i = 0; i < ball_num; i++) {
	//	for (int j = 0; j < block_num; j++) {
	//		//Check_Collision_Ball_Block(i, j);
	//		if (collides(ball[i], brick[j])) {
	//			updateBallDirection(ball[i], brick[j]);
	//		}
	//	}
	//}

	// cout << "Ball Update" << "\n";

	Check_Collision_Ball_Brick();	
	for (i = 0; i < ball_num; i++)
		Check_Collision_Ball_Bar(ball[i]);
	Update_Position();

	Draw_Background();
	Draw_Bar();



	for (i = 0; i < block_num; i++)
		Draw_Brick(i);

	for (i = 0; i < ball_num; i++)
		Draw_Ball(i);

	

	Draw_Score();

	glFlush();
	glutSwapBuffers();
}


void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(width, height);
	glutCreateWindow("Project");
	init();
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(MyReshape);
	glutSpecialFunc(SpecialKey);
	glutIdleFunc(RenderScene);
	glutMainLoop();
}
//*/
