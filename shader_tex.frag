#version 330 core
out vec4 FragColor;

in vec2 ourTex;
in vec3 ourNorm;
in vec3 FragPos;

uniform vec3 viewPos; 
uniform sampler2D wood_texture;

void main()
{
    

    vec3 lightPos = vec3(100.0f,00.0f,100.0f); 
    vec3 lightColor =vec3(1.0f,1.0f,1.0f) ;
    vec3 objectColor = vec3(0.5f,0.5f,0.0f);

    objectColor = vec3(texture(wood_texture, ourTex));

    // ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    vec3 norm = normalize(ourNorm);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}