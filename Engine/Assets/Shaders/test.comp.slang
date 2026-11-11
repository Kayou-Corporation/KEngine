// test.comp.slang

struct Particle
{
    int a;
    int b;
}

struct Input
{
    StructuredBuffer<Particle> Particles;
}
ParameterBlock<Input> InputData;

struct Output
{
    RWStructuredBuffer<int> Result;
}
ParameterBlock<Output> OutputData;

[shader("compute")]
[numthreads(1,1,1)]
void csMain(uint3 threadId : SV_DispatchThreadID)
{
    OutputData.Result[threadId.x] = InputData.Particles[threadId.x].a + InputData.Particles[threadId.x].b;
}