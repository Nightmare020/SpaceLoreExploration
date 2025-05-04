Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer GlowBuffer : register(b0)
{
    float4 glowColor; // Color of the glow (e.g., yellowish)
    float glowThreshold; // Brightness threshold
    float glowIntensity; // Multiplier for glow brightness
    float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float4 texColor = shaderTexture.Sample(SampleType, input.tex);

    // Apply threshold
    float brightness = dot(texColor.rgb, float3(0.299, 0.587, 0.114));
    float glowFactor = saturate((brightness - glowThreshold) * glowIntensity);

    // Combine original color with glow tint (not override!)
    float4 result = texColor + glowColor * glowFactor;
    result.a = texColor.a;

    return saturate(result); // prevent overbright
}