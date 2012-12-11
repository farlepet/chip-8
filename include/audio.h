#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include <iostream> 
#include <queue>
#include <cmath>


struct BeepObject
{
    double freq;
    int samplesLeft;
};
 
class Beeper
{
private:
    double v;
    std::queue<BeepObject> beeps;
public:
    Beeper();
    ~Beeper();
    void beep(double freq, int duration);
    void generateSamples(Sint16 *stream, int length);
    void wait();
};