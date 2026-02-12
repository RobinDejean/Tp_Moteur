#version 330 core

// Ouput data
in vec2 uv;
out vec3 color;
in vec4 position;

uniform sampler2D myRockSampler;
uniform sampler2D myGrassSampler;
uniform sampler2D mySnowRocksSampler;
float rand(vec2 co){
                return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453) * 0.1-0.05;
        }
void main(){
        
        //float r = rand(uv);
        float r = 0;
        if (position.y+r < 0.5) {
            color = texture(myGrassSampler,uv).rgb;
        } else if (position.y+r <0.7) {
            color = texture(myRockSampler,uv).rgb;
        } else {
            color = texture(mySnowRocksSampler,uv).rgb;
        }



}
