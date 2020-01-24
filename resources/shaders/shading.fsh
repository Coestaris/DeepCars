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

struct ShadowLight {
    vec3 Position;
    vec3 Direction;

    float Brightness;

    mat4 lightSpaceMatrix;
    sampler2D shadowMap;
};

const int NR_LIGHTS = 15;

//uniform sampler2D gViewPosition;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;

uniform Light lights[NR_LIGHTS];
uniform ShadowLight shadowLight;
uniform vec3 viewPos;

uniform mat4 view;

vec3 ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 frag_pos, vec3 frag_diffuse, float frag_spec, vec3 viewDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowLight.shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 lightDir = normalize(shadowLight.Position - frag_pos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowLight.shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowLight.shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    lightDir = normalize(-shadowLight.Direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 diffuse = shadowLight.Brightness * diff * frag_diffuse;
    vec3 specular = vec3(1) * spec * frag_spec;
    return (diffuse + specular) * (1 - shadow);
}

void main()
{
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;

    if(FragPos.r == 0 && FragPos.g == 0 && FragPos.b == 0)
    {
        FragColor = vec4(Diffuse, 1.0); // no coordinates specified = skybox
    }
    else
    {
        float dist_to_frag = length(viewPos - FragPos);
        float AmbientOcclusion = 0;
        vec3 lighting = Diffuse;

        if(dist_to_frag > 150)
        {
            float k = max(1 - (dist_to_frag - 120.0f) / 120.0f, 0);
            float ssao = texture(ssao, TexCoords).r;
            AmbientOcclusion = mix(ssao, 1, 1-k);
        }
        else
            AmbientOcclusion = texture(ssao, TexCoords).r;

        if(dist_to_frag < 500)
        {
            vec3 Normal = texture(gNormal, TexCoords).rgb;
            float Specular = texture(gAlbedoSpec, TexCoords).a;
            Normal = vec3(inverse(view) * vec4(Normal, 0));

            lighting  *= Diffuse * 0.1;// hard-coded ambient component
            vec3 viewDir  = normalize(viewPos - FragPos);
            for (int i = 0; i < NR_LIGHTS; ++i)
            {
                float distance = length(lights[i].Position - FragPos);
                if (distance < lights[i].Radius)
                {
                    // diffuse
                    vec3 lightDir = normalize(lights[i].Position - FragPos);
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
            //lighting += calculateDirectional();
            lighting += ShadowCalculation(shadowLight.lightSpaceMatrix * vec4(FragPos, 1.0), Normal, FragPos,
            Diffuse, Specular, viewDir);
        }

        FragColor = vec4(lighting, 1.0) * pow(AmbientOcclusion, 1.5);
    }
}