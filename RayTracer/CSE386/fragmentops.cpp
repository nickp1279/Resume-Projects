/****************************************************
 * 2016-2024 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/

#include <vector>
#include "fragmentops.h"

FogParams FragmentOps::fogParams;
bool FragmentOps::performDepthTest = true;
bool FragmentOps::depthBufferWriteEnabled = true;
bool FragmentOps::colorBufferWriteEnabled = true;
bool FragmentOps::textureMappingEnabled = false;
Image* FragmentOps::textureImage = nullptr;

/**
 * @fn	double FogParams::fogFactor(const dvec3 &fragPos, const dvec3 &eyePos) const
 * @brief	Computes fog factor - f.
 * @param	fragPos	The fragment position.
 * @param	eyePos 	The eye position.
 * @return	The fog factor - f.
 */

double FogParams::fogFactor(const dvec3& fragPos, 
	const dvec3& eyePos) const {

	double fogFactor = 1.0;
	double distanceFromViewPoint = glm::distance(fragPos, eyePos);

	switch (type) {
	case FogType::LINEAR_FOG:
		fogFactor = glm::clamp((end - distanceFromViewPoint) / (end - start), 0.0, 1.0);
		break;
	case FogType::EXPONENTIAL_FOG:
		fogFactor = glm::exp(-density * distanceFromViewPoint);
		break;
	case FogType::EXPONENTIAL_2_FOG:
		fogFactor = glm::exp(-glm::pow(density * distanceFromViewPoint, 2));
		break;
	case FogType::NO_FOG:
		break;
	}
	return fogFactor;
}

/**
 * @fn	color FragmentOps::applyFog(const color &destColor, const dvec3 &eyePos, const dvec3 &fragPos)
 * @brief	Applies fog to a fragment.
 * @param	destColor	Destination color.
 * @param	eyePos   	Eye position.
 * @param	fragPos  	Fragment position.
 * @return	The color after applying the fog.
 */

color FragmentOps::applyFog(const color& destColor,
	const dvec3& eyePos, const dvec3& fragPos) {

	double fogFactor = fogParams.fogFactor(fragPos, eyePos);
	return weightedAverage(fogFactor, destColor, 1.0 - fogFactor, fogParams.color);
}

/**
 * @fn	color FragmentOps::applyBlending(double alpha, const color &srcColor, const color &destColor)
 * @brief	Applies blending to a fragment.
 * @param	alpha	 	Alpha value.
 * @param	srcColor 	Source color.
 * @param	destColor	Destination color.
 * @return	The blended color.
 */

color FragmentOps::applyBlending(double alpha, const color& srcColor, const color& destColor) {
	/* CSE 386 - todo  */
	return weightedAverage(alpha, srcColor, 1.0 - alpha, destColor);
}

/**
 * @fn	void FragmentOps::processFragment(FrameBuffer &frameBuffer,
 *											const dvec3 &eyePositionInWorldCoords,
 *											const vector<LightSourcePtr> lights,
 *											const Fragment &fragment,
 *											const dmat4 &viewingMatrix)
 * @brief	Process the fragment, leaving the results in the framebuffer.
 * @param [in,out]	frameBuffer	                The frame buffer
 * @param 		  	eyePositionInWorldCoords	The eye position in world coordinates.
 * @param 		  	lights						Vector of lights in scene.
 * @param 		  	fragment					Fragment to be processed.
 * @param           eyeFrame                    The camera's frame.
 */

 void FragmentOps::processFragment(FrameBuffer &frameBuffer, const dvec3 &eyePositionInWorldCoords,
 										const vector<LightSourcePtr> lights,
 										const Fragment &fragment,
 										const Frame &eyeFrame) {

 //	const dvec3 &eyePos = eyePositionInWorldCoords;
 //
 //	double Z = fragment.windowPos.z;
 //	int X = (int)fragment.windowPos.x;
 //	int Y = (int)fragment.windowPos.y;
 //	DEBUG_PIXEL = (X == xDebug && Y == yDebug);

	//color C = fragment.material.diffuse;
	//frameBuffer.setColor(X, Y, C);
	//frameBuffer.setDepth(X, Y, Z);

	 const dvec3& eyePos = eyePositionInWorldCoords;

	 double Z = fragment.windowPos.z;
	 int X = (int)fragment.windowPos.x;
	 int Y = (int)fragment.windowPos.y;
	 DEBUG_PIXEL = (X == xDebug && Y == yDebug);

	 // Final color already lit
	 color litColor = fragment.color;

	 // Apply fog
	 color foggedColor = applyFog(litColor, eyePos, fragment.worldPos);

	 // Apply alpha blending
	 color finalColor = applyBlending(fragment.material.alpha, foggedColor, frameBuffer.getColor(X, Y));

	 // Hidden surface check
	 if (!performDepthTest || Z < frameBuffer.getDepth(X, Y)) {
		 if (colorBufferWriteEnabled) {
			 frameBuffer.setColor(X, Y, finalColor);
		 }
		 if (depthBufferWriteEnabled) {
			 frameBuffer.setDepth(X, Y, Z);
		 }
	 }
 }