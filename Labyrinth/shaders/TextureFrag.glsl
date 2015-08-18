#version 400
// this is a pointer to the current 2D texture object
uniform sampler2D tex;
// the vertex UV
smooth in vec2 vertUV;
// the final fragment colour
layout (location =0) out vec4 outColour;

const float blurSizeH = 1.0/300.0;
const float blurSizeV = 1.0/200.0;
void main ()
{
  outColour = texture2D(tex,vertUV);

}
