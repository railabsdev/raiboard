#include "audio_channel.hpp"

AudioChannel::AudioChannel(uint pin) 
    : pin_(pin), slice_num_(0), initialized_(false) {
}

void AudioChannel::init() {
    gpio_set_function(pin_, GPIO_FUNC_PWM);
    slice_num_ = pwm_gpio_to_slice_num(pin_);
    initialized_ = true;
}

void AudioChannel::setPWMFrequency(uint frequency) {
    if (!initialized_ || frequency == 0) {
        pwm_set_gpio_level(pin_, 0);
        return;
    }
    
    uint32_t clock_freq = 125000000; // 125 MHz system clock
    uint32_t divider = 16;
    uint32_t wrap = (clock_freq / (divider * frequency)) - 1;
    
    pwm_set_clkdiv(slice_num_, divider);
    pwm_set_wrap(slice_num_, wrap);
    pwm_set_gpio_level(pin_, wrap / 2);
    pwm_set_enabled(slice_num_, true);
}

void AudioChannel::playTone(uint frequency, uint duration_ms) {
    if (frequency == 0) {
        stop();
        sleep_ms(duration_ms);
        return;
    }
    
    setPWMFrequency(frequency);
    sleep_ms(duration_ms);
    stop();
}

void AudioChannel::playNote(Note note, uint duration_ms) {
    playTone(static_cast<uint>(note), duration_ms);
}

void AudioChannel::stop() {
    if (initialized_) {
        pwm_set_enabled(slice_num_, false);
        pwm_set_gpio_level(pin_, 0);
    }
}

void AudioChannel::playMelody(const Note* notes, const uint* durations, uint length) {
    for (uint i = 0; i < length; i++) {
        playNote(notes[i], durations[i]);
        sleep_ms(10);
    }
}

void AudioChannel::playJump() {
    for (int freq = 200; freq <= 600; freq += 50) {
        playTone(freq, 20);
    }
}

void AudioChannel::playCoin() {
    playNote(Note::E5, 50);
    sleep_ms(10);
    playNote(Note::C6, 100);
}

void AudioChannel::playHit() {
    playTone(400, 30);
    playTone(200, 30);
    playTone(100, 40);
}

void AudioChannel::playDeath() {
    playNote(Note::B4, 100);
    playNote(Note::F4, 100);
    playNote(Note::D4, 100);
    playNote(Note::C4, 200);
    playNote(Note::G3, 300);
}

void AudioChannel::playPowerUp() {
    Note notes[] = {Note::C4, Note::E4, Note::G4, Note::C5, Note::G4, Note::C5, Note::E5, Note::C6};
    uint durations[] = {50, 50, 50, 50, 50, 50, 50, 150};
    playMelody(notes, durations, 8);
}

void AudioChannel::playShoot() {
    for (int freq = 800; freq >= 200; freq -= 100) {
        playTone(freq, 15);
    }
}

void AudioChannel::playExplosion() {
    for (int i = 0; i < 15; i++) {
        playTone(100 + (i * 20), 25);
    }
    playTone(50, 100);
}

void AudioChannel::playSelect() {
    playNote(Note::A5, 50);
    sleep_ms(20);
    playNote(Note::A5, 50);
}

void AudioChannel::playPause() {
    playNote(Note::G4, 100);
    sleep_ms(50);
    playNote(Note::C4, 100);
}

void AudioChannel::playVictory() {
    Note notes[] = {
        Note::C5, Note::C5, Note::C5, Note::C5,
        Note::G4, Note::A4, Note::B4, Note::C5
    };
    uint durations[] = {100, 100, 100, 300, 100, 100, 100, 400};
    playMelody(notes, durations, 8);
}
