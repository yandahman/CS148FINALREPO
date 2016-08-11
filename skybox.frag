#version 330 core
in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox;
//uniform sampler2D rocktexture;

void main()
{    
    color = texture(skybox, TexCoords);
    //color = texture(rocktexture, TexCoords);
}
  