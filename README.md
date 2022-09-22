# opengl-stuff

## 题纲
1. 材质对于光照效果的影响(颜色分量)
```c
struct Material {
    sampler2D diffuse;//漫反射光照下表面的颜色
    sampler2D specular;//表面镜像高光的颜色
    float shininess;//影响镜面高光的散射/半径
};

vec3 res = vec3(texture(material.diffuse, TexCoords))//输入
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
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}
```

4. 顶点着色器中的坐标计算，及主程序中的输入定义
