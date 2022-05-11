#include "HID-Project.h"

#define NUM_BUTTONS 8
#define NUM_POTENCIOMETERS 4

#define BAUD_RATE 9600
#define FIREFOX 3 
#define MUTE_CURRENT 2
#define SNDVOL 4
#define NEXT 5
#define MIC_MUTE 6
#define STOP 7
#define SOUND_MUTE 8
#define PREV 9

#define DEBOUNCE_DELAY 11
unsigned long last_debounce = 0;

bool buttons_state[NUM_BUTTONS];
bool last_button_state[NUM_BUTTONS];
int button_pins[] = {2, 3, 4, 5, 6, 7, 8, 9};

int curr_window_volume;
bool key;

int volume_data[NUM_POTENCIOMETERS];
int analog_pins[] = {0, 1, 2, 3};

String build_str() {
    String data;
    String buff;
    
    for (int i = 0; i < NUM_POTENCIOMETERS; i++) {
      if (volume_data[i] < 100) {
        buff += ' ';
        if (volume_data[i] < 10)
          buff += ' ';
      }

      data += (String(volume_data[i]) + buff + '/');

      buff = "";
    }

    return data;
}

void send_volume_data() {
    String data = build_str();
    Serial.print(data);
}

int read_button(int ix) {
    int curr = digitalRead(button_pins[ix]);

    if (curr != last_button_state[ix])
        last_debounce = millis();
        
    //if ((millis() - last_debounce) > DEBOUNCE_DELAY) {
        if (curr != buttons_state[ix]) {
            buttons_state[ix] = curr;
                      
            if (buttons_state[ix] == LOW)
                return 1;
        }      
    //}
    
    last_button_state[ix] = curr;
    return 0;
}

void button_command(int button) {
  switch(button) {
    case FIREFOX:
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('r');

        Keyboard.releaseAll();

        delay(100);

        Keyboard.println("volKontrol.bat");
        break;
    case MUTE_CURRENT:
      if (key) {
        volume_data[3] = 0;
        send_volume_data();
        volume_data[3] = curr_window_volume;
        key = 0;
      } else {
        send_volume_data();
        key = 1;
      }
      break;
    case SNDVOL:
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('r');

        Keyboard.releaseAll();

        delay(100);

        Keyboard.println("sndvol.exe");
        break;
    case NEXT:
        Consumer.write(MEDIA_NEXT);
        break;
    case MIC_MUTE:
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('r');

        Keyboard.releaseAll();

        delay(100);

        Keyboard.println("nircmd.exe mutesysvolume 2 Microphone");
        break;
    case STOP:
        Consumer.write(MEDIA_PLAY_PAUSE);
        break;
    case SOUND_MUTE:
        Consumer.write(MEDIA_VOLUME_MUTE);
        break;
    case PREV:
        Consumer.write(MEDIA_PREV);
        break;
      return ;
  }
}

void setup() {
  // put your setup code here, to run once:

  for (int i = 0; i < NUM_BUTTONS; i++)
    pinMode(button_pins[i], INPUT_PULLUP);
  
  Serial.begin(BAUD_RATE);
  
  Consumer.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  for (int i = 0; i < NUM_BUTTONS; i++) {
      if (read_button(i))
          button_command(button_pins[i]);
  }

  for (int i = 0; i < NUM_POTENCIOMETERS; i++)
      volume_data[i] = (float)analogRead(analog_pins[i]) / 1023 * 100;

  send_volume_data();

  curr_window_volume = volume_data[3];

  delay(20);
}
