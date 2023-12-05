#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform vec3 lightColor;
out vec3 LightColor;

uniform vec3 lightPos;
uniform mat4 model;
uniform mat4 view; 
uniform mat4 projection; 

void main()
{
    vec3 LightPos = vec3(view * vec4(lightPos, 1.0));  
    vec3 FragPos = vec3(view * model * vec4(aPos, 1.0));
    gl_Position = projection*view*model*vec4(aPos, 1.0);
    vec3 Normal = mat3(view *transpose(inverse(model))) * aNormal; 

    // ambient 
	float ambientStrength = 0.1; 
	vec3 ambient = ambientStrength * lightColor;
	// diffuse  
	vec3 norm = normalize(Normal); 
	vec3 lightDir = normalize(LightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0); 
	vec3 diffuse = diff * lightColor; 

	// specular 
	float specularStrength = 0.5; 
	vec3 viewDir = normalize( - FragPos);
	// 计算光线在法线上的反射方向
	vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256); 
	vec3 specular = specularStrength * spec * lightColor; 
	LightColor = ambient + diffuse + specular;

}