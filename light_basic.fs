#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 frag_pos;  

uniform vec3 light_pos;
uniform vec3 light_col;
uniform vec3 object_col;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * light_col;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light_pos - frag_pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light_col;
    
    vec3 result = (ambient + diffuse) * object_col;
    FragColor = vec4(result, 1.0);
} 