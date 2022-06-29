#include<stdio.h>
#include<stdlib.h>



void printGraph(int** graph, int n) {
	for (int i = 0; i < n; i++) {
		printf("\n");
		for (int j = 0; j < n; printf("%7d ", graph[i][j++]));
	}
	return;
}

char** createGraph(int n) {
	int** graph=malloc(n*sizeof(int*));
	if (!graph) {
		printf("\nAlokcija nije uspela\n");
		exit(1);
	}

	for (int i = 0; i < n; i++) {
		graph[i] = calloc(n, sizeof(int));
		if (!graph[i]) {
			printf("\nAlokacija nije uspela\n");
			exit(1);
		}
	}

	return graph;
}

void loadCSV(char** nodeVector, int*** graph, int* n, FILE* file) {
	fscanf(file, "%d", n);
	int** tempGraph = createGraph(*n);

	char* tempNodeVector = calloc(*n, sizeof(char));
	if (!tempNodeVector) {
		printf("\nAlokacija nije uspela");
		exit(1);
	}


	for (int i = 0; i < *n; i++) {
		for (int j = 0; j < *n; j++) {
			fscanf(file, "%d", &tempGraph[i][j]);
			if (tempGraph[i][j] == 1) {
				tempNodeVector[i] = 1;
				tempNodeVector[j] = 1;
			}
		}
	}

	*nodeVector = tempNodeVector;
	
	*graph = tempGraph;
	return;
}

void insertNode(char* nodeVector, int n) {
	printf("\nUnesite indeks cvora ");
	int nodeNumber;
	scanf("%d", &nodeNumber);

	if (nodeNumber > n - 1 || nodeNumber <= 0) {
		printf("\nZadati cvor je van opsega grafa");
	}
	else if (nodeVector[nodeNumber] == 1) {
		printf("\nZadati cvor vec postoji");
	}
	else {
		nodeVector[nodeNumber] = 1;
	}

	return;
}

void removeNode(char* nodeVector, int** graph,int n) {
	printf("\nUnesite indeks covra koji zelite da uklonite ");
	int nodeIndex;
	scanf("%d", &nodeIndex);

	if (nodeIndex < 0 || nodeIndex >= n) {
		printf("\nZadati cvor je van opsega grafa");
	}
	else if (nodeVector[nodeIndex] == 0) {
		printf("\nZadati cvor nije postojao");
	}
	else {
		nodeVector[nodeIndex] = 0;
		for (int i = 0; i < n; i++) {
			graph[nodeIndex][i] = 0;
			graph[i][nodeIndex] = 0;
		}
	}
	return;
}

void insertBranch(char* nodeVector, int** graph, int n) {
	printf("\nUnesite od kog do kog cvora hocete granu: ");

	int nodeA, nodeB,debt;
	scanf("%d %d", &nodeA, &nodeB);

	if (nodeA < 0 || nodeB < 0 || nodeA >= n || nodeB >= n) {
		printf("\nZadati cvorovi nisu u opsegu");
	}
	else if (nodeVector[nodeA] != 1 && nodeVector[nodeB] != 1) {
		printf("\nNe mozete povezivati nepostojece cvorove");
	}
	else if (graph[nodeA][nodeB] != 0) {
		printf("\nGrana izmedju datih cvorova vec postoji");
	}
	else {
		printf("\nUnesite tezinu grane ");
		scanf("%d", &debt);
		if (debt > 0) {
			graph[nodeA][nodeB] = debt;
		}
		else if (debt < 0) {
			graph[nodeB][nodeA] = debt;
		}
	}
	return;
}

void removeBranch(char* nodeVector, int** graph, int n) {
	printf("\nUnesite cvorove izmedju kojih zelite da uklonite ");
	int nodeA, nodeB;
	scanf("%d %d", &nodeA, &nodeB);
	if (nodeA < 0 || nodeB < 0 || nodeA >= n || nodeB >= n) {
		printf("\nZadati cvorovi su van opsega");
	}
	else if (graph[nodeA][nodeB] == 0) {
		printf("\nZadata grana ne postoji");
	}
	else graph[nodeA][nodeB] = 0;
	
	return;
}

int** DFS(int startIndex,int** graph,int n) {
	int** cycleMatrix=NULL;
	int* tempCycle;
	int len,cnt=0;
	

	int* visited = calloc(n, sizeof(int));
	int* stack = calloc(n, sizeof(int));
	int* columns = calloc(n, sizeof(int));
	int temp;

	int topS = -1;
	int row=startIndex;
	
	while (1) {
		visited[row] = 1;
		while (columns[row] < n && graph[row][columns[row]] == 0) {
			columns[row]++;
		}
		
		if (columns[row] >= n){
			if (topS != -1) {
				row = stack[topS];
				topS--;
			}
			else break;
		}
		else {
			if (visited[columns[row]] == 0) {
				topS++;
				stack[topS] = row;
				row = columns[row];
				columns[row]++;
				
			}
			else {
				temp = topS;
				while (temp > -1 && stack[temp] != columns[row]) temp--;

				if (temp > -1) {
					tempCycle = NULL;
					len = 0;
					while (temp <= topS) {
						tempCycle = realloc(tempCycle, (len + 3)*sizeof(int));
						tempCycle[len++] = stack[temp];
						
						temp++;
					}
				
					tempCycle[len] = row;
					tempCycle[len + 1] = -1;

					cycleMatrix = realloc(cycleMatrix, sizeof(int*) * (cnt + 2));
					cycleMatrix[cnt] = tempCycle;
					cycleMatrix[cnt + 1] = NULL;
					cnt++;
				}
				columns[row]++;
			}
			
		}

	}
	return cycleMatrix;
	}

int match(int* cycle1, int* cycle2) {
	int len1=0, len2=0;
	while (cycle1[len1] != -1)len1++;
	while (cycle2[len2] != -1)len2++;
	
	if (len2 != len1) return 0;

	for (int i = 0; cycle1[i] != -1; i++) {
		int elementFound = 0;
		for (int j = 0; cycle2[j] != -1; j++) {
			if (cycle1[i] == cycle2[j]) {
				elementFound = 1;
				break;
			}
			
		}
		if (!elementFound) return 0;
	}
	return 1;
}

int** findCycle(char* nodeVector, int** graph, int n) {
	int** cycles = calloc(1,sizeof(int*));
	int** newCycles;
	int len=0;

	for (int i = 0; i < n; i++) {
		if (nodeVector[i] == 1) {
			newCycles=DFS(i, graph, n);

			if (newCycles) {

				for (int k = 0; newCycles[k] != NULL; k++) {
					int insert = 1;
						
					for (int a = 0; cycles[a] != NULL;a++) {
						if (match(newCycles[k], cycles[a])) {
							insert = 0;
							break;
						}
					}
					if (insert) {
						cycles = realloc(cycles, (len + 2) * sizeof(int*));
						cycles[len] = newCycles[k];
						cycles[len + 1] = NULL;
						len++;
					}
				}

			}
		}
	}
	return cycles;

}

void makeCompensation(int** graph, int** cycles) {
	if (cycles[0] == NULL) {
		printf("\nGraf nema ciklusa");
		return;
	}
	for (int i = 0; cycles[i] != NULL; i++) {
		int min = 999999999;
		int j = 0;
		while (cycles[i][j+1]!=-1) {
			if (min > graph[cycles[i][j]][cycles[i][j + 1]]) min = graph[cycles[i][j]][cycles[i][j + 1]];
			j++;
		}
		if (min > graph[cycles[i][j]][cycles[i][0]]) min = graph[cycles[i][j]][cycles[i][0]];

		j = 0;
		while (cycles[i][j + 1] != -1) {
			graph[cycles[i][j]][cycles[i][j + 1]] -= min;
			j++;
		}
		graph[cycles[i][j]][cycles[i][0]] -= min;
		}
}



int main() {
	int n;
	char choice,*nodeVector=NULL;
	int** graph = NULL;
	int** cycles;
	char fileName[30];
	FILE* file;
	

	while (1) {
		printf("\n1.Napravi graf\n2.Dodaj cvor\n3.Ukolni cvor\n4.Dodaj granu");
		printf("\n5.Ukloni granu\n6.Ispisi matricu\n7.Izbrisi graf\n8.Ucitaj graf iz datoteke\n9.Izadji");
		printf("\n10.Ispisi ciklus u grafu\n11.Napravi kompenzaciju\n");

		scanf("%d", &choice);
		getchar();
		if (choice != 1 && choice !=8 && graph == NULL) {
			printf("\nGraf ne postoji");
			
			continue;
		}
		
		switch (choice) {
		case 1:
			if (graph != NULL) {
				printf("\nGraf vec postoji");
				continue;
			}
			printf("\nUnesite broj cvorova: ");
			scanf("%d", &n);

			graph = createGraph(n);
		    nodeVector = calloc(n, sizeof(char));
			if (!nodeVector) {
				printf("\nAlokacija nije uspela\n");
				exit(1);
			}
			break;
		case 2:
			insertNode(nodeVector, n);
			break;
		case 3:
			removeNode(nodeVector, graph, n);
			break;
		case 4:
			insertBranch(nodeVector, graph, n);
			break;
		case 5:
			removeBranch(nodeVector, graph, n);
			break;
		case 6:
			printGraph(graph, n);
			break;
		case 7:
			free(nodeVector);
			for (int i = 0; i < n; i++) {
				free(graph[i]);
			}
			free(graph);
			graph = NULL;
			nodeVector = NULL;
			break;
		case 9:
			printf("\nIzasli ste iz programa\n");
			exit(1);
			break;
		case 8:
			if (graph != NULL) {
				printf("\nGraf vec postoji, izbrisite ga");
				continue;
			}
			printf("\nUnesi ime fajla ");
			scanf("%s", &fileName);
			getchar();
			file = fopen(fileName,"r");
			if (!file) {
				printf("\nZadati fajl nije pronadjen: %s abc",fileName);
				continue;
			}
			loadCSV(&nodeVector, &graph, &n, file);
			fclose(file);
			break;
		case 10:
			cycles=findCycle(nodeVector, graph, n);
			if (cycles[0] == NULL) {
				printf("\nGraf nema ciklusa");
			}
			else {
				for (int i = 0; cycles[i] != NULL; i++) {
					printf("\n");
					for (int j = 0; cycles[i][j] != -1; j++) printf("%d ", cycles[i][j]);
				}
			}
			break;
		case 11:
			makeCompensation(graph, findCycle(nodeVector, graph, n));
			break;
		default:
			printf("\nNekorektan unos");
			break;
		}
		
	}

}