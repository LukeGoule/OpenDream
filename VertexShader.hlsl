#include "Matrix.hlsli"

struct VS_INPUT
{
    float4 Pos : POSITION0;
};

struct VS_OUTPUT
{
    float4 Pos      : SV_POSITION;
    float4 PosOut   : TEXCOORD0;
    float3 Normal   : TEXCOORD1;
};

cbuffer VariableBuffer : register(b0)
{
    float time;
};

VS_OUTPUT main(VS_INPUT In)
{
    VS_OUTPUT Out;

    Out.Pos = In.Pos;
    Out.PosOut = In.Pos;
    Out.Normal = float3(In.Pos.x, In.Pos.y, In.Pos.z);

    return Out;
}