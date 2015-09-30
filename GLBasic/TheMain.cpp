#include "Utils.h"

#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "CGameObject.h"

#include "CGLMeshVBOManager.h"
#include "CGameObjectManager.h"	// aka the "world" or "scene"

#include <iostream>

#define WINDOW_TITLE_PREFIX "Chapter 4"

// ****************
// These are for loading the ply file
#include <string>
#include <fstream>
#include <vector>
//#include "CVertex.h"
//#include "CTriangle.h"
//std::vector<CVertex> g_vecVertices;		// "array" of vertics
//std::vector<CTriangle> g_vecTriangles;		// "array" of vertics
//unsigned int g_numVertices = 0;
//unsigned int g_numTriangles = 0;
//bool LoadPlyFile( std::string fileName );
// ****************

#include "CHRTimer.h"	

CHRTimer g_SimTimer;	

//std::vector< CGameObject* >  g_p_vecGameObjects;	// Moved into CGameObjectManager

//CGLMeshVBOManager g_TheMeshManager;
CGLMeshVBOManager* g_pTheMeshManager;	// Changed to a pointer
CGameObjectManager* g_pTheScene;		// ADDED

int
  CurrentWidth = 800,
  CurrentHeight = 600,
  WindowHandle = 0;

unsigned FrameCount = 0;


GLuint ProjectionMatrixUniformLocation = 0;
GLuint ViewMatrixUniformLocation = 0;
GLuint ModelMatrixUniformLocation = 0;
GLuint ObjectColourUniformLocation = 0;			// ID of the "slot" where the shader variable is

//GLuint BufferIds[3] = { 0, 0, 0 };	// VBO, Vertex Buffer, Index Buffer
//GLuint VBO_ID = 0;
//GLuint VertBuf_ID = 0;
//GLuint IndexBuf_ID = 0;

GLuint ShaderIds[3] = { 0, 0, 0 };  // Shader Program, Vertex Shader, Fragment

//Models
unsigned int playerID;
CGameObject* pPlayer;
std::vector<unsigned int> enemiesID;

//Matrix
//  ProjectionMatrix,
//  ViewMatrix,
//  ModelMatrix;
glm::mat4 matModel(1.0f);		// Set to identity matrix	
glm::mat4 matView(1.0f);		// "View" matrix set to identity
glm::mat4 matProjection(1.0f);	// Again with the identity. Gheesh

glm::vec3 g_cameraPos;
glm::vec3 g_cameraTarget;


float CubeRotation = 0;
clock_t LastTime = 0;

void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void ResizeFunction(int, int);
void RenderFunction(void);
void TimerFunction(int);
void IdleFunction(void);
void CreateCube(void);
void DestroyCube(void);
void DrawCube(void);

int main(int argc, char* argv[])
{

  Initialize(argc, argv);

  g_SimTimer.Start();

  glutMainLoop();
  
  exit(EXIT_SUCCESS);
}

void Initialize(int argc, char* argv[])
{
	GLenum GlewInitResult;

	InitWindow(argc, argv);

	glewExperimental = GL_TRUE;
	GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult) {
		fprintf(
			stderr,
			"ERROR: %s\n",
			glewGetErrorString(GlewInitResult)
			);
		exit(EXIT_FAILURE);
	}

	fprintf(
		stdout,
		"INFO: OpenGL Version: %s\n",
		glGetString(GL_VERSION)
		);

	glGetError();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	ExitOnGLError("ERROR: Could not set OpenGL depth testing options");

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	ExitOnGLError("ERROR: Could not set OpenGL culling options");

	//ModelMatrix = IDENTITY_MATRIX;
	//ProjectionMatrix = IDENTITY_MATRIX;
	//ViewMatrix = IDENTITY_MATRIX;
	matModel = glm::mat4(1.0f);				// Set to identity
	matView = glm::mat4(1.0f);
	matProjection = glm::mat4(1.0f);

	//TranslateMatrix(&ViewMatrix, 0, 0, -2);
	matView = glm::mat4(1.0f);		// identity matrix
	matView = glm::lookAt(g_cameraPos,	// Camera (aka "Eye")
			  g_cameraTarget,	// At (aka "target")
			  glm::vec3(0.0f, 1.0f, 0.0f));	// Up


// Note the path: the files are now in a sub folder
//if ( ! LoadPlyFile( "assets/ply/bun_zipper_res3_xyz.ply" ) )



  //if ( ! g_TheMeshManager.LoadMeshModel( "assets/ply/bun_zipper_res3_xyz.ply" ) )
  //{
  //	printf( "Can't find the ply file." );
  //	exit(EXIT_FAILURE);
  //}		
  //if ( ! g_TheMeshManager.LoadMeshModel( "assets/ply/dolphin.ply" ) )
  //{
  //	printf( "Can't find the ply file." );
  //	exit(EXIT_FAILURE);
  //}
  //if ( ! g_TheMeshManager.LoadMeshModel( "assets/ply/Utah_Teapot.ply" ) )
  //{
  //	printf( "Can't find the ply file." );
  //	exit(EXIT_FAILURE);
  //}

	g_pTheMeshManager = new CGLMeshVBOManager();

	g_pTheScene = new CGameObjectManager(g_pTheMeshManager);
	//g_pTheScene = new CGameObjectManager();

	// Now load the models
	if (!g_pTheMeshManager->LoadMeshModel("assets/ply/bun_zipper_res3_xyz.ply"))
	{
		std::cout << "can't load model" << std::endl;
		// Do we continue?
		return;
	}
	if (!g_pTheMeshManager->LoadMeshModel("assets/obj/archangel_hr-dark.obj"))
	{
		std::cout << "can't load model" << std::endl;
		// Do we continue?
		return;
	}
	if (!g_pTheMeshManager->LoadMeshModel("assets/obj/lowelemental_hr-dark.obj"))
	{
		std::cout << "can't load model" << std::endl;
		// Do we continue?
		return;
	}
	g_pTheMeshManager->LoadMeshModel("assets/ply/square_plate_b.ply");




	//PLAYER
	std::string error;
	playerID =
		g_pTheScene->CreateObjectAndAddToWorld("assets/obj/archangel_hr-dark.obj",
			error);
	if (playerID == 0)
	{
		std::cout << error << std::endl;
		// Exit?
	}
	else
	{
		pPlayer = g_pTheScene->pGetGameObjectByID(playerID);
		if (pPlayer != 0)
		{
			pPlayer->position.y = 0.0f;
			pPlayer->position.z = 0.0f;

			//CPhysProps tempProps = pPlayer->GetPhysProps();;
			//tempProps.velocity.x = 0.5f;		// 1 "unit" per second
			//tempProps.velocity.y = 7.0f;		// Throw bunny at 10 units per sec straight up
			//tempProps.accel.y = -9.81f;		// Accel due to gravity in most places on the Earth
			//pPlayer->SetPhysProps( tempProps );

			pPlayer->colour = glm::vec3(1.0f, 0.0f, 0.0f);

			CMeshInfo tempMeshInfo;
			::g_pTheMeshManager->GetInfoByMeshName(pPlayer->meshName, tempMeshInfo);
			pPlayer->scale = 0.01f / tempMeshInfo.scaleToUnitCube;
			pPlayer->bIsWireframe = true;
		}
	}




	std::vector<glm::vec3> randomPos = RandomPosition(10, pPlayer->position, 5);
	std::vector<CGameObject *> vEnemies;
	for (int i = 0; i < 10; i++) {
		unsigned int enemyID = g_pTheScene->CreateObjectAndAddToWorld("assets/obj/lowelemental_hr-dark.obj",
				error);
		CGameObject* pEnemy = g_pTheScene->pGetGameObjectByID(enemyID);
		if (pEnemy != 0)
		{
			pEnemy->position = randomPos[i];

			pEnemy->colour = glm::vec3(1.0f, 0.0f, 0.0f);


			CMeshInfo tempMeshInfo;
			::g_pTheMeshManager->GetInfoByMeshName(pEnemy->meshName, tempMeshInfo);
			pEnemy->scale = 0.01f / tempMeshInfo.scaleToUnitCube;
			pEnemy->bIsWireframe = true;
		}
	}

	
	unsigned int groundID = 
	g_pTheScene->CreateObjectAndAddToWorld( "assets/ply/square_plate_a.ply", 
		                                    error );
	CGameObject* pGround = g_pTheScene->pGetGameObjectByID( groundID );
		if ( pGround != 0 ) 
		{
			pGround->position.y = -1.5f;

			pGround->colour = glm::vec3( 0.6f, 0.6f, 0.6f );


			CMeshInfo tempMeshInfo;
			::g_pTheMeshManager->GetInfoByMeshName( pGround->meshName, tempMeshInfo );
			pGround->scale = 5.0f / tempMeshInfo.scaleToUnitCube;
			pGround->bIsWireframe = true;
		}


  //printf( "Loaded the bunny ply file OK.\n" );

  //// THIS CODE WILL LIKELY BE MOVED
  //CGameObject* pBunny = new CGameObject();	// Heap, not stack (new = heap)

  //pBunny->position.y = -4.0f;	// Place bunny near bottom of screen
  //pBunny->position.x = -4.0f;	// ... and a little to the left
  //
  //pBunny->velocity.x = 0.5f;		// 1 "unit" per second
  //pBunny->velocity.y = 7.0f;		// Throw bunny at 10 units per sec straight up

  //pBunny->accel.y = -9.81f;		// Accel due to gravity in most places on the Earth


  //pBunny->bIsWireframe = true;
  //pBunny->meshName = "assets/ply/bun_zipper_res3_xyz.ply";
  //::g_p_vecGameObjects.push_back( pBunny );			// [0]

  //CGameObject* pBunny2 = new CGameObject();
  ////pBunny2->position.y = 0.5f;
  ////pBunny2->position.x = 5.0f;	// 5 units to the right
  //pBunny2->scale = 1.0f;
  //pBunny2->meshName = "assets/ply/bun_zipper_res3_xyz.ply";
  //pBunny2->bIsWireframe = true;
  //::g_p_vecGameObjects.push_back( pBunny2 );		// [1]


  CreateCube();	// The vertex and index buffers are loaded
}



void InitWindow(int argc, char* argv[])
{
  glutInit(&argc, argv);

  glutInitContextVersion(4, 0);
  glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
  glutInitContextProfile(GLUT_CORE_PROFILE);

  glutSetOption(
    GLUT_ACTION_ON_WINDOW_CLOSE,
    GLUT_ACTION_GLUTMAINLOOP_RETURNS
  );
  
  glutInitWindowSize(CurrentWidth, CurrentHeight);

  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

  WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);

  if(WindowHandle < 1) {
    fprintf(
      stderr,
      "ERROR: Could not create a new rendering window.\n"
    );
    exit(EXIT_FAILURE);
  }
  
  glutReshapeFunc(ResizeFunction);
  glutDisplayFunc(RenderFunction);
  glutIdleFunc(IdleFunction);
  glutTimerFunc(0, TimerFunction, 0);
  glutCloseFunc(DestroyCube);
}

void ResizeFunction(int Width, int Height)
{
	CurrentWidth = Width;
	CurrentHeight = Height;
	glViewport(0, 0, CurrentWidth, CurrentHeight);
	//ProjectionMatrix =
	//  CreateProjectionMatrix(
	//    60,
	//    (float)CurrentWidth / CurrentHeight,
	//    1.0f,			// Only draw thing GT 1.0f away from the front of the camera
	//    100.0f		// Anything further away won't be drawn
	//  );
	float aspectRatio = static_cast<float>(CurrentWidth) / static_cast<float>(CurrentHeight);
	matProjection = glm::perspective( glm::radians(45.0f),	// FOV
                    aspectRatio,	 // 16:9, 4:3, 16:10
                    0.1f,		// Near plane
                    10000.f);		// Far plane


  glUseProgram(ShaderIds[0]);
  //glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, ProjectionMatrix.m);
  glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(matProjection) );

  glUseProgram(0);
}

void RenderFunction(void)
{
  ++FrameCount;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  DrawCube();
  
  glutSwapBuffers();
}

void IdleFunction(void)
{
	//g_IOThing->Update();
	//g_PhysicsThingy->Update();
	//g_CollisionDetect->Update();


	g_SimTimer.Stop();   // (optional)
	float deltaTime = g_SimTimer.GetElapsedSeconds();
	//#include <iostream>
	//std::cout << deltaTime << " seconds" << std::endl;
	g_SimTimer.Start();			// Restart the timer counter (we want this)

	// Update our objects one time step
	//std::vector< CGameObject* > vecObjectsToDraw;
	//::g_pTheScene->GetRenderedObjects( vecObjectsToDraw );
	std::vector< IPhysObject* > vecPhysicsObjects;
	::g_pTheScene->GetPhysicsObjects( vecPhysicsObjects );

	//for ( int index = 0; index != ::g_p_vecGameObjects.size(); index++ )
	for ( int index = 0; index != vecPhysicsObjects.size(); index++ )
	{
		//CGameObject* pCurObject = ::g_p_vecGameObjects[index];
		IPhysObject* pCurObject = vecPhysicsObjects[index];

		CPhysProps tempProps  = pCurObject->GetPhysProps();

		// What's the change in velocity in this step in time
		glm::vec3 deltaVelocity;
		//deltaVelocity.x = pCurObject->accel.x * deltaTime;
		//deltaVelocity.y = pCurObject->accel.y * deltaTime;
		//deltaVelocity.z = pCurObject->accel.z * deltaTime;
		deltaVelocity.x = tempProps.accel.x * deltaTime;
		deltaVelocity.y = tempProps.accel.y * deltaTime;
		deltaVelocity.z = tempProps.accel.z * deltaTime;

		//pCurObject->velocity.x += deltaVelocity.x;
		//pCurObject->velocity.y += deltaVelocity.y;
		//pCurObject->velocity.z += deltaVelocity.z;
		tempProps.velocity.x += deltaVelocity.x;
		tempProps.velocity.y += deltaVelocity.y;
		tempProps.velocity.z += deltaVelocity.z;


		// Move it "forward" based on the velocity
		glm::vec3 deltaDistance;
		//distance.x = pCurObject->velocity.x * deltaTime;
		//distance.y = pCurObject->velocity.y * deltaTime;
		//distance.z = pCurObject->velocity.z * deltaTime;
		deltaDistance.x = tempProps.velocity.x * deltaTime;
		deltaDistance.y = tempProps.velocity.y * deltaTime;
		deltaDistance.z = tempProps.velocity.z * deltaTime;

		// Update position from distance
		//pCurObject->position.x += distance.x;
		//pCurObject->position.y += distance.y;
		//pCurObject->position.z += distance.z;
		tempProps.position.x += deltaDistance.x;
		tempProps.position.y += deltaDistance.y;
		tempProps.position.z += deltaDistance.z;

		// What if the bunny hits the "ground" (-4.0f is where the "ground" is)
		//if ( pCurObject->position.y <= -4.0f )
		//{
		//	pCurObject->velocity.y = -( pCurObject->velocity.y * 0.90f );
		//	//pCurObject->accel.y = 0.0f;
		//}
		if ( tempProps.position.y <= -4.0f )
		{
			tempProps.velocity.y = -( tempProps.velocity.y * 0.90f );
			//pCurObject->accel.y = 0.0f;
		}

		// Now pass these updated physical properties back into the object
		pCurObject->SetPhysProps( tempProps );
	}

	glutPostRedisplay();		// Yo GLUT, redraw the screen
}

void TimerFunction(int Value)
{
  if (0 != Value) {
    char* TempString = (char*)
      malloc(512 + strlen(WINDOW_TITLE_PREFIX));

    sprintf(
      TempString,
      "%s: %d Frames Per Second @ %d x %d",
      WINDOW_TITLE_PREFIX,
      FrameCount * 4,
      CurrentWidth,
      CurrentHeight
    );

    glutSetWindowTitle(TempString);
    free(TempString);
  }

  FrameCount = 0;
  glutTimerFunc(250, TimerFunction, 1);
}

//// *************************************************************
//// Very "good enough for rock-n-roll" copy and paste thing...
//bool LoadPlyFile( std::string fileName )
//{
//	// TODO: Insert magic here...
//	std::ifstream plyFile( fileName.c_str() );		// ASCII (non-binary)
//	if ( ! plyFile.is_open() )
//	{
//		return false;
//	}
//	// We are good to go... file is open
//	// Read until we hit "vertex", then read next number
//	std::string temp;
//	while ( plyFile >> temp )
//	{
//		if ( temp == "vertex" )	{ break; }
//	}
//	// Next thing we read is the number of vertices
//	plyFile >> g_numVertices;
//
//	// Read until "face"
//	while ( plyFile >> temp )
//	{
//		if ( temp == "face" )	{ break; }
//	}
//	plyFile >> g_numTriangles; 
//
//	// Read until "end_header"
//	while ( plyFile >> temp )
//	{
//		if ( temp == "end_header" )	{ break; }
//	}
//	// The next thing is the first x value of the 1st vertex
//	for ( int count = 0; count != g_numVertices; count++ )
//	{
//		// -0.0369122    0.127512    0.00276757    0.850855    0.5 
//		CVertex tempVertex;
//		plyFile >> tempVertex.x;
//		plyFile >> tempVertex.y;
//		plyFile >> tempVertex.z;
////		plyFile >> tempVertex.confidence;
////		plyFile >> tempVertex.intensity;
//		// Scale the bunny while I'm loading it...
//		tempVertex.x *= 5.0f;
//		tempVertex.y *= 5.0f;
//		tempVertex.z *= 5.0f;
//
//		g_vecVertices.push_back( tempVertex );
//	}
//
//	// Load triangles...
//	for ( int count = 0; count != g_numTriangles; count++ )
//	{
//		CTriangle tempTri;
//		int discard = 0;
//		// 3 4 132 80
//		plyFile >> discard;
//		plyFile >> tempTri.v1;
//		plyFile >> tempTri.v2;
//		plyFile >> tempTri.v3;
//		// Add triangle to vector
//		g_vecTriangles.push_back( tempTri );
//	}
//
//	return true;
//}

// *************************************************************

void CreateCube()
{

  //const Vertex VERTICES[8] =
  //{
  //  { { -.5f, -.5f,  .5f, 1 }, { 1, 0, 1, 1 } },	// 0
  //  { { -.5f,  .5f,  .5f, 1 }, { 1, 0, 0, 1 } },	// 1
  //  { {  .5f,  .5f,  .5f, 1 }, { 0, 1, 0, 1 } },
  //  { {  .5f, -.5f,  .5f, 1 }, { 1, 1, 0, 1 } },
  //  { { -.5f, -.5f, -.5f, 1 }, { 1, 1, 1, 1 } },
  //  { { -.5f,  .5f, -.5f, 1 }, { 1, 0, 0, 1 } },
  //  { {  .5f,  .5f, -.5f, 1 }, { 1, 0, 1, 1 } },
  //  { {  .5f, -.5f, -.5f, 1 }, { 0, 0, 1, 1 } }		// 7
  //};


	// A dynamic array in C
	//Vertex* pVertexArray = new Vertex[::g_numVertices];
	//for ( int index = 0; index != ::g_numVertices; index++ )
	//{
	//	pVertexArray[index].Position[0] = ::g_vecVertices[index].x;
	//	pVertexArray[index].Position[1] = ::g_vecVertices[index].y;
	//	pVertexArray[index].Position[2] = ::g_vecVertices[index].z;
	//	pVertexArray[index].Position[3] = 1.0f;	// 1.0f if you're not sure

	//	pVertexArray[index].Color[0] = 1.0f;
	//	pVertexArray[index].Color[1] = 1.0f;
	//	pVertexArray[index].Color[2] = 1.0f;
	//	pVertexArray[index].Color[3] = 1.0f;	// 1.0f if you're not sure
	//}
	//unsigned int numberOfVertices = ::g_numVertices;
	//unsigned int bytesInOneVertex = sizeof( Vertex );
	//unsigned int bytesInVertexArray = numberOfVertices * bytesInOneVertex;


  //unsigned int numberOfIndices = 36;	// NOT numberOfTriangles

 // unsigned int numberOfIndices = ::g_numTriangles * 3;
 // GLuint* pIndices = new GLuint[ numberOfIndices ];

 // unsigned int index = 0;
 // for ( unsigned int triIndex = 0; triIndex != g_numTriangles; triIndex++ )
 // {
	//// Load the index values from the triangle
	//pIndices[index + 0] = ::g_vecTriangles[triIndex].v1;
	//pIndices[index + 1] = ::g_vecTriangles[triIndex].v2;
	//pIndices[index + 2] = ::g_vecTriangles[triIndex].v3;
	//index += 3;	// 
 // }
  //const GLuint INDICES[36] =
  //{
  //  0,2,1,  0,3,2,
  //  4,3,0,  4,7,3,
  //  4,1,5,  4,0,1,
  //  3,6,2,  3,7,6,
  //  1,6,5,  1,2,6,
  //  7,5,6,  7,4,5
  //};
  //unsigned int bytesPerIndex = sizeof( GLuint );
  //unsigned int bytesInIndexArray = numberOfIndices * bytesPerIndex;

  ShaderIds[0] = glCreateProgram();
  ExitOnGLError("ERROR: Could not create the shader program");
  {
    //ShaderIds[1] = LoadShader("SimpleShader.fragment.glsl", GL_FRAGMENT_SHADER);
    //ShaderIds[2] = LoadShader("SimpleShader.vertex.glsl", GL_VERTEX_SHADER);
    ShaderIds[1] = LoadShader("SimpleShader2.fragment.glsl", GL_FRAGMENT_SHADER);
    ShaderIds[2] = LoadShader("SimpleShader2.vertex.glsl", GL_VERTEX_SHADER);
    glAttachShader(ShaderIds[0], ShaderIds[1]);
    glAttachShader(ShaderIds[0], ShaderIds[2]);
  }
  glLinkProgram(ShaderIds[0]);
  ExitOnGLError("ERROR: Could not link the shader program");

  ModelMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ModelMatrix");
  ViewMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ViewMatrix");
  ProjectionMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ProjectionMatrix");

  ObjectColourUniformLocation = glGetUniformLocation( ShaderIds[0], "objectColour" );

  ExitOnGLError("ERROR: Could not get shader uniform locations");


  //// Please create a VBO and return the "name" (really a number) 
  ////	for the created (and empty) VBO
  //glGenVertexArrays(1, &VBO_ID);		// (1, &BufferIds[0]);
  //ExitOnGLError("ERROR: Could not generate the VAO");

  //// Set the "current" VBO to "this one"
  //glBindVertexArray(VBO_ID);		//(BufferIds[0]);
  //ExitOnGLError("ERROR: Could not bind the VAO");

  //glEnableVertexAttribArray(0);
  //glEnableVertexAttribArray(1);
  //ExitOnGLError("ERROR: Could not enable vertex attributes");

  ////glGenBuffers(2, &BufferIds[1]);	// (2, &BufferIds[1]);
  //glGenBuffers(1, &VertBuf_ID );	// (2, &BufferIds[1]);
  ////glGenBuffers(1, &IndexBuf_ID );	// (2, &BufferIds[1]);
  //ExitOnGLError("ERROR: Could not generate the buffer objects");

  //// I want this particular buffer to be a VERTEX buffer
  //glBindBuffer(GL_ARRAY_BUFFER, VertBuf_ID );	// BufferIds[1]);

  //// Copies the vertex data from the CPU RAM (the array above) to the GPU VRAM
  //glBufferData(GL_ARRAY_BUFFER, bytesInVertexArray, pVertexArray, GL_STATIC_DRAW);
  //ExitOnGLError("ERROR: Could not bind the VBO to the VAO");

  // Moved to the render call...
  // Just to create a single variable so we can get the size and offsets
  ////////Vertex tempSingleVertex;
  ////////unsigned int sizeInBytesOfEachVertex = sizeof( tempSingleVertex );

  ////////glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(tempSingleVertex), (GLvoid*) 0);
  ////////ExitOnGLError("ERROR: Could not set VAO attributes");
  ////////glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 
	 ////////                         sizeof(tempSingleVertex), 
	 ////////                         (GLvoid*) sizeof(tempSingleVertex.Position));

  //glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(pVertexArray[0]), (GLvoid*)0);
  //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(pVertexArray[0]), (GLvoid*)sizeof(pVertexArray[0].Position));

  //glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)0);
  //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)sizeof(VERTICES[0].Position));
  //ExitOnGLError("ERROR: Could not set VAO attributes");

  
  //
  //glGenBuffers(1, &IndexBuf_ID );	// (2, &BufferIds[1]);

  //// Point to "this" particular buffer, please, oh great OpenGL...
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuf_ID );	//BufferIds[2]);

  //// Copy the index data from the CPU RAM to the GPU VRAM
  //glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesInIndexArray, pIndices, GL_STATIC_DRAW);
  ////glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesInIndexArray, INDICES, GL_STATIC_DRAW);
  ////glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);
  //ExitOnGLError("ERROR: Could not bind the IBO to the VAO");

  //glBindVertexArray(0);

  //// All done with the arrays, so delete them
  //delete [] pVertexArray;	// note strange array delete syntax
  //delete [] pIndices;	// note strange array delete syntax

}

void DestroyCube()
{
  glDetachShader(ShaderIds[0], ShaderIds[1]);
  glDetachShader(ShaderIds[0], ShaderIds[2]);
  glDeleteShader(ShaderIds[1]);
  glDeleteShader(ShaderIds[2]);
  glDeleteProgram(ShaderIds[0]);
  ExitOnGLError("ERROR: Could not destroy the shaders");

  //::g_TheMeshManager.ShutErDown();
  ::g_pTheMeshManager->ShutErDown();
  //glDeleteBuffers(2, &BufferIds[1]);
  //glDeleteBuffers(1, &VertBuf_ID );
  //glDeleteBuffers(1, &IndexBuf_ID );

  //glDeleteVertexArrays(1, &VBO_ID );	// &BufferIds[0]);
  ExitOnGLError("ERROR: Could not destroy the buffer objects");
}

float zOffset = 0.0f;

void DrawCube(void)
{
  float CubeAngle;
  clock_t Now = clock();			// 12,384.05 seconds

  if (LastTime == 0)
    LastTime = Now;			// LastTime = 12,384.01 seconds

  // 0.04 seconds = Frame time 

  CubeRotation += 45.5f * ((float)(Now - LastTime) / CLOCKS_PER_SEC);
  CubeAngle = DegreesToRadians(CubeRotation);
  LastTime = Now;

  // Loop through our list of objects and draw each one

  std::vector< CGameObject* > vecRenderedObjects;
  ::g_pTheScene->GetRenderedObjects( vecRenderedObjects );

	//vecRenderedObjects = g_pTheScene->GetRenderedObjects();

  //for ( int index = 0; index != ::g_p_vecGameObjects.size(); index++ )
  for ( int index = 0; index != vecRenderedObjects.size(); index++ )
  {
		// This code will be streamlined later... 
		//CGameObject* pTempObject = ::g_p_vecGameObjects[ index ];
		CGameObject* pTempObject = vecRenderedObjects[ index ];

	  	CMeshInfo tempMeshInfo;
		//if ( ! ::g_TheMeshManager.GetInfoByMeshName( pTempObject->meshName, tempMeshInfo ) )
		if ( ! ::g_pTheMeshManager->GetInfoByMeshName( pTempObject->meshName, tempMeshInfo ) )
		{
			// Nothing to draw.
			// We may have to draw children, but not at the moment
			break;
		}


		// ModelMatrix = IDENTITY_MATRIX;
		matModel = glm::mat4(1.0f);		// Clear to identity matrix


		matModel = glm::rotate( matModel, pTempObject->preRotation.x, glm::vec3( 1.0f, 0.0f, 0.0f) );
		matModel = glm::rotate( matModel, pTempObject->preRotation.y, glm::vec3( 0.0f, 1.0f, 0.0f) );
		matModel = glm::rotate( matModel, pTempObject->preRotation.z, glm::vec3( 0.0f, 0.0f, 1.0f) );

		//matModel = glm::translate( matModel, 
		//						   glm::vec3( pTempObject->position.x, 
		//						              pTempObject->position.y, 
		//									  pTempObject->position.z) );
		matModel = glm::translate( matModel, pTempObject->position );		

		matModel = glm::rotate( matModel, pTempObject->postRotation.x, glm::vec3( 1.0f, 0.0f, 0.0f) );
		matModel = glm::rotate( matModel, pTempObject->postRotation.y, glm::vec3( 0.0f, 1.0f, 0.0f) );
		matModel = glm::rotate( matModel, pTempObject->postRotation.z, glm::vec3( 0.0f, 0.0f, 1.0f) );

		// We will assume the scale of the object is that compared to a unit cube
		// That way they all will be relative to each other

		float scale = pTempObject->scale * tempMeshInfo.scaleToUnitCube;
		matModel = glm::scale( matModel, glm::vec3( scale, scale, scale ) );  

		if ( pTempObject->bIsWireframe )
		{	// Wireframe...
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		else
		{	// Not wireframe
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}


		//RotateAboutY(&ModelMatrix, CubeAngle);
		//RotateAboutX(&ModelMatrix, CubeAngle);

		// zOffset = 0.0f;
		//TranslateMatrix( &ModelMatrix, 0.0f, 0.0f, zOffset );
		//zOffset -= 0.005f;

		glUseProgram(ShaderIds[0]);
		ExitOnGLError("ERROR: Could not use the shader program");

		//glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, ModelMatrix.m);
		//glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, ViewMatrix.m);
		glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(matModel) );
		glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(matView) );  

		glUniform4f(ObjectColourUniformLocation, pTempObject->colour.x, 
			                                     pTempObject->colour.y,
												 pTempObject->colour.z,
												 1.0f );		// Not sure? make it 1.0f;

		ExitOnGLError("ERROR: Could not set the shader uniforms");

		//OhGreatMeshManager->GetInfoByMeshName( pTempObject->meshName ):
		//Vertex tempSingleVertex;
		//unsigned int sizeInBytesOfEachVertex = sizeof( tempSingleVertex );

		//std::string meshToRenderName = pTempObject->meshName;
		//if ( ::g_TheMeshManager.GetInfoByMeshName( pTempObject->meshName, 
		//										   tempMeshInfo ) )
		{
			// Oh great OpenGL, please "point" to "this" particular VBO
			//glBindVertexArray( VBO_ID );		//(BufferIds[0]);
			glBindVertexArray( tempMeshInfo.VBO_ID );		//(BufferIds[0]);
			glBindBuffer( GL_ARRAY_BUFFER, tempMeshInfo.VertBuffer_ID );

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			//ExitOnGLError("ERROR: Could not enable vertex attributes");

			//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(tempSingleVertex), (GLvoid*) 0);
			//ExitOnGLError("ERROR: Could not set VAO attributes");
			//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 
			//							sizeof(tempSingleVertex), 
			//							(GLvoid*) sizeof(tempSingleVertex.Position));

			// Yup, it's there
			//unsigned int numberOfIndices = ::g_numTriangles * 3;
			unsigned int numberOfIndices = tempMeshInfo.numberOfTriangles * 3;

			glDrawElements(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, (GLvoid*)0);
			//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)0);
			//ExitOnGLError("ERROR: Could not draw the cube");

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		}

  }	// for ( int index = 0;

  glBindVertexArray(0);
  glUseProgram(0);
}
