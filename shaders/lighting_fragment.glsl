#version 330

uniform vec3 blockColor;
uniform vec3 cameraPosition;

out vec4 FragColor;

in vec3 FragPosition;
in vec3 FragNormal;

void main() {
    vec3 lightPosition = vec3(-50, 500, -50);
    vec3 lightAmbient = vec3(1.0, 1.0, 1.0);
    vec3 lightDiffuse = vec3(1.0, 1.0, 1.0);
    vec3 lightSpecular = vec3(0.5, 0.5, 0.5);

    vec3 blockAmbient = vec3(0.4, 0.4, 0.4);
    vec3 blockDiffuse = vec3(0.5, 0.5, 0.5);
    vec3 blockSpecular = vec3(1.0, 1.0, 1.0);

    vec3 ambient = lightAmbient * blockAmbient;

    vec3 lightDirection = normalize(FragPosition - lightPosition);
    float diff = max(dot(FragNormal, -lightDirection), 0);
    vec3 diffuse = diff * lightDiffuse * blockDiffuse;

    vec3 viewDirection = normalize(cameraPosition - FragPosition);
    vec3 reflectDirection = reflect(lightDirection, FragNormal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0), 32);
    vec3 specular = spec * lightSpecular * blockSpecular;

    FragColor = vec4((ambient + diffuse + specular) * blockColor, 1.0);
}
