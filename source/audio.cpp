#include <audio.h>
 
const int AMPLITUDE = 18000;
const int FREQUENCY = 44100;
 

 
void audio_callback(void*, Uint8*, int);
 
Beeper::Beeper()
{
    SDL_AudioSpec desiredSpec;
 
    desiredSpec.freq = FREQUENCY;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 2048;
    desiredSpec.callback = audio_callback;
    desiredSpec.userdata = this;
 
    SDL_AudioSpec obtainedSpec;
 
    // you might want to look for errors here
    SDL_OpenAudio(&desiredSpec, &obtainedSpec);
 
    // start play audio
    SDL_PauseAudio(0);
}
 
Beeper::~Beeper()
{
    SDL_CloseAudio();
}
 
void Beeper::generateSamples(Sint16 *stream, int length)
{
    int i = 0;
    while (i < length) {
 
        if (beeps.empty()) {
            while (i < length) {
                stream[i] = 0;
                i++;
            }
            return;
        }
        BeepObject& bo = beeps.front();
 
        int samplesToDo = std::min(i + bo.samplesLeft, length);
        bo.samplesLeft -= samplesToDo - i;
 
        while (i < samplesToDo) {
            stream[i++] = AMPLITUDE * (i & 0x40) * (std::sin((float)(i) / 128.0)); 
            //v += bo.freq;
        }
 
        if (bo.samplesLeft == 0) {
            beeps.pop();
        }
    }

}
void Beeper::beep(double freq, int duration)
{
    BeepObject bo;
    bo.freq = freq;
    bo.samplesLeft = duration * FREQUENCY / 1000;
 
    SDL_LockAudio();
    beeps.push(bo);
    SDL_UnlockAudio();
}
 
void Beeper::wait()
{
    int size;
    do {
        SDL_Delay(10);
        SDL_LockAudio();
        size = beeps.size();
        SDL_UnlockAudio();
    } while (size > 0);
 
}
 
void audio_callback(void *_beeper, Uint8 *_stream, int _length)
{
    Sint16 *stream = (Sint16*) _stream;
    int length = _length / 2;
    Beeper* beeper = (Beeper*) _beeper;
 
    beeper->generateSamples(stream, length);
}/*
 
int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_AUDIO);
 
    int duration = 1000;
    double Hz = 440;
 
    Beeper b;
    b.beep(Hz, duration);
    b.wait();
 
    return 0;
}*/