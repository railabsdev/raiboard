#ifndef AUDIO_CHANNEL_HPP
#define AUDIO_CHANNEL_HPP

#include "pico/stdlib.h"
#include "hardware/pwm.h"

// note frequencies
enum class Note {
    C3 = 131, CS3 = 139, D3 = 147, DS3 = 156, E3 = 165, F3 = 175, FS3 = 185, G3 = 196, GS3 = 208, A3 = 220, AS3 = 233, B3 = 247,
    C4 = 262, CS4 = 277, D4 = 294, DS4 = 311, E4 = 330, F4 = 349, FS4 = 370, G4 = 392, GS4 = 415, A4 = 440, AS4 = 466, B4 = 494,
    C5 = 523, CS5 = 554, D5 = 587, DS5 = 622, E5 = 659, F5 = 698, FS5 = 740, G5 = 784, GS5 = 831, A5 = 880, AS5 = 932, B5 = 988,
    C6 = 1047, CS6 = 1109, D6 = 1175, DS6 = 1245, E6 = 1319, F6 = 1397, FS6 = 1480, G6 = 1568, GS6 = 1661, A6 = 1760, AS6 = 1865, B6 = 1976,
    REST = 0
};

class AudioChannel {
public:
    AudioChannel(uint pin);
    
    void init();
    
    void playTone(uint frequency, uint duration_ms);
    
    void playNote(Note note, uint duration_ms);
    
    void stop();
    
    void playJump();
    void playCoin();
    void playHit();
    void playDeath();
    void playPowerUp();
    void playShoot();
    void playExplosion();
    void playSelect();
    void playPause();
    void playVictory();
    
    void playMelody(const Note* notes, const uint* durations, uint length);
    
private:
    uint pin_;
    uint slice_num_;
    bool initialized_;
    
    void setPWMFrequency(uint frequency);
};

#endif
