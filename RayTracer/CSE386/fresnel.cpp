// colorandmaterails.h

	// Added to support transparent materials
	// 
	/** @brief	index of refraction for the material  */
	double dielectricRefractionIndex = 1.0;

	/** @brief	True if is dielectric and therefore transmits and reflects light, false if not */
	bool isDielectric = false;
	
// OpaqueHitRecord

enum RAY_STATUS { ENTERING, LEAVING };

	/** @brief	Added to support transparency. Indicates whether if the ray   
	/** is enter an enclosed object or leaving it. Assumes all rays original 
	outside of any transparent object.*/
	
	RAY_STATUS rayStatus = ENTERING;
	
// IShape.cpp

void VisibleIShape::findClosestIntersection(const Ray& ray, OpaqueHitRecord& hit) const {

	shape->findClosestIntersection(ray, hit);

	if (hit.t < FLT_MAX) {
		hit.material = material;
		if (glm::dot(ray.dir, hit.normal) > 0) {
			
			// Reverse the normal vector for correct lighting
			hit.normal = -hit.normal;

			// Assume the ray is leaving the surface
			hit.rayStatus = LEAVING;
		}
		else {
			// The ray is entering the surface
			hit.rayStatus = ENTERING;
		}
		hit.texture = texture;
		if (hit.texture != nullptr) {
			shape->getTexCoords(hit.interceptPt, hit.u, hit.v);
		}
	}
	
} // end VisibleIShape::findClosestIntersection



// raytracer.cpp

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

// tranceIndividualRay

			// ********** Reflection and Refraction **************** 

		if (recursionLevel > 0) {

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
				totalColor += 0.25 * traceIndividualRay(reflectionRay, theScene, recursionLevel - 1);
			}
		}
