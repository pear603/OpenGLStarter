#version 330 core
in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
out vec4 colour;

uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D texture2D;

vec3 ambientLight()
{
float ambientStrength = 0.8f;
vec3 ambient = lightColour * ambientStrength;
return ambient;
}

vec3 diffuseLight()
{
float diffuseStrength = 0.8;
vec3 lightDir = normalize(lightPos - FragPos);
vec3 norm = normalize(Normal);
float diff = max(dot(norm, lightDir), 0.0f);
vec3 diffuse = lightColour * diff * diffuseStrength;
return diffuse;
}

vec3 specularLight()
{
float specularStrength = 0.8f;
float shininess = 64.0f;
vec3 lightDir = normalize(lightPos - FragPos);
vec3 norm = normalize(Normal);
vec3 reflectDir = reflect(-lightDir, norm);
vec3 viewDir = normalize(viewPos - FragPos);
//Phong specular
float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);

//Blinn-Phong specular
// vec3 halfwayDir = normalize((lightDir + viewDir)/2.0);
// float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);

vec3 specular = specularStrength * spec * lightColour;
return specular;

}

void main()
{
//Phong Shading
colour = texture(texture2D, TexCoord) * vec4(ambientLight() + diffuseLight() + specularLight(), 1.0f);
// colour = texture(texture2D, TexCoord) * vec4(specularLight(), 1.0f);
}