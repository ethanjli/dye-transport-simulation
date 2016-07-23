#include <cstdio>
#include <cstdlib>
#include <GL/glut.h>

#include "src/fluid-sim/fluidsimulator.h"

using namespace std;

static Scalar force, source;

static int dvel;

static int win_id;
static int win_x, win_y;
static int mouse_down[3];
static int xMouseOld, yMouseOld, xMouse, yMouse;

static FluidSimulator fluidSimulator;

static void pre_display(void) {
    glViewport(0, 0, win_x, win_y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 1.0, 0.0, 1.0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

static void post_display(void) {
    glutSwapBuffers();
}

static void draw_velocity(void) {
    Scalar x, y;

    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);

    glBegin(GL_LINES);
    for (Grid::Index i = 1; i <= kGridSize; i++) {
        x = (i - 0.5f) * kGridSpacing;
        for (Grid::Index j = 1; j <= kGridSize; j++) {
            y = (j - 0.5f) * kGridSpacing;

            glVertex2f(x, y);
            glVertex2f(x + fluidSimulator.system.u(i, j), y + fluidSimulator.system.v(i, j));
        }
    }
    glEnd();
}

static void draw_density(void)
{
    glBegin(GL_QUADS);
    for (Grid::Index i=0; i <= kGridSize; i++) {
        Scalar x = (i - 0.5f) * kGridSpacing;
        for (Grid::Index j=0; j <= kGridSize; j++) {
            Scalar y = (j - 0.5f) * kGridSpacing;

            Scalar d00 = fluidSimulator.system.density(i, j);
            Scalar d01 = fluidSimulator.system.density(i, j + 1);
            Scalar d10 = fluidSimulator.system.density(i + 1, j);
            Scalar d11 = fluidSimulator.system.density(i + 1, j + 1);

            glColor3f(1 - 0.4 * d00, 1 - 0.4 * d00, 1);
            glVertex2f(x, y);
            glColor3f(1 - 0.4 * d10, 1 - 0.4 * d10, 1);
            glVertex2f(x + kGridSpacing, y);
            glColor3f(1 - 0.4 * d11, 1 - 0.4 * d11, 1);
            glVertex2f(x + kGridSpacing, y + kGridSpacing);
            glColor3f(1 - 0.4 * d01, 1 - 0.4 * d01, 1);
            glVertex2f(x, y + kGridSpacing);
        }
    }
    glEnd();
}

static void get_from_UI()
{
    int i, j;

    fluidSimulator.system.density_add = Grid::Zero();
    fluidSimulator.system.u_add = Grid::Zero();
    fluidSimulator.system.v_add = Grid::Zero();

    if (!mouse_down[0] && !mouse_down[2]) return;

    i = (int)(( xMouse / (Scalar)win_x) * kGridSize + 1);
    j = (int)(((win_y - yMouse) / (Scalar)win_y) * kGridSize + 1);

    if (i < 1 || i > kGridSize || j < 1 || j > kGridSize) return;

    if (mouse_down[0]) {
        fluidSimulator.system.u_add(i, j) = force * (xMouse-xMouseOld);
        fluidSimulator.system.v_add(i, j) = force * (yMouseOld-yMouse);
    }
    if (mouse_down[2]) {
        fluidSimulator.system.density_add(i, j) = source;
    }

    xMouseOld = xMouse;
    yMouseOld = yMouse;
}

static void key_func(unsigned char key, int x, int y) {
    switch ( key ) {
        case 'c':
        case 'C':
            fluidSimulator.system.clear();
            break;
        case 'q':
        case 'Q':
            exit(0);
            break;
        case 'v':
        case 'V':
            dvel = !dvel;
            break;
    }
}
static void mouse_func(int button, int state, int x, int y) {
    xMouseOld = xMouse = x;
    xMouseOld = yMouse = y;
    mouse_down[button] = state == GLUT_DOWN;
}
static void motion_func(int x, int y) {
    xMouse = x;
    yMouse = y;
}
static void reshape_func(int width, int height) {
    glutSetWindow(win_id);
    glutReshapeWindow(width, height);
    win_x = width;
    win_y = height;
}
static void idle_func(void) {
    get_from_UI();
    fluidSimulator.stepSystem();
    glutSetWindow(win_id);
    glutPostRedisplay();
}
static void display_func(void) {
    pre_display();
    if (dvel) draw_velocity();
    else draw_density();
    post_display();
}

static void open_glut_window() {
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(win_x, win_y);
    win_id = glutCreateWindow("Jos Stam's Demo");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();

    pre_display();

    glutKeyboardFunc(key_func);
    glutMouseFunc(mouse_func);
    glutMotionFunc(motion_func);
    glutReshapeFunc(reshape_func);
    glutIdleFunc(idle_func);
    glutDisplayFunc(display_func);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);

    if (argc != 1 && argc != 6) {
        fprintf(stderr, "usage : %s N dt diff visc force source\n", argv[0]);
        fprintf(stderr, "where:\n");\
        fprintf(stderr, "\t N      : grid resolution\n");
        fprintf(stderr, "\t dt     : time step\n");
        fprintf(stderr, "\t diff   : diffusion rate of the density\n");
        fprintf(stderr, "\t visc   : viscosity of the fluid\n");
        fprintf(stderr, "\t force  : scales the mouse movement that generate a force\n");
        fprintf(stderr, "\t source : amount of density that will be deposited\n");
        return 1;
    }

    if (argc == 1) {
        //N = kGridSize;
        fluidSimulator.dt = 0.1f;
        fluidSimulator.system.diffusionConstant = 0.0f;
        fluidSimulator.system.viscosity = 0.0f;
        force = 5.0f;
        source = 100.0f;
        fprintf(stderr, "Using defaults : N=%d dt=%g diff=%g visc=%g force = %g source=%g\n",
                kGridSize, fluidSimulator.dt, fluidSimulator.system.diffusionConstant, fluidSimulator.system.viscosity, force, source);
    } else {
        //N = atoi(argv[1]);
        fluidSimulator.dt = atof(argv[2]);
        fluidSimulator.system.diffusionConstant = atof(argv[3]);
        fluidSimulator.system.viscosity = atof(argv[4]);
        force = atof(argv[5]);
        source = atof(argv[6]);
    }

    printf("\n\nHow to use this demo:\n\n");
    printf("\t Add densities with the right mouse button\n");
    printf("\t Add velocities with the left mouse button and dragging the mouse\n");
    printf("\t Toggle density/velocity display with the 'v' key\n");
    printf("\t Clear the simulation by pressing the 'c' key\n");
    printf("\t Quit by pressing the 'q' key\n");

    dvel = 0;

    win_x = 512;
    win_y = 512;
    open_glut_window();

    glutMainLoop();

    return 0;
}
