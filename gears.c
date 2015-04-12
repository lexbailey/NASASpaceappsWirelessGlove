#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include "gears.h"

#define SENSITIVITY 360.0f
#define ZOOMMULTIPLIER 1.0f

#include <sys/time.h>
long int getTimeMS(){
	struct timeval t;
	gettimeofday(&t, NULL);
	return (t.tv_sec * 1000)+(t.tv_usec / 1000);
}

//#include "model.h"

float angle1s = 0;
float angle1m = 0;
float angle1h = 0;

int width = 800;
int height = 600;

long int lastRender;
long int firstRender;

float randRange(float min, float max){
	float range = max - min;
	return min + rand()/(float)RAND_MAX * range;
}

void drawGear(int teeth, float radI, float radO, float depth){
	float toothAngle = 360.0f/ teeth;

	//get length of one side of a regular polygon with 'teeth' sides
	float sidelen = 2.0f * radO * tan(3.14159 / teeth);
	int t;
	glPushMatrix();

		for (t = 0; t<teeth; t++){
			glRotatef(toothAngle, 0.0f, 0.0f, 1.0f); 
			glBegin(GL_TRIANGLE_FAN);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glVertex3f(0.0f, 0.0f, depth/2.0f);
			glVertex3f(-sidelen/2.0f, radO, depth/2.0f);
			glVertex3f(-sidelen/4.0f, radO, depth/2.0f);
			glVertex3f(-sidelen/5.0f, radI, depth/2.0f);
			glVertex3f(+sidelen/5.0f, radI, depth/2.0f);
			glVertex3f(+sidelen/4.0f, radO, depth/2.0f);
			glVertex3f(+sidelen/2.0f, radO, depth/2.0f);
			
			glEnd();
		}
	glPopMatrix();
	glPushMatrix();

		for (t = 0; t<teeth; t++){
			glRotatef(toothAngle, 0.0f, 0.0f, 1.0f); 
			glBegin(GL_TRIANGLE_FAN);
			glNormal3f(0.0f, 0.0f, -1.0f);
			glVertex3f(0.0f, 0.0f, -depth/2.0f);
			glVertex3f(-sidelen/2.0f, radO, -depth/2.0f);
			glVertex3f(-sidelen/4.0f, radO, -depth/2.0f);
			glVertex3f(-sidelen/5.0f, radI, -depth/2.0f);
			glVertex3f(+sidelen/5.0f, radI, -depth/2.0f);
			glVertex3f(+sidelen/4.0f, radO, -depth/2.0f);
			glVertex3f(+sidelen/2.0f, radO, -depth/2.0f);
			glEnd();
		}
	glPopMatrix();
	glPushMatrix();

		for (t = 0; t<teeth; t++){
			glRotatef(toothAngle, 0.0f, 0.0f, 1.0f); 
			glBegin(GL_QUADS);

			glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(-sidelen/2.0f, radO, depth/2.0f);
			glVertex3f(-sidelen/4.0f, radO, depth/2.0f);
			glVertex3f(-sidelen/4.0f, radO, -depth/2.0f);
			glVertex3f(-sidelen/2.0f, radO, -depth/2.0f);

			float dx = (-sidelen/5.0f) - (-sidelen/4.0f);
			float dy = (radI) - (radO);
			float dxnorm = dy;
			float dynorm = -dx;
			glEnable(GL_NORMALIZE);
			glNormal3f(dxnorm, dynorm, 0.0f);
			glVertex3f(-sidelen/4.0f, radO, depth/2.0f);
			glVertex3f(-sidelen/5.0f, radI, depth/2.0f);
			glVertex3f(-sidelen/5.0f, radI, -depth/2.0f);
			glVertex3f(-sidelen/4.0f, radO, -depth/2.0f);
			glDisable(GL_NORMALIZE);

			glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(-sidelen/5.0f, radI, depth/2.0f);
			glVertex3f(+sidelen/5.0f, radI, depth/2.0f);
			glVertex3f(+sidelen/5.0f, radI, -depth/2.0f);
			glVertex3f(-sidelen/5.0f, radI, -depth/2.0f);

			glEnable(GL_NORMALIZE);
			glNormal3f(-dxnorm, dynorm, 0.0f);
			glVertex3f(+sidelen/5.0f, radI, depth/2.0f);
			glVertex3f(+sidelen/4.0f, radO, depth/2.0f);
			glVertex3f(+sidelen/4.0f, radO, -depth/2.0f);
			glVertex3f(+sidelen/5.0f, radI, -depth/2.0f);
			glDisable(GL_NORMALIZE);

			glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(+sidelen/4.0f, radO, depth/2.0f);
			glVertex3f(+sidelen/2.0f, radO, depth/2.0f);
			glVertex3f(+sidelen/2.0f, radO, -depth/2.0f);
			glVertex3f(+sidelen/4.0f, radO, -depth/2.0f);

			glEnd();
		}
	glPopMatrix();
}

loopfunc otherloop;

float zoom;

float rotx, roty, rotz;
float panx, pany, panz;

GLfloat difLight[] = {0.8f,0.8f,0.8f,1.0f};
GLfloat difLightPos[] = {4.8f,0.0f,0.0f};
GLfloat ambLight[] = {0.1f,0.1f,0.1f,1.0f};
GLfloat ambLightPos[] = {4.8f,5.0f,0.0f};
GLfloat speLight[] = {0.2f,0.2f,0.2f,1.0f};
GLfloat speLightPos[] = {-4.8f,0.0f,0.0f};

void glLights(){
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, difLight);
	glLightfv(GL_LIGHT0, GL_POSITION, difLightPos);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambLight);
	glLightfv(GL_LIGHT1, GL_POSITION, ambLightPos);

	glLightfv(GL_LIGHT2, GL_SPECULAR, speLight);
	glLightfv(GL_LIGHT2, GL_POSITION, speLightPos);
}

void render(){

	float time_delta;
	long int thisRender = getTimeMS();
	time_delta = (thisRender - lastRender) /1000.0f;
	printf("FPS: %.1f      \r", 1.0f/time_delta); fflush(stdout);
	time_delta *= 6.0f;
	lastRender = thisRender;

	angle1s += (8.0f * time_delta);

	glLoadIdentity();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glTranslatef(0.0f, -1.0f, -10.0f);

	glTranslatef(panx, pany, panz);

	glRotatef(30.0f, 0.0f, 0.0f, 1.0f);

	glScalef(zoom, zoom, zoom);

	glRotatef(rotx * SENSITIVITY, 1.0f, 0.0f, 0.0f);
	glRotatef(roty * SENSITIVITY, 0.0f, 1.0f, 0.0f);
	glRotatef(rotz * SENSITIVITY, 0.0f, 0.0f, 1.0f);

	glPushMatrix();
	glTranslatef(0.0f,0.0f,0.0f);
	glRotatef(angle1s, 0.0f, 0.0f, 1.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	drawGear(20, 3.0f, 3.2f, 1.0f);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0.0f,4.7f,0.0f);
	glRotatef(-2.0f * angle1s + (360.0f/20.0f), 0.0f, 0.0f, 1.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	drawGear(10, 1.4f, 1.6f, 1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(4.0f,0.0f,0.0f);
	glRotatef(-4.0f * angle1s + (360.0f/7.0f), 0.0f, 0.0f, 1.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	drawGear(5, 0.6f, 0.8f, 1.0f);
	glPopMatrix();

	glutSwapBuffers();


	//setRotation(x, y, z);
	//setZoom(1.0f + ((thisRender-firstRender)*(-0.0001)));
	setPan(((thisRender-firstRender)*0.001), 0,0);

	otherloop();

}

void resize(int width_v, int height_v){
	glViewport(0,0,width_v,height_v);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f+zoom, (float)width_v/(float)height_v, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	float desiredAspect = 8.0f/6.0f;
	if (height * desiredAspect > width){
		width = height_v*desiredAspect;
		height = height_v;
		glutReshapeWindow(height * desiredAspect, height);
	}
	else
	{
		width = width_v;
		height = height_v;
	}
}

void setOtherLoop(loopfunc f){
	otherloop = f;
}

void setZoom(int amount){
	printf("Zoom by %d\n",-amount);
	zoom += ZOOMMULTIPLIER*-amount;
	printf("Zoom is now %.1f\n",zoom);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f+zoom, (float)width/(float)height, 0.01f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void setRotation(float x, float y, float z){
	rotx = x;
	roty = y;
	rotz = z;
}

void setZoom(float z){
	zoom = z;
}

void setPan(float x, float y, float z){
	panx = x;
	pany = y;
	panz = z;
}

int glMain(int argc, char **argv){
	zoom = 1.0f;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(width,height);
	glutCreateWindow("Gears");

	glutDisplayFunc(render);
	glutIdleFunc(render);
	glutReshapeFunc(resize);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glLights();

	lastRender = getTimeMS();
	firstRender = getTimeMS();

	glutMainLoop();
	return 0;
}
