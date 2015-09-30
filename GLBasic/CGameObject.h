#ifndef _CGameObject_HG_
#define _CGameObject_HG_

#include <glm\glm.hpp>
#include "IPhysObject.h"	// also includes #include "CPhysProps.h"
#include <string>

class CGameObject : public IPhysObject	// , IRenderedObject
{
public:
	CGameObject();
	virtual ~CGameObject();		// If you are inheriting, use a virtual destructor

	glm::vec3 position;			// xyz
	glm::vec3 preRotation;		// Is this "orbiting" or "spinning"? Who knows!?!?!
	glm::vec3 postRotation;		// Same! Am I right.
	
	// These are part of the IPhysObject interface
	virtual void SetPhysProps( CPhysProps &PhysProps );
	virtual CPhysProps GetPhysProps( void );
	//void GetPhysProps( CPhysProps &PhysProps );

	//CPhysProps m_PhysProps;
	//// Render doesn't need to "know" about these...
	//glm::vec3 positionLast;		// Previous location a "moment" ago
	//glm::vec3 velocity;			// velocity_X, velocity_Y, velocity_Z     +1, 0, 0
	//glm::vec3 accel;		// accel_X, accel_Y, accel_Z;
	//bool bInfiniteMass;		// If this is infinite, then the physics integrator ignores the updates on this

	float scale;
	bool bIsWireframe;		// if true, then wireframe
	bool bIsVisible;
	glm::vec3 colour;	// Material		

	std::string meshName;	// aka "ply file name"			

	std::string name;	// the "friendly" name

	//unsigned int VBO_ID;	// GLuint "vertex buffer object"
	unsigned int GetID(void);

private:
	
	CPhysProps m_PhysProps;


	unsigned int m_UniqueID;

	static unsigned int m_NextID;
};

#endif 
