#include <string.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_VOLUME 65535
#define BAUD_RATE 9600
#define DATA_BYTE_SIZE 15
#define NUM_BUTTONS 8
#define NUM_POTENCIOMETERS 4

typedef struct data {
    int port_number;
    int value; 
    int buff;
}volume_data_t;

volume_data_t data[NUM_POTENCIOMETERS];
char audio_steam[NUM_POTENCIOMETERS][255] = {0};

void print_volume() {
    for(int i = 0; i < NUM_POTENCIOMETERS; i++) {
        printf("%d/", data[i].buff);
    }
    puts("");
}

void parse_data(char *str) {
    for(int i = 0; i < NUM_POTENCIOMETERS; i++) {
            data[i].value = 0;
            data[i].port_number = i;
        for (int pos = i * 4; str[pos] != ' ' && str[pos] != '/' && str[pos]; pos++) {
            data[i].value *= 10;
            data[i].value += str[pos] - '0';
        }
    }
}



char *parse_command(char *command, int ix) {

    switch (data[ix].port_number){
        case 0:
            sprintf(command, "nircmd.exe setsysvolume %d %s\n", data[ix].value * MAX_VOLUME / 100, audio_steam[ix]);
            break;
        case 1:
            sprintf(command, "nircmd.exe setsysvolume %d %s\n", data[ix].value * MAX_VOLUME / 100, audio_steam[ix]);
            break;
        case 2:
            sprintf(command, "nircmd.exe setappvolume %s %f\n", audio_steam[ix], (float)(data[ix].value) / 100);
            break;
        case 3:
            sprintf(command, "nircmd.exe setappvolume %s %f\n", audio_steam[ix], (float)(data[ix].value) / 100);
            break;
    }

    return command;
}

int change() {
    for (int i = 0; i < NUM_POTENCIOMETERS; i++) {
        if(data[i].value != data[i].buff) {
            data[i].buff = data[i].value;
            return i + 1;
        }
    }
    
    return 0;
}

DCB dcb_init() {
    DCB dcb = {0};

    dcb.DCBlength = sizeof(DCB);
    dcb.BaudRate = BAUD_RATE;
    dcb.fBinary = TRUE;
    dcb.ByteSize = DATA_BYTE_SIZE;
    dcb.StopBits = ONESTOPBIT;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fNull = TRUE;

    return dcb;
}

int main() {
    char *command = malloc(sizeof(char) * 100);

    char buff[255];
    scanf("%s", buff);
    char port[255] = {"\\\\.\\"};
    strcat(port, buff);
    
    for (int i  = 0; i < NUM_POTENCIOMETERS; i++)
        scanf("%s", audio_steam[i]);
    
    puts(port);
    
    for (int i  = 0; i < NUM_POTENCIOMETERS; i++)
        puts(audio_steam[i]);

    char str[16];

    DWORD event;

    HANDLE hComm;
    DCB dcb = dcb_init();

    hComm = CreateFileA(port,               //port name
                        GENERIC_READ,       //Read/Write
                        0,                  // No Sharing
                        NULL,               // No Security
                        OPEN_EXISTING,      // Open existing port only
                        0,                  // Non Overlapped I/O
                        NULL);              // Null for Comm Devices  

    if (hComm == INVALID_HANDLE_VALUE) {
        printf("Error in opening serial port %s!\n", port);
        return -1;
    } else printf("Opening serial port successful\n");  

    if (GetCommState(hComm, &dcb) == FALSE) {
        printf("DCB parameters not acceptable!\n");
        return -1;
    } else printf("DCB init successfull!\n"); 

    if (SetCommState(hComm, &dcb) == FALSE) {
        printf("Error\n");
        return -1;
    } else printf("Port Configuration succesfull!\n");  

    if(SetCommMask(hComm, EV_RXCHAR) == FALSE) {
        printf("Error\n");
        return -1;
    } else printf("Monitoring for events!\n");

    PurgeComm(hComm, PURGE_RXCLEAR);

    while(TRUE) {
            if (WaitCommEvent(hComm, &event, NULL)) {

                ReadFile(hComm, &str, DATA_BYTE_SIZE, NULL, NULL);

                str[15] = '\0';

                parse_data(str);

                int place = change();

                if (place) {
                    print_volume();
                    system(parse_command(command, place - 1));
                }
            }
        PurgeComm(hComm, PURGE_RXCLEAR);
    }

    CloseHandle(hComm); 
    return 0;
}
