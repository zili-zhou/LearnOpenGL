#version 450 core

struct material
{//������ʵ����ֹ�����ͷ����
	sampler2D diffuse;//������������ɫ����Ϊ��ͼ
	sampler2D specular;//���ʾ��淴����ɫ����Ϊ��ͼ
	float shininess;
};

//���ֹ�Դ
//����⣬��Ҫ����ⷽ��
struct directionlight
{
	vec3 LightDir;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
};

//���Դ����Ҫ������λ�úͳ����һ����������˥��ϵ��
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

//�۹�ƣ���������Դ������˥��������Ҫ�ⷽ���ھ��Ǻ��⾶��(����)
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

//���㷽��⣬���Դ�;۹��
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
	//������
	vec3 ambientColor = light.ambient * texture(Material.diffuse,TexCoordinate).rgb;
	
	//������
	vec3 lightIn = normalize(-light.LightDir);
	float diffuseStrength = max(dot(lightIn,normal),0.0f);
	vec3 diffuseColor = light.diffuse * texture(Material.diffuse,TexCoordinate).rgb * diffuseStrength;

	//���淴��
	vec3 halfVec = normalize(lightIn + viewDir);
	float specularStrength = pow(max(dot(halfVec, normal),0.0f),Material.shininess);
	vec3 specularColor = light.specular * texture(Material.specular,TexCoordinate).rgb * specularStrength;

	return ambientColor + diffuseColor + specularColor;
}

vec3 CalculatePointLight(pointlight light, vec3 FragPos, vec3 normal, vec3 viewDir)
{
	//������
	vec3 ambientColor = light.ambient * texture(Material.diffuse,TexCoordinate).rgb;
	
	//������
	vec3 lightIn = normalize(light.LightPos - FragPos);
	float diffuseStrength = max(dot(lightIn,normal),0.0f);
	vec3 diffuseColor = light.diffuse * texture(Material.diffuse,TexCoordinate).rgb * diffuseStrength;

	//���淴��
	vec3 halfVec = normalize(lightIn + viewDir);
	float specularStrength = pow(max(dot(halfVec, normal),0.0f),Material.shininess);
	vec3 specularColor = light.specular * texture(Material.specular,TexCoordinate).rgb * specularStrength;

	//˥��
	float dis = length(light.LightPos - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * dis + light.quadratic * dis *dis);

	ambientColor *= attenuation;
	diffuseColor *= attenuation;
	specularColor *= attenuation;

	return ambientColor + diffuseColor + specularColor;
}

vec3 CalculateSpotLight(spotlight light, vec3 FragPos, vec3 normal, vec3 viewDir)
{
	//������
	vec3 ambientColor = light.ambient * texture(Material.diffuse,TexCoordinate).rgb;
	
	//������
	vec3 lightIn = normalize(light.LightPos - FragPos);
	float diffuseStrength = max(dot(lightIn,normal),0.0f);
	vec3 diffuseColor = light.diffuse * texture(Material.diffuse,TexCoordinate).rgb * diffuseStrength;

	//���淴��
	vec3 halfVec = normalize(lightIn + viewDir);
	float specularStrength = pow(max(dot(halfVec, normal),0.0f),Material.shininess);
	vec3 specularColor = light.specular * texture(Material.specular,TexCoordinate).rgb * specularStrength;

	//˥��
	float dis = length(light.LightPos - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * dis + light.quadratic * dis *dis);

	//��Ե��
	float theta = dot(lightIn, normalize(-light.LightDir));//�������۹ⷽ��ļн�����
	float epsilon = light.innerAngle - light.outterAngle;//�ھ�������-�⾶������
	float intensity = clamp((theta - light.outterAngle) / epsilon, 0.0f, 1.0f);//��ǿ�Ȳ�ֵ��0-1֮��

	ambientColor *= attenuation * intensity;
	diffuseColor *= attenuation * intensity;
	specularColor *= attenuation * intensity;

	return ambientColor + diffuseColor + specularColor;
}

