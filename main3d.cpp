/**************************
 * Includes
 *
 **************************/

#include <windows.h>
#include <gl/gl.h>
#include <cmath>
#include <vector>
#include <stdlib.h>
#include <ctime>
#include <algorithm>

using namespace std;

int level = 6;


/**************************
 * Function Declarations
 *
 **************************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
WPARAM wParam, LPARAM lParam);
void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC);
void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC);

void line(float x1, float y1, float x2, float y2){
	glPushMatrix();
    glBegin(GL_LINE_LOOP);
    glColor3f(0, 0, 0);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
    glPopMatrix();
}
 
bool collide(float x, float y, float w, float h, float xx, float yy, float ww, float hh){
	return ((x >= xx && x <= xx + ww) || (x + w >= xx && x + w <= xx + ww) || (x <= xx && x + w >= xx) || (xx + ww >= x && xx + ww <= x + w)) && ((y >= yy && y <= yy + hh) || (y + h >= yy && y + h <= yy + hh) || (y >= yy && y + h <= yy) || (yy + hh >= y && yy + hh <= y + h));
}
 
 int z[10][7] = {
 	{1, 1, 1, 0, 1, 1, 1},
	{0, 1, 0, 0, 1, 0, 0},
	{1, 1, 0, 1, 0, 1, 1},
	{1, 0, 1, 1, 0, 1, 1},
	{0, 0, 1, 1, 1, 0, 1},
	{1, 0, 1, 1, 1, 1, 0},
	{1, 1, 1, 1, 1, 1, 0},
	{0, 0, 1, 0, 0, 1, 1},
	{1, 1, 1, 1, 1, 1, 1},
	{0, 0, 1, 1, 1, 1, 1}
};


void drawSevenSegment(float x, float y, int a){
	if(z[a][0] != 0){
		line(x, y, x + 0.05, y);
	}
	
	if(z[a][1] != 0){
		line(x, y, x, y + 0.05);
	}
	
	if(z[a][2] != 0){
		line(x + 0.05, y, x + 0.05, y + 0.05);
	}
	
	if(z[a][3] != 0){
		line(x, y + 0.05, x + 0.05, y + 0.05);
	}
	
	if(z[a][4] != 0){
		line(x, y + 0.05, x, y + 0.1);
	}
	
	if(z[a][5] != 0){
		line(x, y + 0.1, x + 0.05, y + 0.1);
	}
	
	if(z[a][6] != 0){
		line(x + 0.05, y + 0.05, x + 0.05, y + 0.1);
	}
}

void drawInt(float x, float y, unsigned int z){
	int c = log10(z);
	
	if(z == 0){
		drawSevenSegment(x, y, -0.0);
	} 
	
	for(int i = 0; i <= c; i ++){
		int a = int((z / pow(10, i))) % 10;
		drawSevenSegment(x + 0.08 * (c - i), y, a);
	}
} 


/**************************
 * WinMain
 *
 **************************/
 
float x = 0;
float y = 0;
float r = 0;
float pi = 3.141592653;
float fov = pi / 3;
float w = 5;
float h = 5;
float lose = 0;
int start = 0;
int score = 0;

class Wall{
	public:
	
	float x1;
	float x2;
	float y1;
	float y2;
	
	
	float v[4][2];
	Wall(float xx1, float yy1, float xx2, float yy2){
		x1 = xx1;
		y1 = yy1;
		x2 = xx2;
		y2 = yy2;
		float v[4][2] = {{x1, y1}, {x2, y1}, {x2, y2}, {x1, y2}};
	}
	
	col(float x, float y){
		return x >= x1 && x <= x2 && y >= y1 && y <= y2;
	}
	
	
	draw(){
		float v[4][2] = {{x1, y1}, {x2, y1}, {x2, y2}, {x1, y2}};
		float closest = sqrt(pow(v[0][0] - x, 2) + pow(v[0][1] - y, 2));
		int closestI = 0;
		
		for(int i = 0; i < 4; i++){
			if(sqrt(pow(v[i][0] - x, 2) + pow(v[i][1] - y, 2)) < closest){
				closest = sqrt(pow(v[i][0] - x, 2) + pow(v[i][1] - y, 2));
				closestI = i;
			}
		}
		
		for(int i = 0; i < 4; i++){
			float v1[2] = {0, 0};
			if(i < 2){
				v1[0] = v[(closestI + 2) % 2][0];
				v1[1] = v[(closestI + 2) % 2][1];
			}
			else{
				v1[0] = v[closestI][0];
				v1[1] = v[closestI][1];
			}
			int ii;
			if(i == 2){
				float d1 = sqrt(pow(x - v[(closestI + 1) % 4][0], 2) + pow(y - v[(closestI + 1) % 4][1], 2));
				float d2 = sqrt(pow(x - v[(closestI - 1) + 4 * ((closestI - 1) < 0)][0], 2) + pow(y - v[(closestI - 1) + 4 * ((closestI - 1) < 0)][1], 2));
				if(d1 > d2){
					ii = (closestI + 1) % 4;
				}
				else{
					ii = (closestI - 1) + 4 * ((closestI - 1) < 0);
				}
			}
			else if(i == 3){
				float d1 = sqrt(pow(x - v[(closestI + 1) % 4][0], 2) + pow(y - v[(closestI + 1) % 4][1], 2));
				float d2 = sqrt(pow(x - v[(closestI - 1) + 4 * ((closestI - 1) < 0)][0], 2) + pow(y - v[(closestI - 1) + 4 * ((closestI - 1) < 0)][1], 2));
				if(d1 < d2){
					ii = (closestI + 1) % 4;
				}
				else{
					ii = (closestI - 1) + 4 * ((closestI - 1) < 0);
				}
			}
			else if(i == 0){
				float d1 = sqrt(pow(x - v[(closestI + 2 + 1) % 4][0], 2) + pow(y - v[(closestI + 2 + 1) % 4][1], 2));
				float d2 = sqrt(pow(x - v[(closestI + 2 - 1) + 4 * ((closestI + 2 - 1) < 0)][0], 2) + pow(y - v[(closestI + 2 - 1) + 4 * ((closestI + 2 - 1) < 0)][1], 2));
				if(d1 < d2){
					ii = (closestI + 2 + 1) % 4;
				}
				else{
					ii = ((closestI + 2 - 1) + 4 * ((closestI - 1 + 2) < 0)) % 4;
				}
			}
			else if(i == 1){
				float d1 = sqrt(pow(x - v[(closestI + 2 + 1) % 4][0], 2) + pow(y - v[(closestI + 1 + 2) % 4][1], 2));
				float d2 = sqrt((pow(x - v[(closestI - 1 + 2) + 4 * ((closestI - 1 + 2) < 0) % 4][0], 2) + pow(y - v[(closestI - 1 + 2) + 4 * ((closestI - 1 + 2) < 0) % 4][1], 2)));
				if(d1 > d2){
					ii = (closestI + 2 + 1) % 4;
				}
				else{
					ii = (closestI + 2 - 1) + 4 * ((closestI - 1 + 2) < 0);
				}
			}
			if(ii < 0){
				ii = 3;
			}
			float *v2 = v[ii];
			
			float dv = 1 / tan(fov) * 2;
			
			
			float avx = v1[0] - x;
			float avy = v1[1] - y;
			float bvx = v2[0] - x;
			float bvy = v2[1] - y;
			float a1 = atan2(x - v1[0], y - v1[1]) - r;
			float a2 = atan2(x - v2[0], y - v2[1]) - r;
			float vx1 = avx * cos(r) - avy * sin(r);
			float vy1 = avx * sin(r) + avy * cos(r);
			float d1 = vy1;
			float h1 = dv / d1;
			float vx2 = bvx * cos(r) - bvy * sin(r);
			float vy2 = bvx * sin(r) + bvy * cos(r);
			float d2 = vy2;
			float h2 = dv / d2;
			float xx1 = vx1 * 1 / d1;
			float xx2 = vx2 * 1 / d2;
			
			if(vy1 < 0 && vy2 < 0){
				glBegin(GL_QUADS);
				
				glColor3f(1, 0.7, 0);
				glVertex2f(xx1, -h1 / 2);
				glVertex2f(xx2, -h2 / 2);
				glColor3f(0,  1 - lose, 1 - lose);
				glVertex2f(xx2, h2 / 2);
				glVertex2f(xx1, h1 / 2);
				glEnd();
			}
		}
	}
	
	public:
	
	getRightWall(){
		float rightest = -1;
		for(int i = 0; i < 4; i++){
			float *v1 = v[i];
			float *v2 = v[(i + 1) % 4];
			float d1 = 2 / sqrt(pow(x - v1[0], 2) + pow(y - v1[1], 2));
			float d2 = 2 / sqrt(pow(x - v2[0], 2) + pow(y - v2[1], 2));
			float a1 = atan2(x - v1[0], y - v1[1]) - r;
			a1 = atan2(sin(a1), cos(a1));
			float a2 = atan2(x - v2[0], y - v2[1]) - r;
			a2 = atan2(sin(a2), cos(a2));
			float xx1 = 1 / fov * a1;
			float xx2 = 1 / fov * a2;
			d1 *= cos(a1);
			d2 *= cos(rightest = xx1);
			if(xx1 > xx2 && xx1 > rightest){
				rightest = xx1;
			}
			else if(xx2 > xx1 && xx2 > rightest){
				rightest = xx2;
			}
		}
		return rightest;
	}
	
	public:
	getLeftWall(){
		float leftest = 1;
		for(int i = 0; i < 4; i++){
			float *v1 = v[i];
			float *v2 = v[(i + 1) % 4];
			float d1 = 2 / sqrt(pow(x - v1[0], 2) + pow(y - v1[1], 2));
			float d2 = 2 / sqrt(pow(x - v2[0], 2) + pow(y - v2[1], 2));
			float a1 = atan2(x - v1[0], y - v1[1]) - r;
			a1 = atan2(sin(a1), cos(a1));
			float a2 = atan2(x - v2[0], y - v2[1]) - r;
			a2 = atan2(sin(a2), cos(a2));
			float xx1 = 1 / fov * a1;
			float xx2 = 1 / fov * a2;
			d1 *= cos(a1);
			d2 *= cos(a2);
			if(xx1 < xx2 && xx1 < leftest){
				leftest = xx1;
			}
			else if(xx2 < xx1 && xx2 < leftest){
				leftest = xx2;
			}
		}
		return leftest;
	}
	
	public:
	float getDist(){
		float d;
		
		for(int i = 0; i < 4; i++){
			float dd = sqrt(pow(x - v[i][0], 2) + pow(y - v[i][1], 2));
			if(dd < d){
				d = dd;
			}
		}
		
		return d;
	}
};	

vector<Wall> walls;
Wall edge = Wall(-w, -h, w, h);

float castt(float x, float y, float r){
	float d = 1000;
	float xx = 1 / fov * r;
	for(int i = 0; i < walls.size(); i++){
		if(
			walls[i].getRightWall() >= xx
				&&
			walls[i].getLeftWall() <= xx
				&&
			walls[i].getDist() < d
		){
			if(d > walls[i].getDist()){
				d = walls[i].getDist();
			}
		}
	}
	
	return d;
}

class Object{
	public:
	float xx;
	float yy;
	int health;
	Object(float xxx, float yyy){
		xx = xxx;
		yy = yyy;
		health = (level - 5);
		health = 0;
	}
	
	float col(){
		if(sqrt(pow(x - xx, 2) + pow(y - yy, 2)) < 0.05){
			lose = 1;
		}
	}
	
	void draw(){
		
		float w = 0.005;
		float a = atan2(xx-x, yy-y);
		bool m2 = true;
		
		float d = sqrt(pow(x - xx, 2) + pow(y - yy, 2));
		
		bool h = true;
		
		//see if any of num points between player and object touch a wall
		int num = 50;
		
		for(int i = 0; i < num; i++){
			float xxx = x - (x - xx) / num * i;
			float yyy = y - (y - yy) / num * i;
			for(int i = 0; i < walls.size(); i++){
				if(walls[i].col(xxx, yyy)){
					m2 = false;
				}
			}
		}
		
		if(m2){
			
			float dv = 1 / tan(fov) * 2;
			
			float avx = xx - x;
			float avy = yy - y;
			float vx = avx * cos(r) - avy * sin(r);
			float vy = avx * sin(r) + avy * cos(r);
			float d = vy;
			float xxx = vx / d;
			float s = 1 / d;
			
			if(vy < 0){
				glBegin(GL_QUADS);
				
				glVertex2f((xxx + s / 4), s / 2);
				glVertex2f((xxx + s / 4), -s / 2);
				glVertex2f((xxx - s / 4), -s / 2);
				glVertex2f((xxx - s / 4), s / 2);
				
				glEnd();
			}
		}
		
		float ox = xx;
		float oy = yy;
		
		a = atan2(xx - x, yy - y);
		float v = 0.01 + (level / 200);
		xx -= sin(a) * v;
		yy -= cos(a) * v;
		for(int i = 0; i < walls.size(); i++){
			if(walls[i].col(xx, yy)){
				xx = ox;
				yy = oy;
			}
		}
	}
	
	getKilled(){
		float d = sqrt(pow(x - xx, 2) + pow(y - yy, 2));
		float a = atan2(x - xx, y - yy) - r;
		a = atan2(sin(a), cos(a));
		float xxx = 1 / fov * a;
		float s = 1 / d;
		
		if(abs(xxx) < s / 2){
			score++;
			return true;
		}
		
		return false;
	}
};

Object makeObject(){
	Object obj = Object(rand() % (int(w) * 2) - int(w), rand() % (int(h) * 2) - int(h));
	bool c = true;
	while(c){
		obj.xx = rand() % (int(w) * 2) - int(w);
		obj.yy = rand() % (int(h) * 2) - int(h);
		c = false;
		for(int i = 0; i < walls.size(); i++){
			if(walls[i].col(obj.xx, obj.yy)){
				c = true;
				obj.xx = rand() % (int(w) * 2) - int(w);
				obj.yy = rand() % (int(h) * 2) - int(h);
				break;
			}
		}
		
		if(sqrt(pow(x - obj.xx, 2) + pow(y - obj.yy, 2)) < 2){
			c = true;
		}
	}
	
	return obj;
}

int random(int x, int y){
	int z = y - x;
	return (rand() % z) + x;
}

class Particle{
	public:
	float x;
	float y;
	float vx;
	float vy;
	float s;
	Particle(float xx, float yy, float vxx, float vyy, float ss){
		x = xx;
		y = yy;
		vx = vxx;
		vy = vyy;
		s = ss;
	}
	
	void draw(){
		glBegin(GL_QUADS);
		
		glVertex2f(x - s / 2, y - s / 2);
		glVertex2f(x + s / 2, y - s / 2);
		glVertex2f(x + s / 2, y + s / 2);
		glVertex2f(x - s / 2, y + s / 2);
		
		glEnd();
		
		x += vx;
		y += vy;
		vy -= 0.01;
	}
};

//later global variables
vector<Object> objects;
float ox = x;
float oy = y;
int shootFrames = 40;
vector<Particle> particles;
bool pause = true;
float fps = 60;
bool spawned = false;
float v = 0.024;


void move(){
	//move forward if w or up arrow is pressed
    if(GetAsyncKeyState(0x57) | GetAsyncKeyState(VK_UP)){
       	x -= sin(r) * v;
       	y -= cos(r) * v;
	}
	
	//move backward if s or down arrow is pressed
	if(GetAsyncKeyState(0x53) || GetAsyncKeyState(VK_DOWN)){
      	x += sin(r) * v;
      	y += cos(r) * v;
	}
	
	//move right if d is pressed
	if(GetAsyncKeyState(0x44)){
      	x -= sin(r + pi / 2) * v;
       	y -= cos(r + pi / 2) * v;
	}
	
	//move left if s is pressed
	if(GetAsyncKeyState(0x41)){
       	x += sin(r + pi / 2) * v;
       	y += cos(r + pi / 2) * v;
	}
}

void stopWalkingThroughWalls(){
	for(int i = 0; i < walls.size(); i++){
		if(walls[i].col(x, y)){		
			x = ox;
			y = oy;
		}
	}
			
	if(abs(x) > w || abs(y) > h){
		x = ox;
		y = oy;
	}
}

void drawFloor(){
	glPushMatrix();
            
    glBegin(GL_QUADS);
            
    glColor3f(0, 0.6, 0);
    glVertex2f(-1, 0);
    glVertex2f(1, 0);
    glColor3f(0, 1, 0);
	glVertex2f(1, -1);
    glVertex2f(-1, -1);
            
    glEnd();
            
    glPopMatrix();
}

void drawRoof(){
	glPushMatrix();
            
    glBegin(GL_QUADS);
            
    
    glColor3f(0, 0, 1);
    glVertex2f(-1, 0);
    glVertex2f(1, 0);
    glColor3f(0, 1 - lose, 1 - lose);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);
            
    glEnd();
            
    glPopMatrix();
}

void drawWalls(){
	glPushMatrix();
            
    for(int i = 0; i < walls.size(); i++){
       	walls[i].draw();
	}
            
    glPopMatrix();
}

void shoot(){
	if(GetAsyncKeyState(VK_SPACE) && shootFrames > fps / 2){
		shootFrames = 0;
		float vx = sin(r);
		float vy = cos(r);
		float xx = x;
		float yy = y;
		/*bool c = true;
		while(c){
			if(abs(xx) > w || abs(yy) > h){
				c = false;
			}
					
			for(int i = 0; i < objects.size(); i++){
				if(sqrt(pow(xx - objects[i].xx, 2) + pow(yy - objects[i].yy, 2)) <= 0.4){
					objects.erase(objects.begin() + i);
					c = false;
				}
			}
				
			for(int i = 0; i < walls.size(); i++){
				if(walls[i].col(xx, yy)){
					c = false;
				}
			}
					
			xx -= vx * 0.005;
			yy -= vy * 0.005;
		}*/
		
		for(int i = 0; i < objects.size(); i++){
			if(objects[i].getKilled()){
				objects.erase(objects.begin() + i);
				break;
			}
		}
				
		for(int i = 0; i < 50; i++){
			float r = random(-pi * 50 / 3, pi * 50 / 3);
			particles.push_back(Particle(0, -0.8, sin(r) * 0.05, -cos(r) * 0.08, float(rand() % 1000) / 20000));
		}
			
		glBegin(3);
				
		glVertex2f(0, -1);
		glVertex2f(0, 0);
				
		glEnd();
	}
}

void spawn(){
	if(time(0) % 2 == 0){
		if(!spawned){
			objects.push_back(makeObject());
		}
			
		spawned = true;
	}
	else{
		spawned = false;
	}
}

void sort(){
	vector<Wall> nwalls = walls;
	for(int i = walls.size() - 1; i > 0; i--){
		if(abs(walls[i].x1 - x) + abs(walls[i].y1 - y) > abs(walls[i - 1].x1 - x) + abs(walls[i - 1].y1 - y)){
			Wall swapper = walls[i];
			walls[i] = walls[i - 1];
			walls[i - 1] = swapper;
		}
	}
}

void run(HDC hDC){
	/* OpenGL animation code goes here */
    glClearColor (1.0f, 1.0f - lose, 1.0f - lose, 0.0f);
    glClear (GL_COLOR_BUFFER_BIT);
    
    sort();
    stopWalkingThroughWalls();
    drawFloor();
    drawRoof();
    drawWalls();        
    
            
    glPushMatrix();
            
    glColor3f(1 - lose, 0, 0);
            
    ox = x;
    oy = y;
            
    for(int i = 0; i < objects.size(); i++){
       	objects[i].col();
       	objects[i].draw();
	}
			
	if(lose == 1 && GetAsyncKeyState(VK_RETURN)){
		lose = 0;
		particles = {};
		level = 6;
		objects = {};
	}
			
	shoot();
			
	glPopMatrix();
            
    r += 0.000000625 * GetAsyncKeyState(VK_LEFT);
    r -= 0.000000625 * GetAsyncKeyState(VK_RIGHT);
    move();
			
	drawInt(0, 0.8, score);
	if(shootFrames < fps / 2)	
		drawInt(0, 0.68, fps * 2 - shootFrames);
			
		glColor4f(1, 0.5, 0, 0);
			
		for(int i = 0; i < particles.size(); i++){
			particles[i].draw();
			if(particles[i].y < -1){
				particles.erase(particles.begin() + i);
			}
		}
			
		glColor4f(0.5, 0.5, 0.5, 0);
		glBegin(GL_QUADS);
			
		glVertex2f(0.2, -1);
		glVertex2f(0.29, -0.7);
		glVertex2f(-0.29, -0.7);
		glVertex2f(-0.2, -1);
			
			
		glEnd();
			
			
			
		for(int i = 0; i < objects.size(); i++){
			float xx = objects[i].xx;
			float yy = objects[i].yy;
			float rr = atan2(xx - x, yy - y) - r;
			float d = sqrt(pow(x - xx, 2) + pow(y - yy, 2));
				
			glBegin(GL_LINE_LOOP);
			glColor3f(1, 0, 0);
			for(float j = -0.5 / d; j < 0.5 / d; j += 0.005){
				 
					
				glVertex2f(-0.8 - sin(rr + j) * 0.05, -0.8 - cos(rr + j) * 0.05);                                    
					
			}
			glEnd();
		}
			
		//drawInt(0, -0.5, walls.size());
			
		SwapBuffers (hDC);
		float dfps = 120;
		Sleep(1000 / dfps * (fps / dfps));
		shootFrames++;
		
		spawn();
}

void theWalls(){
	walls.push_back(Wall(-5.5,-5.5,-5,-5)); 
	walls.push_back(Wall(5,-5.5,5.5,-5)); 
	walls.push_back(Wall(-5.5,5,-4.5,5.5)); 
	walls.push_back(Wall(-5,-5.5,-4.5,-5)); 
	walls.push_back(Wall(-5.5,-5,-5,-4.5)); 
	walls.push_back(Wall(5,-5,5.5,-4.5)); 
	walls.push_back(Wall(-5,5,-4,5.5)); 
	walls.push_back(Wall(-4.5,-5.5,-4,-5)); 
	walls.push_back(Wall(-5.5,-4.5,-5,-4)); 
	walls.push_back(Wall(5,-4.5,5.5,-4)); 
	walls.push_back(Wall(-4.5,5,-3.5,5.5)); 
	walls.push_back(Wall(-4,-5.5,-3.5,-5)); 
	walls.push_back(Wall(-5.5,-4,-5,-3.5)); 
	walls.push_back(Wall(5,-4,5.5,-3.5)); 
	walls.push_back(Wall(-4,5,-3,5.5)); 
	walls.push_back(Wall(-3.5,-5.5,-3,-5)); 
	walls.push_back(Wall(-5.5,-3.5,-5,-3)); 
	walls.push_back(Wall(5,-3.5,5.5,-3)); 
	walls.push_back(Wall(-3.5,5,-2.5,5.5)); 
	walls.push_back(Wall(-3,-5.5,-2.5,-5)); 
	walls.push_back(Wall(-5.5,-3,-5,-2.5)); 
	walls.push_back(Wall(5,-3,5.5,-2.5)); 
	walls.push_back(Wall(-3,5,-2,5.5)); 
	walls.push_back(Wall(-2.5,-5.5,-2,-5)); 
	walls.push_back(Wall(-5.5,-2.5,-5,-2)); 
	walls.push_back(Wall(5,-2.5,5.5,-2)); 
	walls.push_back(Wall(-2.5,5,-1.5,5.5)); 
	walls.push_back(Wall(-2,-5.5,-1.5,-5)); 
	walls.push_back(Wall(-5.5,-2,-5,-1.5)); 
	walls.push_back(Wall(5,-2,5.5,-1.5)); 
	walls.push_back(Wall(-2,5,-1,5.5)); 
	walls.push_back(Wall(-1.5,-5.5,-1,-5)); 
	walls.push_back(Wall(-5.5,-1.5,-5,-1)); 
	walls.push_back(Wall(5,-1.5,5.5,-1)); 
	walls.push_back(Wall(-1.5,5,-0.5,5.5)); 
	walls.push_back(Wall(-1,-5.5,-0.5,-5)); 
	walls.push_back(Wall(-5.5,-1,-5,-0.5)); 
	walls.push_back(Wall(5,-1,5.5,-0.5)); 
	walls.push_back(Wall(-1,5,0,5.5)); 
	walls.push_back(Wall(-0.5,-5.5,0,-5)); 
	walls.push_back(Wall(-5.5,-0.5,-5,0)); 
	walls.push_back(Wall(5,-0.5,5.5,0)); 
	walls.push_back(Wall(-0.5,5,0.5,5.5)); 
	walls.push_back(Wall(0,-5.5,0.5,-5)); 
	walls.push_back(Wall(-5.5,0,-5,0.5)); 
	walls.push_back(Wall(5,0,5.5,0.5)); 
	walls.push_back(Wall(0,5,1,5.5)); 
	walls.push_back(Wall(0.5,-5.5,1,-5)); 
	walls.push_back(Wall(-5.5,0.5,-5,1)); 
	walls.push_back(Wall(5,0.5,5.5,1)); 
	walls.push_back(Wall(0.5,5,1.5,5.5)); 
	walls.push_back(Wall(1,-5.5,1.5,-5)); 
	walls.push_back(Wall(-5.5,1,-5,1.5)); 
	walls.push_back(Wall(5,1,5.5,1.5)); 
	walls.push_back(Wall(1,5,2,5.5)); 
	walls.push_back(Wall(1.5,-5.5,2,-5)); 
	walls.push_back(Wall(-5.5,1.5,-5,2)); 
	walls.push_back(Wall(5,1.5,5.5,2)); 
	walls.push_back(Wall(1.5,5,2.5,5.5)); 
	walls.push_back(Wall(2,-5.5,2.5,-5)); 
	walls.push_back(Wall(-5.5,2,-5,2.5)); 
	walls.push_back(Wall(5,2,5.5,2.5)); 
	walls.push_back(Wall(2,5,3,5.5)); 
	walls.push_back(Wall(2.5,-5.5,3,-5)); 
	walls.push_back(Wall(-5.5,2.5,-5,3)); 
	walls.push_back(Wall(5,2.5,5.5,3)); 
	walls.push_back(Wall(2.5,5,3.5,5.5)); 
	walls.push_back(Wall(3,-5.5,3.5,-5)); 
	walls.push_back(Wall(-5.5,3,-5,3.5)); 
	walls.push_back(Wall(5,3,5.5,3.5)); 
	walls.push_back(Wall(3,5,4,5.5)); 
	walls.push_back(Wall(3.5,-5.5,4,-5)); 
	walls.push_back(Wall(-5.5,3.5,-5,4)); 
	walls.push_back(Wall(5,3.5,5.5,4)); 
	walls.push_back(Wall(3.5,5,4.5,5.5)); 
	walls.push_back(Wall(4,-5.5,4.5,-5)); 
	walls.push_back(Wall(-5.5,4,-5,4.5)); 
	walls.push_back(Wall(5,4,5.5,4.5)); 
	walls.push_back(Wall(4,5,5,5.5)); 
	walls.push_back(Wall(4.5,-5.5,5,-5)); 
	walls.push_back(Wall(-5.5,4.5,-5,5)); 
	walls.push_back(Wall(5,4.5,5.5,5)); 
	walls.push_back(Wall(4.5,5,5.5,5.5)); 
	walls.push_back(Wall(-3.65,-3.8,-3.15,-3.3)); 
	walls.push_back(Wall(-0.7,-3.8,-0.2,-3.3)); 
	walls.push_back(Wall(-1.65,-2,-1.15,-1.5)); 
	walls.push_back(Wall(-3.2,-0.35,-2.7,0.15)); 
	walls.push_back(Wall(-1.65,1.2,-1.15,1.7)); 
	walls.push_back(Wall(-0.1,0.3,0.4,0.8)); 
	walls.push_back(Wall(1.1,-0.9,1.6,-0.4)); 
	walls.push_back(Wall(0.6,-1.95,1.1,-1.45)); 
	walls.push_back(Wall(1.5,-2.55,2,-2.05)); 
	walls.push_back(Wall(2.75,-1.3,3.25,-0.8)); 
	walls.push_back(Wall(1.95,0.9,2.45,1.4)); 
	walls.push_back(Wall(0.85,2.2,1.35,2.7)); 
	walls.push_back(Wall(-0.75,3.15,-0.25,3.65)); 
	walls.push_back(Wall(-2.2,2.3,-1.7,2.8)); 
	walls.push_back(Wall(-3.4,2.85,-2.9,3.35)); 
	walls.push_back(Wall(-3.3,1.5,-2.8,2)); 
	walls.push_back(Wall(-4,-1.85,-3.5,-1.35)); 
	walls.push_back(Wall(-3.5,-1.35,-3,-0.85)); 
	walls.push_back(Wall(-3.5,-2.3,-3,-1.8)); 
	walls.push_back(Wall(-3.5,-2.7,-3,-2.2)); 
	walls.push_back(Wall(-2.5,-3.1,-2,-2.6)); 
	walls.push_back(Wall(-1.35,-3.9,-0.85,-3.4)); 
}

int WINAPI WinMain (HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int iCmdShow)
{
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;        
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "GLSample";
    RegisterClass (&wc);
    
    srand(time(0));
    float frames = 0;
    int tim = time(0);
    int fps = 120;
    
    theWalls();
	
	
	for(int i = 0; i < walls.size(); i++){
		if((abs(walls[i].x1) <= w && abs(walls[i].x2) <= w) && (abs(walls[i].y1) <= h && abs(walls[i].y2) <= h)){
			start = i;
			break;
		}
	}

    /* create main window */
    hWnd = CreateWindow (
      "GLSample", "OpenGL Sample", 
      WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
      0, 0, 512, 512,
      NULL, NULL, hInstance, NULL);

    /* enable OpenGL for the window */
    EnableOpenGL (hWnd, &hDC, &hRC);

    
    //variable declarations
    for(int i = 0; i < 5; i++){
    	//objects.push_back(makeObject());
	}

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
            }
        }
        else
        {
            run(hDC);
        }
    }

    /* shutdown OpenGL */
    
    DisableOpenGL (hWnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow (hWnd);

    return msg.wParam;
}


/********************
 * Window Procedure
 *
 ********************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
                          WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        PostQuitMessage (0);
        return 0;

    case WM_DESTROY:
        return 0;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
        }
        return 0;

    default:
        return DefWindowProc (hWnd, message, wParam, lParam);
    }
}


/*******************
 * Enable OpenGL
 *
 *******************/

void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC (hWnd);

    /* set the pixel format for the DC */
    ZeroMemory (&pfd, sizeof (pfd));
    pfd.nSize = sizeof (pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | 
      PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat (*hDC, &pfd);
    SetPixelFormat (*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext( *hDC );
    wglMakeCurrent( *hDC, *hRC );

}


/******************
 * Disable OpenGL
 *
 ******************/

void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext (hRC);
    ReleaseDC (hWnd, hDC);
}
