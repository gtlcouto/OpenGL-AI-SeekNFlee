#ifndef _Vertex_HG_
#define _Vertex_HG_

//typedef struct Vertex
//{
//  float Position[4];  // 0 is x, 1 is y, 2 is z
//  float Color[4];
//} Vertex;

typedef struct cFloat4
{
  float x, y, z, w;
} cFloat4;

typedef struct Vertex_p4n4c4		// Pos, Normal, Colour
{
  cFloat4 Pos;  
  cFloat4 Normal;
  cFloat4 Colour;
} Vertex_p4n4c4;

#endif