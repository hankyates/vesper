#include "CloudSeed/Default.h"
#include "CloudSeed/ReverbController.h"
#include "CloudSeed/FastSin.h"
#include "CloudSeed/AudioLib/ValueTables.h"
#include "daisysp.h"
#include "daisy_pod.h"

using namespace daisysp;
using namespace daisy;
using namespace CloudSeed;
bool isInitialized = false;

static DaisyPod pod;

extern "C"
{
	ReverbController* Create(int samplerate)
	{
		if (!isInitialized)
		{
			AudioLib::ValueTables::Init();
			FastSin::Init();
			isInitialized = true;
		}

		return new ReverbController(samplerate);
	}

	 void Delete(ReverbController* item)
	{
		delete item;
	}

	 int GetSamplerate(ReverbController* item)
	{
		return item->GetSamplerate();
	}

	 void SetSamplerate(ReverbController* item, int samplerate)
	{
		return item->SetSamplerate(samplerate);
	}

	 int GetParameterCount(ReverbController* item)
	{
		return item->GetParameterCount();
	}

	 void ClearBuffers(ReverbController* item)
	{
		item->ClearBuffers();
	}

	 void Process(ReverbController* item, float* input, float* output, int bufferSize)
	{
		item->Process(input, output, bufferSize);
	}
}
ReverbController* cloud_seed_rvb = 0;

void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{
    float insf[2], outsf[2];

    pod.ProcessAnalogControls();
    pod.ProcessDigitalControls();

    insf[0] = in[0];
    insf[1] = in[1];
    Process(cloud_seed_rvb, insf, outsf, 1);
    out[0] = outsf[0];
    out[1] = outsf[1];

}

int main(void)
{
    float sample_rate;
    pod.Init();
    pod.SetAudioBlockSize(4);
    sample_rate = pod.AudioSampleRate();

    SetSamplerate(cloud_seed_rvb, sample_rate);

    pod.StartAdc();
    pod.StartAudio(AudioCallback);

    while(1) {}
}
