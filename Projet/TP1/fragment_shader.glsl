#version 330 core

// Ouput data
in vec2 uv;
out vec4 color;
in vec4 position;
in float TerrainNoise;
in mat3 TBN;

uniform sampler2D myRockSampler;
uniform sampler2D myGrassSampler;
uniform sampler2D mySnowRocksSampler;
uniform sampler2D myPlaneteSampler;



uniform sampler2D myDiffuseSampler;
uniform sampler2D myMetallicSampler;
uniform sampler2D myRoughnessSampler;
uniform sampler2D myNormalSampler;
uniform int mode;
vec4 waterColor = vec4(0.0f, 0.5f, 1.0f, 0.3f); // bleu semi-transparent

uniform vec3 lightPos = vec3(-10.0f, 10.0f, -10.0f);
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
float intensity = 2.0f;
/* uniform vec3 F0 = vec3(0.04); */
uniform vec3 camPos;
const float PI = 3.14159265359;


float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

void main(){
    vec3 radiance = lightColor * intensity;
    vec3 albedo = pow(texture(myDiffuseSampler, uv).rgb, vec3(2.2)); // Correction Gamma
    //float metallic = texture(myMetallicSampler, uv).r;
    float metallic = 0.0;
    float roughness = texture(myRoughnessSampler, uv).r;
    roughness = clamp(roughness, 0.05, 1.0);
    vec3 lightDir = normalize(lightPos - position.xyz);
    vec3 V = normalize(camPos - position.xyz);
    vec3 Normal = texture(myNormalSampler, uv).rgb;
    //Normal = vec3(0.0, 1.0, 0.0);
    
    vec3 N = Normal * 2.0 - 1.0;
    N = normalize(TBN * N);
    
    //vec3 N = vec3(0.0, 1.0, 0.0);
    float cosTheta = dot(lightDir, N);  

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);

    vec3 L = normalize(lightPos - position.xyz);
    vec3 H = normalize(V + L);
    
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 finalColor = Lo / (Lo + vec3(1.0));
    finalColor = pow(finalColor, vec3(1.0/2.2));

    color = vec4(finalColor, 1.0);
    //color = vec4(abs(TBN[0]), 1.0);
    /* vec3 n = texture(myNormalSampler, uv).rgb;
    color = vec4(n, 1.0);
 */    



    if(mode == 2){
        color = waterColor;
    }
    else if(mode == 1){ // mode 1 pour le terrain
        if (position.y+TerrainNoise < 0.2) {
            color = vec4(texture(myGrassSampler,uv).rgb,1.);
        } else if (position.y+TerrainNoise <0.35) {
            color = vec4(texture(myRockSampler,uv).rgb,1.);
        } else {
            color = vec4(texture(mySnowRocksSampler,uv).rgb,1.);
        }
    }else{ // sinon planete
        //color = vec4(texture(myPlaneteSampler,uv).rgb,1.);
    }


}
