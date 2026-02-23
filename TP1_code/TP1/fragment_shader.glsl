#version 330 core

// Ouput data
in vec2 uv;
out vec3 color;
in vec4 position;
in float TerrainNoise;

uniform sampler2D myRockSampler;
uniform sampler2D myGrassSampler;
uniform sampler2D mySnowRocksSampler;
uniform sampler2D myPlaneteSampler;
uniform int mode;

void main(){
        if(mode == 1){ // mode 1 pour le terrain
            if (position.y+TerrainNoise < 0.5) {
                color = texture(myGrassSampler,uv).rgb;
            } else if (position.y+TerrainNoise <0.7) {
                color = texture(myRockSampler,uv).rgb;
            } else {
                color = texture(mySnowRocksSampler,uv).rgb;
            }
        }else{ // sinon planete
            color = texture(myPlaneteSampler,uv).rgb;
        }


}
