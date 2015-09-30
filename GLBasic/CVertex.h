#ifndef _CVertex_HG_
#define _CVertex_HG_

class CVertex
{
public: 
	CVertex();
	//CVertex( float x, float y, float z );
	~CVertex();
	float x;
	float y;
	float z;
	float confidence;	// Ignore
	float intensity;	// Ignore
};

#endif 
