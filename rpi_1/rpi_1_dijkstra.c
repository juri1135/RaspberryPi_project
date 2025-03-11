#include <stdio.h>
#include "rpi_1.h"
#include <string.h>
#define INF 9999999
#define INF 9999999
#define MAX_NODES 6  // 정점 수는 6으로 설정 (A부터 F까지)
int arr[MAX_NODES];      // 거리 저장 배열
int prev[MAX_NODES];     // 이전 노드 추적 배열
int visit[MAX_NODES];    // 방문 체크 배열
int edge[MAX_NODES][MAX_NODES]; // 간선 가중치 저장 배열
int V, E;
void build_path(int dest, char buffer[], int* len) {
    if (prev[dest] == -1) {
        *len += snprintf(buffer + *len, 128 - *len, "%c", dest + 'A');
        return;
    }
    build_path(prev[dest], buffer, len);
    *len += snprintf(buffer + *len, 128 - *len, "->%c", dest + 'A');
}
int checkVisit(){
    for(int i=0; i<V; i++){
        if(visit[i]==0) return 0;
    }
    return 1;
}
void findShortestPath(char source, char destination, char buffer[], int* len) {
    //src, dest node를 index로 변환환
    int src_idx = source - 'A';  
    int dest_idx = destination - 'A';  

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

    //모든 거리 inf, 미방문 처리, 이전 node 배열도 초기화
    //가중치도 다 inf로 초기화. 
    //거리나 가중치 등을 다 min값 기준으로 찾아갈 거라서 큰 수로 초기화함 
    for (int i = 0; i < V; i++) {
        arr[i] = INF;
        prev[i] = -1;
        visit[i] = 0;
        for (int j = 0; j < V; j++) {
            edge[i][j] = INF;  
        }
        edge[i][i] = 0; 
    }
    //시작 node 거리 0으로 초기화화
    arr[src_idx] = 0; 

    // 간선 정보 읽기
    for (int i = 0; i < E; i++) {
        char u, v;
        int w;
        if (fscanf(file, " %c %c %d", &u, &v, &w) != 3) {
            perror("간선 데이터 읽기 실패");
            fclose(file);
            return;
        }
        edge[u - 'A'][v - 'A'] = w;  
    }
    fclose(file);

    //모든 node에 방문할 때까지 반복
    while (!checkVisit()) {
        int min = INF, s = -1;
         //미방문 정점 중 거리가 가장 작은 정점을 현재 정점으로 선택 
        for (int i = 0; i < V; i++) {
            if (!visit[i] && arr[i] < min) {
                min = arr[i];
                s = i;
            }
        }
        //만약 다 돌았는데 min보다 작은 게 없거나 도착지라면 종료
        if (s == -1 || s == dest_idx) break;
        //현재 정점에 방문 표시
        visit[s] = 1;
         //연결된 미방문 정점들의 거리를 간선의 가중치를 이용하여 갱신
        for (int i = 0; i < V; i++) {
            //현재 정점과 연결되어 있고 아직 미방문이고 자기 자신이 아니라면
            if (edge[s][i] != INF && !visit[i]) {  
                //기존 거리보다 갱신 가중치가 더 크면 update 
                if (arr[s] + edge[s][i] < arr[i]) {
                    arr[i] = arr[s] + edge[s][i];
                    prev[i] = s;
                }
            }
        }
    }

    
    if (prev[dest_idx] == -1) {
        snprintf(buffer, 128, "No path");
        *len = strlen(buffer);
        return;
    }

    *len = 0;
    buffer[0] = '\0';
    build_path(dest_idx, buffer, len);
    printf("Shortest path: %s\n", buffer);
}
