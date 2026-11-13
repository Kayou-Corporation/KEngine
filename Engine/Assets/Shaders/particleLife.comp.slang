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
    uint32_t nbParticles;
};

struct EffectData
{
    ConstantBuffer<GlobalUniform> globalUniform;
};
ParameterBlock<EffectData> effectData;

[shader("compute")]
[numthreads(256U,1,1)]
void csMain(uint3 threadId : SV_DispatchThreadID)
{
    uint index = threadId.x;
    if (index >= effectData.globalUniform.nbParticles)
        return;

    float4 position = particles.inPositions[index];
    float4 velocity = particles.inVelocities[index];

    // Update velocity with gravity
    velocity.y -= effectData.globalUniform.deltaTime * effectData.globalUniform.gravity;

    // Bounce on invisible floor
    if (position.y < -5.0f && velocity.y < 0)
        velocity.y += -velocity.y * 1.5f;

    if (abs(position.x) >= 12.f)
        velocity.x += -velocity.x * 1.5f;

    if (abs(position.z) >= 8.f)
        velocity.z += -velocity.z * 1.5f;

    // Update position with velocity
    position += velocity * effectData.globalUniform.deltaTime;

    // Write back to output buffers
    particles.outPositions[index] = position;
    particles.outVelocities[index] = velocity;
}