#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Light {
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
    float Radius;
};

const int NR_LIGHTS = 8;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;

uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

uniform mat4 view;

void main()
{
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    float AmbientOcclusion = texture(ssao, TexCoords).r;

    if(FragPos.r == 0 && FragPos.g == 0 && FragPos.b == 0)
    {
        FragColor = vec4(Diffuse, 1.0); // no coordinates specified = skybox
    }
    else
    {
        // then calculate lighting as usual
        vec3 ambient = vec3(0.2 * Diffuse * AmbientOcclusion);
        vec3 lighting  = ambient;

        vec3 viewDir  = normalize(vec3(view * vec4(viewPos, 0)) - FragPos);
        for (int i = 0; i < NR_LIGHTS; ++i)
        {
            vec3 light_pos = vec3(view * vec4(lights[i].Position, 0));
            // calculate distance between light source and current fragment


            float distance = length(light_pos - FragPos);
            if (distance < lights[i].Radius)
            {
                // diffuse
                vec3 lightDir = normalize(light_pos - FragPos);
                vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
                // specular
                vec3 halfwayDir = normalize(lightDir + viewDir);
                float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
                vec3 specular = lights[i].Color * spec * Specular;
                // attenuation
                float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
                diffuse *= attenuation;
                specular *= attenuation;
                lighting += diffuse + specular;
            }
        }
        FragColor = vec4(lighting, 1.0) * (AmbientOcclusion * 2);
    }
}