#version 400

uniform mat4 projMat, viewMat, modelMat;

in vec3 vPosition;
in vec4 vColor;
in vec3 vNormal;
out vec4 surfel_color;

void main()
{
    gl_Position = projMat*viewMat*modelMat*vec4(vPosition, 1.0f);   
    //surfel_color = vec4(abs(vNormal.x),abs(vNormal.y),abs(vNormal.z),1.0f);
    surfel_color=vColor;    
}
