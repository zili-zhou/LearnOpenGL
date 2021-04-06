#version 450 core

struct material
{//������ʵ����ֹ�����ͷ����
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct light
{//�����Դ�����ֹ������λ��
	vec3 LightPos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 shininess;
};

in vec3 VertexColor;
in vec2 TexCoordinate;
in vec3 VertexNormal;
in vec3 FragPos;

uniform sampler2D outtexture1;
uniform sampler2D outtexture2;

uniform float mixvalue;

//uniform vec3 LightColor;

uniform light Light;
uniform material Material;

uniform vec3 CameraPos;

out vec4 FragColor;
void main()
{
	vec4 TextureColor = mix(texture(outtexture1,TexCoordinate),texture(outtexture2,TexCoordinate),mixvalue);

	//�������
	//������
	float ambientStrength = 0.4f;
	vec3 ambientColor = Light.ambient * Material.ambient;
	
	//������
	vec3 normal = normalize(VertexNormal);
	vec3 lightIn = normalize(Light.LightPos - FragPos);
	float diffuseStrength = max(dot(lightIn,normal),0.0f);
	vec3 diffuseColor = Light.diffuse * Material.diffuse * diffuseStrength;

	//���淴��
	vec3 viewDir = normalize(CameraPos - FragPos);
	vec3 halfVec = normalize(lightIn + viewDir);
	int specularCoefficient = 32;
	float specularStrength = pow(max(dot(halfVec, normal),0.0f),specularCoefficient) * 0.5f;
	vec3 specularColor = Light.specular * Material.specular * specularStrength;

	FragColor=vec4(ambientColor + diffuseColor + specularColor,1.0f)*TextureColor;
}
