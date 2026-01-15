/****************************************************
 * 2016-2024 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/

#include "light.h"
#include "io.h"
#include "ishape.h"

 /**
  * @fn	color ambientColor(const color &matAmbient, const color &lightColor)
  * @brief	Computes the ambient color produced by a single light at a single point.
  * @param	mat  	Ambient material property.
  * @param	lightColor	Light's color.
  * @return	Ambient color.
   */

color ambientColor(const color& ambientMatColor, const color& lightColor) {
	/* CSE 386 - todo  */
	return ambientMatColor * lightColor;
}

/**
 * @fn	color diffuseColor(const color &mat, const color &light, const dvec3 &l, const dvec3 &n)
 * @brief	Computes diffuse color produce by a single light at a single point.
 * @param	mat		 	Material.
 * @param	lightColor	 	The light's color.
 * @param	l		 	Light vector.
 * @param	n		 	Normal vector.
 * @return	Diffuse color.
 */

color diffuseColor(const color& diffuseMatColor, const color& lightColor,
	const dvec3& l, const dvec3& n) {
	/* CSE 386 - todo  */
	return glm::max(glm::dot(l, n), 0.0)* diffuseMatColor * lightColor;

	// Can do either implementation
	/*return glm::clamp(glm::dot(l, n), 0.0, 1.0) * diffuseMatColor * lightColor;*/
}

/**
 * @fn	color specularColor(const color &mat, const color &lightColor, double shininess,
 *							const dvec3 &r, const dvec3 &v)
 * @brief	Computes specular color produce by a single light at a single point.
 * @param	mat		 	Material.
 * @param	lightColor	 	The light's color.
 * @param	shininess	Material shininess.
 * @param	r		 	Reflection vector.
 * @param	v		 	Viewing vector.
 * @return	Specular color.
 */

color specularColor(const color& specularMatColor, const color& lightColor,
	double shininess,
	const dvec3& r, const dvec3& v) {
	/* CSE 386 - todo  */
	return lightColor * specularMatColor * glm::pow(glm::max(glm::dot(r, v), 0.0), shininess);
}

/**
 * @fn	color totalColor(const Material &mat, const color &lightColor,
 *						const dvec3 &viewingDir, const dvec3 &normal,
 *						const dvec3 &lightPos, const dvec3 &intersectionPt,
 *						bool attenuationOn, const LightAttenuationParameters &ATparams)
 * @brief	Color produced by a single light at a single point. This includes the summation
 * of the ambient, diffuse, and specular components. Attenuation is also applied, if applicable.
 * @param	mat			  	Material.
 * @param	lightColor	  	The light's color.
 * @param	v	  			The v vector.
 * @param	n   		  	Normal vector.
 * @param	lightPos	  	Light position.
 * @param	intersectionPt	(x,y,z) of intersection point.
 * @param	attenuationOn 	true if attenuation is on.
 * @param	ATparams	  	Attenuation parameters.
 * @return	Color produced by a single light at a single point. This includes the summation
 * of the ambient, diffuse, and specular components. Attenuation is also applied, if applicable.
 */

color totalColor(const Material& mat, const color& lightColor,
	const dvec3& v, const dvec3& n,
	const dvec3& lightPos, const dvec3& intersectionPt,
	bool attenuationOn,
	const LightATParams& ATparams) {
	/* CSE 386 - todo  */

	dvec3 lightVector = glm::normalize(lightPos - intersectionPt);
	dvec3 reflectionVector = glm::reflect(-lightVector, n);
	double distance = glm::distance(lightPos, intersectionPt);

	double atenFactor = attenuationOn ? ATparams.factor(distance) : 1.0;

	color ambientReflection = ambientColor(mat.ambient, lightColor);
	color diffuseReflection = diffuseColor(mat.diffuse, lightColor, lightVector, n);
	color specularReflection = specularColor(mat.specular, lightColor, mat.shininess, 
		reflectionVector, v);

	// Check if components are greater than 1 and if they are cap them at 1
	color totalColor = ambientReflection + atenFactor * (diffuseReflection + specularReflection);

	return glm::min(totalColor, color(1.0, 1.0, 1.0));
}

/**
 * @fn	color PositionalLight::illuminate(const dvec3 &interceptWorldCoords,
 *										const dvec3 &normal, const Material &material,
 *										const Frame &eyeFrame, bool inShadow) const
 * @brief	Computes the color this light produces in RAYTRACING applications.
 * @param	interceptWorldCoords	(x, y, z) at the intercept point.
 * @param	normal				The normal vector.
 * @param	material			The object's material properties.
 * @param	eyeFrame			The coordinate frame of the camera.
 * @param	inShadow			true if the point is in a shadow.
 * @return	The color produced at the intercept point, given this light.
 */

color PositionalLight::illuminate(const dvec3& interceptWorldCoords,
	const dvec3& normal,
	const Material& material,
	const Frame& eyeFrame,
	bool inShadow) const {
	/* CSE 386 - todo  */

	if (!isOn) {
		return black;
	}
	if (inShadow) {
		return ambientColor(material.ambient, this->lightColor);
	}

	dvec3 v = glm::normalize(eyeFrame.origin - interceptWorldCoords);

	// Can replace this->actualPosition() with this->pos
	color colorForThisLight = totalColor(material, this->lightColor,
		v, normal, this->actualPosition(eyeFrame), interceptWorldCoords, 
		this->attenuationIsTurnedOn, this->atParams);

	return colorForThisLight;
}

/*
* @fn PositionalLight::actualPosition(const Frame& eyeFrame) const
* @brief	Returns the global world coordinates of this light.
* @param	eyeFrame	The camera's frame
* @return	The global world coordinates of this light. This will be the light's
*			raw position. Or, it will be the position relative to the camera's
*			frame (transformed from camera's frame into the world coordinate frame).
*/

dvec3 PositionalLight::actualPosition(const Frame& eyeFrame) const {
	return isTiedToWorld ? pos : eyeFrame.frameCoordsToGlobalCoords(pos);
}

/**
* @fn	bool PositionalLight::pointIsInAShadow(const dvec3& intercept, const dvec3& normal, const vector<VisibleIShapePtr>& objects, const Frame& eyeFrame) const
* @brief	Determines if an intercept point falls in a shadow.
* @param	intercept	the position of the intercept.
* @param	normal		the normal vector at the intercept point
* @param	objects		the collection of opaque objects in the scene
*/

bool PositionalLight::pointIsInAShadow(const dvec3& intercept,
	const dvec3& normal,
	const vector<VisibleIShapePtr>& objects,
	const Frame& eyeFrame) const {
	/* CSE 386 - todo  */
	OpaqueHitRecord shadowHit;

	/*Ray shadowFeeler(intercept + EPSILON * normal, this->pos - intercept);*/ // This is another way to do it

	Ray shadowFeeler = getShadowFeeler(intercept, normal, eyeFrame);

	VisibleIShape::findIntersection(shadowFeeler, objects, shadowHit);

	if (shadowHit.t < glm::distance(this->actualPosition(eyeFrame), intercept)) {
		
		return true;
	}
	else {
		return false;
	}
}

/**
* @fn	Ray PositionalLight::getShadowFeeler(const dvec3& interceptWorldCoords, const dvec3& normal, const Frame &eyeFrame) const
* @brief	Returns the shadow feeler for this light.
* @param	interceptWorldCoords	the position of the intercept.
* @param	normal		The normal vector at the intercept point
* @param	eyeFrame	The coordinate frame of the camera.
*/

Ray PositionalLight::getShadowFeeler(const dvec3& interceptWorldCoords,
	const dvec3& normal,
	const Frame& eyeFrame) const {
	/* 386 - todo */
	dvec3 origin = interceptWorldCoords + EPSILON * normal;
	dvec3 dir = this->actualPosition(eyeFrame) - interceptWorldCoords;

	Ray shadowFeeler(origin, dir);

	return shadowFeeler;
}

/**
 * @fn	color SpotLight::illuminate(const dvec3 &interceptWorldCoords,
 *									const dvec3 &normal, const Material &material,
 *									const Frame &eyeFrame, bool inShadow) const
 * @brief	Computes the color this light produces in raytracing applications.
 * @param	interceptWorldCoords				The surface properties of the intercept point.
 * @param	normal					The normal vector.
 * @param	material			The object's material properties.
 * @param	eyeFrame			The coordinate frame of the camera.
 * @param	inShadow			true if the point is in a shadow.
 * @return	The color produced at the intercept point, given this light.
 */

color SpotLight::illuminate(const dvec3& interceptWorldCoords,
	const dvec3& normal,
	const Material& material,
	const Frame& eyeFrame,
	bool inShadow) const {

	/* CSE 386 - todo  */
	if (isInSpotlightCone(pos, this->spotDir, this->fov, interceptWorldCoords)) { // If in the cone, illuminate it

		double spotCutOffCos = glm::cos(this->fov / 2.0); // Note: spotFOV is stored in radians
		dvec3 negLightVec = glm::normalize(interceptWorldCoords - pos);
		double spotCos = glm::dot(negLightVec, spotDir);

		double fallOffFactor = 1.0 - (1 - spotCos) / (1 - spotCutOffCos);

		/*return fallOffFactor * PositionalLight::illuminate(interceptWorldCoords, normal, material,
			eyeFrame, inShadow);*/

		return PositionalLight::illuminate(interceptWorldCoords, normal, material,
			eyeFrame, inShadow);
	}
	else {

		return black;
	}
}

/**
* @fn	void setDir (double dx, double dy, double dz)
* @brief	Sets the direction of the spotlight.
* @param	dx		x component of the direction
* @param	dy		y component of the direction
* @param	dz		z component of the direction
*/

void SpotLight::setDir(double dx, double dy, double dz) {
	spotDir = glm::normalize(dvec3(dx, dy, dz));
}

/**
* @fn	SpotLight::isInSpotlightCone(const dvec3& spotPos, const dvec3& spotDir, double spotFOV, const dvec3& intercept)
* @brief	Determines if an intercept point falls within a spotlight's cone.
* @param	spotPos		where the spotlight is positioned
* @param	spotDir		normalized direction of spotlight's pointing direction
* @param	spotFOV		spotlight's field of view, which is 2X of the angle from the viewing axis
* @param	intercept	the position of the intercept.
*/

bool SpotLight::isInSpotlightCone(const dvec3& spotPos,
	const dvec3& spotDir,
	double spotFOV,
	const dvec3& intercept) {
	/* CSE 386 - todo  */

	double spotCutOffCos = glm::cos(spotFOV / 2.0); // Note: spotFOV is stored in radians
	dvec3 negLightVec = glm::normalize(intercept - spotPos);
	double spotCos = glm::dot(negLightVec, spotDir);

	if (spotCos > spotCutOffCos) {

		return true;
	}
	else {

		return false;
	}

	return false;
}

/**
* 
*/

color DirectionalLight::illuminate(const dvec3& interceptWorldCoords,
	const dvec3& normal,
	const Material& material,
	const Frame& eyeFrame,
	bool inShadow) const {

	// If light is off just return black
	if (!isOn) {

		return black;
	}

	// Even if in the shadow the ambient contributes
	color ambientReflection = ambientColor(material.ambient, lightColor);

	if (inShadow) {

		return ambientReflection;
	}

	// Remember to reverse the direction
	dvec3 l = glm::normalize(-this->dir);
	dvec3 v = glm::normalize(eyeFrame.origin - interceptWorldCoords);
	dvec3 r = glm::reflect(-l, normal);

	// Calculate colors based on vectors 
	color diffuseReflection = diffuseColor(material.diffuse, lightColor, l, normal);
	color specularReflection = specularColor(material.specular, lightColor,
		material.shininess, r, v);

	// Add all colors up
	color total = ambientReflection + diffuseReflection + specularReflection;

	return glm::min(total, color(1.0, 1.0, 1.0));
}

/**
*/

Ray DirectionalLight::getShadowFeeler(const dvec3& interceptWorldCoords,
	const dvec3& normal,
	const Frame& eyeFrame) const {

	dvec3 origin = interceptWorldCoords + EPSILON * normal;
	dvec3 dir = -glm::normalize(this->dir);

	return Ray(origin, dir);
}

/**
*/

bool DirectionalLight::pointIsInAShadow(const dvec3& intercept,
	const dvec3& normal,
	const vector<VisibleIShapePtr>& objects,
	const Frame& eyeFrame) const {

	Ray shadowFeeler = getShadowFeeler(intercept, normal, eyeFrame);
	OpaqueHitRecord shadowHit;

	VisibleIShape::findIntersection(shadowFeeler, objects, shadowHit);

	return shadowHit.t != std::numeric_limits<double>::max();
}