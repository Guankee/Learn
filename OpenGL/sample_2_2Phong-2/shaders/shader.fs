#version 330 core 
out vec4 FragColor; 
uniform vec3 objectColor; 
in vec3 LightColor;

void main() { 

	FragColor = vec4(objectColor*LightColor, 1.0); 
}
