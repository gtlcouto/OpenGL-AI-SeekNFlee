#include "CGLMeshVBOManager.h"
#include <fstream>

#include "Vertex.h"	// What the shader wants, what it really, really wants.

CGLMeshVBOManager::CGLMeshVBOManager()
{
	// TODO: Insert sexy code here
	return;
}

CGLMeshVBOManager::~CGLMeshVBOManager()
{
	// Maybe some code here, too
	return;
}

bool CGLMeshVBOManager::GetInfoByMeshName( std::string meshName, CMeshInfo &meshInfo )
{
	// A MESS, need to fix
	//meshInfo.VBO_ID = this->VBO_ID;
	//meshInfo.numberOfTriangles = this->g_numTriangles;

	// Scan through the vector of CMeshInfo objects until we find the one 
	//	we are looking for (or not)
	// std::vector< CMeshInfo > m_vecMeshes;
	for ( int index = 0; index != this->m_vecMeshes.size(); index++ )
	{
		if ( this->m_vecMeshes[index].meshFileName == meshName )
		{	// Hazzah!! We have found it!!
			// Copy the information from the vector location to the object we passed in
			meshInfo = this->m_vecMeshes[index];
			// Let's get outta here!
			return true;
		}
	}

	// Oh no Batman, Holy Missing Mesh File!
	return false;	// Everything ends in tears...
}

bool CGLMeshVBOManager::LoadMeshModel( std::string fileName )
{
	// What happen if we've already loaded this model...
	// How about we ignore the call (as the model is already loaded)
	CMeshInfo curMesh;
	if (this->GetInfoByMeshName(fileName, curMesh))
	{	// Already loaded it, yo!
		// Don't need to load it again
		return true;
	}

	// It's a new model, so load it
	curMesh.meshFileName = fileName;

	if (fileName.substr(fileName.find_last_of(".") + 1) == "ply") {
		// Forgot to negate this... doh!
		if (!this->m_LoadPlyFile(fileName))
		{
			return false;
		}

	}
	else if (fileName.substr(fileName.find_last_of(".") + 1) == "obj") {
		if (!this->m_LoadObjFile(fileName))
		{
			return false;
		}
	}
	// Now we know how many triangles we have loaded
	curMesh.numberOfTriangles = this->m_numTriangles;
	curMesh.numberOfVertices = this->m_numVertices;


	if ( ! this->m_LoadIntoVBO(curMesh) )
	{
		return false;
	}
	//curMesh.VBO_ID = this->VBO_ID;

	// TODO: Calculate the extents, max, min, scale, etc.
	this->m_vecMeshes.push_back( curMesh );


	return true;
}

// *************************************************************
// Very "good enough for rock-n-roll" copy and paste thing...
bool CGLMeshVBOManager::m_LoadPlyFile( std::string fileName )
{
	this->m_vecTriangles.clear();
	this->m_vecVertices.clear();
	this->m_numTriangles = 0;
	this->m_numVertices = 0;

	// TODO: Insert magic here...
	std::ifstream plyFile( fileName.c_str() );		// ASCII (non-binary)
	if ( ! plyFile.is_open() )
	{
		return false;
	}
	// We are good to go... file is open
	// Read until we hit "vertex", then read next number
	std::string temp;
	while ( plyFile >> temp )
	{
		if ( temp == "vertex" )	{ break; }
	}
	// Next thing we read is the number of vertices
	plyFile >> this->m_numVertices;

	// Read until "face"
	while ( plyFile >> temp )
	{
		if ( temp == "face" )	{ break; }
	}
	plyFile >> this->m_numTriangles; 

	// Read until "end_header"
	while ( plyFile >> temp )
	{
		if ( temp == "end_header" )	{ break; }
	}
	// The next thing is the first x value of the 1st vertex
	for ( int count = 0; count != this->m_numVertices; count++ )
	{
		// -0.0369122    0.127512    0.00276757    0.850855    0.5 
		CVertex tempVertex;
		plyFile >> tempVertex.x;
		plyFile >> tempVertex.y;
		plyFile >> tempVertex.z;
//		plyFile >> tempVertex.confidence;
//		plyFile >> tempVertex.intensity;
		// Scale the bunny while I'm loading it...
		//tempVertex.x *= 5.0f;
		//tempVertex.y *= 5.0f;
		//tempVertex.z *= 5.0f;

		this->m_vecVertices.push_back( tempVertex );
	}





	// Load triangles...
	for ( int count = 0; count != this->m_numTriangles; count++ )
	{
		CTriangle tempTri;
		int discard = 0;
		// 3 4 132 80
		plyFile >> discard;
		plyFile >> tempTri.v1;
		plyFile >> tempTri.v2;
		plyFile >> tempTri.v3;
		// Add triangle to vector
		this->m_vecTriangles.push_back( tempTri );
	}

	return true;
}

bool CGLMeshVBOManager::m_LoadObjFile(std::string fileName)
{
	this->m_vecTriangles.clear();
	this->m_vecVertices.clear();
	this->m_numTriangles = 0;
	this->m_numVertices = 0;

	std::ifstream objFile(fileName.c_str());		// ASCII (non-binary)
	if (!objFile.is_open())
	{
		return false;
	}
	//File is open

	std::string temp;
	while (objFile >> temp)
	{
		//if it hits v then we want to store the vertex coordinates x  y  z
		//v 8.576171875 82.548828125 11.732421875
		if (temp == "v") {
			CVertex tempVertex;
			objFile >> tempVertex.x;
			objFile >> tempVertex.y;
			objFile >> tempVertex.z;
			this->m_vecVertices.push_back(tempVertex);
		}
		else if (temp == "f") {
			CTriangle tempTri;
			//f 1167/1167/1167 1166/1166/1166 1197/1197/1197
			//   keep discard  keep discard  keep discard
			std::string discard;
			unsigned int tempInt;
			objFile >> tempInt;
			tempTri.v1 = tempInt - 1;
			objFile >> discard;
			objFile >> tempInt;
			tempTri.v2 = tempInt - 1;
			objFile >> discard;
			objFile >> tempInt;
			tempTri.v3 = tempInt - 1;
			objFile >> discard;
			this->m_vecTriangles.push_back(tempTri);
		}
	}

	this->m_numVertices = this->m_vecVertices.size();
	this->m_numTriangles = this->m_vecTriangles.size();

	return true;
}

// Takes whatever is in the vecTriangles and vecVertices and 
// put them into a new VBO
bool CGLMeshVBOManager::m_LoadIntoVBO( CMeshInfo &meshInfo )
{
	// Sorry, forgot to call this...
	this->m_CalcCurrentExtents( meshInfo );

	unsigned int vertexArrayAlignedSize = ( ( this->m_numVertices + 64 ) / 64 ) * 64;

	//Vertex* pVertexArray = new Vertex[g_numVertices];
	//Vertex* pVertexArray = new Vertex[vertexArrayAlignedSize];
	Vertex_p4n4c4* pVertexArray = new Vertex_p4n4c4[vertexArrayAlignedSize];

	// Clear array
	//unsigned int numBytesVertexArray = vertexArrayAlignedSize * sizeof( Vertex );
	unsigned int numBytesVertexArray = vertexArrayAlignedSize * sizeof( Vertex_p4n4c4 );
	memset( pVertexArray, 0, numBytesVertexArray );

	// Load the vertex array from the vector of vertices
	for ( int index = 0; index != this->m_numVertices; index++ )
	{
		//pVertexArray[index].Position[0] = this->m_vecVertices[index].x;
		//pVertexArray[index].Position[1] = this->m_vecVertices[index].y;
		//pVertexArray[index].Position[2] = this->m_vecVertices[index].z;
		//pVertexArray[index].Position[3] = 1.0f;	// 1.0f if you're not sure
		pVertexArray[index].Pos.x = this->m_vecVertices[index].x;
		pVertexArray[index].Pos.y = this->m_vecVertices[index].y;
		pVertexArray[index].Pos.z = this->m_vecVertices[index].z;
		pVertexArray[index].Pos.w = 1.0f;	// 1.0f if you're not sure

		pVertexArray[index].Normal.x = 1.0f;
		pVertexArray[index].Normal.y = 0.0f;
		pVertexArray[index].Normal.z = 0.0f;
		pVertexArray[index].Normal.w = 1.0f;	// 1.0f if you're not sure

		//pVertexArray[index].Color[0] = 1.0f;
		//pVertexArray[index].Color[1] = 1.0f;
		//pVertexArray[index].Color[2] = 1.0f;
		//pVertexArray[index].Color[3] = 1.0f;	// 1.0f if you're not sure
		pVertexArray[index].Colour.x = 1.0f;
		pVertexArray[index].Colour.y = 1.0f;
		pVertexArray[index].Colour.z = 1.0f;
		pVertexArray[index].Colour.w = 1.0f;	// 1.0f if you're not sure
	}
	unsigned int numberOfVertices = this->m_numVertices;
	unsigned int bytesInOneVertex = sizeof( Vertex_p4n4c4 );
	unsigned int bytesoffsetToNormal = offsetof( Vertex_p4n4c4, Normal );
	unsigned int bytesoffsetToColour = offsetof( Vertex_p4n4c4, Colour );

	unsigned int bytesInVertexArray = numberOfVertices * bytesInOneVertex;

	// Loads the index buffer into the array
	unsigned int numberOfIndices = this->m_numTriangles * 3;
	unsigned int indexArrayAlignedSize = ( ( numberOfIndices + 64 ) / 64 ) * 64;

	GLuint* pIndices = new GLuint[ indexArrayAlignedSize ];
	// Clear array
	unsigned int numBytesIndexArray = indexArrayAlignedSize * sizeof( GLuint );
	memset( pIndices, 0, numBytesIndexArray );

	unsigned int index = 0;
	for ( unsigned int triIndex = 0; triIndex != this->m_numTriangles; triIndex++ )
	{
		// Load the index values from the triangle
		pIndices[index + 0] = this->m_vecTriangles[triIndex].v1;
		pIndices[index + 1] = this->m_vecTriangles[triIndex].v2;
		pIndices[index + 2] = this->m_vecTriangles[triIndex].v3;
		index += 3;	// 
	}

	unsigned int bytesPerIndex = sizeof( GLuint );
	unsigned int bytesInIndexArray = numberOfIndices * bytesPerIndex;

	// Please create a VBO and return the "name" (really a number) 
	//	for the created (and empty) VBO
	glGenVertexArrays(1, &(meshInfo.VBO_ID) );		// (1, &BufferIds[0]);
	if (this->m_bGetGLError())	{ return false; }
	//ExitOnGLError("ERROR: Could not generate the VAO");

	// Set the "current" VBO to "this one"
	glBindVertexArray(meshInfo.VBO_ID);		//(BufferIds[0]);
	if (this->m_bGetGLError())	{ return false; }
	//ExitOnGLError("ERROR: Could not bind the VAO");

	//glGenBuffers(2, &BufferIds[1]);	// (2, &BufferIds[1]);
	glGenBuffers(1, &(meshInfo.VertBuffer_ID) );	// (2, &BufferIds[1]);
	if (this->m_bGetGLError())	{ return false; }
	//glGenBuffers(1, &IndexBuf_ID );	// (2, &BufferIds[1]);
	//ExitOnGLError("ERROR: Could not generate the buffer objects");

	// I want this particular buffer to be a VERTEX buffer
	glBindBuffer(GL_ARRAY_BUFFER, meshInfo.VertBuffer_ID );	// BufferIds[1]);
	if (this->m_bGetGLError())	{ return false; }

	// Copies the vertex data from the CPU RAM (the array above) to the GPU VRAM
	glBufferData(GL_ARRAY_BUFFER, bytesInVertexArray, pVertexArray, GL_STATIC_DRAW);
	if (this->m_bGetGLError())	{ return false; }
	//ExitOnGLError("ERROR: Could not bind the VBO to the VAO");


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	//Vertex tempSingleVertex;
	//unsigned int sizeInBytesOfEachVertex = sizeof( tempSingleVertex );
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(tempSingleVertex), (GLvoid*) 0);
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 
	//							sizeof(Vertex), 
	//							(GLvoid*) sizeof(tempSingleVertex.Position)); 

	unsigned int sizeInBytesOfEachVertex = sizeof( Vertex_p4n4c4 );
	unsigned int offsetInBytesOfNormal = offsetof( Vertex_p4n4c4, Normal );
	unsigned int offsetInBytesOfColour = offsetof( Vertex_p4n4c4, Colour );

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 
		                        sizeInBytesOfEachVertex,					// or "sizeof(Vertex)"
								(GLvoid*) 0);	
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 
								sizeInBytesOfEachVertex,					// or "sizeof(Vertex)"
								(GLvoid*) offsetInBytesOfNormal );		// or "offsetof( Vertex, Color )"
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 
								sizeInBytesOfEachVertex,					// or "sizeof(Vertex)"
								(GLvoid*) offsetInBytesOfColour );		// or "offsetof( Vertex, Color )"


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);




	glGenBuffers(1, &(meshInfo.IndexBuffer_ID) );	// (2, &BufferIds[1]);
	if (this->m_bGetGLError())	{ return false; }

	// Point to "this" particular buffer, please, oh great OpenGL...
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshInfo.IndexBuffer_ID );	//BufferIds[2]);
	if (this->m_bGetGLError())	{ return false; }

	// Copy the index data from the CPU RAM to the GPU VRAM
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesInIndexArray, pIndices, GL_STATIC_DRAW);
	if (this->m_bGetGLError())	{ return false; }
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesInIndexArray, INDICES, GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);
	//ExitOnGLError("ERROR: Could not bind the IBO to the VAO");

	glBindVertexArray(0);
	if (this->m_bGetGLError())	{ return false; }

	// All done with the arrays, so delete them
	delete [] pVertexArray;	// note strange array delete syntax
	delete [] pIndices;	// note strange array delete syntax

	return true;
}

void CGLMeshVBOManager::ShutErDown(void)
{
	for ( std::vector< CMeshInfo >::iterator itMesh = this->m_vecMeshes.begin(); itMesh != this->m_vecMeshes.end(); itMesh++ )
	{
		CMeshInfo tempMesh = *itMesh;
		glDeleteBuffers(1, &(tempMesh.VertBuffer_ID) );
		glDeleteBuffers(1, &(tempMesh.IndexBuffer_ID) );

		glDeleteVertexArrays(1, &(tempMesh.VBO_ID) );	// &BufferIds[0]);
	}
	//glDeleteBuffers(1, &VertBuf_ID );
	//glDeleteBuffers(1, &IndexBuf_ID );

	//glDeleteVertexArrays(1, &VBO_ID );	// &BufferIds[0]);

	return;
}

// This might be backwards, but returns an error if there IS an error
// This way, you can write if ( GetGLError ) { HandleError...}
// Also note: this is a very expensive operation that you should 
//	only really call during debug, and "never" during the render loop
//  (at least not often)
bool CGLMeshVBOManager::m_bGetGLError( std::string &errortext )
{
	const GLenum ErrorValue = glGetError();

	if (ErrorValue == GL_NO_ERROR)
	{
		errortext = "GL_NO_ERROR";
		return false;
	}

	errortext = "Unknown or undefined error";
	switch (ErrorValue)
	{
	case GL_INVALID_ENUM:
		//An unacceptable value is specified for an enumerated argument. 
		// The offending command is ignored and has no other side effect than to set the error flag.
		errortext = "GL_INVALID_ENUM";
		break;
	case GL_INVALID_VALUE:
		//A numeric argument is out of range. The offending command is ignored and has no other 
		// side effect than to set the error flag.
		errortext = "GL_INVALID_VALUE";
		break;
	case GL_INVALID_OPERATION:
		//The specified operation is not allowed in the current state. The offending command 
		// is ignored and has no other side effect than to set the error flag.
		errortext = "GL_INVALID_OPERATION";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		//The framebuffer object is not complete. The offending command is ignored and 
		// has no other side effect than to set the error flag.
		errortext = "GL_INVALID_FRAMEBUFFER_OPERATION";
		break;
	case GL_OUT_OF_MEMORY:
		//There is not enough memory left to execute the command. The state of the GL is 
		// undefined, except for the state of the error flags, after this error is recorded.
		errortext = "GL_OUT_OF_MEMORY";
		break;
	case GL_STACK_UNDERFLOW:
		//An attempt has been made to perform an operation that would cause an internal stack to underflow.
		errortext = "GL_STACK_UNDERFLOW";
		break;
	default:
		// Who knows?
		errortext = "Unknown error";
		break;
	}
	return true;
}

bool CGLMeshVBOManager::m_bGetGLError( void )
{
	std::string discard;
	return this->m_bGetGLError( discard );
}

void CGLMeshVBOManager::m_CalcCurrentExtents( CMeshInfo &MeshInfo )
{
	// Any vertices?
	if ( this->m_vecVertices.empty() )
	{	// Nope
		return;
	}

	// Set max and min to first vertex
	MeshInfo.minX = this->m_vecVertices[0].x;
	MeshInfo.minY = this->m_vecVertices[0].y;
	MeshInfo.minZ = this->m_vecVertices[0].z;
	MeshInfo.maxX = this->m_vecVertices[0].x;
	MeshInfo.maxY = this->m_vecVertices[0].y;
	MeshInfo.maxZ = this->m_vecVertices[0].z;

	for ( std::vector< CVertex >::iterator itVert = this->m_vecVertices.begin(); itVert != this->m_vecVertices.end(); itVert++ )
	{
		if ( itVert->x < MeshInfo.minX ) { MeshInfo.minX = itVert->x; }
		if ( itVert->y < MeshInfo.minY ) { MeshInfo.minY = itVert->y; }
		if ( itVert->z < MeshInfo.minZ ) { MeshInfo.minZ = itVert->z; }

		if ( itVert->x > MeshInfo.maxX ) { MeshInfo.maxX = itVert->x; }
		if ( itVert->y > MeshInfo.maxY ) { MeshInfo.maxY = itVert->y; }
		if ( itVert->z > MeshInfo.maxZ ) { MeshInfo.maxZ = itVert->z; }
	}
	// Extent is dif between max and min
	MeshInfo.extentX = MeshInfo.maxX - MeshInfo.minX;
	MeshInfo.extentY = MeshInfo.maxY - MeshInfo.minY;
	MeshInfo.extentZ = MeshInfo.maxZ - MeshInfo.minZ;

	MeshInfo.maxExtent = MeshInfo.extentX;		// Assume max extent is x to start
	if ( MeshInfo.extentY > MeshInfo.maxExtent ) {	MeshInfo.maxExtent = MeshInfo.extentY; }
	if ( MeshInfo.extentZ > MeshInfo.maxExtent ) {	MeshInfo.maxExtent = MeshInfo.extentZ; }

	// Calculate unit square scale from max extent
	MeshInfo.scaleToUnitCube = 1.0f / MeshInfo.maxExtent;


	return;
}
