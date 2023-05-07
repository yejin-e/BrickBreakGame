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
int		width = 700;
int		height = 700;
float	point_size = 3.0;
int		n = 0;

int		backLeft = 20;
int		backBottom = 30;
int		backRight = 480;
int		backTop = 670;

int		polygon_num = 36;		
int		ball_num = 1;			  
int		block_num = 30;			

int		collide = 0;
int		ranking[5] = { 234, 197, 120, 88, 36 };
int		score = 12;
int		time_limit = 321;


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

struct Check {
	Color	color;
	int		count;
};

struct Ball {
	Point	center;
	float	radius = 10.0;
	float	mass;		
	Color	color;
	Vector	velocity;	
};

struct Block {
	Point	point;
	int		width = 50;
	int		height = 30;
	Color	color;
	int		count;
	//Check	count;		
};

Ball*	ball;
Block*	block;
Check*	check;
Point*	polygon_vertex;

float	delta_x, delta_y;
float	fix_radius;


void init(void) {
	float	delta;
	float	distribution_delta;
	float	x, y;
	int		r, g, b;
	int		a = 0, i = 0, k = 0;

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
		ball[i].center.x = (backRight - backLeft) / 2.0;
		ball[i].center.y = (backTop - backBottom) / 4.0;
		ball[i].velocity.x = -0.0;
		ball[i].velocity.y = -0.0;					// 시작하기 버튼 클릭시 -1.0으로 변경
		ball[i].mass = 1.5 + sin((double)(i));
		ball[i].color.r = (float)(1.0 - (i % 3) / 2);
		ball[i].color.g = (float)(1.0 - (ball_num - i) / ball_num);
		ball[i].color.b = (float)(1.0 * (i % 10) / 9);
	}


	block = new Block[block_num];
	for (int j = 1; j <= 3; j++) {
		k = i;
		while (true) {
			y = backTop - block[0].height * 1.5 * j;
			//x = j * 50;
			x = (y - 750) / (-4) + 30.0 + (block[0].width + 10.0) * (i-k);
			cout << y << " " << x << "\n";
		
			if (((y + 1250) / 4) < x + block[0].width) { cout << "충돌\n"; break; }

			block[i].point.x = x;
			block[i].point.y = y;
			block[i].count = rand() % 4;
			switch (block[i].count)
			{
				case 0:		block[i].color.r = 255.0;	block[i].color.g = 255.0;	break;
				case 1:		block[i].color.r = 255.0;	break;
				case 2:		block[i].color.g = 255.0;	break;
				case 3:		block[i].color.b = 255.0;	break;
				default:	break;
			}
			i++;
		}	
	}
}


void Draw_Background() {
	
	//float r = 330.0;
	//float delta_theta = 2 * PI / 7;

	//glBegin(GL_POLYGON);
	//for (int i = 0; i < 7; i++) {
	//	glVertex2f(350 + r * cos(delta_theta * i), 350 + r * sin(delta_theta * i));
	//}
	//glEnd();

	glColor3f(255.0, 255.0, 255.0);
	glBegin(GL_POLYGON);
		glVertex2f(20.0,  30.0);
		glVertex2f(100.0, 350.0);
		glVertex2f(400.0, 350.0);
		glVertex2f(480.0, 30.0);
	glEnd();
	glBegin(GL_POLYGON);
		glVertex2f(30.0,  670.0);
		glVertex2f(100.0, 350.0);
		glVertex2f(400.0, 350.0);
		glVertex2f(480.0, 670.0);
	glEnd();

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
		glVertex2f(20.0, 30.0);
		glVertex2f(100.0, 350.0);	
		glVertex2f(30.0, 670.0);
		glVertex2f(480.0, 670.0);
		glVertex2f(400.0, 350.0);
		glVertex2f(480.0, 30.0);
	glEnd();

	glColor3f(255.0, 255.0, 255.0);
	glBegin(GL_POLYGON);
		glVertex2f(500.0, 20.0);
		glVertex2f(500.0, 680.0);
		glVertex2f(680.0, 680.0);
		glVertex2f(680.0, 20.0);
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


void Draw_Block(int indexBlock) {
	float	x, y, width, height;

	x = block[indexBlock].point.x;
	y = block[indexBlock].point.y;
	width = block[indexBlock].width;
	height = block[indexBlock].height;

	glColor3f(block[indexBlock].color.r, block[indexBlock].color.g, block[indexBlock].color.b);

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

// =====================================
// Function to check if a point is inside a heptagon
//bool pointInsideHeptagon(Point p, Wall w) {
//	// Check distance from center
//	float d = sqrt(pow(p.x - w.center.x, 2) + pow(p.y - w.center.y, 2));
//	if (d > w.radius) {
//		return false;
//	}
//	// Check angle from center
//	float theta = atan2(p.y - w.center.y, p.x - w.center.x);
//	if (theta < 0) {
//		theta += 2 * M_PI;
//	}
//	int index = round(theta / ANGLE);
//	float delta = index * ANGLE - theta;
//	if (delta > ANGLE / 2) {
//		delta -= ANGLE;
//	}
//	if (fabs(delta) <= M_PI / NUM_SIDES) {
//		return true;
//	}
//	return false;
//}
//
//// Function to check if a ball collides with a heptagonal wall
//bool ballCollidesWithHeptagon(Ball b, Wall w) {
//	Point p = b.position;
//	float r = b.radius;
//	if (pointInsideHeptagon(p, w)) {
//		return true;
//	}
//	Point v = b.velocity;
//	float A = pow(v.x, 2) + pow(v.y, 2);
//	float B = 2 * (p.x * v.x - w.center.x * v.x + p.y * v.y - w.center.y * v.y);
//	float C = pow(p.x, 2) - 2 * p.x * w.center.x + pow(w.center.x, 2) + pow(p.y, 2) - 2 * p.y * w.center.y + pow(w.center.y, 2) - pow(r + w.radius, 2);
//	float delta = pow(B, 2) - 4 * A * C;
//	if (delta < 0) {
//		return false;
//	}
//	float t1 = (-B + sqrt(delta)) / (2 * A);
//	float t2 = (-B - sqrt(delta)) / (2 * A);
//	if (t1 >= 0 || t2 >= 0) {
//		return true;
//	}
//	return false;
//}

// ========================================
void Check_Collision_Ball_Wall(Ball& ball) {
	float normX = 0, normY = 0;
	
	if (ball.center.y <= 350.0) {
		if (ball.center.y - ball.radius < 30.0 && ball.velocity.y < 0.0) {		// 아래 면
			ball.velocity.y *= -1;
		}
		else if ((4 * ball.center.x + 50) < ball.center.y && ball.velocity.x < 0.0) {	// 1 면
			ball.velocity.x *= -1;
		}
		else if (((-4) * ball.center.x + ball.radius + 1950) < ball.center.y && ball.velocity.x > 0.0) {	// 2 면
			ball.velocity.x *= -1;
		}
	}
	else if (ball.center.y > 350.0) {
		if (ball.center.y + ball.radius > 670.0 && ball.velocity.y > 0.0) {		// 위 면
			ball.velocity.y *= -1;
		}
		else if (((-4) * ball.center.x + 750) > ball.center.y && ball.velocity.x < 0.0) {	// 3 면
			ball.velocity.x *= -1;
		}
		else if ((4 * ball.center.x - 1250) > ball.center.y && ball.velocity.x > 0.0) {		// 4 면
			ball.velocity.x *= -1;
		}
	}
	

	//else if (ball.center.x + ball.radius > backRight && ball.velocity.x > 0.0) {
	//	normX = 0;
	//	normY = 0;
	//}
	//else if (ball.center.x - ball.radius < backLeft && ball.velocity.x < 0.0) {
	//	normX = 0;
	//	normY = 0;
	//}
	//else if (ball.center.y + ball.radius > backTop && ball.velocity.y > 0.0) {
	//	normX = 0;
	//	normY = 0;
	//}
	//else if (ball.center.y - ball.radius < backBottom && ball.velocity.y < 0.0) {
	//	normX = 0;
	//	normY = 0;
	//}


}


void Update_Position(void) {
	for (int i = 0; i < ball_num; i++) {
		ball[i].center.x += ball[i].velocity.x;
		ball[i].center.y += ball[i].velocity.y;
	}
}


void SpecialKey(int key, int x, int y) {
}


// Detect collision between ball and block
bool collides(const Ball& ball, const Block& block) {
	// Check if ball is within block's bounds

	/*if (block.point.x <= ball.center.x + ball.radius && ball.center.x + ball.radius < block.point.x + block.width
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
*/

	return false;
}

// Calculate angle of incidence and angle of reflection after collision with block
void updateBallDirection(Ball& ball, Block& block) {
	// Calculate the normal vector of the block's surface at the point of collision
	
	//float normX, normY;
	//if (ball.center.x < block.point.x) {
	//	normX = -1;
	//}
	//else if (ball.center.x > block.point.x + block.width) {
	//	normX = 1;
	//}
	//else {
	//	normX = 0;
	//}
	//if (ball.center.y < block.point.y) {
	//	normY = -1;
	//}
	//else if (ball.center.y > block.point.y + block.height) {
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
	//glColor3d(0, 0, 0);
	//glBegin(GL_POLYGON);
	//glVertex2f(width + 3, 0);
	//glVertex2f(width + gameWidth, 0);
	//glVertex2f(width + gameWidth, height);
	//glVertex2f(width, height);
	//glEnd();


	
	char str_1[] = "1";
	char str_2[] = "2";
	char str_3[] = "3";
	char str_4[] = "4";
	char str_5[] = "5";
	drawBitmapText(str_1, 520, 610);
	drawBitmapText(str_2, 520, 580);
	drawBitmapText(str_3, 520, 550);
	drawBitmapText(str_4, 520, 520);
	drawBitmapText(str_5, 520, 490);

	char str_Rank[] = "Rank";
	drawBitmapText(str_Rank, 570, 650);
	for (int i = 0; i < 5; i++) {
		int str_r = ranking[i];
		drawBitmapText(itoa(str_r, str_Rank, 10), 600, 610 - i * 30);
	}
	
	char str_Score[] = "Score";
	drawBitmapText(str_Score, 560, 400);
	drawBitmapText(itoa(score, str_Score, 10), 580, 350);


	char str_Time[] = "Time";
	char colon[] = ":";
	drawBitmapText(str_Time, 570, 250);
	drawBitmapText(itoa(time_limit / 60, str_Time, 10), 560, 200);
	drawBitmapText(colon, 590, 200);
	drawBitmapText(itoa(time_limit % 60, str_Time, 10), 610, 200);
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
	//		if (collides(ball[i], block[j])) {
	//			updateBallDirection(ball[i], block[j]);
	//		}
	//	}
	//}

	// cout << "Ball Update" << "\n";

	Update_Position();

	Draw_Background();

	for (i = 0; i < ball_num; i++)
		Draw_Ball(i);

	for (i = 0; i < block_num; i++)
		Draw_Block(i);

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