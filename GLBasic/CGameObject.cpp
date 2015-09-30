#include "CGameObject.h"

//static 
unsigned int CGameObject::m_NextID = 300;		// 4,000,000,000;

CGameObject::CGameObject()
{
	this->bIsWireframe = false;
	this->scale = 1.0f;
	//
	//this->VBO_ID = 0;	// Not sure

	// If you are a pessimist (or realist)
	this->position = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->preRotation = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->postRotation = glm::vec3( 0.0f, 0.0f, 0.0f );
	this->colour = glm::vec3( 0.0f, 0.0f, 0.0f );	// black

	//this->bInfiniteMass = false;
	this->m_PhysProps.bInfiniteMass = false;

	this->m_UniqueID = CGameObject::m_NextID;
	CGameObject::m_NextID++;

	return;
}

CGameObject::~CGameObject()
{
	return;
}

unsigned int CGameObject::GetID(void)
{
	return this->m_UniqueID;
}

void CGameObject::SetPhysProps( CPhysProps &PhysProps )
{
	this->m_PhysProps = PhysProps;
	// Update the shared position value (temporary becasue we need an interface for render, too)
	this->position = this->m_PhysProps.position;
	return;
}

CPhysProps CGameObject::GetPhysProps( void )
{
	this->m_PhysProps.position = this->position;
	return this->m_PhysProps;
}


