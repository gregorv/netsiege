#version 150

/* Cel shading fragment program for single-pass rendering */
// uniform vec4 diffuse;
// uniform vec4 specular;
uniform sampler2D normalMap;
uniform sampler2D blendMap0;
uniform sampler2D blendMap1;
uniform sampler2D layer0;
uniform sampler2D layer1;
uniform sampler2D layer2;
uniform sampler2D layer3;
uniform sampler2D layer4;
uniform sampler2D layer5;
uniform sampler2D layer6;
uniform sampler2D layer7;
uniform sampler2D layer8;

in vec2 texcoordNormal;
in vec2 texcoord;
in vec4 pixelPos;
uniform float texScale0;
uniform float texScale1;
uniform float texScale2;
uniform float texScale3;
uniform float texScale4;
uniform float texScale5;
uniform float texScale6;
uniform float texScale7;
uniform float texScale8;


uniform vec4 lightPositionWorld; // object space
uniform vec4 lightDiffuse; // object space
uniform vec4 lightAttenuation;
uniform vec3 eyePosition;   // object space
uniform float lightNumber;
// in float diffuseIn;
// in float specularIn;
// in float edge;


/*uniform lighting
{
	vec4 diffuse;
	vec4 specular;
} LightingParams;*/

float blendStrength(vec4 b0, vec4 b1, float layerIdx)
{
    vec4 mask0 = min(max((vec4(1.0, 2.0, 3.0, 4.0)-layerIdx), 0.0), 1.0);
    vec4 mask1 = min(max((vec4(5.0, 6.0, 7.0, 8.0)-layerIdx), 0.0), 1.0);
    return min(1.0, max(0.0, 1.0 - (dot(b0, mask0) + dot(b1, mask1))));
}

void main()
{
     vec4 blend0 = texture(blendMap0, texcoordNormal);
     vec4 blend1 = texture(blendMap1, texcoordNormal);
     vec3 color = texture(layer0, texcoord*texScale0).xyz * blendStrength(blend0, blend1, 0)
		+ texture(layer1, texcoord*texScale1).xyz * blendStrength(blend0, blend1, 1) * blend0.x
		+ texture(layer2, texcoord*texScale2).xyz * blendStrength(blend0, blend1, 2) * blend0.y
		+ texture(layer3, texcoord*texScale3).xyz * blendStrength(blend0, blend1, 3) * blend0.z
		+ texture(layer4, texcoord*texScale4).xyz * blendStrength(blend0, blend1, 4) * blend0.w
		+ texture(layer5, texcoord*texScale5).xyz * blendStrength(blend0, blend1, 5) * blend1.x
		+ texture(layer6, texcoord*texScale6).xyz * blendStrength(blend0, blend1, 6) * blend1.y
		+ texture(layer7, texcoord*texScale7).xyz * blendStrength(blend0, blend1, 7) * blend1.z
		+ texture(layer8, texcoord*texScale8).xyz * blend1.w;
     gl_FragColor = vec4(color * 0.1, 1.0);
}
