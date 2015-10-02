#include "Utils.h"

#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>


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


bool KeyPressed[256];

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
CPhysProps* physPlayer;
std::vector<unsigned int> enemiesID;

//Matrix
//  ProjectionMatrix,
//  ViewMatrix,
//  ModelMatrix;
glm::mat4 matModel(1.0f);		// Set to identity matrix	
glm::mat4 matView(1.0f);		// "View" matrix set to identity
glm::mat4 matProjection(1.0f);	// Again with the identity. Gheesh
glm::mat4 rotationMatrix(1.0f);
glm::mat4 playerRotationMatrix(1.0f);

glm::vec3 g_cameraPos;
glm::vec3 g_cameraTarget;
float camY = 5.0f;
float camZ = -10.0f;


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
void KeyboardDownFunction(unsigned char, int, int);
void KeyboardUpFunction(unsigned char, int, int);

int main(int argc, char* argv[])
{

  Initialize(argc, argv);

  for (int idxKey = 0; idxKey < 256; idxKey++)
  {
	  KeyPressed[idxKey] = false;
  }

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
	
	g_cameraPos = glm::vec3(0.0f, 10.0f, -10.0f);
	g_cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
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

			CPhysProps tempProps = pPlayer->GetPhysProps();;
			pPlayer->SetPhysProps( tempProps );

			pPlayer->colour = glm::vec3(0.0f, 1.0f, 1.0f);

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
			CPhysProps tempProps = pEnemy->GetPhysProps();
			
			pEnemy->SetPhysProps( tempProps );
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
  
  glutKeyboardFunc(KeyboardDownFunction);
  glutKeyboardUpFunc(KeyboardUpFunction);
  glutReshapeFunc(ResizeFunction);
  glutDisplayFunc(RenderFunction);
  glutIdleFunc(IdleFunction);
  glutTimerFunc(0, TimerFunction, 0);
  glutCloseFunc(DestroyCube);
}

void KeyboardUpFunction(unsigned char ch, int x, int y)
{
	CPhysProps tempProps = pPlayer->GetPhysProps();
	KeyPressed[ch] = false;
	tempProps.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	pPlayer->SetPhysProps(tempProps);
}

void KeyboardDownFunction(unsigned char ch, int x, int y)
{
	KeyPressed[ch] = true;
	CPhysProps tempProps = pPlayer->GetPhysProps();

	float someAmount = 0.8f;
	switch (ch)
	{

	case 's':
	{
		tempProps.velocity.z = -someAmount;
	}
	break;
	case 'd':
	{
		tempProps.velocity.x = -someAmount;
	}
	break;
	case 'a':
	{
		tempProps.velocity.x = +someAmount;
	}
	break;
	case 'w':
	{
		tempProps.velocity.z = someAmount;
	}
	break;
	case '1':
	{
		camY = 2.0f;
		camZ = -10.0f;
	}
	break;
	case '2':
	{
		camY = 5.0f;
		camZ = -10.0f;
	}
	break;
	case '3':
	{
		camY = 10.0f;
		camZ = -10.0f;
	}
	break;

	}
	pPlayer->SetPhysProps(tempProps);
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


		
	//pPlayer->position.z += 0.01f;
	g_cameraTarget = pPlayer->position;

	matView = glm::lookAt(glm::vec3((pPlayer->position.x), (pPlayer->position.y + camY), (pPlayer->position.z + camZ)),	// Camera (aka "Eye")
		g_cameraTarget,	// At (aka "target")
		glm::vec3(0.0f, 1.0f, 0.0f));	// Up



	g_SimTimer.Stop();   // (optional)
	float deltaTime = g_SimTimer.GetElapsedSeconds();
	g_SimTimer.Start();			// Restart the timer counter (we want this)

	// Update our objects one time step
	//std::vector< IPhysObject* > vecPhysicsObjects;
	//::g_pTheScene->GetPhysicsObjects(vecPhysicsObjects);
	std::vector< CGameObject* > vecRenderedObjects;
	g_pTheScene->GetRenderedObjects(vecRenderedObjects);

	glm::quat MyQuaternion;

	// Direct specification of the 4 components
	// You almost never use this directly
	

	//for ( int index = 0; index != ::g_p_vecGameObjects.size(); index++ )
	for (int index = 0; index != (vecRenderedObjects.size() - 1); index++)
	{

		CGameObject* pCurObject = vecRenderedObjects[index];
		CPhysProps tempProps = pCurObject->GetPhysProps();
		//the player
		if (index == 0)
		{
			clock_t Now = clock();
			float dt = (float)(Now - LastTime) / CLOCKS_PER_SEC;

			if (LastTime != 0)
			{
				if (KeyPressed['a'])
				{
					// Creates an identity quaternion (no rotation)
					
				/*	MyQuaternion = glm::angleAxis(glm::degrees((glm::pi<float>() / 3)) * dt, glm::vec3(0.0f, 1.0f, 0.0f));
					playerRotationMatrix = glm::mat4_cast(MyQuaternion);*/


					/*glm::vec3 dRot = glm::rotateY(pPlayer->position, (glm::pi<float>() / 3) * deltaTime);
					pPlayer->preRotation = dRot;*/
					
				}
			}
			LastTime = Now;
			// What's the change in velocity in this step in time
			glm::vec3 deltaVelocity;
			deltaVelocity.x = tempProps.accel.x * deltaTime;
			deltaVelocity.y = tempProps.accel.y * deltaTime;
			deltaVelocity.z = tempProps.accel.z * deltaTime;

			tempProps.velocity.x += deltaVelocity.x;
			tempProps.velocity.y += deltaVelocity.y;
			tempProps.velocity.z += deltaVelocity.z;


			// Move it "forward" based on the velocity
			glm::vec3 deltaDistance;
			deltaDistance.x = tempProps.velocity.x * deltaTime;
			deltaDistance.y = tempProps.velocity.y * deltaTime;
			deltaDistance.z = tempProps.velocity.z * deltaTime;

			// Update position from distance
			tempProps.position.x += deltaDistance.x;
			tempProps.position.y += deltaDistance.y;
			tempProps.position.z += deltaDistance.z;
		}
		//the enemies
		else {
			/*rotationMatrix = glm::mat4(1.0f);*/
			CPhysProps playerProps = pPlayer->GetPhysProps();
			glm::vec3 o_p;
			glm::vec3 r;
			float theDot;
			o_p = pPlayer->position - tempProps.position;
			theDot = glm::dot(playerProps.velocity, o_p);

		

			if (theDot > 0)
			{
				//seek
				r = glm::cross(o_p, playerProps.velocity);
				pCurObject->colour = glm::vec3(1.0f, 0.0f, 0.0f);
				
			}
			else {
				//flee
				r = glm::cross(playerProps.velocity, o_p);
				pCurObject->colour = glm::vec3(0.0f, 1.0f, 0.0f);
			}
			srand(time(NULL));
			float randomSpeed = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.8f));
			tempProps.velocity = playerProps.velocity * randomSpeed;

			
			
			//rotationMatrix = glm::mat4_cast(dRot);

		
			// What's the change in velocity in this step in time
			glm::vec3 deltaVelocity;
			deltaVelocity.x = tempProps.accel.x * deltaTime;
			deltaVelocity.y = tempProps.accel.y * deltaTime;
			deltaVelocity.z = tempProps.accel.z * deltaTime;

			tempProps.velocity.x += deltaVelocity.x;
			tempProps.velocity.y += deltaVelocity.y;
			tempProps.velocity.z += deltaVelocity.z;


			// Move it "forward" based on the velocity
			glm::vec3 deltaDistance;
			deltaDistance.x = tempProps.velocity.x * deltaTime;
			deltaDistance.y = tempProps.velocity.y * deltaTime;
			deltaDistance.z = tempProps.velocity.z * deltaTime;

			// Update position from distance
			tempProps.position.x += deltaDistance.x;
			tempProps.position.y += deltaDistance.y;
			tempProps.position.z += deltaDistance.z;
			// Now pass these updated physical properties back into the object
			
		}
		pCurObject->SetPhysProps(tempProps);
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


void CreateCube()
{


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


}

void DestroyCube()
{
  glDetachShader(ShaderIds[0], ShaderIds[1]);
  glDetachShader(ShaderIds[0], ShaderIds[2]);
  glDeleteShader(ShaderIds[1]);
  glDeleteShader(ShaderIds[2]);
  glDeleteProgram(ShaderIds[0]);
  ExitOnGLError("ERROR: Could not destroy the shaders");


  ::g_pTheMeshManager->ShutErDown();

  ExitOnGLError("ERROR: Could not destroy the buffer objects");
}

float zOffset = 0.0f;

void DrawCube(void)
{
  float CubeAngle;
  clock_t Now = clock();			// 12,384.05 seconds

  if (LastTime == 0)
    LastTime = Now;			// LastTime = 12,384.01 seconds



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

		if (index == 0)
		{
			matModel *= playerRotationMatrix;
		}

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


		
		{
			glBindVertexArray( tempMeshInfo.VBO_ID );		//(BufferIds[0]);
			glBindBuffer( GL_ARRAY_BUFFER, tempMeshInfo.VertBuffer_ID );

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			unsigned int numberOfIndices = tempMeshInfo.numberOfTriangles * 3;

			glDrawElements(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, (GLvoid*)0);


			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		}

  }	// for ( int index = 0;

  glBindVertexArray(0);
  glUseProgram(0);
}
