/*H**********************************************************************
* FILENAME :        main.c            
* DESCRIPTION :
*       For Windows only
*       This program samples pixels on Windows (from 10) screen, and sends the data out to serial comm
* AUTHOR :    Leo Ly        START DATE :    
*
*H*/

//gcc main_onlyHor.c -o out -lgdi32
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include <wingdi.h>

#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

int main(void) {
    //Serial prep
    HANDLE hComm;  // Handle to the Serial port
    DCB dcbSerialParams = { 0 };  // Initializing DCB structure
    DWORD BytesWritten = 1;          // No of bytes written to the port
    DWORD dwEventMask;     // Event mask to trigger
    char  ReadData;        //temperory Character
    DWORD NoBytesRead;     // Bytes read by ReadFile()
    unsigned char loop = 0;

    //Open the serial com port
    hComm = CreateFile("COM8", 
                       GENERIC_WRITE,      // Read/Write Access
                       0,                                 // No Sharing, ports cant be shared
                       NULL,                              // No Security
                       OPEN_EXISTING,                     // Open existing port only
                       0,                                 // Non Overlapped I/O
                       NULL);                             // Null for Comm Devices
    if (hComm == INVALID_HANDLE_VALUE) {
        printf("\n Port can't be opened\n\n");
    }
    //Setting the Parameters for the SerialPort
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    int Status = GetCommState(hComm, &dcbSerialParams); //retreives  the current settings
    if (Status == FALSE) {
        printf("\nError to Get the Com state\n\n");
    }

    dcbSerialParams.BaudRate = CBR_115200;      
    dcbSerialParams.ByteSize = 8;            
    dcbSerialParams.StopBits = ONESTOPBIT;    
    dcbSerialParams.Parity = NOPARITY;      
    Status = SetCommState(hComm, &dcbSerialParams);
    if (Status == FALSE) {
        printf("\nError to Setting DCB Structure\n\n");
    }

    //Setting Timeouts
    COMMTIMEOUTS timeouts = { 0 };  //Initializing timeouts structure
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (SetCommTimeouts(hComm, &timeouts) == FALSE) {
        printf("\nError to Setting Time outs");
    }


    //Screen scraping prep
    int scn_height = GetSystemMetrics(SM_CYSCREEN);
    int scn_width = GetSystemMetrics(SM_CXSCREEN);
    // printf("scn_height=%d scn_width=%d\n", scn_height, scn_width);
    HWND desktop =  GetDesktopWindow();
    HDC desktopHdc = GetDC(desktop);

    int cnt = 0, x, y = 30; 
    int no_Yportions = 3;
    int y_arr[no_Yportions];
    for (y = 1; y < scn_height; y+=scn_height/no_Yportions) {
        y_arr[cnt] = y;
        cnt += 1;
    }
    int y_centre = scn_height/2;

    cnt = 0;
    int no_Xportions = 16;
    int x_arr[no_Xportions];
    for (x = 1; x < scn_width; x+=scn_width/no_Xportions) {
        x_arr[cnt] = x;
        cnt += 1;
    }

    // uint8_t ret_size = no_Xportions*2+no_Yportions*2-4;
    // COLORREF ret_arr[ret_size];

    COLORREF ret_arr[no_Xportions];
    COLORREF colour, colour_centre, colour_above, colour_below;
    uint8_t ret_arr_cnt = 0;

    while (1) {

        for (int i = 0; i < no_Xportions; i++) {
            colour_centre = GetPixel(desktopHdc, x_arr[i], y_centre);//y_arr[i]);
            colour_above = GetPixel(desktopHdc, x_arr[i], y_centre+80);
            colour_below = GetPixel(desktopHdc, x_arr[i], y_centre-80);

            //average hex colour
            for (int k = 0; k < 3; k++) {
                int a = ((colour_above >> 8*k) & 0xFF);
                int b = ((colour_centre >> 8*k) & 0xFF);
                int c = ((colour_below >> 8*k) & 0xFF);
                int sum = a + c;
                int res = sum/3;

                colour |= ((res & 0xFF) << 8*k);
            }

            ret_arr[i] = colour;
        }
        for (int i = 0; i < no_Xportions; i++) {

            for (int j = 0; j < 3; j++) {
                uint8_t buf = ((ret_arr[i] >> j*8) & 0xFF);
                WriteFile(hComm, (void*) &buf, sizeof(uint8_t), &BytesWritten, NULL);
            }
        }
    }

    return 0;
}

    // while (1) {
    //         for (int i = 0; i < no_Xportions; i++) {
    //             colour = GetPixel(desktopHdc, x_arr[i], y_centre);//y_arr[i]);
    //             ret_arr[i] = colour;
    //             // ret_arr_cnt++;

    //             for (int j = 0; j < 3; j++) {
    //                 buf[0] = ((ret_arr[j] >> j*8) & 0xFF);
    //                 WriteFile(hComm, (void*) buf, sizeof(uint8_t), &BytesWritten, NULL);
    //             }
    //         }
    //         // ret_arr_cnt = 0;


    //         // for (int i = 0; i < no_Xportions; i++) {
    //             // printf("%06X \n\r", ret_arr[i]);
    //             // for (int j = 0; j < 3; j++) {
    //             //     buf[0] = ((ret_arr[i] >> j*8) & 0xFF);
    //             //     WriteFile(hComm, (void*) buf, sizeof(uint8_t), &BytesWritten, NULL);
    //             // }
    //             // Sleep(1);
    //         // }
    //     // for (int i = 0; i < no_Yportions; i++) {
    //     // }
    // }
