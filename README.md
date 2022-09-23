# opengl-stuff

## 题纲
1. 材质对于光照效果的影响(颜色分量)
```c
struct Material {
    sampler2D diffuse;//漫反射光照下表面的颜色
    sampler2D specular;//表面镜像高光的颜色
    float shininess;//反光度，代表反射光的能力，反射光的能力越强，散射得越少，高光点就会越小
};
```

2. 三种投光物光照形式（DirLight、PointLight、SpotLight）在片段着色器中的表现形式，及主程序中的输入形式；
```c
//定向光
struct DirLight {
    //不需要位置信息
    vec3 direction;//在计算时需要对direction取反，
    vec3 ambient;//环境光照
    vec3 diffuse;//漫反射光照
    vec3 specular;//镜面光照
};
//点射光
struct PointLight {
    vec3 position;//点光源位置
    //没有方向信息
    float constant;//衰减：常数项
    float linear;//衰减：一次项
    float quadratic;//衰减：二次项
	
    vec3 ambient;//同定向光，三种光照向量
    vec3 diffuse;
    vec3 specular;
};
//聚光
struct SpotLight {
    vec3 position;//光源位置
    vec3 direction;//光线方向向量
    float cutOff;//内切广角，代表聚光半径
    float outerCutOff;//外切光角，内切光到外切光之间会逐渐衰减
  
    float constant;//衰减项，同点射光
    float linear;
    float quadratic;
  
    vec3 ambient;//同定向光，三种光照向量
    vec3 diffuse;
    vec3 specular;       
};
```

3. 三种光照在片段着色器中的计算形式
```c
//定向光计算
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);//需要一个从片段至光源的光线方向，而direction的物理性质为从光源到片段的方向（全局方向），因此需要取反
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);//避免夹角大于90°时，点乘为负，因此需要做输出限制
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);//视线方向与反射方向的点乘（并确保它不是负值），然后取它的反光度的次幂
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

//点射光计算，与定向光的差异是需要计算光的方向，以及考虑衰减（attenuation）
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);//计算光的方向，片段指向光源
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation 
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));//通过衰减公式，计算衰减attenuation    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;//各光分量乘以同样的衰减系数
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

//聚光计算，与点射光的差异主要是需要通过内外光锥完成光边缘的软化/平滑
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); //LightDir向量和SpotDir向量之间的夹角余弦,即光显示的范围
    float epsilon = light.cutOff - light.outerCutOff;//内圆锥与外圆锥之间的余弦差
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);//通过公式计算聚光的强度
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation * intensity;//各光分量乘以衰减系数乘以聚光平滑系数
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
```
最终将三种光类型计算的结果求和，得到光照组合后的最终结果。

4. 顶点着色器中的坐标计算，及主程序中的输入定义
```GLSL
#version 330 core
layout (location = 0) in vec3 aPos; //通过VAO/VBO配置
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
```

顶点数据的存储形式为(节选)：
```GLSL
float vertices[] = {
        // positions          // normals           // texture coords
	...
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	...
	};	
```
