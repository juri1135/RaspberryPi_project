#include <stdio.h>
#include "rpi_1.h"
#define INF 9999999
#define MAX_NODES 100

int arr[MAX_NODES];      // 거리 저장 배열
int prev[MAX_NODES];     // 이전 노드 추적 배열
int visit[MAX_NODES];    // 방문 체크 배열
int edge[MAX_NODES][MAX_NODES]; // 간선 가중치 저장 배열
const char* node_names[] = {"A", "B", "S", "C", "E", "D"}; 

void build_path(int dest, char buffer[], int* len) {
    if (prev[dest] == -1) {
        *len += snprintf(buffer + *len, 128 - *len, "%s", node_names[dest]);
        return;
    }
    build_path(prev[dest], buffer, len);
    *len += snprintf(buffer + *len, 128 - *len, "->%s", node_names[dest]);
}

void findShortestPath(int source, int destination, char buffer[], int* len) {
    int V, E;
    
    FILE* file = fopen("data.txt", "r");
    if (!file) {
        perror("파일 열기 실패");
        return;
    }

    if (fscanf(file, "%d %d", &V, &E) != 2) {
        perror("파일 데이터 읽기 실패");
        fclose(file);
        return;
    }

    // 배열 크기 V 기준으로 초기화
    for (int i = 0; i < V; i++) {
        arr[i] = INF;
        prev[i] = -1;
        visit[i] = 0;
        for (int j = 0; j < V; j++) {
            edge[i][j] = 0;
        }
    }

    // 간선 정보 읽기
    for (int i = 0; i < E; i++) {
        int u, v, w;
        if (fscanf(file, "%d %d %d", &u, &v, &w) != 3) {
            perror("간선 데이터 읽기 실패");
            fclose(file);
            return;
        }
        edge[u - 1][v - 1] = w; // 1-based -> 0-based
    }
    fclose(file);

    // 1-based -> 0-based 변환
    source--;
    destination--;

    // source와 destination이 같다면 처리
    if (source == destination) {
        *len = snprintf(buffer, 128, "%s", node_names[source]);
        return;
    }

    arr[source] = 0;

    for (int k = 0; k < V; k++) {
        int min = INF;
        int s = -1;
        for (int i = 0; i < V; i++) {
            if (!visit[i] && arr[i] < min) {
                min = arr[i];
                s = i;
            }
        }
        if (s == -1 || s == destination) break;

        visit[s] = 1;

        for (int i = 0; i < V; i++) {
            if (edge[s][i] != 0 && !visit[i]) {
                if (arr[s] + edge[s][i] < arr[i]) {
                    arr[i] = arr[s] + edge[s][i];
                    prev[i] = s;
                }
            }
        }
    }

    *len = 0;
    buffer[0] = '\0';
    build_path(destination, buffer, len);
}
