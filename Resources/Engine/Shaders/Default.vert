#version 460

layout(std140) uniform Matrices
{
	mat4 ModelViewProjectionMatrix;
	mat4 ModelMatrix;
	mat4 ViewProjectMatrix;
};

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec2 in_TextureCoords;
layout(location = 2) in vec3 in_Normal;
layout(location = 3) in vec3 in_Tangent;

out vec3 FragPos;
out vec4 FragPosLightSpace;
out vec2 TextureCoords;
out vec3 Normal;
out vec3 Tangent;

void main()
{
	FragPos = vec3(ModelMatrix * vec4(in_Position, 1.f));
	TextureCoords = in_TextureCoords;
	Normal = mat3(transpose(inverse(ModelMatrix))) * in_Normal;
	Tangent = in_Tangent;
	FragPosLightSpace = ViewProjectMatrix * vec4(FragPos, 1.f);

	gl_Position = ModelViewProjectionMatrix * vec4(in_Position, 1.f);
}
