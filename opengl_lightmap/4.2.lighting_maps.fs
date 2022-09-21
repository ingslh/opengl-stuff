#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;//环境光等同于漫反射光
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);//指向光源方向
    float diff = max(dot(norm, lightDir), 0.0);//对norm和lightDir向量进行点乘(确定夹角)，计算光源对当前片段实际的漫发射影响
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb; //光的颜色*漫反射分量*纹理颜色   
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);//计算视线方向向量
    vec3 reflectDir = reflect(-lightDir, norm);//对lightDir向量进行了取反,reflect函数要求第一个向量是从光源指向片段位置的向量   
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
} 
