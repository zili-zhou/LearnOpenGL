#version 450 core

struct material
{//定义材质的三种光分量和反光度
	sampler2D diffuse;//材质漫反射颜色定义为贴图
	sampler2D specular;//材质镜面反射颜色定义为贴图
	float shininess;
};

//三种光源
//方向光，需要定义光方向
struct directionlight
{
	vec3 LightDir;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
};

//点光源，需要定义光的位置和常数项，一次项二次项的衰减系数
struct pointlight
{
	vec3 LightPos;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

};

//聚光灯，类似与点光源，存在衰减。但需要光方向，内径角和外径角(余弦)
struct spotlight
{
	vec3 LightPos;
	vec3 LightDir;
	
	float innerAngle;
	float outterAngle;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define NUM_POINT_LIGHT 4

in vec3 VertexColor;
in vec2 TexCoordinate;
in vec3 VertexNormal;
in vec3 FragPos;


uniform directionlight DirLight;
uniform pointlight PointLights[NUM_POINT_LIGHT];
uniform spotlight SpotLight;
uniform material Material;

uniform vec3 CameraPos;

uniform bool IsDirectionLight = false;
uniform bool IsPointLight = false;
uniform bool IsSpotLight = false;

//计算方向光，点光源和聚光灯
vec3 CalculateDirLight(directionlight light, vec3 normal,vec3 viewDir);
vec3 CalculatePointLight(pointlight light, vec3 FragPos, vec3 normal, vec3 viewDir);
vec3 CalculateSpotLight(spotlight light, vec3 FragPos, vec3 normal, vec3 viewDir);


out vec4 FragColor;
void main()
{
	vec3 normal = normalize(VertexNormal);
	vec3 viewDir = normalize(CameraPos - FragPos);

	vec3 result = vec3(0.0f);

	if(IsDirectionLight)
		result += CalculateDirLight(DirLight, normal, viewDir);

	if(IsPointLight)
	{
		for(int i = 0; i < NUM_POINT_LIGHT; i++)
			result += CalculatePointLight(PointLights[i], FragPos, normal, viewDir);
	}

	if(IsSpotLight)
		result += CalculateSpotLight(SpotLight, FragPos, normal, viewDir);
	
	FragColor = vec4(result,1.0f);
}


vec3 CalculateDirLight(directionlight light, vec3 normal,vec3 viewDir)
{	
	//环境光
	vec3 ambientColor = light.ambient * texture(Material.diffuse,TexCoordinate).rgb;
	
	//漫反射
	vec3 lightIn = normalize(-light.LightDir);
	float diffuseStrength = max(dot(lightIn,normal),0.0f);
	vec3 diffuseColor = light.diffuse * texture(Material.diffuse,TexCoordinate).rgb * diffuseStrength;

	//镜面反射
	vec3 halfVec = normalize(lightIn + viewDir);
	float specularStrength = pow(max(dot(halfVec, normal),0.0f),Material.shininess);
	vec3 specularColor = light.specular * texture(Material.specular,TexCoordinate).rgb * specularStrength;

	return ambientColor + diffuseColor + specularColor;
}

vec3 CalculatePointLight(pointlight light, vec3 FragPos, vec3 normal, vec3 viewDir)
{
	//环境光
	vec3 ambientColor = light.ambient * texture(Material.diffuse,TexCoordinate).rgb;
	
	//漫反射
	vec3 lightIn = normalize(light.LightPos - FragPos);
	float diffuseStrength = max(dot(lightIn,normal),0.0f);
	vec3 diffuseColor = light.diffuse * texture(Material.diffuse,TexCoordinate).rgb * diffuseStrength;

	//镜面反射
	vec3 halfVec = normalize(lightIn + viewDir);
	float specularStrength = pow(max(dot(halfVec, normal),0.0f),Material.shininess);
	vec3 specularColor = light.specular * texture(Material.specular,TexCoordinate).rgb * specularStrength;

	//衰减
	float dis = length(light.LightPos - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * dis + light.quadratic * dis *dis);

	ambientColor *= attenuation;
	diffuseColor *= attenuation;
	specularColor *= attenuation;

	return ambientColor + diffuseColor + specularColor;
}

vec3 CalculateSpotLight(spotlight light, vec3 FragPos, vec3 normal, vec3 viewDir)
{
	//环境光
	vec3 ambientColor = light.ambient * texture(Material.diffuse,TexCoordinate).rgb;
	
	//漫反射
	vec3 lightIn = normalize(light.LightPos - FragPos);
	float diffuseStrength = max(dot(lightIn,normal),0.0f);
	vec3 diffuseColor = light.diffuse * texture(Material.diffuse,TexCoordinate).rgb * diffuseStrength;

	//镜面反射
	vec3 halfVec = normalize(lightIn + viewDir);
	float specularStrength = pow(max(dot(halfVec, normal),0.0f),Material.shininess);
	vec3 specularColor = light.specular * texture(Material.specular,TexCoordinate).rgb * specularStrength;

	//衰减
	float dis = length(light.LightPos - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * dis + light.quadratic * dis *dis);

	//边缘软化
	float theta = dot(lightIn, normalize(-light.LightDir));//入射光与聚光方向的夹角余弦
	float epsilon = light.innerAngle - light.outterAngle;//内径角余弦-外径角余弦
	float intensity = clamp((theta - light.outterAngle) / epsilon, 0.0f, 1.0f);//将强度插值在0-1之间

	ambientColor *= attenuation * intensity;
	diffuseColor *= attenuation * intensity;
	specularColor *= attenuation * intensity;

	return ambientColor + diffuseColor + specularColor;
}

