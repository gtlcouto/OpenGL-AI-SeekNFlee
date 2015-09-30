#ifndef _CGameObjectManager_HG_
#define _CGameObjectManager_HG_

// This adds things to the "world" or "scene"
// Each object is given a unique identifier 

#include "CGLMeshVBOManager.h"		// For the CMeshManager
#include "CGameObject.h"
#include <string>

class CGameObjectManager 
{
public:
	//CGameObjectManager();
	CGameObjectManager( CGLMeshVBOManager* &pMeshVBOManager );
	~CGameObjectManager();

	// Object placed at the origin, scale = 1.0f, colour = white (1,1,1)
	// Returns unique ID (or 0 if fails)
	unsigned int CreateObjectAndAddToWorld( std::string meshName, std::string &error );
	// Extra information passed into the CMeshInfo object
	// Returns unique ID (or 0 if fails)
	unsigned int CreateObjectAndAddToWorld( CMeshInfo meshInits, std::string &error );

	bool GetRenderedObjects( std::vector< CGameObject* > &vecRenderedObjects );
	// Returns a list of objects that are to be simulated by the "phsyics thingy"
	bool GetPhysicsObjects( std::vector< IPhysObject* > &vecPhysicsObjects );

	// returns zero if we don't find it
	// NOTE: We want to be REALLY careful with this (and WILL change it soon)
	CGameObject* pGetGameObjectByID( unsigned int ID );
	CGameObject* pGetFirstGameObjectByName( std::string name );


	// Maybe we really want something like this...?
	//bool GetRenderedObjects( std::vector< CRenderInfo > &vecRenderedObjects );
	//bool GetPhysicsObjects( std::vector< CPhysInfo > &vecPhsyicsObjects );

private:
	// Note we can't call this because it can't really do it's work without the GLMeshVBOManager
	CGameObjectManager() {};	
	
	// This will check to see if the mesh is already loaded and if not, will load it
	bool m_LoadMeshIfNotPresent( std::string meshName );

	CGLMeshVBOManager* m_pMeshVBOManager;

	std::vector< CGameObject* >  m_vec_pGameObjects;
};

#endif
