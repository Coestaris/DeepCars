#version 330 core
out float FragColor;

in vec2 TexCoord;

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
#define kernelSize 32
#define radius  4
#define bias 0.015

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(1200.0/4.0, 800.0/4.0);

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[kernelSize];
uniform mat4 projection;

void main()
{
    // get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, TexCoord).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoord).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoord * noiseScale).xyz);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; i++)
    {
        // get sample position
        vec3 tbnsample = TBN * samples[i]; // from tangent to view-space
        tbnsample = fragPos + tbnsample * radius;

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(tbnsample, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= tbnsample.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);
    //FragColor = pow(occlusion, 4);
    FragColor = occlusion;
}