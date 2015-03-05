#version 110

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

varying vec2 texcoordNormal;
varying vec2 texcoord;
varying vec4 pixelPos;
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
     vec3 normal = normalize(texture2D(normalMap, texcoordNormal).xyz - 0.5);
#ifndef AMBIENT
     vec3 lightDir = normalize(lightPositionWorld.xyz - pixelPos.xyz*lightPositionWorld.w);
     float light = dot(normal, lightDir);
     float dist = distance(pixelPos.xyz, lightPositionWorld.xyz);
     float attenuate = min(1.0, 1.0 / (lightAttenuation.y + lightAttenuation.z*dist + lightAttenuation.w*dist*dist));
     attenuate *= clamp(lightAttenuation.x - dist, 0.0, 1.0);
#endif

     vec4 blend0 = texture2D(blendMap0, texcoordNormal);
     vec4 blend1 = texture2D(blendMap1, texcoordNormal);
     vec3 color = texture2D(layer0, texcoord*texScale0).xyz * blendStrength(blend0, blend1, 0)
		+ texture2D(layer1, texcoord*texScale1).xyz * blendStrength(blend0, blend1, 1) * blend0.x
		+ texture2D(layer2, texcoord*texScale2).xyz * blendStrength(blend0, blend1, 2) * blend0.y
		+ texture2D(layer3, texcoord*texScale3).xyz * blendStrength(blend0, blend1, 3) * blend0.z
		+ texture2D(layer4, texcoord*texScale4).xyz * blendStrength(blend0, blend1, 4) * blend0.w
		+ texture2D(layer5, texcoord*texScale5).xyz * blendStrength(blend0, blend1, 5) * blend1.x
		+ texture2D(layer6, texcoord*texScale6).xyz * blendStrength(blend0, blend1, 6) * blend1.y
		+ texture2D(layer7, texcoord*texScale7).xyz * blendStrength(blend0, blend1, 7) * blend1.z
		+ texture2D(layer8, texcoord*texScale8).xyz * blend1.w;

#ifdef AMBIENT
    gl_FragColor = vec4(color*0.1, 1.0);
#else
//      gl_FragColor = vec4(color, 1.0)*0.1;
     gl_FragColor = vec4(color * lightDiffuse.xyz * attenuate * light, 1.0);
#endif
//      gl_FragColor = vec4(vec3(1.0, 1.0, 1.0) * 0.0, 1.0);
//      gl_FragColor = texture(normalMap, texcoordNormal);
//      gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0) * dist/200.0;
//      gl_FragColor = pixelPos/400.0;
//      gl_FragColor = vec4(lightDir*0.5 + 0.5, 1.0);

//      gl_FragColor = vec4(1.0, lightNumber, lightNumber-1.0, 1.0);
// 	// Step functions from textures
// 	float diffuseStep = texture(diffuseRamp, diffuseIn).x;
// 	float specularStep = texture(specularRamp, specularIn).x;
// 	float edgeStep = texture(edgeRamp, edge).x;
//
// 	fragColour = edgeStep * ((diffuse * diffuseStep) +
//                             (specular * specularStep));
//	fragColour = edgeStep * ((LightingParams.diffuse * diffuseStep) +
//                        (LightingParams.specular * specularStep));
}
