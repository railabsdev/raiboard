#include <stdio.h>
#include "pico/stdlib.h"
#include "audio_channel.hpp"

#define BUZZER_PIN 14

int main() {
    stdio_init_all();
    
    sleep_ms(2000);
    
    printf("AudioChannel Test Starting on GP%d\n", BUZZER_PIN);
    
    AudioChannel audio(BUZZER_PIN);
    audio.init();
    
    printf("Testing sound effects...\n\n");
    
    while (true) {
        printf("Playing Jump Sound\n");
        audio.playJump();
        sleep_ms(500);
        
        printf("Playing Coin Sound\n");
        audio.playCoin();
        sleep_ms(500);
        
        printf("Playing Hit Sound\n");
        audio.playHit();
        sleep_ms(500);
        
        printf("Playing Shoot Sound\n");
        audio.playShoot();
        sleep_ms(500);
        
        printf("Playing Select Sound\n");
        audio.playSelect();
        sleep_ms(500);
        
        printf("Playing Power-Up Sound\n");
        audio.playPowerUp();
        sleep_ms(500);
        
        printf("Playing Pause Sound\n");
        audio.playPause();
        sleep_ms(500);
        
        printf("Playing Explosion Sound\n");
        audio.playExplosion();
        sleep_ms(500);
        
        printf("Playing Victory Fanfare\n");
        audio.playVictory();
        sleep_ms(500);
        
        printf("Playing Death Sound\n");
        audio.playDeath();
        sleep_ms(1000);
        
        printf("\nPlaying a musical scale\n");
        Note scale[] = {Note::C4, Note::D4, Note::E4, Note::F4, Note::G4, Note::A4, Note::B4, Note::C5};
        uint durations[] = {200, 200, 200, 200, 200, 200, 200, 400};
        audio.playMelody(scale, durations, 8);
        sleep_ms(500);
        
        sleep_ms(2000);
    }
    
    return 0;
}
