#include "camera.h"
#include "model.h"
#include "draw.h"

#include <SOIL/SOIL.h>

#include <GL/glut.h>

#include <math.h>
#include <stdio.h>

typedef int bool;
#define true 1
#define false 0

#define VIEWPORT_RATIO (4.0 / 3.0)
#define VIEWPORT_ASPECT 50.0

#define CAMERA_SPEED 10.0

typedef GLubyte Pixel[3];

int mouse_x, mouse_y;

bool drawMenu = false;

double rotateX;
double rotateY;

struct Camera camera;
struct Model model[3];

struct Action
{
    int move_forward;
    int move_backward;
    int step_left;
    int step_right;
};

struct Action action;
int time;
GLuint texture_names[3];
Pixel* images[3];

GLuint load_texture(char* filename, Pixel* image)
{

    GLuint texture_name;
    int width;
    int height;

    glGenTextures(1, &texture_name);

    image = (Pixel*)SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);

    glBindTexture(GL_TEXTURE_2D, texture_name);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (Pixel*)image);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture_name;
}

void special_key_handler(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_F1:
		drawMenu = TRUE;
		break;
	}
}

void draw_origin()
{
/*	glBegin(GL_LINES);

	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 2, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);

	glEnd();

	*/

	if (drawMenu)
	{
		if (45 > camera.pose.x > -45) {
			camera.pose.x = 0;
			camera.pose.y = 0;
			camera.pose.z = 0;
		}

        glBindTexture(GL_TEXTURE_2D, texture_names[2]);
		glPushMatrix();
		//glTranslatef(0, 0, 2);
		//glRotatef(90, 1, 1, 0);
		glBegin(GL_POLYGON);

		glTexCoord2f(0.0, 0.0);
		glVertex3d(camera.position.x + 2.125, camera.position.y - 1.125, camera.position.z - 1.0);

		glTexCoord2f(0.0, 1.0);
		glVertex3d(camera.position.x + 2.125, camera.position.y - 1.125, camera.position.z + 1.0);

		glTexCoord2f(1.0, 1.0);
		glVertex3d(camera.position.x + 2.125, camera.position.y + 1.125, camera.position.z + 1.0);

		glTexCoord2f(1.0, 0.0);
		glVertex3d(camera.position.x + 2.125, camera.position.y + 1.125, camera.position.z - 1.0);

		glEnd();
		glPopMatrix();
	}

	glBindTexture(GL_TEXTURE_2D, texture_names[1]);
	glBegin(GL_QUADS);
//	glColor3f(0.0f, 1.0f, 0.0f);
//	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0,0.0);
	glVertex3f(200.5f, -200.5f, 0.1f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(200.5f, 200.5f, 0.1f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-200.5f, 200.5f, 0.1f);
	glTexCoord2f(1.0,0.0);
	glVertex3f(-200.5f, -200.5f, 0.1f);

	glEnd();

    glClear(GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
    glTranslatef(8, 8, 7.2f);
    glScalef(10, 10, 10);
    glRotatef(90, 1, 0, 0);
    draw_model(&model[0]);
	glPopMatrix();

	glPushMatrix();
    glTranslatef(28, 18, 0.2f);
    glScalef(10, 10, 10);
    glRotatef(90, 1, 0, 0);
    glRotatef(-15, 1, 0, 0);
    draw_model(&model[1]);
	glPopMatrix();

	glPushMatrix();
    glTranslatef(28, 18, 0.2f);
    glScalef(10, 10, 10);
    glRotatef(90, 1, 0, 0);
    draw_model(&model[2]);
	glPopMatrix();

}

void draw_skybox()
{
    double theta, phi1, phi2;
    double x1, y1, z1;
    double x2, y2, z2;
    double u, v1, v2;

    int n_slices, n_stacks;
    double radius;
    int i, k;

    n_slices = 25;
    n_stacks = 12;
    radius = 100;
    glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, texture_names[0]);

    glScaled(radius, radius, radius);

    glColor3f(1, 1, 1);

	glBegin(GL_TRIANGLE_STRIP);

    for (i = 0; i < n_stacks; ++i) {
        v1 = (double)i / n_stacks;
        v2 = (double)(i + 1) / n_stacks;
        phi1 = v1 * M_PI / 2.0;
        phi2 = v2 * M_PI / 2.0;
        for (k = 0; k <= n_slices; ++k) {
            u = (double)k / n_slices;
            theta = u * 2.0 * M_PI;
            x1 = cos(theta) * cos(phi1);
            y1 = sin(theta) * cos(phi1);
            z1 = sin(phi1);
            x2 = cos(theta) * cos(phi2);
            y2 = sin(theta) * cos(phi2);
            z2 = sin(phi2);
            glTexCoord2d(u, 1.0 - v1);
            glVertex3d(x1, y1, z1);
            glTexCoord2d(u, 1.0 - v2);
            glVertex3d(x2, y2, z2);
        }
    }

    glEnd();

    glPopMatrix();
}

void update_camera_position(struct Camera* camera, double elapsed_time)
{
    double distance;

    distance = elapsed_time * CAMERA_SPEED;

    if (action.move_forward == TRUE) {
		move_camera_forward(camera, distance);
    }

    if (action.move_backward == TRUE) {
		move_camera_backward(camera, distance);
    }

    if (action.step_left == TRUE) {
	    step_camera_left(camera, distance);
    }

    if (action.step_right == TRUE) {
		step_camera_right(camera, distance);
    }
}

double calc_elapsed_time()
{
    int current_time;
    double elapsed_time;

    current_time = glutGet(GLUT_ELAPSED_TIME);
    elapsed_time = (double)(current_time - time) / 1000.0;
    time = current_time;

    return elapsed_time;
}

void display()
{
    double elapsed_time;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_position[][4] = {{ 50.0f, 5.0f, 5.0f, 1.0f },{ -5.0f, 5.0f, 5.0f, 1.0f },{ 5.0f, -5.0f, 5.0f, 1.0f },{ -5.0f, -5.0f, 5.0f, 1.0f }};
//glEnable(GL_LIGHTING);
//glEnable(GL_LIGHT0);
//glEnable(GL_LIGHT1);
//glEnable(GL_LIGHT2);

glPushMatrix();
glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
glLightfv(GL_LIGHT0, GL_POSITION, light_position[0]);
glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
glLightfv(GL_LIGHT1, GL_POSITION, light_position[1]);
glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
glLightfv(GL_LIGHT2, GL_POSITION, light_position[2]);
glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
    elapsed_time = calc_elapsed_time();
    update_camera_position(&camera, elapsed_time);
	set_view_point(&camera);
    draw_origin();
    draw_skybox();
	glutSwapBuffers();
}


void reshape(GLsizei width, GLsizei height)
{
    int x, y, w, h;
    double ratio;

    ratio = (double)width / height;
    if (ratio > VIEWPORT_RATIO) {
        w = (int)((double)height * VIEWPORT_RATIO);
        h = height;
        x = (width - w) / 2;
        y = 0;
    }
    else {
        w = width;
        h = (int)((double)width / VIEWPORT_RATIO);
        x = 0;
        y = (height - h) / 2;
    }

    glViewport (x, y, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(50.0, VIEWPORT_RATIO, 0.01, 10000.0);
}

void mouse_handler(int button, int state, int x, int y)
{
	mouse_x = x;
	mouse_y = y;
}

void motion_handler(int x, int y)
{
	double horizontal, vertical;

	horizontal = mouse_x - x;
	vertical = mouse_y - y;

	rotate_camera(&camera, horizontal, vertical);

	mouse_x = x;
	mouse_y = y;

    glutPostRedisplay();
}

void key_handler(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w':
        action.move_forward = TRUE;
		break;
	case 's':
        action.move_backward = TRUE;
		break;
	case 'a':
        action.step_left = TRUE;
		break;
	case 'd':
        action.step_right = TRUE;
		break;
	}

	switch(key){
		case 27:
			drawMenu = FALSE;
			break;
}


	glutPostRedisplay();
}

void key_up_handler(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w':
        action.move_forward = FALSE;
		break;
	case 's':
        action.move_backward = FALSE;
		break;
	case 'a':
        action.step_left = FALSE;
		break;
	case 'd':
        action.step_right = FALSE;
		break;
	}
	switch(key){
		case 27:
			drawMenu = FALSE;
			break;
}


	glutPostRedisplay();
}

void idle()
{
    glutPostRedisplay();
}

void initialize()
{
    glShadeModel(GL_SMOOTH);

    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);

//    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    glClearDepth(1.0);
	unsigned int i;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    char texture_filenames[][32] = {
        "badSkybox.png",
        "grass.jpg",
        "help.jpg"
    };

	for (i = 0; i < 3; ++i) {
        texture_names[i] = load_texture(texture_filenames[i], images[i]);
	}

	char obj_filenames[][32] = {
	    "objs/swingwing.obj",
	    "objs/libikoka_kozepe.obj",
	    "objs/libikoka_szele.obj",


	};
	for(i = 0; i < 3; i++){
    load_model(obj_filenames[i], &model[i]);
    print_model_info(&model[i]);
    print_bounding_box(&model[i]);
	}

    glEnable(GL_TEXTURE_2D);
}

/**
 * Main function
 */
int main(int argc, char* argv[])
{

	rotateX = 0.0;
    rotateY = 0.0;

    glutInit(&argc, argv);

	glutInitWindowSize(640, 480);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	int window = glutCreateWindow("GLUT Window");
	glutSetWindow(window);

    initialize();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key_handler);
    glutKeyboardUpFunc(key_up_handler);
    glutSpecialFunc(special_key_handler);
    glutMouseFunc(mouse_handler);
    glutMotionFunc(motion_handler);
    glutIdleFunc(idle);

    init_camera(&camera);

    action.move_forward = FALSE;
    action.move_backward = FALSE;
    action.step_left = FALSE;
    action.step_right = FALSE;

    glutMainLoop();

    return 0;
}

