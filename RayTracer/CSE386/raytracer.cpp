/****************************************************
 * 2016-2024 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/

#include "raytracer.h"
#include "ishape.h"
#include "io.h"

 /**
  * @fn	RayTracer::RayTracer(const color &defa)
  * @brief	Constructs a raytracers.
  * @param	defa	The clear color.
  */

RayTracer::RayTracer(const color& defa)
	: defaultColor(defa) {
}

/**
 * @fn	void RayTracer::raytraceScene(FrameBuffer &frameBuffer, int depth, const IScene &theScene) const
 * @brief	Raytrace scene
 * @param [in,out]	frameBuffer	Framebuffer.
 * @param 		  	depth	   	The current depth of recursion.
 * @param 		  	theScene   	The scene.
 */

void RayTracer::raytraceScene(FrameBuffer& frameBuffer, int depth,
	const IScene& theScene, int N) {

	color defaultColor = frameBuffer.getClearColor();

	this->initialRecursionDepth = depth;

	for (int y = 0; y < frameBuffer.getWindowHeight(); ++y) {
		for (int x = 0; x < frameBuffer.getWindowWidth(); ++x) {
			DEBUG_PIXEL = (x == xDebug && y == yDebug);
			if (DEBUG_PIXEL) {
				cout << "";
			}
			/* CSE 386 - todo  */
			Ray ray = theScene.camera->getRay(x, y);

			// Check N, if more than 1 do it the new way, otherwise do it the normal way
			if (N > 1) {

				vector<Ray> rays = theScene.camera->getRaysAA(x, y, N);

				color colorForPixel = black;

				for (auto& ray : rays) {

					colorForPixel += traceIndividualRay(ray, theScene, depth);
					frameBuffer.showAxes(x, y, ray, 0.25);
				}

				colorForPixel /= rays.size();
				frameBuffer.setColor(x, y, colorForPixel);
			}
			else {

				Ray ray = theScene.camera->getRay(x, y);
				color colorForPixel = traceIndividualRay(ray, theScene, depth);
				frameBuffer.setColor(x, y, colorForPixel);
			}

			frameBuffer.showAxes(x, y, ray, 0.25);	// Displays R/x, G/y, B/z axes

			//OpaqueHitRecord hit;
			//VisibleIShape::findIntersection(ray, theScene.opaqueObjs, hit);
			//double val = hit.t;
		}
	}

	//frameBuffer.showColorBuffer();
}

/**
 * @fn	static double fresnel(const dvec3& i, const dvec3& n, const double& etai, const double& etat)
 *
 * @brief	Compute Fresnel equation
 *
 * @param	i   	is the incident view direction.
 * @param	n   	is the normal at the intersection point.
 * @param	etai	is the refractive index of the material the light is leaving.
 * @param	etat	is the refractive index of the material the light is entering.
 *
 * @returns	kr is the percentage of light reflected As a consequence of the conservation of
 * 			energy, transmittance is given by: kt = 1 - kr
 * 			https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
 * 			http://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-overview/light-transport-ray-tracing-whitted
 * 			https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf.
 * 			https://www.cs.cornell.edu/courses/cs4620/2012fa/lectures/36raytracing.pdf
 */

static double fresnel(const dvec3& i, const dvec3& n, const double& etai, const double& etat)
{
	// Percentage of light that is reflected
	// Percentage of light that is refracted is equal to 1-kr
	double kr;

	// Calculate the dot product of the incidence vector and the normal 
	// vector of the surface the the light is entering
	double cosi = glm::clamp(-1.0, 1.0, glm::dot(i, n));

	// Compute the sine of the angle of refraction using Snell's law
	double sint = etai / etat * sqrt(glm::max(0.0, 1.0 - cosi * cosi));

	// Check if angle of incidence exceeds critical angle 
	if (sint >= 1.0) {

		kr = 1.0; // Total internal reflection
	}
	else {

		// Calculate the percentage of light that will be reflected
		double cost = sqrt(glm::max(0.0, 1.0 - sint * sint));
		cosi = fabs(cosi);
		double Rs = ((etat * cosi) - (etai * cost)) / ((etai * cost) + (etat * cosi)); // S polarized light (parallel)
		double Rp = ((etai * cosi) - (etat * cost)) / ((etat * cost) + (etai * cosi)); // P polarized light (perpendicular)
		kr = (Rs * Rs + Rp * Rp) / 2.0;
	}

	return kr;

} // end fresnel

/**
 * @fn	color RayTracer::traceIndividualRay(const Ray &ray,
 *											const IScene &theScene,
 *											int recursionLevel) const
 * @brief	Trace an individual ray.
 * @param	ray			  	The ray.
 * @param	theScene	  	The scene.
 * @param	recursionLevel	The recursion level.
 * @return	The color to be displayed as a result of this ray.
 */

color RayTracer::traceIndividualRay(const Ray& ray, const IScene& theScene, int recursionLevel) const {
	/* CSE 386 - todo  */
	OpaqueHitRecord theHit;
	TransparentHitRecord transHit;	// Check for transparent objects

	// Check ray for intersection against all the shapes to find the closest 
	VisibleIShape::findIntersection(ray, theScene.opaqueObjs, theHit);
	TransparentIShape::findIntersection(ray, theScene.transparentObjs, transHit);


	// Check which hit was first
	bool hitOpaque = (theHit.t < transHit.t);


	// Check if there was an intersection
	if (hitOpaque && theHit.t != FLT_MAX) {

		color totalColor = black;	// Could be initialized to hit.material.emissive

		if (theHit.texture != nullptr) {

			color texel = theHit.texture->getPixelUV(theHit.u, theHit.v);

			theHit.material.ambient = 0.15 * texel;
			theHit.material.diffuse = texel;
		}

		for (auto& light : theScene.lights) {

			bool inShadow = light->pointIsInAShadow(theHit.interceptPt, theHit.normal,
				theScene.opaqueObjs, theScene.camera->getFrame());

			color c = light->illuminate(theHit.interceptPt, theHit.normal,
				theHit.material, theScene.camera->getFrame(), inShadow);

			totalColor += c;
		}

		if (recursionLevel > 0) {

			// ********** Reflection and Refraction **************** 
			if (theHit.material.isDielectric == true) {

				double etai, etat;

				if (theHit.rayStatus == ENTERING) {

					etai = 1.0; // Air
					etat = theHit.material.dielectricRefractionIndex;
				}
				else { // closestHit.rayStatus == LEAVING

					etai = theHit.material.dielectricRefractionIndex;
					etat = 1.0; // Air	
				}

				// Calculate the percentage of reflected light
				double kr = fresnel(ray.dir, theHit.normal, etat, etai);

				// Calculate the percentage of refracted (transmitted light) to reflected light
				double kt = (1.0 - kr);

				// Get reflection ray direction
				dvec3 reflection = glm::normalize(glm::reflect(ray.dir, theHit.normal));

				// Create reflection ray
				// Avoid "surface acne"
				Ray reflectRay(theHit.interceptPt + EPSILON * theHit.normal, reflection);

				totalColor += kr * traceIndividualRay(reflectRay, theScene, recursionLevel - 1);

				// Check that this is not a case of total reflection
				if (kr < 1.0) {

					dvec3 refraction = glm::normalize(glm::refract(ray.dir, theHit.normal, etat / etai));

					// Create the refracted ray
					// Avoid "surface acne"
					Ray refractRay = Ray(theHit.interceptPt + EPSILON * -theHit.normal, refraction);

					totalColor += kt * traceIndividualRay(refractRay, theScene, recursionLevel - 1);
				}
			}
			else {

				// ********** Reflection Only ****************
				//dvec3 normal = glm::dot(ray.dir, closestHit.surfaceNormal) > 0 ? closestHit.surfaceNormal : -closestHit.surfaceNormal;

				// Get reflection ray direction
				dvec3 reflection = glm::normalize(glm::reflect(ray.dir, theHit.normal));

				// Create reflection ray
				// Avoid "surface acne"
				Ray reflectionRay(theHit.interceptPt + EPSILON * theHit.normal, reflection);

				// Trace the reflection ray
				totalColor += 1.0 / (2.0 * initialRecursionDepth) * traceIndividualRay(reflectionRay, theScene, recursionLevel - 1);

				if (theHit.material.alpha < 1.0) {

					// Create a ray that is refracted through the material
					dvec3 refractionDir = ray.dir;

					// Create the refracted ray
					Ray refractRay(theHit.interceptPt - EPSILON * theHit.normal, refractionDir);

					// Trace the refracted ray
					color refractColor = traceIndividualRay(refractRay, theScene, recursionLevel);

					// Blend with the illuminated color
					totalColor = (1.0 - theHit.material.alpha) * totalColor + theHit.material.alpha * refractColor;
				}
			}
		}

		return glm::clamp(totalColor, 0.0, 1.0);
	}
	else if (transHit.t < FLT_MAX) {

		// Check if we are hitting a transparent object first
		if (!hitOpaque && transHit.t < FLT_MAX) {

			dvec3 offsetPt = transHit.interceptPt + EPSILON * ray.dir;
			Ray refractedRay(offsetPt, ray.dir);

			color bgColor = traceIndividualRay(refractedRay, theScene, recursionLevel - 1);
			color fgColor = transHit.transColor;

			color blended = (1.0 - transHit.alpha) * bgColor + transHit.alpha * fgColor;

			return glm::clamp(blended, 0.0, 1.0);
		}
	}
	else {

		return defaultColor;
	}
}
