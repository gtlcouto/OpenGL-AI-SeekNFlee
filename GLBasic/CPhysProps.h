#ifndef _CPhysProps_HG_
#define _CPhysProps_HG_

#include <glm\glm.hpp>

// These are some of the physical properties of the object
// the ones that are only really needed by the Physics updater thingy

class CPhysProps		// for "Physical Properties"
{
public:
	// Render doesn't need to "know" about these...
	glm::vec3 position;  // Both render and physics need this...
	glm::vec3 positionLast;		// Previous location a "moment" ago
	glm::vec3 velocity;			// velocity_X, velocity_Y, velocity_Z     +1, 0, 0
	glm::vec3 accel;		// accel_X, accel_Y, accel_Z;
	bool bInfiniteMass;		// If this is infinite, then the physics integrator ignores the updates on this
};

#endif

