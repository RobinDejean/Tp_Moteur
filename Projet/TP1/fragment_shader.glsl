#version 330 core

// Ouput data
in vec2 uv;
out vec4 color;
in vec4 position;
in float TerrainNoise;

uniform sampler2D myRockSampler;
uniform sampler2D myGrassSampler;
uniform sampler2D mySnowRocksSampler;
uniform sampler2D myPlaneteSampler;
uniform int mode;
vec4 waterColor = vec4(0.0f, 0.5f, 1.0f, 0.3f); // bleu semi-transparent

void main(){
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
        color = vec4(texture(myPlaneteSampler,uv).rgb,1.);
    }


}
