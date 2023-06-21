#include <stdio.h>
#define INF 99999
#define MAX_NODES 20

struct node
{
    int dist[MAX_NODES];
    int from[MAX_NODES];
};

void distance_vector_routing(int costmat[MAX_NODES][MAX_NODES], int nodes)
{
    struct node rt[MAX_NODES];
    int i, j, k, count;

    // Initialize distance and next hop vectors
    for (i = 0; i < nodes; i++)
    {
        for (j = 0; j < nodes; j++)
        {
            rt[i].dist[j] = costmat[i][j];
            rt[i].from[j] = j;
        }
    }

    // Bellman-Ford algorithm to update distance and next hop vectors
    do
    {
        count = 0;
        for (i = 0; i < nodes; i++)
        {
            for (j = 0; j < nodes; j++)
            {
                for (k = 0; k < nodes; k++)
                {
                    if (rt[i].dist[j] > costmat[i][k] + rt[k].dist[j])
                    {
                        rt[i].dist[j] = rt[i].dist[k] + rt[k].dist[j];
                        rt[i].from[j] = k;
                        count++;
                    }
                }
            }
        }
    } while (count != 0);

    // Print routing table
    for (i = 0; i < nodes; i++)
    {
        printf("Routing table for node %d:\n", i + 1);
        printf("%-15s %-15s %-15s\n", "Destination", "Next hop", "Distance");
        for (j = 0; j < nodes; j++)
        {
            printf("%-15d %-15d %-15d\n", j + 1, rt[i].from[j] + 1, rt[i].dist[j]);
        }
        printf("\n");
    }
}

int main()
{
    int costmat[MAX_NODES][MAX_NODES];
    int nodes, i, j;

    // Read number of nodes and cost matrix from user
    printf("Enter the number of nodes: ");
    scanf("%d", &nodes);
    printf("Enter the cost matrix:\n");
    for (i = 0; i < nodes; i++)
    {
        for (j = 0; j < nodes; j++)
        {
            scanf("%d", &costmat[i][j]);
            if (costmat[i][j] == 0)
            {
                costmat[i][j] = INF;
            }
        }
    }

    distance_vector_routing(costmat, nodes);

    return 0;
}