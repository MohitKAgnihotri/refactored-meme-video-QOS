
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
    char currentline[100];

    float over_all_avg_q_delay = 0;
    float over_all_avg_q_size = 0;
    int over_all_received_packets = 0;
    int over_all_received_bytes = 0;
    int over_all_dropped_packets = 0;
    int over_all_dropped_bytes = 0;

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
    while (fgets(currentline, sizeof(currentline), fp) != NULL) 
    {
        sscanf(currentline, "%f %d", &arrival, &bytes_in);

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
            printf("Average Queue Time = %f \n", pktsinq > 0 ? (float)Total_Q_time/pktsinq : 0);
            printf("\n");
            bytesin = bytesinq; // Queued packets are counted in the next second

            over_all_avg_q_delay += Total_Q_time;
            over_all_avg_q_size += pktsinq;
            over_all_received_packets += pktin;
            over_all_received_bytes += bytesin;
            over_all_dropped_packets += pktloss;
            over_all_dropped_bytes += byteloss;

            pktin = 0;
            pktloss = 0;
            byteloss = 0;
            pktsinq = 0;
            bytesinq = 0;
            Total_Q_time = 0;
        }
    }

    printf("\n\n\n");
    printf("Total Bytes Received : %d\n", over_all_received_bytes);
    printf("Total Packets Received : %d\n", over_all_received_packets);
    printf("Total Packets Dropped : %d\n", over_all_dropped_packets);
    printf("Total Bytes Dropped : %d\n", over_all_dropped_bytes);
    printf("Total Packets in the queue : %f\n", over_all_avg_q_size);
    printf("Percentage of lost packets = %f \n", (float)over_all_dropped_packets/over_all_received_packets);
    printf("Percentage of lost bytes = %f \n", (float)over_all_dropped_bytes/over_all_received_bytes);
    printf("Average Queue Time = %f \n", (float)over_all_avg_q_delay/over_all_avg_q_size);

    return 0;
}
