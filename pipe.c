/*
 * pipe.c
 *
 *  Created on: Apr 2, 2014
 *      Author: ouzhan
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <unistd.h>

/**
 * Node Struct
 * */
typedef struct NodeStruct {

	char* program;
	char** args;
	char** successors;
	int indegree;
	int outdegree;
	int repeat;

} Node;

/**
 * initialize and returns a node struct with given id,program,arguments and successors
 *
 * @param id        : id of the program
 * @param splitted  : array that caontains name,arguments,successors of the node to be created respectively.
 *                    Also, it ends with "NULL"
 */

Node* createNode(char** splitted) {
	Node *tempNode = malloc(sizeof(Node));
	int i = 0, j = 0,size=0;
	tempNode->repeat =0;
	tempNode->program = splitted[i++];

	tempNode->successors = malloc(sizeof(char*));
	while (strcmp(splitted[i++], ":")) {

			size++;
		}
	tempNode->args = malloc((size+1)*sizeof(char*));
	i=1;
	while (strcmp(splitted[i++], ":")) {

		tempNode->args[i - 2] = splitted[i - 1];
	}
	tempNode->args[i - 2] = "-1";
	while (strcmp(splitted[i++], "NULL")) {

		tempNode->successors[j++] = splitted[i - 1];
		if (realloc(tempNode->successors, (j + 1) * sizeof(char*)) != NULL) {
			;
		};
	}
	tempNode->successors[j] = "-1";
	tempNode->outdegree = j;
	tempNode->indegree = 0;
	return tempNode;

}

/**
 * splits the given string by ' '.
 * @param string: The string to be splitted
 */
char** split(char* string) {
	int i = 0, size = 0;
	while (1) {

		char c = string[i++];

		if (c == '\n') {
			size+=2;
			i = 0;
			break;
		}
		if (c == ' ')
			size++;

	}

	char** result = malloc(size * sizeof(char*));
	int numberOfSplits = 0, j = 0;
	char *temp = malloc(sizeof(char));
	while (1) {
		char c = string[i++];

		if (c == '\n') {
			result[numberOfSplits] = malloc((j + 1) * sizeof(char));
			strcpy(result[numberOfSplits++], temp);
			free(temp);
			break;
		}
		if (c != ' ') {

			temp[j++] = c;
			if (realloc(temp, (j + 1) * sizeof(char)) != NULL) {
				;
			};
		} else {

			result[numberOfSplits] = malloc((j + 1) * sizeof(char));
			strcpy(result[numberOfSplits++], temp);
			free(temp);
			temp = malloc(sizeof(char));
			j = 0;
		}

	}
	result[numberOfSplits] = "NULL";
	return result;
}

/**

 reads a line from stdin and returns it.

 */
char* readline() {
	size_t nbytes = 100;
	char *my_string = (char *) malloc(nbytes + 1);
	getline(&my_string, &nbytes, stdin);

	return my_string;

}

/**
 *
 * create the graph as array of node structs
 * @param numberOfNodes : number of nodes in the graph to be printed
 */
Node** createGraph(int numberOfNodes) {
	int i = 0;

	Node** allNodes = malloc(numberOfNodes * sizeof(Node*));

	for (i = 0; i < numberOfNodes; i++) {
		allNodes[i] = createNode(split(readline()));

	}

	for (i = 0; i < numberOfNodes; i++) {
		int j = 0;
		for (j = 0; j < allNodes[i]->outdegree; j++) {

			allNodes[atoi(allNodes[i]->successors[j])]->indegree++;
		}
	}

	for (i = 0; i < numberOfNodes; i++) {
			if(allNodes[i] ->outdegree >1){
				allNodes[i]->repeat =1;
			}
		}

	return allNodes;
}
/**
 *
 * test function.
 * prints the all nodes of graph in the form id,name,indegree,outdegree
 *@param allNodes : array of nodes in the graph to be printed
 *
 * */
void printGraph(Node** allNodes,int number) {

	int i = 0;
	for (i = 0; i < number; i++) {
		printf("name:%s , in: %d , out: %d\n",
				allNodes[i]->program, allNodes[i]->indegree,
				allNodes[i]->outdegree);

	}

}
/***
 *creates a pipe for each node of the graph, returns as an array of pipes.
 *result[i] is the pipe of the process whose id is i.
 *@param numberOfNodes : number of pipes to be created
 */
int ** createPipes(int numberOfNodes) {
	int i = 0, **result;
	result = malloc(2*numberOfNodes * sizeof(int*));
	for (i = 0; i < 2*numberOfNodes; i++) {
		result[i] = malloc(2 * sizeof(int));
		pipe(result[i]);
	}
	return result;

}
char* getArgs(Node* p){
	int i=0,j=0,size=0;
	char* temp;
	for(i=0;;i++){
		if(!p->program[i]) break;
		size++;
	}

	//printf("%d\n",size);
	for(i=0;;i++){
		if(!strcmp(p->args[i],"-1")) break;
		else size++;
		for(j=0;;j++){
			if(!p->args[i][j]) break;
			size++;
		}
	}
	//printf("%d\n",size);
	temp=malloc(size*sizeof(char));
	//printf("%d\n",size);
	size=0;
	for(i=0;;i++){
			if(!p->program[i]) break;
			temp[size++]=p->program[i];
		}

		for(i=0;;i++){

			if(!strcmp(p->args[i],"-1")) break;
			else temp[size++]=' ';
			for(j=0;;j++){
				if(!p->args[i][j]) break;
				temp[size++]=p->args[i][j];
			}
		}
		temp[size++]='\0';
		//printf("%s\n",temp);

	return temp;
}

void parseandexec(char *cmd) {
	//printf("in exec: %s\n",cmd);
	char *sptr, *parse;
	char *args[30];
	int  n = 0;
	parse = strtok_r(cmd, " 	", &sptr);
	args[n] = parse;
	while (parse != NULL)  {
		parse = strtok_r(NULL, " 	", &sptr);
		++n;
		args[n] = parse;
	}
	if (execvp(args[0],args)) {
		printf("error") ;
	}
}
int main() {
	int numberOfNodes = atoi(readline()),i=0,id=0,numOfRepeat=0;
	Node** graph = createGraph(numberOfNodes);
	int ** pipes = createPipes(numberOfNodes);
	//printf("%s\n",getArgs(graph[1]));
	//printGraph(graph,numberOfNodes);

	for(id=0;id<numberOfNodes;id++){
		Node* cp = graph[id];            // cp : current program
		if(! cp->repeat){

		if(! fork()){

			if(cp->indegree ){
				dup2(pipes[id][0],0);
			}

			if(cp->outdegree){

				int successor= atoi(cp->successors[0]);
				dup2(pipes[successor][1],1);
			}

			for (i = 0; i < 2*numberOfNodes; i++) {
			  close(pipes[i][0]);
				close(pipes[i][1]);
			}
			parseandexec(getArgs(cp));
			return 5;
		}

		}else{
			numOfRepeat++;
			if(! fork()){

						if(cp->indegree ){
							dup2(pipes[id][0],0);
						}

						if(cp->outdegree){
							int successor= id+ numberOfNodes;
							dup2(pipes[successor][1],1);
						}

						for (i = 0; i < 2*numberOfNodes; i++) {
							close(pipes[i][0]);
							close(pipes[i][1]);
						}
						parseandexec(getArgs(cp));
						return 5;
					}
			if(!fork()){
				
				
		   		
				int i=0;
				char ch;
				int f1 = pipes[id+numberOfNodes][0];
				char *string = malloc(1);
				close(pipes[id+numberOfNodes][1]);

				 while (read(f1, string, 1)){
					for(i=0;i<cp->outdegree;i++){
					write(pipes[atoi(cp->successors[i])][1],string,1);
					}
			    }
				for (i = 0; i < 2*numberOfNodes; i++) {
		    	close(pipes[i][0]);
			    close(pipes[i][1]);
		   }
			return 150;
			}
		}

	}
	for (i = 0; i < 2*numberOfNodes; i++) {
			close(pipes[i][0]);
			close(pipes[i][1]);
		}
	
	for(i=0;i<numberOfNodes+numOfRepeat;i++){
		int w;
				wait(&w);
/*
if (WIFEXITED(w) && WEXITSTATUS(w) ) {
					printf("Child %d exitted with error: %d\n",  i,
						 WEXITSTATUS(w));
				} else if (WIFSIGNALED(w)) {
					printf("Child %d signalled: %d\n", i,
						WTERMSIG(w));
				}*/
	}
	return 3;
}


