
#version 450 core

struct material
{//定义材质的三种光分量和反光度
	sampler2D diffuse;//材质漫反射颜色定义为贴图
	sampler2D specular;//材质镜面反射颜色定义为贴图
	float shininess;
};

struct light
{//定义光源的三种光分量和位置
	vec3 LightPos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
};

in vec3 VertexColor;
in vec2 TexCoordinate;
in vec3 VertexNormal;
in vec3 FragPos;


uniform light Light;
uniform material Material;

uniform vec3 CameraPos;

out vec4 FragColor;

float near = 0.01f;
float far = 100.0f;

float ZView(float depth)
{
	float z_view = 2 * far * near/(( far + near ) + depth * ( near - far));
	return z_view;
}

void main()
{
	//计算光照
	//环境光
	float ambientStrength = 0.4f;
	vec3 ambientColor = Light.ambient * texture(Material.diffuse,TexCoordinate).rgb;
	
	//漫反射
	vec3 normal = normalize(VertexNormal);
	vec3 lightIn = normalize(Light.LightPos - FragPos);
	float diffuseStrength = max(dot(lightIn,normal),0.0f);
	vec3 diffuseColor = Light.diffuse * texture(Material.diffuse,TexCoordinate).rgb * diffuseStrength;

	//镜面反射
	vec3 viewDir = normalize(CameraPos - FragPos);
	vec3 halfVec = normalize(lightIn + viewDir);
	float specularStrength = pow(max(dot(halfVec, normal),0.0f),Material.shininess);
	vec3 specularColor = Light.specular * texture(Material.specular,TexCoordinate).rgb * specularStrength;


	FragColor=vec4(ambientColor + diffuseColor + specularColor ,1.0f);
	//FragColor = vec4(vec3(ZView(gl_FragCoord.z)), 1.0);
}
