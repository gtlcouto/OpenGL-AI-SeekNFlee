#ifndef UTILS_H
#define UTILS_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <glm\glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

//#include "d:\dev\include\GL\freeglut.h"

static const double PI = 3.14159265358979323846;

#include "Vertex.h"
//typedef struct Vertex
//{
//  float Position[4];  // 0 is x, 1 is y, 2 is z
//  float Color[4];
//} Vertex;

//typedef struct Matrix
//{
//  float m[16];
//} Matrix;

//extern const Matrix IDENTITY_MATRIX;

float Cotangent(float angle);
float DegreesToRadians(float degrees);
float RadiansToDegrees(float radians);
glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);
//Matrix MultiplyMatrices(const Matrix* m1, const Matrix* m2);
//void RotateAboutX(Matrix* m, float angle);
//void RotateAboutY(Matrix* m, float angle);
//void RotateAboutZ(Matrix* m, float angle);
//void ScaleMatrix(Matrix* m, float x, float y, float z);
//void TranslateMatrix(Matrix* m, float x, float y, float z);

//Matrix CreateProjectionMatrix(
//  float fovy,
//  float aspect_ratio,
//  float near_plane,
//  float far_plane
//);

void ExitOnGLError(const char* error_message);
GLuint LoadShader(const char* filename, GLenum shader_type);

std::vector<glm::vec3> RandomPosition(int count, glm::vec3 position, int range);
bool contains(std::vector<glm::vec3> v, glm::vec3 x);
bool operator==(const glm::vec3 &vecA, const glm::vec3 &vecB);


#endif
