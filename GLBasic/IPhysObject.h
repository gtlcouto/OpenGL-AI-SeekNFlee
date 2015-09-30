#ifndef _IPhysObject_HG_
#define _IPhysObject_HG_

#include "CPhysProps.h"

// This is the interface that the "physics" thing will see
class IPhysObject
{
public:
	virtual ~IPhysObject() {};
	// Note the virtual and the "= 0". If ALL of these 
	// methods have this, then it's a "pure virtual" or 
	// "interface class"
	virtual void SetPhysProps( CPhysProps &PhysProps ) = 0;
	virtual CPhysProps GetPhysProps( void ) = 0;

};

#endif

