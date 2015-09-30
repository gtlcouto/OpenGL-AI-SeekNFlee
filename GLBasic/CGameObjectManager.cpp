#include "CGameObjectManager.h"

#include "CGameObject.h"

CGameObjectManager::CGameObjectManager( CGLMeshVBOManager* &pMeshVBOManager )
{
	this->m_pMeshVBOManager = pMeshVBOManager;
	return;
}


CGameObjectManager::~CGameObjectManager()
{
	return;
}

// This will check to see if the mesh is already loaded and if not, will load it
bool CGameObjectManager::m_LoadMeshIfNotPresent( std::string meshName )
{
	// Loaded this already?
	CMeshInfo meshInfo;
	if ( ! this->m_pMeshVBOManager->GetInfoByMeshName( meshName, meshInfo ) )
	{	// Attempt to load it
		if ( ! this->m_pMeshVBOManager->LoadMeshModel( meshName ) )
		{	// Can't load it
			return false;
		}
	}
	// Either we already loaded it, or we just loaded
	return true;
}

// Object placed at the origin, scale = 1.0f, colour = white (1,1,1)
// Returns unique ID (or 0 if fails)
unsigned int CGameObjectManager::CreateObjectAndAddToWorld( std::string meshName, std::string &error )
{
	// Loaded this already?
	if ( ! this->m_LoadMeshIfNotPresent( meshName ) )
	{	// couldn't load it
		return 0;
	}

	CMeshInfo meshInfo;
	if ( ! this->m_pMeshVBOManager->GetInfoByMeshName( meshName, meshInfo ) )
	{	// What the what!?? Should 'never' happen, but...
		return 0;
	}

	meshInfo.meshFileName = meshName;
	unsigned int objectID = this->CreateObjectAndAddToWorld( meshInfo, error );

	// If it didn't work, the call above returned zero
	return objectID;
}

// Extra information passed into the CMeshInfo object
// Returns unique ID (or 0 if fails)
unsigned int CGameObjectManager::CreateObjectAndAddToWorld( CMeshInfo meshInits, std::string &error )
{
	CGameObject* pTempObject = new CGameObject();

	pTempObject->meshName = meshInits.meshFileName;

	this->m_vec_pGameObjects.push_back( pTempObject );

	return pTempObject->GetID();
}

bool CGameObjectManager::GetRenderedObjects( std::vector< CGameObject* > &vecRenderedObjects )
{
	vecRenderedObjects.clear();
	// Set aside enough space (might be faster than allowing vector to grow, if the vector is really big)
	vecRenderedObjects.reserve( this->m_vec_pGameObjects.size() );
	
	for ( int count = 0; count != this->m_vec_pGameObjects.size(); count++ )
	{
		// Check if is Visible, etc.
		if ( this->m_vec_pGameObjects[count]->bIsVisible ) 
		{
			vecRenderedObjects.push_back( this->m_vec_pGameObjects[count] );
		}
	}

	return true;
}

bool CGameObjectManager::GetPhysicsObjects( std::vector< IPhysObject* > &vecPhysicsObjects )
{
	vecPhysicsObjects.clear();
	// Set aside enough space (might be faster than allowing vector to grow, if the vector is really big)
	vecPhysicsObjects.reserve( this->m_vec_pGameObjects.size() );
	
	for ( int count = 0; count != this->m_vec_pGameObjects.size(); count++ )
	{
		// Is this an object that is to be updated with the "integrator"
		// Is the object "infinite mass"
		// Is the object bIncludeInPhysics set to true, tc.
		vecPhysicsObjects.push_back( this->m_vec_pGameObjects[count] );
	}

	return true;
}


CGameObject* CGameObjectManager::pGetGameObjectByID( unsigned int ID )
{
	CGameObject* pGO = 0;
	for ( int index = 0; index != this->m_vec_pGameObjects.size(); index++ )
	{
		// Is this the one?
		if ( this->m_vec_pGameObjects[index]->GetID() == ID )
		{	
			pGO = this->m_vec_pGameObjects[index];
			// Yup, so return it
			return pGO;
		}
	}
	// At this point, we HAVEN'T found the object since 
	// the loop ran all the way through. 
	// But the pointer was set to zero at the start, so return it
	return pGO;
}

CGameObject* CGameObjectManager::pGetFirstGameObjectByName( std::string name )
{
	CGameObject* pGO = 0;

	return pGO;
}

