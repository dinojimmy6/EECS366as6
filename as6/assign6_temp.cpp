/*

EECS 366/466 COMPUTER GRAPHICS
Template for Assignment 6-Local Illumination and Shading

*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include <string.h>
#include <GL/glut.h>
#include <windows.h>
#include "glprocs.h"
#include <fstream>


#define PI 3.14159265359

using namespace std;

//Illimunation and shading related declerations
char *shaderFileRead(char *fn);
GLuint vertex_shader,fragment_shader,p;
int illuminationMode = 0;
int shadingMode = 1;
int lightSource = 0;
int lightColor = 0;
float secondLightPos[4] = { 7, 7, 7, 1 };
float diffk[4] = { .005, .005, .005, 1.0 };
float lightIk2[4] = { 1.0, 1.0, 1.0, 0 };
float ambient[4] = { .2, .2, .2, 0 };

void materialFileRead(char *fn);
float ambient_p[3];
float diffuse_p[3];
float specular_p[3];
float specular_exponent_p;
float ambient_c[3];
float d_c;
float Rd_c[3];
float s_c;
float f0_c[3];
float m_c[2];
float w_c[2];




//Projection, camera contral related declerations
int WindowWidth,WindowHeight;
bool LookAtObject = false;
bool ShowAxes = true;



float CameraRadius = 10;
float CameraTheta = PI / 2;
float CameraPhi = PI / 2;
int MouseX = 0;
int MouseY = 0;
bool MouseLeft = false;
bool MouseRight = false;



void DisplayFunc(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLint p1 = glGetUniformLocationARB(p, "mls.position");
	printf("p1 is: %d\n", p1);
	glUniform4fvARB(p1, 1, secondLightPos);
	GLint p2 = glGetUniformLocationARB(p, "mls.diffuse");
	printf("p2 is: %d\n", p2);
	glUniform4fvARB(p2, 1, lightIk2);
	GLint p3 = glGetUniformLocationARB(p, "mls.intensity");
	printf("p3 is: %d\n", p3);
	glUniform4fvARB(p3, 1, lightIk2);
	GLint p4 = glGetUniformLocationARB(p, "mls.ambient");
	printf("p4 is: %d\n", p4);
	glUniform4fvARB(p4, 1, ambient);
	GLint p5 = glGetUniformLocationARB(p, "shadingMode");
	printf("p5 is: %d\n", p5);
	glUniform1iARB(p5, shadingMode);
	p5 = glGetUniformLocationARB(p, "illumMode");
	printf("p5 is: %d\n", p5);
	glUniform1iARB(p5, illuminationMode);
	p5 = glGetUniformLocationARB(p, "lightSource");
	glUniform1iARB(p5, lightSource);
	//GLint p3 = glGetUniformLocationARB(p, "mm.diffuse");
	//printf("p3 is: %d\n", p3);
	//glUniform4fvARB(p3, 1, diffk);
	//load projection and viewing transforms
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//GLint p1 = glGetUniformLocationARB(p, "gl_LightSource[0].position");
	//glUniform4fvARB(p1, 1, secondLightPos);
        
	gluPerspective(60,(GLdouble) WindowWidth/WindowHeight,0.01,10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(CameraRadius*cos(CameraTheta)*sin(CameraPhi),
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),
			  CameraRadius*cos(CameraPhi),
			  0,0,0,
			  0,0,1);
	glEnable(GL_DEPTH_TEST);
	
	glutSolidTeapot(1);
	glutSwapBuffers();
}

void ReshapeFunc(int x,int y)
{
    glViewport(0,0,x,y);
    WindowWidth = x;
    WindowHeight = y;
}


void MouseFunc(int button,int state,int x,int y)
{
	MouseX = x;
	MouseY = y;

    if(button == GLUT_LEFT_BUTTON)
		MouseLeft = !(bool) state;
	if(button == GLUT_RIGHT_BUTTON)
		MouseRight = !(bool) state;
}

void MotionFunc(int x, int y)
{
	if(MouseLeft)
	{
        CameraTheta += 0.01*PI*(MouseX - x);
		CameraPhi += 0.01*PI*(MouseY - y);
	}
	if(MouseRight)
	{
        CameraRadius += 0.2*(MouseY-y);
		if(CameraRadius <= 0)
			CameraRadius = 0.2;
	}
    
	MouseX = x;
	MouseY = y;

	glutPostRedisplay();
}


void setShaders() {

	char *vs = NULL,*fs = NULL;

	//create the empty shader objects and get their handles
	vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	
	
	//read the shader files and store the strings in corresponding char. arrays.
	vs = shaderFileRead("sampleshader.vert");
	fs = shaderFileRead("sampleshaderfrag.frag");

	const char * vv = vs;
	const char * ff = fs;

	//set the shader's source code by using the strings read from the shader files.
	glShaderSourceARB(vertex_shader, 1, &vv,NULL);
	glShaderSourceARB(fragment_shader, 1, &ff,NULL);

	free(vs);free(fs);

	//Compile the shader objects
	glCompileShaderARB(vertex_shader);
	glCompileShaderARB(fragment_shader);


	//create an empty program object to attach the shader objects
	p = glCreateProgramObjectARB();

	//attach the shader objects to the program object
	glAttachObjectARB(p,vertex_shader);
	glAttachObjectARB(p,fragment_shader);

	/*
	**************
	Programming Tip:
	***************
	Delete the attached shader objects once they are attached.
	They will be flagged for removal and will be freed when they are no more used.
	*/
	glDeleteObjectARB(vertex_shader);
	glDeleteObjectARB(fragment_shader);

	//Link the created program.
	/*
	**************
	Programming Tip:
	***************
	You can trace the status of link operation by calling 
	"glGetObjectParameterARB(p,GL_OBJECT_LINK_STATUS_ARB)"
	*/
	
	glLinkProgramARB(p);

	//Start to use the program object, which is the part of the current rendering state
	glUseProgramObjectARB(p);
	
}


//Motion and camera controls
void KeyboardFunc(unsigned char key, int x, int y)
{
    switch(key)
	{
	case 'A':
	case 'a':
		ShowAxes = !ShowAxes;
		break;
	case 'Q':
	case 'q':
		exit(1);
		break;
	case 'w':
	case 'W':
		if (illuminationMode == 0)
		{
			illuminationMode = 1;
		}
		else
		{
			illuminationMode = 0;
		}
		break;
	case 'e':
	case 'E':
		if (shadingMode == 0)
		{
			shadingMode =1;
		}
		else
		{
			shadingMode =0;
		}
		break;
	case 'd':
	case 'D':
		if (lightSource == 0)
		{
			lightSource =1;
		}
		else
		{
			lightSource =0;
		}
		break;
	case 'f':
	case 'F':
		if (lightSource == 1)
		{
			//change color of the secondary light source at each key press, 
			//light color cycling through pure red, green, blue, and white.
			if (lightColor == 0) {
				lightColor = 1;
				lightIk2[0] = 1.0; lightIk2[1] = 0.0; lightIk2[2] = 0.0;
			}
			else if (lightColor == 1) {
				lightColor = 2;
				lightIk2[0] = 0.0; lightIk2[1] = 1.0; lightIk2[2] = 0.0;
			}
			else if (lightColor == 2) {
				lightColor = 3;
				lightIk2[0] = 0.0; lightIk2[1] = 0.0; lightIk2[2] = 1.0;
			}
			else if (lightColor == 3) {
				lightColor = 0;
				lightIk2[0] = 1.0; lightIk2[1] = 1.0; lightIk2[2] = 1.0;
			}
		}
		break;

    default:
		break;
    }

	glutPostRedisplay();
}

int main(int argc, char **argv) 
{			  

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("Assignment 6");



	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);



	
	materialFileRead("material.dat");
	setShaders();

	glutMainLoop();

	return 0;
}


//Read the shader files, given as parameter.
char *shaderFileRead(char *fn) {


	FILE *fp = fopen(fn,"r");
	if(!fp)
	{
		cout<< "Failed to load " << fn << endl;
		return " ";
	}
	else
	{
		cout << "Successfully loaded " << fn << endl;
	}
	
	char *content = NULL;

	int count=0;

	if (fp != NULL) 
	{
		fseek(fp, 0, SEEK_END);
		count = ftell(fp);
		rewind(fp);

		if (count > 0) 
		{
			content = (char *)malloc(sizeof(char) * (count+1));
			count = fread(content,sizeof(char),count,fp);
			content[count] = '\0';
		}
		fclose(fp);
	}
	return content;
}

void materialFileRead(char *fn) {
	ifstream fp(fn, ios::in);
	if (!fp || !fp.is_open()) {
		cout << "Failed to load " << fn << endl;
		return;
	}


	char type;
	while (!fp.eof()) {
		fp >> type;
		if (type == 'P') {
			fp >> ambient_p[0] >> ambient_p[1] >> ambient_p[2] >>
				diffuse_p[0] >> diffuse_p[1] >> diffuse_p[2] >>
				specular_p[0] >> specular_p[1] >> specular_p[2] >>
				specular_exponent_p;
		}
		else if (type == 'C') {
			fp >> ambient_c[0] >> ambient_c[1] >> ambient_c[2] >>
				d_c >> Rd_c[0] >> Rd_c[1] >> Rd_c[2] >>
				s_c >> f0_c[0] >> f0_c[1] >> f0_c[2] >>
				m_c[0] >> w_c[0] >> m_c[1] >> w_c[1];
		}
	}

	fp.close();
}