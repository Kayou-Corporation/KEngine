// particleLife.comp.slang

struct Particles
{
    StructuredBuffer<float4> inPositions;
    StructuredBuffer<float4> inVelocities;
    RWStructuredBuffer<float4> outPositions;
    RWStructuredBuffer<float4> outVelocities;
};
ParameterBlock<Particles> particles;

struct GlobalUniform
{
    float deltaTime;
    float gravity;
};

struct FrameData
{
    ConstantBuffer<GlobalUniform> globalUniform;
};
ParameterBlock<FrameData> frameData;

[shader("compute")]
[numthreads(256U,1,1)]
void csMain(uint3 threadId : SV_DispatchThreadID)
{
    uint index = threadId.x;

    float4 position = particles.inPositions[index];
    float4 velocity = particles.inVelocities[index];

    // Update velocity with gravity
    velocity.y -= frameData.globalUniform.deltaTime * frameData.globalUniform.gravity;

    // Bounce on invisible floor
    if (position.y < -5.0f && velocity.y < 0)
        velocity.y = -velocity.y;

    // Update position with velocity
    position += velocity * frameData.globalUniform.deltaTime;

    // Write back to output buffers
    particles.outPositions[index] = position;
    particles.outVelocities[index] = velocity;
}