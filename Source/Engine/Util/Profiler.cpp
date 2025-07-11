#include "Profiler.h"

#include <cassert>
#include <cinttypes>

#include <SDL.h>

uint64_t Profiler::sFrameNumber = 0L;
std::vector<Sample> Profiler::sActiveSamples;

Sample::Sample(const char* name) :
    mName(name)
{

}

Sample::~Sample()
{
    printf("[%s] %.2f ms\n", mName, mTimer.GetMilliseconds());
}

/*static*/ void Profiler::BeginFrame()
{
    sActiveSamples.clear();

    printf("===== Begin Frame %" PRIu64 " =====\n", sFrameNumber);
    BeginSample("Total");
}

/*static*/ void Profiler::EndFrame()
{
    // There should only be one sample active (from BeginFrame) at this point.
    // If not, there are mismatched begin/end sample calls somewhere.
    assert(sActiveSamples.size() == 1);

    // End overall frame sample.
    EndSample();
    printf("===== End Frame %" PRIu64 " =====\n", sFrameNumber);

    // Increment frame number at end of frame (if you do this at beginning, it just means there's no frame 0).
    sFrameNumber++;
}

/*static*/ void Profiler::BeginSample(const char* name)
{
    // Put sample in stack.
    sActiveSamples.emplace_back(name);
}

/*static*/ void Profiler::EndSample()
{
    // Pop the sample off the stack (prints sample info to log).
    sActiveSamples.pop_back();
}