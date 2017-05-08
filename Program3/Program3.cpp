//Damian Moskała

//Program 3 - Kula z teksturą planety Wenus

#include <GL/glut.h>
#include "glext.h"
#include <stdlib.h>
#include <stdio.h>
#include "colors.h"
#include "targa.h"
#include <GLFW/glfw3.h>
#include <iostream>

PFNGLWINDOWPOS2IPROC glWindowPos2i = NULL;

enum
{
	VENUS_TEX,
	FULL_WINDOW,
	ASPECT_1_1,
	EXIT
};


int aspect = FULL_WINDOW;

#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

int button_state = GLUT_UP;
int button_x, button_y;
const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near = 3.0;
const GLdouble far = 7.0;
GLfloat rotatex = 270.0;
GLfloat rotatez = 0.0;
GLfloat scale = 1.5;
GLuint texture;
GLint min_filter = GL_LINEAR_MIPMAP_LINEAR;


void DisplayScene()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -(near + far) / 2);
	glRotatef(rotatex, 1.0, 0.0, 0.0);
	glRotatef(rotatez, 0.0, 0.0, 1.0);
	glScalef(scale, scale, scale);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	GLUquadricObj *quadobj = gluNewQuadric();
	gluQuadricDrawStyle(quadobj, GLU_FILL);
	gluQuadricNormals(quadobj, GLU_SMOOTH);
	gluQuadricTexture(quadobj, GL_TRUE);
	gluSphere(quadobj, 1.0, 30, 30);
	gluDeleteQuadric(quadobj);
	glDisable(GL_TEXTURE_2D);
	char string[200];
	GLfloat var;
	glColor3fv(Black);
	glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY, &var);
	sprintf_s(string, "GL_TEXTURE_PRIORITY = %f", var);
	glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_RESIDENT, &var);

	if (var == GL_FALSE)
	{
		strcpy_s(string, "GL_TEXTURE_RESIDENT = GL_FALSE");
	}
	else
	{
		strcpy_s(string, "GL_TEXTURE_RESIDENT = GL_TRUE");
	}

	glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &var);
	sprintf_s(string, "GL_TEXTURE_WIDTH = %f", var);
	glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &var);
	sprintf_s(string, "GL_TEXTURE_HEIGHT = %f", var);
	glFlush();
	glutSwapBuffers();
}

void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (aspect == ASPECT_1_1)
	{
		if (width < height && width > 0)
		{
			glFrustum(left, right, bottom*height / width, top*height / width, near, far);
		}
		else
		{
			if (width >= height && height > 0)
			{
				glFrustum(left*width / height, right*width / height, bottom, top, near, far);
			}
		}
	}
	else
	{
		glFrustum(left, right, bottom, top, near, far);
	}

	DisplayScene();
}


void Keyboard(unsigned char key, int x, int y)
{
	if (key == '+')
	{
		scale += 0.05;
	}
	else
	{
		if (key == '-' && scale > 0.05)
		{
			scale -= 0.05;
		}
	}

	DisplayScene();
}

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		button_state = state;

		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

void MouseMotion(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rotatez += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) * (x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) * (button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

void Menu(int value)
{
	switch (value)
	{
		case GL_NEAREST_MIPMAP_NEAREST:
		case GL_NEAREST_MIPMAP_LINEAR:
		case GL_LINEAR_MIPMAP_NEAREST:
		case GL_LINEAR_MIPMAP_LINEAR:
		{
			min_filter = value;
			DisplayScene();
		}
		break;

		case FULL_WINDOW:
		{
			aspect = FULL_WINDOW;
			Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		}
		break;

		case ASPECT_1_1:
		{
			aspect = ASPECT_1_1;
			Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		}
		break;

		case EXIT:
		{
			exit(0);
		}
	}
}

void GenerateTextures()
{
	GLsizei width, height;
	GLenum format, type;
	GLvoid *pixels;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLboolean error = load_targa("saturn.tga", width, height, format, type, pixels);

	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku venus.tga");
		exit(0);
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, format, type, pixels);
	delete[](unsigned char*)pixels;
}

void ExtensionSetup()
{
	const char *version = (char*)glGetString(GL_VERSION);
	int major = 0, minor = 0;

	if (sscanf_s(version, "%d.%d", &major, &minor) != 2)
	{
		printf("Błędny format wersji OpenGL\n");
		exit(0);
	}

	if (major > 1 || minor >= 4)
	{
		glWindowPos2i = (PFNGLWINDOWPOS2IPROC)wglGetProcAddress("glWindowPos2i");
	}
	else
	{
		if (glutExtensionSupported("GL_ARB_window_pos"))
		{
			glWindowPos2i = (PFNGLWINDOWPOS2IPROC)wglGetProcAddress
			("glWindowPos2iARB");
		}
		else
		{
			printf("Brak rozszerzenia ARB_window_pos!\n");
			exit(0);
		}
	}
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Tekstura 2D");
	glutDisplayFunc(DisplayScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);

	int MenuMinFilter = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR);

	int MenuAspect = glutCreateMenu(Menu);
	glutAddMenuEntry("Aspekt obrazu - całe okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);
	glutCreateMenu(Menu);
	glutAddSubMenu("Filtr pomniejszający", MenuMinFilter);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjście", EXIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	GenerateTextures();
	ExtensionSetup();
	glutMainLoop();
	return 0;
}
