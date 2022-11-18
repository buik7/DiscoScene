#version 330 core

struct PointLight {
    vec3 position;
    vec3 attenuation; // format : (kc, kl, kq)
    vec3 ambient;
    vec3 diffuse;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
out vec4 FragColor;

uniform PointLight pointLight;
uniform sampler2D ourTexture;

void main()
{
    vec3 objectColor = texture(ourTexture, TexCoord).rgb;

    // ambient
    vec3 ambient = pointLight.ambient * objectColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pointLight.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = pointLight.diffuse * diff * objectColor;

    // attenuation
    float dist = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.attenuation.x + pointLight.attenuation.y * dist + pointLight.attenuation.z * dist * dist);

    diffuse *= attenuation;

    FragColor = vec4(ambient + diffuse, 1.0);
}