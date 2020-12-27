struct VS_OUTPUT
{
	float4 PosOut : TEXCOORD0;
	float4 Normal : TEXCOORD1;
};

#define HLSL_INCLUDE
#include "VariableBuffer.hpp"

/*
cbuffer VariableBuffer : register(b0)
{
	float time; // represents the seconds since the rendering started.
	float forced_intensity;
	float red_mod;
	float green_mod;
	float blue_mod;
	float mousex;
	float mousey;
	float flick_rate;
	float lock_to_mouse;
};*/

float dist(float2 pointA, float2 pointB)
{
	return sqrt(
		pow(
			abs(pointB.x - pointA.x), 
			2) + 
		pow(
			abs(pointB.y - pointA.y), 
			2)
		);
}

float4 main(VS_OUTPUT In) : SV_TARGET
{
	float out_alpha = 1.f;
	float4 out_colour;


	float x = In.Normal.x;
	float y = In.Normal.y;

	float2 p = float2(mousex, mousey);

	float calculated_intensity = sin(time * flick_rate * 10);
	
	// float n = (1.f - dist(p, In.Normal)) * calculated_intensity; // produces a sort of circle instead of a flat surface.

	float n = /*(1.f - dist(p, In.Normal)) **/ calculated_intensity;

	out_colour = float4(n * red_mod * forced_intensity, n * green_mod * forced_intensity, n * blue_mod * forced_intensity, 1.f);

	return out_colour;
}