// Calculate and display the Mandelbrot set
// ECE6122 final project, Spring 2017
// Pranjali Borkar
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "complex.h"
#include <vector>
#define WINDOW_SIZE 512

using namespace std;

Complex  minC(-2.0, -1.2);
Complex  maxC( 1.0, 1.8);
int      Iter = 2000;     
int      nThreads = 16;    
int      P = nThreads + 1;
pthread_mutex_t MBMutex;
bool* localSense;
bool globalSense;
int elementCount = 0;
int a;

struct Point
{
	float x,y;
	Point()
	{
		x = 0.0;
		y = 0.0;
	}
};
Point A,B;
float dx,dy,d;

Complex* c = new Complex[WINDOW_SIZE * WINDOW_SIZE];
int pixels[WINDOW_SIZE * WINDOW_SIZE];
bool mouse_flag;



Complex complexNumber(int i, int j)
{
	double diff_real = maxC.real - minC.real;
	double diff_imag = maxC.imag - minC.imag;
	double Real = (double) i/WINDOW_SIZE;
	double Imag = (double) j/WINDOW_SIZE;
	return minC + Complex(Real*diff_real,Imag*diff_imag);
}

void CalculateC()
{
	for (int i = 0; i < 512; i++)
	{
		for (int j = 0; j < 512; j++)
		{
			c[i*WINDOW_SIZE + j] = complexNumber(i,j);
		}
	}
}
class PointsVector                                                                                    
{
	public:
		float minC_real, minC_imag, maxC_real, maxC_imag;
		PointsVector(float a, float b, float c, float d)
		{
			minC_real = a;
			minC_imag = b;
			maxC_real = c;
			maxC_imag = d;
		}
};
vector <PointsVector> mv;  

class RGB
{
public:
  RGB()
    : r(0), g(0), b(0) {}
  RGB(double r0, double g0, double b0)
    : r(r0), g(g0), b(b0) {}
public:
  double r;
  double g;
  double b;
};

RGB* colors = 0; 

void color_init()
{
  colors = new RGB[Iter + 1];
  for (int i = 0; i < Iter; ++i)
    {
      if (i < 5)
        { 
          colors[i] = RGB(1, 1, 1);
        }
      else
        {
          colors[i] = RGB(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
        }
    }
  colors[Iter] = RGB(); 
}

void MyBarrier_Init()
{	
	elementCount = P;
	pthread_mutex_init(&MBMutex,0);
	localSense = new bool[P];
	for (int i = 0; i < P; i++)
	{
		localSense[i] = true;
	}
	globalSense = true;
}


void MyBarrier(int myId) 
{
	localSense[myId] = !localSense[myId];
	pthread_mutex_lock(&MBMutex);
	int myCount = elementCount;
	elementCount--;
	pthread_mutex_unlock(&MBMutex);
	if (myCount == 1)
	{
		elementCount = P;
		globalSense = localSense[myId];
	}
	
	else
	{
		while (globalSense != localSense[myId])
		{
			
		}
	}
}

void* thread_draw(void* v)
{
	unsigned long myId = (unsigned long) v;
	unsigned long localCount = 0;
	int rowsPerThread = WINDOW_SIZE/nThreads;
	int startingRow = myId * rowsPerThread;
	for (int i = 0; i < rowsPerThread; i++)
	{
		int thisRow = startingRow + i;
		for (int j = 0; j < WINDOW_SIZE; j++)
		{
			
			Complex z = c[thisRow*WINDOW_SIZE + j];
			
			pixels[thisRow*WINDOW_SIZE + j] = 0;
			localCount++;
			
			while(pixels[thisRow*WINDOW_SIZE+j] < Iter && z.Mag2() < 4.0)
			{
				pixels[thisRow*WINDOW_SIZE + j]++;
				z = (z*z) + c[thisRow*WINDOW_SIZE + j];
			}	
		}
	}
}



void CreateThread()
{ 
  MyBarrier_Init();   
  for (int i = 0; i < nThreads; ++i) 
  {
	  pthread_t pt;
	  pthread_create(&pt,0,thread_draw,(void*)i);
  }
}


void drawMBSet()
{	
	CalculateC();
	CreateThread();
		
}


void displayMBSet()
{
	glBegin(GL_POINTS);
	for (int i = 0; i < WINDOW_SIZE; i++)
	{
		for (int j = 0; j < WINDOW_SIZE; j++)
		{
			glColor3f(colors[pixels[i*WINDOW_SIZE + j]].r,colors[pixels[i*WINDOW_SIZE + j]].g,colors[pixels[i*WINDOW_SIZE + j]].b);
			glVertex2d(i,j);
		}
	}
	glEnd();
}


void Square()
{
	glColor3f(1,0,0);
	glBegin(GL_LINE_LOOP);
	glLineWidth((GLfloat)3);
	glVertex2f(A.x,A.y);
	glVertex2f(B.x,A.y);
	glVertex2f(B.x,B.y);
	glVertex2f(A.x,B.y);
	glEnd();
	glutSwapBuffers();
} 
void mouse(int button, int state, int x, int y)
{ // Your mouse click processing here
  // state == 0 means pressed, state != 0 means released
  // Note that the x and y coordinates passed in are in
  // PIXELS, with y = 0 at the top.
   if(state==0 && button == GLUT_LEFT_BUTTON )
   {
	  A.x = x;
	  B.x = x;
	  A.y = y;
	  B.y = y;
	  mouse_flag = 1;
	  
   }
    
	if(state!=0 && button == GLUT_LEFT_BUTTON )
	{	mv.push_back(PointsVector(minC.real,minC.imag,maxC.real,maxC.imag));
		if (x > A.x && y > A.y)
	  {
		  B.x = A.x + d;
		  B.y = A.y + d;
		  
		  for (int i = 0; i < WINDOW_SIZE; i++)
		  {
			  for (int j = 0; j < WINDOW_SIZE; j++)
			  {
				  if (i == A.x && j == A.y)
				  {
					  minC.real = c[i*WINDOW_SIZE + j].real;
					  minC.imag = c[i*WINDOW_SIZE + j].imag;
				  }
				  
				  if (i == B.x && j == B.y)
				  {
					  maxC.real = c[i*WINDOW_SIZE + j].real;
					  maxC.imag = c[i*WINDOW_SIZE + j].imag; 
				  }
			  }
		  }
	  }
	  
	  if (x < A.x && y < A.y)
	  {
		  B.x = A.x - d;
		  B.y = A.y - d;
		  for (int i = 0; i < WINDOW_SIZE; i++)
		  {
			  for (int j = 0; j < WINDOW_SIZE; j++)
			  {
				  
				  
				  if (i == A.x && j == A.y)
				  {
					  maxC.real = c[i*WINDOW_SIZE + j].real;
					  maxC.imag = c[i*WINDOW_SIZE + j].imag; 
				  }
				  if (i == B.x && j == B.y)
				  {
					  minC.real = c[i*WINDOW_SIZE + j].real;
					  minC.imag = c[i*WINDOW_SIZE + j].imag;
				  }
			  }
		  }		  
	  }

	  if (x < A.x && y > A.y)
	  {
		  B.x = A.x - d;
		  B.y = A.y + d;
		  for (int i = 0; i < WINDOW_SIZE; i++)
		  {
			  for (int j = 0; j < WINDOW_SIZE; j++)
			  {
				  
				  
				  if (i == A.x && j == B.y)
				  {
					  maxC.real = c[i*WINDOW_SIZE + j].real;
					  maxC.imag = c[i*WINDOW_SIZE + j].imag; 
				  }
				  if (i == B.x && j == A.y)
				  {
					  minC.real = c[i*WINDOW_SIZE + j].real;
					  minC.imag = c[i*WINDOW_SIZE + j].imag;
				  }
			  }
		  }		  
	  }

	  if (x > A.x && y < A.y)
	  {
		  B.x = A.x + d;
		  B.y = A.y - d;
		  for (int i = 0; i < WINDOW_SIZE; i++)
		  {
			  for (int j = 0; j < WINDOW_SIZE; j++)
			  {
				  
				  
				  if (i == B.x && j == A.y)
				  {
					  maxC.real = c[i*WINDOW_SIZE + j].real;
					  maxC.imag = c[i*WINDOW_SIZE + j].imag; 
				  }
				  if (i == A.x && j == B.y)
				  {
					  minC.real = c[i*WINDOW_SIZE + j].real;
					  minC.imag = c[i*WINDOW_SIZE + j].imag;
				  }
			  }
		  }		  
	  }
	  mouse_flag = 0;
	  Square();
	  drawMBSet();
	  glutPostRedisplay();
  } 
}
void display(void)
{ // Your OpenGL display code here
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(1.0,1.0,1.0,1.0);
  glLoadIdentity();
  displayMBSet();  
  glutSwapBuffers();
}   
void motion(int x, int y)                                                                     
{
	dx = abs(x - A.x);
	dy = abs(y - A.y);
	

	if(x < A.x && y < A.y)
	{
		if(dx > dy)
		{
			d = dy;
		}
		if(dx < dy)
		{
			d = dx;
		}
		B.x = A.x - d;  
		B.y = A.y - d;	
	}
	if(x > A.x && y > A.y)
	{
		if(dx > dy)
		{
			d = dy;
		}
	
		if(dx < dy)
		{
			d = dx;
		}
		B.x = A.x + d;  
		B.y = A.y + d;
	}

	if(x > A.x && y < A.y)
	{
		if(dx > dy)
		{
			d = dy;
		}
		if(dx < dy)
		{
			d = dx;
		}
		B.x = A.x + d;  
		B.y = A.y - d;
	}

	if(x < A.x && y > A.y)
	{
		if(dx > dy)
		{
			d = dy;
		}
	
		if(dx < dy)
		{
			d = dx;
		}
		B.x = A.x - d;  
		B.y = A.y + d;
	}
	glutPostRedisplay();
}

void keyboard(unsigned char c, int x, int y)
{ // Your keyboard processing here
	if (c == 'b' || c == 'B')
	{
		if(mv.size() > 0)
		{
			PointsVector Z = mv.back();
			mv.pop_back();
			minC.real = Z.minC_real;
			minC.imag = Z.minC_imag;
			maxC.real = Z.maxC_real;
			maxC.imag = Z.maxC_imag;
			drawMBSet();
			glutPostRedisplay();
		}
	}
}
	

void init()
{ // Your OpenGL initialization code here
	color_init();
	glViewport(0,0,WINDOW_SIZE,WINDOW_SIZE);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,WINDOW_SIZE,WINDOW_SIZE,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
} 

int main(int argc, char** argv)
{
  // Initialize OpenGL, but only on the "master" thread or process.
  // See the assignment writeup to determine which is "master" 
  // and which is slave.
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(WINDOW_SIZE,WINDOW_SIZE);
  glutInitWindowPosition(400,200);
  glutCreateWindow("Mandelbrot Set ");
  
  
  drawMBSet();
  init();
  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutMainLoop();
  return 0;
  
}

