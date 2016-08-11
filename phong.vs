// CS148 Summer 2016 Homework 3 - Shaders

#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textCoor;

out vec3 Normal;
out vec3 FragPos;
out vec3 Position;
out vec2 TextCoor;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    // TODO: Your code here
    // Remember to set gl_Position (correctly) or you will get a black screen...
    vec4 posVec = vec4(position,1.0f);
    gl_Position = projection*view*model*posVec;
    Normal = normal;//pass normal out
    
    //determine fragment position by transforming position to world coords
    FragPos = vec3(model*posVec); 
    Position = position;
    TextCoor = textCoor;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
} 
