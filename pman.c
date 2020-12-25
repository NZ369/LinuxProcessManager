/* CSC 360 WINTER 2020 */
/* Assignment #1 */
/* Student name: Yuying Zhang (Nina) */
/* Student #: V00924070 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/types.h>      
#include <sys/wait.h>          
#include <signal.h>          

char* commands_list[] = {
    "bg",
    "bglist",
    "bgkill",
    "bgstop",
    "bgstart",
    "pstat"
};


/* *************** { Process List Code Start } *************** */


/* Declaration of node_t structure for linked list */
typedef struct node_t {
	pid_t pid;
	int currActive;
    char* cmdName;
	char* processName;
	struct node_t* next;
} node_t;

/* Linked list head */
node_t* processListHead = NULL;

/* Returns the node containing the searched pid */
node_t* findProcess(pid_t pid) {
	node_t* curr = processListHead;
	while (curr != NULL) {
		if (curr->pid == pid) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

/* Checks if the current pid is a running process of the system */
int activeProcess( pid_t pid ) {
	node_t* curr = processListHead;
	while ( curr != NULL ) {
		if ( curr->pid == pid ) {
			return 1;
		}
		curr = curr->next;
	}
	return 0;
}

/* Adds a process to the process list, called in bg command */
void addProcess(pid_t pid, char* command, char* process) {
	node_t* node = (node_t*)malloc(sizeof(node_t));
	node->pid = pid;
    node->currActive = 1;
	node->cmdName = command;
    node->processName = process;
	node->next = NULL;

	if (processListHead == NULL) {
		processListHead = node;
	} else {
		node_t* curr = processListHead;
		while (curr->next != NULL) {
			curr = curr->next;
		}
		curr->next = node;
	}
}

/* Deletes a process present within the process list */
void deleteProcess(pid_t pid) {
	if (activeProcess(pid)) {
        node_t* curr1 = processListHead;
        node_t* curr2 = NULL;

        while (curr1 != NULL) {
            if (curr1->pid == pid) {
                if (curr1 == processListHead) {
                    processListHead = processListHead->next;
                } else {
                    curr2->next = curr1->next;
                }
                free(curr1);
                return;
            }
            curr2 = curr1;
            curr1 = curr1->next;
        }
    }
    else {
        return;
    }
}


/* *************** { Process List Code End } *************** */


/* *************** { Commands Code Start } *************** */


/* Executes the bg command through forking, adds the new process to process list */
void bgForking(char **commandArray, char exe[30]) {
  
    pid_t pid = fork();

    if (pid == 0) {
        execvp(exe, commandArray);
        perror("Error: The command [ bg ] failed in execvp. \n");
        exit(1);
    }
    else if (pid > 0) {
        printf("The background process [ %s ] commences. \n", commandArray[0]);
        addProcess(pid, "bg", commandArray[0]);
        sleep(1);
        return;
    }
    else {
        perror("Error: Command [ bg ] has failed. \n");
        exit(0);
    }

}

/* Executes the bglist command and prints off the list of processes currently running */
void bgList() {
    int totalProcesses = 0;
    node_t* curr = processListHead;

    while (curr != NULL) {
		char terminated[20] = "";
		if (!curr->currActive) {
			strcpy(terminated,"[ Stopped ]");
		}
		printf("%d:\t%s ./%s %s\n", curr->pid, curr->cmdName, curr->processName, terminated);
        totalProcesses++;
		curr = curr->next;
	}
	printf("Total background jobs:\t%d\n", totalProcesses);
}

/* Executes the bgkill command and terminates an existing running process */
void bgKill(pid_t pid) {

    node_t* node = findProcess(pid);
	if ((node->currActive) == 0) {
        printf("The process [ %d ] is currently stopped, to kill the process restart it first. \n", pid);
        return;
    }

    if(activeProcess(pid)) {
        if(!kill(pid, SIGTERM)) {
            sleep(1);
        }
        else {
            perror("Error: The command [ bgkill ] failed. \n");
            return;
        }
    }
    else {
        printf("Error: Process [ %d ] does not exist. \n", pid);
        return;
    }
}

/* Executes the bgstop command and stops a running process */
void bgStop(pid_t pid) {
    if(activeProcess(pid)) {
        if(!kill(pid, SIGSTOP)) {
            sleep(1);
        }
        else {
            perror("Error: The command [ bgstop ] failed. \n");
            return;
        }
    }
    else {
        printf("Error: Process [ %d ] does not exist. \n", pid);
        return;
    }
}

/* Executes the bgstart command and starts a stopped process */
void bgStart(pid_t pid) {
    if(activeProcess(pid)) {
        if(!kill(pid, SIGCONT)) {
            sleep(1);
        }
        else {
            perror("Error: The command [ bgstart ] failed. \n");
            return;
        }
    }
    else {
        printf("Error: Process [ %d ] does not exist. \n", pid);
        return;
    }
}

/* Processes the /proc/PID/stat file contents */
void statProcessor(char *file, char **contentContainer) {
	FILE *fp;
    char line_buf[1200];
	fp = fopen(file, "r");

	if (fp == NULL) {
        perror("Error: The /proc/PID/stat file cannot be opened. \n");
        exit(0);
    }
    int index = 0;
	while ( fgets(line_buf, 1200, fp) != NULL ) {
		char* elem = strtok(line_buf, " ");
		while (elem != NULL) {
			contentContainer[index] = elem;
			elem = strtok(NULL, " ");
			index++;
		}
	}
	fclose(fp);
}

/* Executes the pstat command and retrieves information from /proc/PID/stat and /proc/PID/status */
void pStat(pid_t pid) {
    if(activeProcess(pid)) {
        
        char statPath[150], *statInfo[150];
        char statusPath[150], statusInfo[150][150];

		sprintf(statPath, "/proc/%d/stat", pid);
		sprintf(statusPath, "/proc/%d/status", pid);

		statProcessor(statPath, statInfo);

	    FILE *fp = fopen(statusPath, "r");

	    if (fp == NULL) {
            perror("Error: The /proc/PID/status file cannot be opened. \n");
            return;
        }

        int index = 0;
        while ( fgets(statusInfo[index], 150, fp) != NULL ) {
            index++;
        }

	    fclose(fp);

		long unsigned int u_time = atol(statInfo[14]) / sysconf(_SC_CLK_TCK);
		long unsigned int s_time = atol(statInfo[15]) / sysconf(_SC_CLK_TCK);

		printf("comm: %s \n", statInfo[1]);
		printf("state: %s \n", statusInfo[2]);
		printf("utime: %lu \n", u_time);
		printf("stime: %lu \n", s_time);
		printf("rss: %s \n", statInfo[24]);
		printf("voluntary ctxt switches: %s \n", statusInfo[39]);
		printf("nonvoluntary ctxt switches: %s \n", statusInfo[40]);

    }
    else {
        printf("Error: Process [ %d ] does not exist. \n", pid);
        return;
    }
}


/* *************** { Commands Code End } *************** */


/* *************** { Operation Code Start } *************** */


/* Recieves signals from processes and updates the process status accordingly */
void refreshProcessStatuses() {
    pid_t pid;
	int	update;
	while (1) {
		pid = waitpid(-1, &update, WCONTINUED | WNOHANG | WUNTRACED);
		if (pid > 0) {
			if (WIFSIGNALED(update)) {
				printf("Update: Background process %d was killed. \n", pid);
				deleteProcess(pid);
			}
            if (WIFSTOPPED(update)) {
				printf("Update: Background process %d was stopped. \n", pid);
				node_t* node = findProcess(pid);
				node->currActive = 0;
			}
			if (WIFCONTINUED(update)) {
				printf("Update: Background process %d was started. \n", pid);
				node_t* node = findProcess(pid);
				node->currActive = 1;
			}
			if (WIFEXITED(update)) {
				printf("Update: Background process %d terminated. \n", pid);
				deleteProcess(pid);
			}
		} 
        else {
			break;
		}
	}
}

/* Recieves the command and arguments to launch the corresponding command */
void launchCommand(int command, char **commandArray) {

    /* Creates the program execution string */
    char exe[30], process[20];
    if (commandArray != NULL) {
        strcpy(exe, "./");
        strcpy(process, commandArray[0]);
        strcat(exe,process);
    }

    switch(command) {
        case 0: {
            commandArray[3] = NULL;
            bgForking(commandArray, exe);
            break;
        }
        case 1: {
            bgList();
            break;
        }
        case 2: {
            pid_t pid = atoi(commandArray[0]);
            bgKill(pid);
            break;
        }
        case 3: {
            pid_t pid = atoi(commandArray[0]);
            bgStop(pid);
            break;
        }
        case 4: {
            pid_t pid = atoi(commandArray[0]);
            bgStart(pid);
            break;
        }
        case 5: {
            pid_t pid = atoi(commandArray[0]);
            pStat(pid);
            break;
        }
        default:
            return;
    }
    free(commandArray);
}

/* Creates the command array from user input for bg command processing */
char** createCommandArray(char input[20][30]) {
    char** commandArray = malloc(20 *sizeof(char*));
    for (int i = 1; i <= 20; i++) {
        if (strcmp(input[i], "") != 0) {
            commandArray[i-1] = malloc(30 *sizeof(char));
            strcpy(commandArray[i-1],input[i]);
        }
        else {
            commandArray[i] = NULL;
            return commandArray;
            break;
        }
    }
    return commandArray;
    free(commandArray);
}

/* Checks the command to ensure it is a valid command before mapping it to a corresponding integer */
int checkCommand(char *cmd) {
    int i = 0;
    while (i < 6) {
        if (strcmp(cmd, commands_list[i]) == 0) {
            return i;
        }
        i++;
    }
    return -1;
}

/* Processes the user input and runs the PMan shell */
void inputProcessing() {
	char* inputLine;
    char input[20][30] = { "" };
    int inputIndex = 0;
    
    while((inputLine = readline("PMan: > ")) != NULL) {
        if (strlen(inputLine) > 0) {
            add_history(inputLine);
        }

        char *elem = strtok(inputLine, " ");
        inputIndex = 0;

	    while ( elem != NULL && inputIndex < 20 ) {
            strncpy(input[inputIndex], elem, 30);
            inputIndex++;
            elem = strtok(NULL, " ");
	    }

        int command = checkCommand(input[0]);

        if (command >= 0) {
            char** commandArray;
            if (strcmp(input[1], "") != 0) {
                commandArray = createCommandArray(input);
            }
            else {
                commandArray = NULL;
            }
            launchCommand(command, commandArray);
        }
        else {
            printf("Error: Command [ %s ] not found. \n", input[0]);
            return;
        }
        free(inputLine);
        refreshProcessStatuses();
    }
}

/* Command line processing */
int main( int argc, char *argv[] )
{
    int running = 1;
    while(running) {
        inputProcessing();
    }

    return 0;
}

/* *************** { Operation Code End } *************** */