#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screentexture;

void main()
{
	vec3 TexColor = texture(screentexture,TexCoords).rgb;
	//FragColor=vec4(TexColor,1.0f);//������ɫ
	//FragColor = vec4(1-TexColor,1.0f);//��ɫ
	/*//�Ҷ�
	float average = 0.2216*TexColor.r + 0.7152*TexColor.g + 0.0722*TexColor.b;
	FragColor = vec4(average,average,average, 1.0f);
	*/
    //��/ģ��
    const float offset = 1.0 / 300.0;  
	vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // ����
        vec2( 0.0f,    offset), // ����
        vec2( offset,  offset), // ����
        vec2(-offset,  0.0f),   // ��
        vec2( 0.0f,    0.0f),   // ��
        vec2( offset,  0.0f),   // ��
        vec2(-offset, -offset), // ����
        vec2( 0.0f,   -offset), // ����
        vec2( offset, -offset)  // ����
    );

    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screentexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i];

    FragColor = vec4(col/9.0f, 1.0);
}