#include <stdio.h>
#include <math.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define UPPER_DURATION_MS 5000.0
#define LOWER_DELAY_MS    2000.0
#define LOWER_DURATION_MS 5000.0
#define TOTAL_DURATION_MS 7000.0

#define TICK_MS 10

int main()
{
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;

    LARGE_INTEGER frequency;
    LARGE_INTEGER startCounter;
    LARGE_INTEGER currentCounter;

    printf("Robot TaiJi motion controller started!\n");

   

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed!\n");
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == INVALID_SOCKET)
    {
        printf("Socket creation failed!\n");
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(5000);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Connecting to visualizer...\n");

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("Connection failed!\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Connected to visualizer!\n");
    printf("Starting precise TaiJi motion...\n\n");

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startCounter);

    while (1)
    {
        QueryPerformanceCounter(&currentCounter);

        double elapsedSeconds =
            (double)(currentCounter.QuadPart - startCounter.QuadPart)
            / (double)frequency.QuadPart;

        double elapsedMs = elapsedSeconds * 1000.0;

     
        if (elapsedMs >= TOTAL_DURATION_MS)
        {
            elapsedMs = TOTAL_DURATION_MS;
        }

   

        double upperProgress =
            elapsedMs / UPPER_DURATION_MS;

        if (upperProgress > 1.0)
            upperProgress = 1.0;

        int upperAngle =
            (int)(45.0 * upperProgress);

        

        int lowerAngle = 0;

        if (elapsedMs >= LOWER_DELAY_MS)
        {
            double lowerElapsed =
                elapsedMs - LOWER_DELAY_MS;

            double lowerProgress =
                lowerElapsed / LOWER_DURATION_MS;

            if (lowerProgress > 1.0)
                lowerProgress = 1.0;

            lowerAngle =
                (int)(90.0 * lowerProgress);
        }

    
        char message[100];

        snprintf(
            message,
            sizeof(message),
            "%d,%d\n",
            lowerAngle,
            upperAngle
        );

        send(
            sock,
            message,
            (int)strlen(message),
            0
        );

      

        printf(
            "\rTIME: %6.3f s | LOWER: %3d° | UPPER: %3d°",
            elapsedMs / 1000.0,
            lowerAngle,
            upperAngle
        );

        
        if (elapsedMs >= TOTAL_DURATION_MS)
        {
            break;
        }

        Sleep(TICK_MS);
    }

    printf("\n\n");
    printf("Upper arm:  5.000 seconds\n");
    printf("Lower delay: 2.000 seconds\n");
    printf("Lower arm:  5.000 seconds\n");
    printf("Total:      7.000 seconds\n");
    printf("TaiJi movement completed!\n");

    closesocket(sock);
    WSACleanup();

    return 0;
}
