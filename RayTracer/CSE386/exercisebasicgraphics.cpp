/****************************************************
 * 2016-2024 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/

#include <ctime>
#include <vector>
#include "defs.h"
#include "utilities.h"
#include "framebuffer.h"
#include "colorandmaterials.h"
#include "rasterization.h"
#include "io.h"

FrameBuffer frameBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

void render() {
	frameBuffer.clearColorBuffer();
	drawLine(frameBuffer, 0, 0, 100, 100, red);
	drawLine(frameBuffer, 100, 100, 200, 100, blue);
	frameBuffer.showColorBuffer();
}

void resize(int width, int height) {
	frameBuffer.setFrameBufferSize(width, height);
	glutPostRedisplay();
}

//int main(int argc, char* argv[]) {
//	graphicsInit(argc, argv, __FILE__);
//
//	glutDisplayFunc(render);
//	glutReshapeFunc(resize);
//	glutKeyboardFunc(keyboardUtility);
//	glutMouseFunc(mouseUtility);
//
//	frameBuffer.setClearColor(paleGreen);
//
//	glutMainLoop();
//
//	return 0;
//}

//int main(int argc, char *argv[]) {
//	/*
//	* Test functions of utilities.cpp
//	*/
//
//	// Test swap
//	cout << "Test swap" << endl;
//	double a = 1.0;
//	double b = 2.0;
//	cout << "Before swap: a = " << a << ", b = " << b << endl;
//	swap(a, b);
//	cout << "After swap: a = " << a << ", b = " << b << endl;
//	cout << "Expected: a = 2, b = 1" << endl;
//
//	// Test approximatelyEqual
//	cout << "\nTest approximatelyEqual" << endl;
//	cout << "1.0 and 1.0000000001 are approximately equal: " << approximatelyEqual(1.0, 1.0000000001) << endl;
//	cout << "3.000000 and 3.1 are approximately equal: " << approximatelyEqual(3.000000, 3.1) << endl;
//	cout << "Expected: 1, 0" << endl;
//
//	// Test approximatelyZero
//	cout << "\nTest approximatelyZero" << endl;
//	cout << "0.0000001 is approximately zero: " << approximatelyZero(0.0000001) << endl;
//	cout << "0.1 is approximately zero: " << approximatelyZero(0.1) << endl;
//	cout << "Expected: 1, 0" << endl;
//
//	// Test normalizeDegrees
//	cout << "\nTest normalizeDegrees" << endl;
//	cout << "normalizeDegrees(0) = " << normalizeDegrees(0) << endl;
//	cout << "normalizeDegrees(1.75) = " << normalizeDegrees(1.75) << endl;
//	cout << "normalizeDegrees(-1) = " << normalizeDegrees(-1) << endl;
//	cout << "normalizeDegrees(-721) = " << normalizeDegrees(-721) << endl;
//	cout << "Expected: 0, 1.75, 359, 359" << endl;
//
//	// Test normalizeRadians
//	cout << "\nTest normalizeRadians" << endl;
//	cout << "normalizeRadians(0) = " << normalizeRadians(0) << endl;
//	cout << "normalizeRadians(1) = " << normalizeRadians(1) << endl;
//	cout << "normalizeRadians(3 * PI) = " << normalizeRadians(3 * PI) << endl;
//	cout << "normalizeRadians(-31 * PI) = " << normalizeRadians(-31 * PI) << endl;
//	cout << "Expected: 0, 1, 3.14159, 3.14159" << endl;
//
//	// Test rad2deg
//	cout << "\nTest rad2deg" << endl;
//	cout << "rad2deg(PI) = " << rad2deg(PI) << endl;
//	cout << "rad2deg(2 * PI) = " << rad2deg(2 * PI) << endl;
//	cout << "rad2deg(PI / 2) = " << rad2deg(PI / 2) << endl;
//	cout << "Expected: 180, 360, 90" << endl;
//
//	// Test deg2rad
//	cout << "\nTest deg2rad" << endl;
//	cout << "deg2rad(180) = " << deg2rad(180) << endl;
//	cout << "deg2rad(360) = " << deg2rad(360) << endl;
//	cout << "deg2rad(90) = " << deg2rad(90) << endl;
//	cout << "Expected: 3.14159, 6.28319, 1.5708" << endl;
//
//	// Test min
//	cout << "\nTest min" << endl;
//	cout << "min(1, 2, 3) = " << min(1, 2, 3) << endl;
//	cout << "min(3, 2, 1) = " << min(3, 2, 1) << endl;
//	cout << "min(2, 3, 1) = " << min(2, 3, 1) << endl;
//	cout << "Expected: 1, 1, 1" << endl;
//
//	// Test max
//	cout << "\nTest max" << endl;
//	cout << "max(1, 2, 3) = " << max(1, 2, 3) << endl;
//	cout << "max(3, 2, 1) = " << max(3, 2, 1) << endl;
//	cout << "max(2, 3, 1) = " << max(2, 3, 1) << endl;
//	cout << "Expected: 3, 3, 3" << endl;
//
//	// Test distanceFromOrigin
//	cout << "\nTest distanceFromOrigin" << endl;
//	cout << "distanceFromOrigin(0, 0) = " << distanceFromOrigin(0, 0) << endl;
//	cout << "distanceFromOrigin(1, 0) = " << distanceFromOrigin(1, 0) << endl;
//	cout << "distanceFromOrigin(1, 1) = " << distanceFromOrigin(1, 1) << endl;
//	cout << "Expected: 0, 1, 1, 1.41421" << endl;
//
//	// Test distanceBetween
//	cout << "\nTest distanceBetween" << endl;
//	cout << "distanceBetween(0, 0, 1, 1) = " << distanceBetween(0, 0, 1, 1) << endl;
//	cout << "distanceBetween(1, 1, 0, 0) = " << distanceBetween(1, 1, 0, 0) << endl;
//	cout << "distanceBetween(10, 10, 11, 11) = " << distanceBetween(10, 10, 11, 11) << endl;
//	cout << "distanceBetween(100, 100, 99, 99) = " << distanceBetween(100, 100, 99, 99) << endl;
//	cout << "distanceBetween(54, 1, -34, -99) = " << distanceBetween(54, 1, -34, -99) << endl;
//	cout << "Expected: 1.41421, 1.41421, 1.41421, 1.41421, 133.207" << endl;
//
//	// Test areaOfTriangle
//	cout << "\nTest areaOfTriangle" << endl;
//	cout << "areaOfTriangle(3, 4, 5) = " << areaOfTriangle(3, 4, 5) << endl;		// Valid triangles
//
//	// Illegal triangles
//	cout << "areaOfTriangle(-3, 4, 5) = " << areaOfTriangle(-3, 4, 5) << endl;		// Negative lengths
//	cout << "areaOfTriangle(3, -4, 5) = " << areaOfTriangle(3, -4, 5) << endl;
//	cout << "areaOfTriangle(3, 4, -5) = " << areaOfTriangle(3, 4, -5) << endl;
//
//	cout << "areaOfTriangle(3, 4, 10) = " << areaOfTriangle(3, 4, 10) << endl;		// Triangle inequality
//	cout << "areaOfTriangle(3, 10, 4) = " << areaOfTriangle(3, 10, 4) << endl;
//	cout << "areaOfTriangle(10, 3, 4) = " << areaOfTriangle(10, 3, 4) << endl;
//	cout << "areaOfTriangle(3, 4, 50) = " << areaOfTriangle(3, 4, 50) << endl;
//	cout << "Expected: 6, -1, -1, -1, -1, -1, -1, -1" << endl;
//
//	// Test areaOfTriangle with points
//	cout << "\nTest areaOfTriangle with points" << endl;
//	cout << "areaOfTriangle(0, 0, 3, 0, 0, 4) = " << areaOfTriangle(0, 0, 3, 0, 0, 4) << endl;
//	cout << "Expected: 6" << endl;
//
//	// Test pointOnUnitCircle
//	cout << "\nTest pointOnUnitCircle" << endl;
//	double x = 1;
//	double y = 0;
//	pointOnUnitCircle(0, x, y);
//	cout << "pointOnUnitCircle(0, 1, 0) = (" << x << ", " << y << ")" << endl;
//	x = 0;
//	y = 1;
//	pointOnUnitCircle(90, x, y);
//	cout << "pointOnUnitCircle(90, 0, 1) = (" << x << ", " << y << ")" << endl;
//	cout << "Expected: (1, 0), (-0.448074, 0.893997)" << endl;
//
//	// Test pointOnCircle
//	cout << "\nTest pointOnCircle" << endl;
//	cout << "pointOnCircle(center, 1, 0) = " << pointOnCircle(dvec2(0, 0), 1, 0) << endl;
//	cout << "pointOnCircle(center, 1, 90) = " << pointOnCircle(dvec2(0, 0), 1, 90) << endl;
//	cout << "Expected: [ 1 0 ], [ -0.448074 0.893997 ]" << endl;
//
//	// Test directionInRadians w/ reference point
//	cout << "\nTest directionInRadians" << endl;
//	cout << "directionInRadians(dvec2(0, 0), dvec2(2, 2)) = " << directionInRadians(dvec2(0, 0), dvec2(2, 2)) << endl;
//	cout << "directionInRadians(dvec2(2, 10), dvec2(3, 11)) = " << directionInRadians(dvec2(2, 10), dvec2(3, 11)) << endl;
//	cout << "directionInRadians(dvec2(2, 2), dvec2(2, 0)) = " << directionInRadians(dvec2(2, 2), dvec2(2, 0)) << endl;
//	cout << "directionInRadians(dvec2(1, -1), dvec2(1.3420, -1.93969)) = " << directionInRadians(dvec2(1, -1), dvec2(1.3420, -1.93969)) << endl;
//	cout << "Expected: 0.785398, 0.785398, 4.712388, 5.06144" << endl;
//
//	// Test directionInRadians w/ origin
//	cout << "\nTest directionInRadians" << endl;
//	cout << "directionInRadians(dvec2(2, 2)) = " << directionInRadians(dvec2(2, 2)) << endl;
//	cout << "directionInRadians(dvec2(0, -2)) = " << directionInRadians(dvec2(0, -2)) << endl;
//	cout << "Expected: 0.785398, 4.71239" << endl;
//
//	// Test directionInRadians w/ coordinates
//	cout << "\nTest directionInRadians" << endl;
//	cout << "directionInRadians(0, 0, 2, 2) = " << directionInRadians(0, 0, 2, 2) << endl;
//	cout << "directionInRadians(2, 10, 3, 11) = " << directionInRadians(2, 10, 3, 11) << endl;
//	cout << "directionInRadians(2, 2, 2, 0) = " << directionInRadians(2, 2, 2, 0) << endl;
//	cout << "Expected: 0.785398, 0.785398, 4.71239" << endl;
//
//	// Test map
//	cout << "\nTest map" << endl;
//	cout << "map(2, 0, 5, 10, 11) = " << map(2, 0, 5, 10, 11) << endl;
//	cout << "map(1, 0, 1, 0, 1) = " << map(1, 0, 1, 0, 1) << endl;
//	cout << "map(-5, -10, 0, 0, 100) = " << map(-5, -10, 0, 0, 100) << endl;
//	cout << "map(-1, 0, 5, 10, 11) = " << map(-1, 0, 5, 10, 11) << endl;
//	cout << "map(6, 0, 5, 10, 11) = " << map(6, 0, 5, 10, 11) << endl;
//	cout << "Expected: 10.4, 1, 50, 9.8, 11.2" << endl;
//
//	// Test quadratic w/ vector
//	cout << "\nTest quadratic w/ vector" << endl;
//
//	vector<double> roots = quadratic(1, 4, 3);
//	cout << "quadratic(1, 4, 3) = [";
//	for (int i = 0; i < roots.size(); i++) {
//		cout << roots[i];
//		if (i < roots.size() - 1) {
//			cout << ", ";
//		}
//	}
//	cout << "]" << endl;
//
//	roots = quadratic(1, 0, 0);
//	cout << "quadratic(1, 0, 0) = [";
//	for (int i = 0; i < roots.size(); i++) {
//		cout << roots[i];
//		if (i < roots.size() - 1) {
//			cout << ", ";
//		}
//	}
//	cout << "]" << endl;
//
//	roots = quadratic(-4, -2, -1);
//	cout << "quadratic(-4, -2, -1) = [";
//	for (int i = 0; i < roots.size(); i++) {
//		cout << roots[i];
//		if (i < roots.size() - 1) {
//			cout << ", ";
//		}
//	}
//	cout << "]" << endl;
//
//	cout << "Expected: [-3, -1], [0], []" << endl;
//
//	// Test quadratic w/ array
//	cout << "\nTest quadratic w/ array" << endl;
//	double arr[2];
//
//	int numRoots = quadratic(1, 4, 3, arr);
//	cout << "quadratic(1, 4, 3) = " << numRoots << " and fills in arr with: [" << arr[0] << ", " << arr[1] << "]" << endl;
//
//	numRoots = quadratic(1, 0, 0, arr);
//	cout << "quadratic(1, 0, 0) = " << numRoots << " and fills in arr with: [" << arr[0] << "]" << endl;
//
//	numRoots = quadratic(-4, -2, -1, arr);
//	cout << "quadratic(-4, -2, -1) = " << numRoots << " and does not modify arr." << endl;
//
//	cout << "Expected: 2 and fills in arr with: [-3, -1], 1 and fills in arr with: [0], 0 and does not modify arr." << endl;
//
//	// Test doubleIt
//	cout << "\nTest doubleIt" << endl;
//	dvec2 v = dvec2(1.0, 2.0);
//	dvec2 result = doubleIt(v);
//	cout << "doubleIt(1.0, 2.0) = (" << result.x << ", " << result.y << ")" << endl;
//	cout << "Expected: (2, 4)" << endl;
//
//	// Test myNormalize
//	cout << "\nTest myNormalize" << endl;
//	dvec3 v1 = dvec3(3.0, 4.0, 5.0);
//	dvec3 v2 = dvec3(0.6, 0.8, 1.0);
//	cout << "myNormalize(3.0, 4.0) = (" << myNormalize(v1).x << ", " << myNormalize(v1).y << ", " << myNormalize(v1).z << ")" << endl;
//	cout << "myNormalize(0.6, 0.8) = (" << myNormalize(v2).x << ", " << myNormalize(v2).y << ", " << myNormalize(v2).z << ")" << endl;
//	cout << "Expected: (0.424264, 0.565685, 0.707107), (0.424264, 0.565685, 0.707107)" << endl;
//
//	// Test isOrthogonal
//	cout << "\nTest isOrthogonal" << endl;
//	dvec3 v3 = dvec3(1.0, 0.0, 0.0);
//	dvec3 v4 = dvec3(0.0, 1.0, 0.0);
//	cout << "isOrthogonal(1.0, 0.0, 0.0, 0.0, 1.0, 0.0) = " << isOrthogonal(v3, v4) << endl;
//	cout << "isOrthogonal(1.0, 0.0, 0.0, 1.0, 0.0, 0.0) = " << isOrthogonal(v3, v3) << endl;
//	cout << "Expected: 1, 0" << endl;
//
//	// Test formAcuteAngle
//	cout << "\nTest formAcuteAngle" << endl;
//	dvec3 v5 = dvec3(1.0, 0.0, 0.0);
//	dvec3 v6 = dvec3(0.0, 1.0, 0.0);
//	cout << "formAcuteAngle(1.0, 0.0, 0.0, 0.0, 1.0, 0.0) = " << formAcuteAngle(v5, v6) << endl;
//	cout << "formAcuteAngle(1.0, 0.0, 0.0, 1.0, 0.0, 0.0) = " << formAcuteAngle(v5, v5) << endl;
//	cout << "Expected: 0, 1" << endl;
//
//	// Test cosBetween w/ dvec2
//	cout << "\nTest cosBetween w/ dvec2" << endl;
//	dvec2 v7 = dvec2(1.0, 0.0);
//	dvec2 v8 = dvec2(1.0, 0.0);
//	cout << "cosBetween(1.0, 0.0, 1.0, 0.0) = " << cosBetween(v7, v8) << endl;
//
//	v7 = dvec2(1.0, 0.0);
//	v8 = dvec2(1.0, 1.0);
//	cout << "cosBetween(1.0, 0.0, 1.0, 1.0) = " << cosBetween(v7, v8) << endl;
//
//	v7 = dvec2(-1.0, glm::sqrt(3.0));
//	v8 = dvec2(-1.0, 0.0);
//	cout << "cosBetween(-1.0, sqrt(3.0), -1.0, 0.0) = " << cosBetween(v7, v8) << endl;
//	cout << "Expected: 1, 0.707107, 0.5" << endl;
//
//	// Test cosBetween w/ dvec3
//	cout << "\nTest cosBetween w/ dvec3" << endl;
//	dvec3 v9 = dvec3(1.0, 0.0, 0.0);
//	dvec3 v10 = dvec3(1.0, 0.0, 0.0);
//	cout << "cosBetween(1.0, 0.0, 0.0, 1.0, 0.0, 0.0) = " << cosBetween(v9, v10) << endl;
//
//	v9 = dvec3(1.0, 0.0, 0.0);
//	v10 = dvec3(1.0, 1.0, 0.0);
//	cout << "cosBetween(1.0, 0.0, 0.0, 1.0, 1.0, 0.0) = " << cosBetween(v9, v10) << endl;
//
//	v9 = dvec3(-1.0, glm::sqrt(3.0), 0.0);
//	v10 = dvec3(-1.0, 0.0, 0.0);
//	cout << "cosBetween(-1.0, sqrt(3.0), 0.0, -1.0, 0.0, 0.0) = " << cosBetween(v9, v10) << endl;
//	cout << "Expected: 1, 0.707107, 0.5" << endl;
//
//	// Test cosBetween w/ dvec4
//	cout << "\nTest cosBetween w/ dvec4" << endl;
//	dvec4 v11 = dvec4(1.0, 0.0, 0.0, 0.0);
//	dvec4 v12 = dvec4(1.0, 0.0, 0.0, 0.0);
//	cout << "cosBetween(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0) = " << cosBetween(v11, v12) << endl;
//
//	v11 = dvec4(1.0, 0.0, 0.0, 0.0);
//	v12 = dvec4(1.0, 1.0, 0.0, 0.0);
//	cout << "cosBetween(1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0) = " << cosBetween(v11, v12) << endl;
//
//	v11 = dvec4(-1.0, glm::sqrt(3.0), 0.0, 0.0);
//	v12 = dvec4(-1.0, 0.0, 0.0, 0.0);
//	cout << "cosBetween(-1.0, sqrt(3.0), 0.0, 0.0, -1.0, 0.0, 0.0, 0.0) = " << cosBetween(v11, v12) << endl;
//	cout << "Expected: 1, 0.707107, 0.5" << endl;
//
//	// Test areaOfParallelogram
//	cout << "\nTest areaOfParallelogram" << endl;
//	dvec3 v13 = dvec3(1.0, 0.0, 0.0);
//	dvec3 v14 = dvec3(0.0, 1.0, 0.0);
//	cout << "areaOfParallelogram(1.0, 0.0, 0.0, 1.0, 0.0, 0.0) = " << areaOfParallelogram(v13, v14) << endl;
//
//	v13 = dvec3(1.0, 0.0, 0.0);
//	v14 = dvec3(1.0, 1.0, 0.0);	
//	cout << "areaOfParallelogram(1.0, 0.0, 0.0, 1.0, 1.0, 0.0) = " << areaOfParallelogram(v13, v14) << endl;
//
//	v13 = dvec3(-1.0, glm::sqrt(3.0), 0.0);
//	v14 = dvec3(-1.0, 0.0, 0.0);
//	cout << "areaOfParallelogram(-1.0, sqrt(3.0), 0.0, -1.0, 0.0, 0.0) = " << areaOfParallelogram(v13, v14) << endl;
//	cout << "Expected: 1, 1, 1.73205" << endl;
//
//	// Test areaOfTriangle
//	cout << "\nTest areaOfTriangle" << endl;
//	cout << "areaOfTriangle(3, 4, 5) = " << areaOfTriangle(3, 4, 5) << endl;
//	cout << "areaOfTriangle(-3, 4, 5) = " << areaOfTriangle(-3, 4, 5) << endl;
//	cout << "areaOfTriangle(3, -4, 5) = " << areaOfTriangle(3, -4, 5) << endl;
//	cout << "Expected: 6, -1, -1" << endl;
//
//	// Test pointingVector
//	cout << "\nTest pointingVector" << endl;
//	dvec3 v15 = dvec3(1.0, 0.0, 0.0);
//	dvec3 v16 = dvec3(0.0, 1.0, 0.0);
//	cout << "pointingVector(1.0, 0.0, 0.0, 0.0, 1.0, 0.0) = (" << pointingVector(v15, v16).x << ", " << pointingVector(v15, v16).y << ", " << pointingVector(v15, v16).z << ")" << endl;
//	cout << "Expected: (-0.707107, 0.707107, 0)" << endl;
//
//	return 0;
//}