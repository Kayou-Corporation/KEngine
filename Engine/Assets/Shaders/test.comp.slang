// test.comp.slang

struct Input
{
    int a;
    int b;
}
ParameterBlock<Input> InputUniform;
RWStructuredBuffer<int> OutputBuffer;

[shader("compute")]
[numthreads(1,1,1)]
void csMain(uint3 threadId : SV_DispatchThreadID)
{
    OutputBuffer[threadId.x] = InputUniform.a + InputUniform.b;
}