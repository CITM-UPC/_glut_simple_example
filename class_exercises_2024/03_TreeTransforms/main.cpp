#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Camera.h"
#include "Texture.h"
#include "Mesh.h"
#include "GraphicObject.h"
#include "PreFabsFactory.h"
#include "intersect.h"
#include "debugDraws.h"
using namespace std;

using hrclock = chrono::high_resolution_clock;
static const auto FPS = 60;
static const auto FRAME_TIME = 1.0s / FPS;

static bool paused = true;
static Camera camera;
static GraphicObject scene;

static void display_func() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(&camera.view()[0][0]);

	scene.draw();

	//draw debug
	drawFloorGrid(100, 1);
	drawDebugInfoForGraphicObject(scene);

	glutSwapBuffers();
}

static void init_opengl() {
	glewInit();

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.5, 0.5, 0.5, 1.0);
}

static void reshape_func(int width, int height) {
	glViewport(0, 0, width, height);
	camera.aspect = static_cast<double>(width) / height;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(&camera.projection()[0][0]);
}

static void mouseWheel_func(int wheel, int direction, int x, int y) {
	camera.transform().translate(vec3( 0, 0, direction * 1));
}

static void idle_func() {
	static auto last_time = hrclock::now();
	const auto current_time = hrclock::now();
	const auto delta_time = current_time - last_time;
	if (delta_time >= FRAME_TIME) {
		if (!paused) {
			//rotate everything 360 degrees per 10 seconds
			const auto angle_vel = glm::radians(360.0) / 10.0;
			scene.children().front().transform().rotate(angle_vel * FRAME_TIME.count(), vec3(0, 0, 1));
			scene.children().front().children().front().transform().rotate(angle_vel * FRAME_TIME.count(), vec3(0, 0, 1));
			scene.children().front().children().front().children().front().transform().rotate(angle_vel * FRAME_TIME.count(), vec3(0, 0, 1));
		}

		glutPostRedisplay();
		last_time = current_time;
	}
}

static void initScene() {

	const auto triangle_mesh = PreFabsFactory::MakeTriangleMesh(0.5);
	const auto quad_mesh = PreFabsFactory::MakeQuadMesh(0.5);
	const auto chess_texture_image = PreFabsFactory::MakeChessTextureImage(64, 64, 8);

	auto& triangle = scene.emplaceChild();
	triangle.transform().pos() = vec3(0, 0, 0);
	triangle.setMesh(triangle_mesh);
	triangle.setDefaultMaterial();
	triangle.material().color = Colors::Red;

	auto& child_textured_triangle = triangle.emplaceChild();
	child_textured_triangle.transform().pos() = vec3(2, 0, 0);
	child_textured_triangle.setMesh(triangle_mesh);
	child_textured_triangle.setDefaultMaterial();
	child_textured_triangle.material().color = Colors::Green;
	child_textured_triangle.material().texture.setImage(chess_texture_image);

	auto& child_textured_quad = child_textured_triangle.emplaceChild();
	child_textured_quad.transform().pos() = vec3(2, 0, 0);
	child_textured_quad.setMesh(quad_mesh);
	child_textured_quad.setDefaultMaterial();
	child_textured_quad.material().color = Colors::Blue;
	child_textured_quad.material().texture.setImage(chess_texture_image);
}

int main(int argc, char* argv[]) {
	// Init window and context
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Tree Transform");

	// Init OpenGL
	init_opengl();

	// Init Scene from file
	initScene();

	//Init camera relative to the scene
	const auto scene_bbox = scene.boundingBox();
	camera.zFar = scene_bbox.size().x * 4.0;
	camera.transform().pos() = scene_bbox.center() + vec3(0, 2, scene_bbox.size().x);
	camera.transform().rotate(glm::radians(180.0), vec3(0, 1, 0));

	// Set Glut callbacks
	glutDisplayFunc(display_func);
	glutIdleFunc(idle_func);
	glutReshapeFunc(reshape_func);
	glutMouseWheelFunc(mouseWheel_func);
	glutKeyboardFunc([](unsigned char key, int x, int y) { paused = !paused; });

	// Enter glut main loop
	glutMainLoop();

	return EXIT_SUCCESS;
}