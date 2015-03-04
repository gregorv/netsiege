#version 110

// General functions
// parameters
uniform mat4 worldViewProj;
uniform mat4 world;

attribute vec4 vertex;
// attribute vec4 uv;

varying vec2 texcoordNormal;
varying vec2 texcoord;
varying vec4 pixelPos;

/* Vertex program which includes specular component */
void main()
{
	// calculate output position
	pixelPos = world * vertex;
	gl_Position = worldViewProj * vertex;

	// pass the main uvs straight through unchanged
	//texcoord = vec2(vertex.x + vertex.y, vertex.z + vertex.y);
	texcoord = vec2(pixelPos.xz);
	texcoordNormal = gl_MultiTexCoord0.xy;

// 	// calculate tangent space light vector
// 	// Get object space light direction
// 	vec3 lightDir = normalize(lightPosition.xyz - (vertex * lightPosition.w).xyz);
//
// 	// Calculate the binormal (NB we assume both normal and tangent are
// 	// already normalised)
// 	vec3 binormal = cross(normal, tangent);
//
// 	// Form a rotation matrix out of the vectors
// 	mat3 rotation = mat3(vec3(tangent[0], binormal[0], normal[0]),
// 						vec3(tangent[1], binormal[1], normal[1]),
// 						vec3(tangent[2], binormal[2], normal[2]));
//
// 	// Transform the light vector according to this matrix
// 	oTSLightDir = rotation * lightDir;
//
// 	// Calculate half-angle in tangent space
// 	vec3 eyeDir = normalize(eyePosition - vertex.xyz);
// 	vec3 halfAngle = normalize(eyeDir + lightDir);
// 	oTSHalfAngle = rotation * halfAngle;
}
