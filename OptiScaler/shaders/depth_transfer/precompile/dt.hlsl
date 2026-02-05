#ifdef VK_MODE
[[vk::binding(0, 0)]]
#endif
Texture2D<float> SourceTexture : register(t0);

#ifdef VK_MODE
[[vk::binding(1, 0)]]
#endif
RWTexture2D<float> DestinationTexture : register(u0);

// Shader to perform the conversion
[numthreads(16, 16, 1)]
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float srcColor = SourceTexture.Load(int3(dispatchThreadID.xy, 0));
    DestinationTexture[dispatchThreadID.xy] = srcColor;
}