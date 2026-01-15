/****************************************************
 * 2016-2023 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/

#include <ctime>
#include "defs.h"
#include "io.h"
#include "ishape.h"
#include "framebuffer.h"
#include "raytracer.h"
#include "iscene.h"
#include "light.h"
#include "image.h"
#include "camera.h"
#include "rasterization.h"

Image im1("usflag.ppm");
Image im2("earth.ppm");
Image im3("mercury.ppm");
Image im4("venus_atmosphere.ppm");
Image im5("mars.ppm");
Image im6("jupiter.ppm");
Image im7("saturn.ppm");
Image im8("uranus.ppm");
Image im9("neptune.ppm");
Image im10("sun.ppm");
Image im11("stars.ppm");


int currLight = 0;
double angle = 0.5;
const int MINZ = -10;
const int MAXZ = 4;
double z = MINZ;
double inc = 0.4;
bool isAnimated = false;
int numReflections = 0;
int antiAliasing = 1;
bool multiViewOn = false;
double spotDirX = -1;
double spotDirY = 0;
double spotDirZ = 0;

dvec3 cameraPos(16, 8.5, -2);
//dvec3 cameraPos(16, 6, -2);
dvec3 cameraFocus(0, 2, -2);
dvec3 cameraUp = Y_AXIS;

//double cameraFOV = glm::radians(120.0);
double cameraFOV = glm::radians(60.0);


/* ********** Lighhs ********** */
vector<PositionalLightPtr> lights = {
						new PositionalLight(dvec3(15, 15, 15), white),

						new SpotLight(dvec3(10, 5, -2),
										dvec3(spotDirX, spotDirY, spotDirZ),
										glm::radians(90.0),
										white),

						new DirectionalLight(dvec3(-1, -1, -1), white)
};

/* ********** Light pointers ********** */
PositionalLightPtr posLight = dynamic_cast<PositionalLightPtr>(lights[0]);
SpotLightPtr spotLight = dynamic_cast<SpotLightPtr>(lights[1]);
DirectionalLightPtr dirLight = dynamic_cast<DirectionalLightPtr>(lights[2]);

FrameBuffer frameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
RayTracer rayTrace(black);
IScene scene;

IPlane* clearPlane = new IPlane(dvec3(0.0, 0.0, MINZ), dvec3(0.0, 0.0, 1.0));


void buildScene() {

	/* ********** Add objects to scene ********** */
	scene.addOpaqueObject(new VisibleIShape(new IPlane(dvec3(0, -2, 0), Y_AXIS), blackRubber)); // Plane

	scene.addOpaqueObject(new VisibleIShape(new IDisk(dvec3(0, 4.9, -2), Y_AXIS, 2.5), yellowPlastic)); // Disk

	scene.addOpaqueObject(new VisibleIShape(new ICylinderY(dvec3(0, 6, -2), 0.8, 3.0), blackPlastic, &im1)); // Textured 

	scene.addOpaqueObject(new VisibleIShape(new ICylinderY(dvec3(0, 5.75, -5.5), 0.8, 2.3), whitePlastic)); // Untextured different size

	scene.addOpaqueObject(new VisibleIShape(new IClosedCylinderY(dvec3(0, 5.75, 1.5), 0.8, 2.3), redPlastic)); // Untextured

	scene.addOpaqueObject(new VisibleIShape(new IBasicSphere(dvec3(0, 6.75, 5), 2.0), copper)); // Basic sphere

	scene.addOpaqueObject(new VisibleIShape(new ITriangle(dvec3(0, 5, -6.5), dvec3(-1, 7, -7.5), dvec3(0, 5, -8.5)), copper)); // Triangle

	// Star background
	Material theVoid(color(0.0, 0.0, 0.0), color(0.0, 0.0, 0.0), color(0.0, 0.0, 0.0), 0.0);

	// SECOND TEXTUREMAPPED OBJECT
	scene.addOpaqueObject(new VisibleIShape(new IRectangle(dvec3(-15, 0, -2), 0.5, 25, 90), theVoid, &im11));

	// Top of box
	scene.addOpaqueObject(new VisibleIShape(new IRectangle(dvec3(0, 4.5, -2), 9, 0.5, 26), chrome));

	// Bottom of box
	scene.addOpaqueObject(new VisibleIShape(new IRectangle(dvec3(0, -2, -2), 9, 0.5, 26), chrome));

	// Back of box
	scene.addOpaqueObject(new VisibleIShape(new IRectangle(dvec3(-2.5, 1, -3), 0.5, 7, 26.5), chrome));

	// Right side of box
	scene.addOpaqueObject(new VisibleIShape(new IRectangle(dvec3(0, 1, -15), 9, 7, 0.5), chrome));

	//Left side of box
	scene.addOpaqueObject(new VisibleIShape(new IRectangle(dvec3(0, 1, 11), 9, 7, 0.5), chrome));

	// Glass for the front of the box
	//Material glass(color(0.15, 0.3, 0.15), color(0.55, 0.55, 0.55), color(1.0, 1.0, 1.0), 128.0);
	//glass.alpha = 0.85;
	//scene.addOpaqueObject(new VisibleIShape(new IRectangle(dvec3(4, 1, -2), 0.5, 7, 25), glass));

	scene.addTransparentObject(new TransparentIShape(new IRectangle(dvec3(4, 1, -2), 0.5, 7, 25), color(0.25, 0.25, 0.25), 0.2));


	// Planet struct for iterating the planets
	struct Planet {
		string name;
		dvec3 pos;
		double radius;
		Material mat;
		Image* texture;
	};

	vector<Planet> planets = {
		{"Neptune",  dvec3(0, 0, -11), 1.0, cyanPlastic, &im9},
		{"Uranus",   dvec3(0, 0, -8), 1.0, turquoise, &im8},
		{"Saturn",   dvec3(0, 0, -5),  1.2, pewter, &im7},
		{"Jupiter",  dvec3(0, 0, -2),  1.3, polishedGold, &im6},
		{"Mars",     dvec3(0, 0,  0),  0.5, redRubber, &im5},
		{"Earth",    dvec3(0, 0,  2),  0.65, silver, &im2},
		{"Venus",    dvec3(0, 0,  4),  0.6, copper, &im4},
		{"Mercury",  dvec3(0, 0,  6),  0.3, polishedBronze, nullptr},
	};


	// Sun and stand
	scene.addOpaqueObject(new VisibleIShape(new ISphere(dvec3(0, 2, 9), 2.0), gold, &im10));
	scene.addOpaqueObject(new VisibleIShape(new IClosedCylinderY(dvec3(0, 0, 9), 0.3, 4.0), polishedGold));

	// Planets and their stands
	for (const auto& planet : planets) {

		scene.addOpaqueObject(new VisibleIShape(new ISphere(planet.pos + dvec3(0, 2, 0), 
			planet.radius), planet.mat, planet.texture));

		scene.addOpaqueObject(new VisibleIShape(new IClosedCylinderY(planet.pos + 
			dvec3(0, -0.5, 0), 0.1, 4.4), tin));
	}
	

	/* ********** Add lights to scene ********** */
	scene.addLight(lights[0]);
	scene.addLight(lights[1]);
	scene.addLight(lights[2]);
	lights[0]->isOn = true;
	lights[1]->isOn = false;
	lights[2]->isOn = false;
}

void render() {

	int frameStartTime = glutGet(GLUT_ELAPSED_TIME);
	int width = frameBuffer.getWindowWidth();
	int height = frameBuffer.getWindowHeight();
	frameBuffer.clearColorBuffer();

	scene.camera = new PerspectiveCamera(cameraPos, cameraFocus, cameraUp, cameraFOV, width, height);
	rayTrace.raytraceScene(frameBuffer, numReflections, scene, antiAliasing);

	frameBuffer.showColorBuffer();
	int frameEndTime = glutGet(GLUT_ELAPSED_TIME); // Get end time
	double totalTimeSec = (frameEndTime - frameStartTime) / 1000.0;

	if (isAnimated) {
		cout << "Transparent plane's z value: " << clearPlane->a.z << endl;
	}
	cout << "Render time: " << totalTimeSec << " sec." << endl;
}

void resize(int width, int height) {
	frameBuffer.setFrameBufferSize(width, height);
	glutPostRedisplay();
}
void incrementClamp(double& v, double delta, double lo, double hi) {
	v = glm::clamp(v + delta, lo, hi);
}

void incrementClamp(int& v, int delta, int lo, int hi) {
	v = glm::clamp(v + delta, lo, hi);
}

void timer(int id) {
	if (isAnimated) {
		z += inc;
		if (z <= MINZ) {
			inc = -inc;
		}
		else if (z >= MAXZ) {
			inc = -inc;
		}
	}
	clearPlane->a = dvec3(0, 0, z);
	glutTimerFunc(TIME_INTERVAL, timer, 0);
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	//int W, H;
	const double INC = 0.5;
	switch (key) {
	case 'A':
	case 'a':	currLight = 0;
		cout << *lights[0] << endl;
		break;
	case 'B':
	case 'b':	currLight = 1;
		cout << *lights[1] << endl;
		break;
	case 'C':
	case 'c':	dirLight->isOn = !dirLight->isOn;
		cout << "Directional light:  " << (dirLight->isOn ? "ON" : "OFF") << endl;
		break;
	case 'O':
	case 'o':	lights[currLight]->isOn = !lights[currLight]->isOn;
		cout << (lights[currLight]->isOn ? "ON" : "OFF") << endl;
		break;
	case 'X':
	case 'x': lights[currLight]->pos.x += (isupper(key) ? INC : -INC);
		cout << lights[currLight]->pos << endl;
		break;
	case 'Y':
	case 'y': lights[currLight]->pos.y += (isupper(key) ? INC : -INC);
		cout << lights[currLight]->pos << endl;
		break;
	case 'Z':
	case 'z': lights[currLight]->pos.z += (isupper(key) ? INC : -INC);
		cout << lights[currLight]->pos << endl;
		break;
	case 'J':
	case 'j':	spotDirX += (isupper(key) ? INC : -INC);
		spotLight->setDir(spotDirX, spotDirY, spotDirZ);
		cout << spotLight->spotDir << endl;
		break;
	case 'K':
	case 'k':	spotDirY += (isupper(key) ? INC : -INC);
		spotLight->setDir(spotDirX, spotDirY, spotDirZ);
		cout << spotLight->spotDir << endl;
		break;
	case 'L':
	case 'l':	spotDirZ += (isupper(key) ? INC : -INC);
		spotLight->setDir(spotDirX, spotDirY, spotDirZ);
		cout << spotLight->spotDir << endl;
		break;
	case 'P':
	case 'p':	isAnimated = !isAnimated;
		cout << "Animation: " << (isAnimated ? "on" : "off") << endl;
		break;
	case '+':	antiAliasing = 3;
		cout << "Anti aliasing: " << antiAliasing << endl;
		break;
	case '-':	antiAliasing = 1;
		cout << "Anti aliasing: " << antiAliasing << endl;
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':	numReflections = key - '0';
		cout << "Num reflections: " << numReflections << endl;
		break;
	case ESCAPE:
		glutLeaveMainLoop();
		break;
	default:
		cout << (int)key << "unmapped key pressed." << endl;
	}

	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	graphicsInit(argc, argv, __FILE__);

	glutDisplayFunc(render);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseUtility);
	glutTimerFunc(TIME_INTERVAL, timer, 0);
	buildScene();

	glutMainLoop();

	return 0;
}