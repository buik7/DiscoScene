#version 330 core

struct SpotLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 attenuation;
	vec3 position;
	vec3 direction;
	float cutoffAngle;    // The cosine value of cutoff angle
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
out vec4 FragColor;

uniform SpotLight spotlights[3];
uniform sampler2D ourTexture;

void main()
{
    vec3 objectColor = texture(ourTexture, TexCoord).rgb;
    vec3 norm = normalize(Normal);

    vec3 result = vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 3; i++) {
        // ambient
        vec3 ambient = spotlights[i].ambient * objectColor;
        vec3 lightDir = normalize(spotlights[i].position - FragPos);

        float theta = dot(lightDir, normalize(-spotlights[i].direction));
        if (theta > spotlights[i].cutoffAngle) {
            // diffuse
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = spotlights[i].diffuse * diff * objectColor;

            // attenuation
            float dist = length(spotlights[i].position - FragPos);
            float attenuation = 1.0 / (spotlights[i].attenuation.x + spotlights[i].attenuation.y * dist + spotlights[i].attenuation.z * dist * dist);

            diffuse *= attenuation;
            result += diffuse;
        }
        result += ambient;
    }

    FragColor = vec4(result, 1.0);
}