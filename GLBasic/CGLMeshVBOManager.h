#ifndef _CGLMeshVBOManager_HG_
#define _CGLMeshVBOManager_HG_

#include <string>
#include <GL/GLEW.h>		// For the GLuint

#include <vector>
#include "CVertex.h"
#include "CTriangle.h"

// This will:
// - Load many ply (model) files
// - Store the information about the models (CMeshInfo)
// - Create the VBOs needed to render them
// - Do lookups from model names to VBO and mesh information

class CMeshInfo
{
public:
	CMeshInfo() : maxX(0.0f), maxY(0.0f), maxZ(0.0f), 
		          minX(0.0f), minY(0.0f), minZ(0.0f),
				  extentX(0.0f), extentY(0.0f), extentZ(0.0f),
				  maxExtent(0.0f),
				  scaleToUnitCube( 1.0f ),
				  VBO_ID (0), VertBuffer_ID(0), IndexBuffer_ID(0),
				  numberOfVertices(0), numberOfTriangles(0)	{};
	~CMeshInfo() {};
	float maxX, maxY, maxZ, minX, minY, minZ;
	float extentX, extentY, extentZ;
	float maxExtent;
	float scaleToUnitCube;	
	std::string meshFileName;

	GLuint VBO_ID;
	GLuint VertBuffer_ID;			// Added this, too
	GLuint IndexBuffer_ID;			// Needed for the DeleteBuffer when shut down
	unsigned int numberOfTriangles;
	unsigned int numberOfVertices;	// We might need this...who knows?
};

class CGLMeshVBOManager
{
public:

	CGLMeshVBOManager();
	~CGLMeshVBOManager();

	// Loads a model. If we haven't seen it, then make a new VBO
	bool LoadMeshModel( std::string fileName );
	bool LoadMeshModels( std::vector< std::string > vecFileNames, std::string &error );		// ADDED

	// Look up of the mesh name (file name) we loaded
	bool GetInfoByMeshName( std::string meshName, CMeshInfo &meshInfo );

	// Delete all the buffers
	void ShutErDown(void);

private:
	bool m_LoadObjFile(std::string fileName);
	bool m_LoadPlyFile( std::string fileName );
	void m_CalcCurrentExtents( CMeshInfo &MeshInfo );
	// Assumes that g_vecVertices and g_vecTriangels have something
	bool m_LoadIntoVBO( CMeshInfo &meshInfo );

	std::vector<CVertex> m_vecVertices;		// "array" of vertics
	std::vector<CTriangle> m_vecTriangles;		// "array" of vertics
	unsigned int m_numVertices;
	unsigned int m_numTriangles;

	//GLuint VBO_ID;// = 0;
	//GLuint VertBuf_ID;// = 0;
	//GLuint IndexBuf_ID;// = 0;

	bool m_bGetGLError( std::string &errortext );
	bool m_bGetGLError( void );

	// This will be replaced by a "map" objects
	// "Dictionary": array that is indexed by something like a string
	std::vector< CMeshInfo > m_vecMeshes;
	//std::map< std::string /*filename*/, CMeshInfo > m_mapMeshes;

};

#endif // CGLMeshVBOManager


