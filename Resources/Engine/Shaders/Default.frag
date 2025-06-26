#version 460

struct Material
{
	vec3 BaseColor;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Metallic;
};

layout(std140) uniform Matrices
{
	mat4 ModelViewProjectionMatrix;
	mat4 ModelMatrix;
	mat4 ViewProjectMatrix;
};

uniform vec3 CameraPosition;

uniform vec3 LightPosition;
uniform vec4 LightColor;

uniform sampler2D BaseColorTexture;
uniform sampler2D DiffuseTexture;
uniform sampler2D NormalMapTexture;
uniform sampler2D RoughnessTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D MetallicTexture;
uniform sampler2D HeighMapTexture;

in vec3 FragPos;
in vec4 FragPosLightSpace;
in vec2 TextureCoords;
in vec3 Normal;
in vec3 Tangent;

out vec4 FragColor;

//float ShadowCalculation(vec4 fragPosLightSpace)
//{
//    // Perform perspective divide
//    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//    // Transform to [0,1] range
//    projCoords = projCoords * 0.5 + 0.5;
//    // Get closest depth value from light's perspective
//    float closestDepth = texture(shadowMap, projCoords.xy).r;
//    // Get depth of current fragment from light's perspective
//    float currentDepth = projCoords.z;
//    // Calculate bias (based on depth map resolution and slope)
//    vec3 normal = normalize(Normal);
//    vec3 lightDir = normalize(lightPos - FragPos);
//    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
//    // Check whether current frag pos is in shadow
//    // PCF for softer shadows
//    float shadow = 0.0;
//    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
//    for(int x = -1; x <= 1; ++x)
//    {
//        for(int y = -1; y <= 1; ++y)
//        {
//            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
//            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
//        }
//    }
//    shadow /= 9.0;
//    
//    // Keep shadow at 0.0 when outside the far plane region of the light's frustum
//    if(projCoords.z > 1.0)
//        shadow = 0.0;
//        
//    return shadow;
//}

void main()
{
	vec3 RealLightningColor = LightColor.rgb * LightColor.w;

	vec3 ObjectColor = texture(DiffuseTexture, TextureCoords).rgb;
	vec3 _Normal = normalize(Normal);

	float AmbientStrength = 0.1f;
	vec3 AmbientColor = AmbientStrength * RealLightningColor;

	vec3 LightDirection = normalize(LightPosition - FragPos);
	vec3 DiffuseColor = max(dot(LightDirection, _Normal), 0.f) * RealLightningColor;

	float SpecularStrength = 0.5f;
	vec3 ViewDirection = normalize(CameraPosition - FragPos);
	vec3 ReflectDirection = reflect(-LightDirection, _Normal); 
	vec3 SpecularColor = SpecularStrength * pow(max(dot(ViewDirection, ReflectDirection), 0.f), 32.f) * RealLightningColor;

	float Shadow = 1.f;                          // *
	vec3 Lightning = (AmbientColor + (1.f - Shadow) + (DiffuseColor + SpecularColor)) * ObjectColor;

	FragColor = vec4(Lightning, 1.f);
}
