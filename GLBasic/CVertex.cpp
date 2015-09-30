#include "CVertex.h"

CVertex::CVertex()
{
	this->x = 0.0f; 
	this->y = 0.0f;
	this->z = 0.0f;
	this->confidence = 0.0f;
	this->intensity = 0.0f;
	return;
}

CVertex::~CVertex()
{
	return;
}
