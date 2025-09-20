#version 330

uniform mat4 mvp;
uniform mat4 matModel;

in vec3 vertexPosition;
in vec3 vertexNormal;

out vec3 FragPosition;
out vec3 FragNormal;

void main() {
    FragPosition = vec3(matModel * vec4(vertexPosition, 1.0));
    FragNormal = normalize(mat3(transpose(inverse(matModel))) * vertexNormal);

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
