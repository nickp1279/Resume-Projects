/****************************************************
 * 2016-2024 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/

#include <fstream>
#include <sstream>
#include "eshape.h"
 /**
  * @fn	EShapeData EShape::createEDisk(const Material &mat, int slices)
  * @brief	Creates a disk with radius 1, centered on origin and lying at z = 0
  * @param	mat   	Material.
  * @param	slices	Number of slices.
  * @return	The new disk.
  */

EShapeData EShape::createEDisk(const Material& mat, int slices) {
	EShapeData result;

	double angleInc = TWO_PI / slices;

	for (int i = 0; i < slices; i++) {
		double A1 = i * angleInc;
		double A2 = A1 + angleInc;
		dvec4 A(0.0, 0.0, 0.0, 1.0);
		dvec4 B(std::cos(A1), std::sin(A1), 0.0, 1.0);
		dvec4 C(std::cos(A2), std::sin(A2), 0.0, 1.0);
		VertexData::addTriVertsAndComputeNormal(result, A, B, C, mat);
	}

	return result;
}

/**
 * @fn	EShapeData EShape::createECylinder(const Material &mat, int slices)
 * @brief	Creates cylinder, which is centered on (0,0,0) and aligned with y axis and with
 *			height = 1 and radius = 1
 * @param	mat   	Material.
 * @param	slices	Slices.
 * @return	The new cylinder.
 */

EShapeData EShape::createECylinder(const Material& mat, int slices) {
	/* CSE 386 - todo  */

	//EShapeData result;

	//double sliceInc = TWO_PI / slices;

	//for (int i = 0; i < slices; i++) {
	//	double angle1 = i * sliceInc;
	//	double angle2 = (i + 1) * sliceInc;

	//	dvec4 bottom1(cos(angle1), 0.0, sin(angle1), 1.0);
	//	dvec4 bottom2(cos(angle2), 0.0, sin(angle2), 1.0);

	//	dvec4 top1(cos(angle1), 1.0, sin(angle1), 1.0);
	//	dvec4 top2(cos(angle2), 1.0, sin(angle2), 1.0);

	//	VertexData::addTriVertsAndComputeNormal(result, bottom1, bottom2, top1, mat);
	//	VertexData::addTriVertsAndComputeNormal(result, top1, bottom2, top2, mat);
	//}

	//return result;

	EShapeData data;
	double topY = 0.5;
	double bottomY = -topY;
	double angInc = TWO_PI / slices;

	for (int i = 0; i < slices; i++) {
		double thisAng = i * angInc;
		double nextAng = thisAng + angInc;
		double x1 = glm::cos(thisAng);
		double z1 = glm::sin(thisAng);
		double x2 = glm::cos(nextAng);
		double z2 = glm::sin(nextAng);
		dvec4 thisNormal(x1, 0, z1, 0);
		dvec4 nextNormal(x2, 0, z2, 0);
		thisNormal = glm::normalize(thisNormal);
		nextNormal = glm::normalize(nextNormal);

		dvec2 thisTexCoordBottom(map(thisAng, 0.0, TWO_PI, 0.0, 1.0), 0.0);
		dvec2 thisTexCoordTop(map(thisAng, 0.0, TWO_PI, 0.0, 1.0), 1.0);
		dvec2 nextTexCoordBottom(map(nextAng, 0.0, TWO_PI, 0.0, 1.0), 0.0);
		dvec2 nextTexCoordTop(map(nextAng, 0.0, TWO_PI, 0.0, 1.0), 1.0);

		dvec3 dummyWorldPos(0.0, 0.0, 0.0);

		VertexData c1(dvec4(x1, topY, z1, 1), thisNormal, mat, dummyWorldPos, thisTexCoordTop);
		VertexData c2(dvec4(x1, bottomY, z1, 1), thisNormal, mat, dummyWorldPos, thisTexCoordBottom);
		VertexData c3(dvec4(x2, bottomY, z2, 1), nextNormal, mat, dummyWorldPos, nextTexCoordBottom);
		VertexData c4(dvec4(x2, topY, z2, 1), nextNormal, mat, dummyWorldPos, nextTexCoordTop);
		data.push_back(c1);
		data.push_back(c3);
		data.push_back(c2);
		data.push_back(c1);
		data.push_back(c4);
		data.push_back(c3);
	}
	return data;
}

/**
 * @fn	EShapeData EShape::createECone(const Material &mat, int slices)
 * @brief	Creates cone, which is aligned with y axis. Height and radius = 1
 * @param	mat   	Material.
 * @param	slices	Slices.
 * @return	The new cone.
 */

EShapeData EShape::createECone(const Material& mat, int slices) {
	/* CSE 386 - todo  */
	//EShapeData result;

	//double sliceInc = TWO_PI / slices;
	//dvec4 tipVertex(0.0, 1.0, 0.0, 1.0);

	//for (int i = 0; i < slices; i++) {

	//	double angle1 = i * sliceInc;
	//	double angle2 = angle1 + sliceInc;

	//	dvec4 vertexOne(cos(angle1), 0.0, sin(angle1), 1.0);
	//	dvec4 vertexTwo(cos(angle2), 0.0, sin(angle2), 1.0);

	//	VertexData::addTriVertsAndComputeNormal(result, tipVertex, vertexOne, vertexTwo, mat);
	//}

	//return result;

	EShapeData result;
	double INC = TWO_PI / slices;

	double radius = 1.0;
	double height = 4.0;

	for (int i = 0; i < slices; i++) {

		double A1 = i * INC;
		double A2 = A1 + INC;
		dvec4 tip(0.0, height, 0.0, 1.0);
		dvec4 A(radius * cos(A1), 0.0, radius * sin(A1), 1.0);
		dvec4 B(radius * cos(A2), 0.0, radius * sin(A2), 1.0);

		dvec3 thisfaceNormal = normalFrom3Points(tip.xyz(), B.xyz(), A.xyz());

		result.push_back(VertexData(tip, glm::normalize(tip), mat));

		result.push_back(VertexData(B, glm::normalize(dvec3(cos(A2), atan(radius / height), sin(A2))), mat));

		result.push_back(VertexData(A, glm::normalize(dvec3(cos(A1), atan(radius / height), sin(A1))), mat));

	}
	return result;
}

/**
 * @fn	EShapeData EShape::createETriangle(const Material &mat,
 *											const dvec4& A, const load& B, const dvec4& C)
 * @brief	Creates one triangles from 3 vertices
 * @param	mat	Material.
 * @param	A  	First vertex.
 * @param	B  	Second vertex.
 * @param	C  	Third vertex.
 * @return	The new triangles.
 */

EShapeData EShape::createETriangle(const Material& mat,
	const dvec4& A, const dvec4& B, const dvec4& C) {
	EShapeData result;
	VertexData::addTriVertsAndComputeNormal(result, A, B, C, mat);
	return result;
}

/**
 * @fn	EShapeData EShape::createECheckerBoard(const Material &mat1, const Material &mat2, double WIDTH, double HEIGHT, int DIV)
 * @brief	Creates checker board pattern.
 * @param	mat1  	Material #1.
 * @param	mat2  	Material #2.
 * @param	WIDTH 	Width of overall plane.
 * @param	HEIGHT	Height of overall plane.
 * @param	DIV   	Number of divisions.
 * @return	The vertices in the checker board.
 */

EShapeData EShape::createECheckerBoard(const Material& mat1, const Material& mat2,
	double WIDTH, double HEIGHT, int DIV) {
	EShapeData result;

	const double INC = WIDTH / DIV;
	for (int X = 0; X < DIV; X++) {
		bool isMat1 = X % 2 == 0;
		for (double Z = 0; Z < DIV; Z++) {
			dvec4 V0(-WIDTH / 2.0 + X * INC, 0.0, -WIDTH / 2 + Z * INC, 1.0);
			dvec4 V1 = V0 + dvec4(0.0, 0.0, INC, 0.0);
			dvec4 V2 = V0 + dvec4(INC, 0.0, INC, 0.0);
			dvec4 V3 = V0 + dvec4(INC, 0.0, 0.0, 0.0);
			const Material& mat = isMat1 ? mat1 : mat2;
			dvec4 Y(0, 1, 0, 0);
			result.push_back(VertexData(V0, Y, mat));
			result.push_back(VertexData(V1, Y, mat));
			result.push_back(VertexData(V2, Y, mat));

			result.push_back(VertexData(V2, Y, mat));
			result.push_back(VertexData(V3, Y, mat));
			result.push_back(VertexData(V0, Y, mat));
			isMat1 = !isMat1;
		}
	}
	return result;
}

// This code provided by Jack Duval
EShapeData EShape::createEObj(const string& filename) {
	EShapeData result;
	std::ifstream in(filename);
	if (!in.is_open()) {
		cout << "Error: Cannot open file " << filename << endl;
		return result;
	}

	// Loads all the vertices into a vector and faces into a vector
	vector<dvec4> vertices;
	vector<glm::ivec3> faces;
	std::string line;
	int faceCount = 0;
	while (std::getline(in, line)) {
		if (line.substr(0, 2) == "v ") {
			std::istringstream s(line.substr(2));
			double x, y, z;
			s >> x >> y >> z;
			vertices.push_back(dvec4(x, y, z, 1.0));
		} else if (line.substr(0, 2) == "f ") {
			// Splits the line into a vector of strings
			std::istringstream s(line.substr(2));
			std::vector<std::string> face;
			std::string temp;
			while (s >> temp) {
				face.push_back(temp);
			}

			// Faces are read as f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3. 
			// We need to convert them to f v1 v2 v3
			for (int i = 0; i < face.size(); i++) {
				std::istringstream s(face[i]);
				int v;
				s >> v;
				if (i == 0) {
					faces.push_back(glm::ivec3(v, 0, 0));
				} else if (i == 1) {
					faces.at(faceCount) = glm::ivec3(faces.at(faceCount).x, v, 0);
				} else if (i == 2) {
					faces.at(faceCount) = glm::ivec3(faces.at(faceCount).x, faces.at(faceCount).y, v);
				}
			}
			faceCount++;
		}
	}

	// Constructs the triangles from the vertices
	for (int i = 0; i < faces.size(); i++) {
		Material mat = redPlastic;
		dvec4 A = vertices.at(faces.at(i).x - 1);
		dvec4 B = vertices.at(faces.at(i).y - 1);
		dvec4 C = vertices.at(faces.at(i).z - 1);
		VertexData::addTriVertsAndComputeNormal(result, A, B, C, mat);
	}

	return result;
}