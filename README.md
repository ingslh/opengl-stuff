# opengl-stuff

## 题纲
0. 材质对于光照效果的影响(颜色分量)
```c
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
```

1. 三种光照形式（DirLight、PointLight、SpotLight）在片段着色器中的表现形式，及主程序中的输入形式；
```c
//材质
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
```

2. 三种光照在片段着色器中的计算形式
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

3. 顶点着色器中的坐标计算，及主程序中的输入定义
