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
float fov = pi / 12;
float w = 5;
float h = 5;
float lose = 0;
int start = 0;

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
			
			if(((xx1 > -2 && xx1 < 2) || (xx2 > -2 && xx2 < 2))){
				glBegin(GL_QUADS);
				glColor3f(0.7 - lose * 0.7, 0, 1 - lose);
				glVertex2f(xx1, d1 / 2);
				glColor3f(0.7 - lose * 0.7, 0, 0.23 - lose * 0.23);
				glVertex2f(xx1, -d1 / 2);
				glColor3f(0.2 - lose * 0.2, 0, 1 - lose);
				glVertex2f(xx2, -d2 / 2);
				glColor3f(0.9 - lose * 0.9, 0, 1 - lose);
				glVertex2f(xx2, d2 / 2);
				glEnd();
			}
		}
	}
};

vector<Wall> walls;
Wall edge = Wall(-w, -h, w, h);

float check(float x1, float y1, float x2, float y2, float x3, float y3, float r){
	float x4 = x3 - sin(r);
	float y4 = y3 - cos(r);
	float den = (x1 - x2)*(y3-y4)-(y1-y2)*(x3-x4);
	if(den == 0){
		return 1000;
	}
	
	float t = ((x1-x3)*(y3-y4)-(y1-y3)*(x3-x4)) / den;
	float u = -((x1-x2)*(y1-y3)-(y1-y2)*(x1-x3)) / den;
	float xx = x1 + t * (x2 - x1);
	float yy = y1 + t * (y2 - y1);
	
	if((0 < t && t < 1 && 0 < u)){
		return sqrt(pow(x3 - xx, 2) + pow(y3 - yy, 2));
	}
	return 1000;
	
}

float castt(float x, float y, float r){
	float d = 1000;
	for(int i = 0; i < walls.size(); i++){
		for(int j = 0; j < 4; j++){
			float cd = check(
			 walls[i].v[(j+1) % 4][0],
			 walls[i].v[(j+1) % 4][1],
			 walls[i].v[j][0],
			 walls[i].v[j][1],
			 x, y, r
			);
			if(cd < d){
				d = cd;
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
	
	col(){
		if(sqrt(pow(x - xx, 2) + pow(y - yy, 2)) < 0.05){
			lose = 1;
		}
	}
	
	draw(){
		
		float w = 0.005;
		float a = atan2(xx-x, yy-y);
		bool m2 = true;
		
		float d = sqrt(pow(x - xx, 2) + pow(y - yy, 2));
		
		bool h = true;
		
		if(castt(xx, yy, a) <= d){
			m2 = false;
		}
		
		a = atan2(x - xx, y - yy);
		
		if(m2){
			
			a = atan2(x - xx, y - yy) - r;
			a = atan2(sin(a), cos(a));
			float xxx = 1 / fov * a;
			float s = 1 / d;
			
			glBegin(GL_QUADS);
			
			glVertex2f((xxx + s / 4), s / 2);
			glVertex2f((xxx + s / 4), -s / 2);
			glVertex2f((xxx - s / 4), -s / 2);
			glVertex2f((xxx - s / 4), s / 2);
			
			glEnd();
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
	
	draw(){
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

vector<Object> objects;
float ox = x;
float oy = y;
int shootFrames = 40;
vector<Particle> particles;
bool pause = true;
float fps = 60;

void run(HDC hDC){
	/* OpenGL animation code goes here */
    glClearColor (1.0f, 1.0f - lose, 1.0f - lose, 0.0f);
    glClear (GL_COLOR_BUFFER_BIT);
    
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
            
    glPushMatrix();
            
	glColor3f(0, 0, 0);
            
    glBegin(GL_QUADS);
            
    glVertex2f(-1, 0);
    glVertex2f(1, 0);
    glVertex2f(1, -1);
    glVertex2f(-1, -1);
            
    glEnd();
            
    glPopMatrix();
            
    glPushMatrix();
            
	glColor3f(0, 1 - lose, 1 - lose);
            
    glBegin(GL_QUADS);
            
    glVertex2f(-1, 0);
    glVertex2f(1, 0);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);
            
    glEnd();
            
    glPopMatrix();
            
    glPushMatrix();
            
    for(int i = 0; i < walls.size(); i++){
       	walls[i].draw();
	}
	
	//rayCast();
            
    glPopMatrix();
            
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
			
	if(GetAsyncKeyState(VK_SPACE) && shootFrames > fps * 2){
		shootFrames = 0;
		float vx = sin(r);
		float vy = cos(r);
		float xx = x;
		float yy = y;
		bool c = true;
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
			
	glPopMatrix();
            
    r += 0.000000625 * GetAsyncKeyState(VK_LEFT);
    r -= 0.000000625 * GetAsyncKeyState(VK_RIGHT);
    float v = 0.024;
    if(GetAsyncKeyState(0x57)){
       	x -= sin(r) * v;
       	y -= cos(r) * v;
	}
			
	if(GetAsyncKeyState(0x53)){
      	x += sin(r) * v;
      	y += cos(r) * v;
	}
			
	if(GetAsyncKeyState(0x44)){
      	x -= sin(r + pi / 2) * v;
       	y -= cos(r + pi / 2) * v;
	}
	if(GetAsyncKeyState(0x41)){
       	x += sin(r + pi / 2) * v;
       	y += cos(r + pi / 2) * v;
	}
			
	drawInt(0, 0.8, objects.size());
	if(shootFrames < fps * 2)	
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
			
		if(objects.size() == 0){
			for(int i = 0; i < level; i++){
    		objects.push_back(makeObject());
		}
		level++;
	}
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
    
    walls.push_back(Wall(-6,-6,-5,-5)); 
	walls.push_back(Wall(5,-6,6,-5)); 
	walls.push_back(Wall(-6,5,-4,6)); 
	walls.push_back(Wall(-5,-6,-4,-5)); 
	walls.push_back(Wall(-6,-5,-5,-4)); 
	walls.push_back(Wall(5,-5,6,-4)); 
	walls.push_back(Wall(-5,5,-3,6)); 
	walls.push_back(Wall(-4,-6,-3,-5)); 
	walls.push_back(Wall(-6,-4,-5,-3)); 
	walls.push_back(Wall(5,-4,6,-3)); 
	walls.push_back(Wall(-4,5,-2,6)); 
	walls.push_back(Wall(-3,-6,-2,-5)); 
	walls.push_back(Wall(-6,-3,-5,-2)); 
	walls.push_back(Wall(5,-3,6,-2)); 
	walls.push_back(Wall(-3,5,-1,6)); 
	walls.push_back(Wall(-2,-6,-1,-5)); 
	walls.push_back(Wall(-6,-2,-5,-1)); 
	walls.push_back(Wall(5,-2,6,-1)); 
	walls.push_back(Wall(-2,5,0,6)); 
	walls.push_back(Wall(-1,-6,0,-5)); 
	walls.push_back(Wall(-6,-1,-5,0)); 
	walls.push_back(Wall(5,-1,6,0)); 
	walls.push_back(Wall(-1,5,1,6)); 
	walls.push_back(Wall(0,-6,1,-5)); 
	walls.push_back(Wall(-6,0,-5,1)); 
	walls.push_back(Wall(5,0,6,1)); 
	walls.push_back(Wall(0,5,2,6)); 
	walls.push_back(Wall(1,-6,2,-5)); 
	walls.push_back(Wall(-6,1,-5,2)); 
	walls.push_back(Wall(5,1,6,2)); 
	walls.push_back(Wall(1,5,3,6)); 
	walls.push_back(Wall(2,-6,3,-5)); 
	walls.push_back(Wall(-6,2,-5,3)); 
	walls.push_back(Wall(5,2,6,3)); 
	walls.push_back(Wall(2,5,4,6)); 
	walls.push_back(Wall(3,-6,4,-5)); 
	walls.push_back(Wall(-6,3,-5,4)); 
	walls.push_back(Wall(5,3,6,4)); 
	walls.push_back(Wall(3,5,5,6)); 
	walls.push_back(Wall(4,-6,5,-5)); 
	walls.push_back(Wall(-6,4,-5,5)); 
	walls.push_back(Wall(5,4,6,5)); 
	walls.push_back(Wall(4,5,6,6)); 
	walls.push_back(Wall(-4,-3.9,-3.5,-3.4)); 
	walls.push_back(Wall(3.65,-3.9,4.15,-3.4)); 
	walls.push_back(Wall(3.65,2.2,4.15,2.7)); 
	walls.push_back(Wall(3.25,2.7,3.75,3.2)); 
	walls.push_back(Wall(2.9,2.7,3.4,3.2)); 
	walls.push_back(Wall(2.25,2.7,2.75,3.2)); 
	walls.push_back(Wall(2.5,2.7,3,3.2)); 
	walls.push_back(Wall(1.9,2.7,2.4,3.2)); 
	walls.push_back(Wall(1.45,2.7,1.95,3.2)); 
	walls.push_back(Wall(0.95,2.7,1.45,3.2)); 
	walls.push_back(Wall(0.5,2.7,1,3.2)); 
	walls.push_back(Wall(0.05,2.7,0.55,3.2)); 
	walls.push_back(Wall(-0.3,2.7,0.2,3.2)); 
	walls.push_back(Wall(-0.7,2.7,-0.2,3.2)); 
	walls.push_back(Wall(-1.15,2.7,-0.65,3.2)); 
	walls.push_back(Wall(-1.6,2.7,-1.1,3.2)); 
	walls.push_back(Wall(-2,2.7,-1.5,3.2)); 
	walls.push_back(Wall(-2.45,2.7,-1.95,3.2)); 
	walls.push_back(Wall(-2.95,2.7,-2.45,3.2)); 
	walls.push_back(Wall(-3.3,2.7,-2.8,3.2)); 
	walls.push_back(Wall(-3.75,2.7,-3.25,3.2)); 
	walls.push_back(Wall(-4.15,2.3,-3.65,2.8)); 
	
	
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
      0, 0, 800, 800,
      NULL, NULL, hInstance, NULL);

    /* enable OpenGL for the window */
    EnableOpenGL (hWnd, &hDC, &hRC);

    
    //variable declarations
    for(int i = 0; i < 5; i++){
    	objects.push_back(makeObject());
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
