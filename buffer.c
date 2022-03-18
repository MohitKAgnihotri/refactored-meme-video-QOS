
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 100
#define MAX_OUTPUT_LINK_CAPACITY (6000000 >>3) // 3Mb/8

int main(int argc, char *argv[]) {
    float now = 0, prev = 0, elapsed = 0;
    int pktin = 0, pktout = 0, pktloss= 0;
    int bytesin = 0, bytesout = 0, byteloss= 0;
    int pktsinq = 0, bytesinq = 0;
    int maxpkts = 0, maxbytes = 0;
    int size= 0, sizes[MAX_BUFFER_SIZE] = {0};
    float Total_Q_time = 0;

    float arrival;
    int bytes_in;

    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(0);
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Error opening file %s\n", argv[1]);
        exit(0);
    }

    int next_check_time = 1;
    while (!feof(fp))
    {
        while(fgetc(fp) != '\n')
        {
            fscanf(fp, "%f %d", &arrival, &bytes_in);
            bytesin += bytes_in;
            pktin++;

            if (bytesin > MAX_OUTPUT_LINK_CAPACITY)
            {
                if (pktsinq > MAX_BUFFER_SIZE)
                {
                    pktloss++;
                    byteloss += bytes_in;
                }
                else
                {
                    pktsinq++;
                    bytesinq += bytes_in;
                    Total_Q_time += next_check_time - arrival;
                }
            }

            if (arrival >= next_check_time)
            {
                next_check_time += 1;
                printf("Total Bytes Received for the last second: %d\n", bytesin);
                printf("Total Packets Received for the last second: %d\n", pktin);
                printf("Total Packets Dropped for the last second: %d\n", pktloss);
                printf("Total Bytes Dropped for the last second: %d\n", byteloss);
                printf("Total Packets in the queue for the last second: %d\n", pktsinq);
                printf("Total Bytes in the queue for the last second: %d\n", bytesinq);
                printf("Percentage of lost packets = %f \n", (float)pktloss/pktin);
                printf("Percentage of lost bytes = %f \n", (float)byteloss/bytesin);
                printf("Average Queue Time = %f \n", (float)Total_Q_time/pktsinq);
                printf("\n");
                bytesin = bytesinq; // Queued packets are counted in the next second
                pktin = 0;
                pktloss = 0;
                byteloss = 0;
                pktsinq = 0;
                bytesinq = 0;
            }
        }
    }
    return 0;
}