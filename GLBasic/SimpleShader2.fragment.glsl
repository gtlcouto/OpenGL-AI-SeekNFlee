#version 400

in vec4 ex_Normal;
in vec4 ex_Color;
out vec4 out_Color;		// The colour of the pixel

uniform vec4 objectColour;

void main(void)
{
// Super sexy stuff
  //out_Color = ex_Color;
  out_Color = objectColour;
  
  //out_Color = vec4( 1.0f, 0.0f, 1.0f, 1.0f );	// RGB
}