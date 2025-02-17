#include <stdio.h>
#define n 6
#define INF 9999999

// 거리 계산
int arr[n];
// 경로 추적
int prev[n];
// 방문 여부
int visit[n];
// 간선 가중치
int edge[n][n];
const char* node_names[] = {"A", "B", "S", "C", "E", "D"};

void build_path(int dest, char buffer[], int* len) {
    if (prev[dest] == -1) {
        *len += sprintf(buffer + *len, "%s", node_names[dest]); // 노드 이름 출력
        return;
    }
    build_path(prev[dest], buffer, len);
    *len += sprintf(buffer + *len, "->%s", node_names[dest]);
}

void findShortestPath(int source, int destination, char buffer[], int* len) {
    for (int i = 0; i < n; i++) {
        visit[i] = 0;
        arr[i] = INF;
        prev[i] = -1;
        for (int k = 0; k < n; k++) {
            edge[i][k] = 0;
        }
    }
// 파일에서 간선 정보 읽기
    FILE* file = fopen("data.txt", "r");
    if (!file) {
        perror("파일 열기 실패");
        return;
    }

    int V, E;
    fscanf(file, "%d %d", &V, &E);
    for (int i = 0; i < E; i++) {
        int u, v, w;
        fscanf(file, "%d %d %d", &u, &v, &w);
        edge[u - 1][v - 1] = w; // 1-based -> 0-based
    }
    fclose(file);


    
    
    arr[source] = 0;

    for (int k = 0; k < n; k++) {
        int min = INF;
        int s = -1;
        for (int i = 0; i < n; i++) {
            if (!visit[i] && min > arr[i]) {
                min = arr[i];
                s = i;
            }
        }
        if (s == -1 || s == destination) break;
        visit[s] = 1;

        for (int i = 0; i < n; i++) {
            if (edge[s][i] != 0 && !visit[i]) {
                if (arr[s] + edge[s][i] < arr[i]) {
                    arr[i] = arr[s] + edge[s][i];
                    prev[i] = s; // 이전 노드 기록
                }
            }
        }
    }

    *len = 0;
    buffer[0] = '\0';
    build_path(destination, buffer, len);
}


