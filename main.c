#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct Node {
    char fName[50];
    char lName[50];
    char DateOfBirth[20];
    float height;
    int weight;
    char nutritionStatus[100];
    struct Node *next;
};

struct ListRecords {
    struct Node *head;
    struct Node *tail;
    int listSize;
};

typedef struct ListRecords List;

void CreateEmptyList(List *);

List *initialisePatients(char *);

void Sort(List *, struct Node *);

void InsertNewPatient(List *);

void printPatients(List *);

void UpdateNutritionalStatus(List *);

void SearchPatient(List *, char[]);

void initialiseUnhealthyPatientList(List *, List *);

void copyNode(struct Node *, struct Node *);

struct Node* getNodeAddress(List* , int );

int main(int argc, char *argv[]) {
    char fileName[100];

    if (argc < 2) {
        printf("Please enter the name of the file:\n");
        scanf("%s", fileName);
    } else
        strcpy(fileName, argv[1]);

    List *patientsList;
    patientsList = initialisePatients(fileName);

    List *unhealthyPatients = (struct ListRecords *) malloc(sizeof(struct ListRecords));
    if (unhealthyPatients == NULL) {
        printf("Error creating unhealthy patient list.\n");
    }
    
    CreateEmptyList(unhealthyPatients);


    int command;
    do {
        printf("\n----- MENU -----\n"
               "1. Add patient\n"
               "2. Show patients\n"
               "3. Search patient\n"
               "4. Create an unhealthy patient list\n"
               "5. Show unhealthy patients\n"
               "6. Exit \n");
        printf("Enter your option:\n");
        scanf("%d", &command);

        switch (command) {
            fflush(stdin);
            case 1: {
                InsertNewPatient(patientsList);
                break;
            }
            case 2: {
                printPatients(patientsList);
                break;
            }
            case 3: {
                char Name[50];
                printf("Enter the name of the patient you want to search:\n");
                scanf("%s", Name);
                SearchPatient(patientsList, Name);
            }
            case 4: {
                initialiseUnhealthyPatientList(patientsList, unhealthyPatients);
                break;
            }
            case 5: {
                if (unhealthyPatients->listSize == 0)
                    printf("The list is empty:\n");
                else {
                    printf("Unhealthy patients are as follows:\n");
                    printPatients(unhealthyPatients);
                }
                break;
            }
            case 6: {
                printf("\n");
                exit(1);
            }
            default: {
                printf("Invalid option.\n");
            }
        }
    } while (command != 6);

    CreateEmptyList(unhealthyPatients);

    printPatients(patientsList);

    InsertNewPatient(patientsList);

    printPatients(patientsList);

    printf("The size of the list is %d\n", patientsList->listSize);

    UpdateNutritionalStatus(patientsList);
    printPatients(patientsList);

    free(patientsList);
    return 0;
}

//Creates an emtpy list by creating a dummy node
void CreateEmptyList(List *newList) {
    newList->head = malloc(sizeof(struct Node));
    if (newList->head == NULL) {
        printf("ERROR! Failed to allocate memory for new List.\n");
        exit(1);
    }

    newList->head->next = NULL;
    newList->tail = newList->head;
    newList->listSize = 0;

}


List *initialisePatients(char *fileName) {
    //    Create an empty list
    List *patientsList = malloc(sizeof(struct ListRecords));
    if (patientsList == NULL) {
        printf("The patientList list could not be created.\n");
        exit(1);
    }
    CreateEmptyList(patientsList);

    FILE *filein;
    filein = fopen(fileName, "r");
    if (filein == NULL) {
        printf("The file could not be read successfully. Goodbye.\n");
        exit(1);
    }

    char fName[50];
    char lName[50];
    char DateOfBirth[20];
    float height;
    int weight;
    char nutritionStatus[100];

    struct Node *newNode;

    char line[400];

    while (fgets(line, sizeof(line), filein)) {

        if (sscanf(line, "%49[^;];%49[^;];%19[^;];%f;%d;%99[^\n]", fName, lName, DateOfBirth, &height, &weight,
                   nutritionStatus) == 6) {


            newNode = (struct Node *) malloc(sizeof(struct Node));
            if (newNode == NULL) {
                printf("newNode could not be allocated memory.\n");
                exit(1);
            }


            strcpy(newNode->fName, fName);
            strcpy(newNode->lName, lName);
            strcpy(newNode->DateOfBirth, DateOfBirth);
            newNode->height = height;
            newNode->weight = weight;
            strcpy(newNode->nutritionStatus, nutritionStatus);
            newNode->next = NULL;

        }
        Sort(patientsList, newNode);
    }


    UpdateNutritionalStatus(patientsList);


    return patientsList;
}


void Sort(List *sortedList, struct Node *newNode) {
    if (sortedList->head->next == NULL) {
        sortedList->head->next = newNode;
        sortedList->tail = newNode;

    } else {
        struct Node *temp;
        temp = sortedList->head;
        int found = 0;

        while (temp->next != NULL && !found) {
            if (strcmp(newNode->fName, temp->next->fName) < 0) {
                newNode->next = temp->next;
                temp->next = newNode;
                found = 1;
            } else
                temp = temp->next;
        }
        if (!found) {
            temp->next = newNode;
            sortedList->tail = newNode;
        }


    }
    sortedList->listSize++;
}


void InsertNewPatient(List *insert_List) {
    struct Node *newNode;
    newNode = (struct Node *) malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("Error!! A new node to insert cannot be allocated memory.\n");
        exit(1);
    }

    printf("Enter the patients first name:\n");
    scanf("%s", newNode->fName);
    printf("Enter the patients last name:\n");
    scanf("%s", newNode->lName);
    printf("Enter patient's date of birth:\n");
    scanf("%s", newNode->DateOfBirth);
    printf("Enter patient's height:\n");
    scanf("%f", &newNode->height);
    printf("Enter patient's weight:\n");
    scanf("%d", &newNode->weight);
    strcpy(newNode->nutritionStatus, "NA");

    Sort(insert_List, newNode);
    UpdateNutritionalStatus(insert_List);
    printf("The patient has been added!\n");
}

void printPatients(List *Print_List) {
    struct Node *temp;
    temp = Print_List->head->next;

    printf("patients are as follows:\n");
    while (temp != NULL) {
        printf("%s;%s;%s;%.2f;%d;%s\n", temp->fName, temp->lName, temp->DateOfBirth, temp->height, temp->weight,
               temp->nutritionStatus);
        temp = temp->next;

    }
}

void UpdateNutritionalStatus(List *myList) {

    float BMI;
    struct Node *temp;
    temp = myList->head->next;
    while (temp != NULL) {
        if (strcmp(temp->nutritionStatus, "NA") == 0) {
            BMI = (temp->weight) / (pow(temp->height, 2));
            if (BMI < 18.5)
                strcpy(temp->nutritionStatus, "UnderWeight");
            else if (BMI <= 24.9)
                strcpy(temp->nutritionStatus, "NormalWeight");
            else if (BMI <= 29.9)
                strcpy(temp->nutritionStatus, "Pre_Obesity");
            else if (BMI <= 34.9)
                strcpy(temp->nutritionStatus, "ObeseClass_I");
            else if (BMI <= 39.9)
                strcpy(temp->nutritionStatus, "ObeseClass_II");
            else
                strcpy(temp->nutritionStatus, "ObeseClass_III");

        }
        temp = temp->next;
    }

}

void SearchPatient(List *searchList, char fName[]) {
    printf("Result:\n");

    int left = 0;
    int right = (searchList->listSize) - 1;

    while (left <= right){

        int mid = left + (right - left)/2;

        struct Node *searchNode;
        searchNode = getNodeAddress(searchList,mid);

        int compare = strcmp(fName,searchNode->fName);
        if(compare == 0){
            printf("%s;%s;%s;%.2f;%d;%s\n", searchNode->fName, searchNode->lName, searchNode->DateOfBirth, searchNode->height, searchNode->weight,
                   searchNode->nutritionStatus);
            return;
        }
        else if(compare < 0)
            right = mid - 1;

        else
            left = mid + 1;

    }

    printf("Patient not found.\n");

}

void initialiseUnhealthyPatientList(List *patientsList, List *unhealthyPatients) {


//    temp is for traversing through the patients List and temp2 is for Unhealthy patient list
    struct Node *temp, *temp2, *deleteNode;

    temp = patientsList->head;
    temp2 = unhealthyPatients->head;

    while (temp->next != NULL) {
        if (strcmp(temp->next->nutritionStatus, "ObeseClass_I") == 0 ||
            strcmp(temp->next->nutritionStatus, "ObeseClass_II") == 0 ||
            strcmp(temp->next->nutritionStatus, "ObeseClass_III") == 0) {

            struct Node *newNode = (struct Node *) malloc(sizeof(struct Node));
            if (newNode == NULL) {
                printf("The newNode in Unhealthy Patients list could not be assigned memory.\n");
                exit(1);
            }
            newNode->next = NULL;
            temp2->next = newNode;
            temp2 = newNode;
            copyNode(temp->next, temp2);

            unhealthyPatients->tail = temp2;
            unhealthyPatients->listSize++;

            deleteNode = temp->next;
            temp->next = temp->next->next;
            free(deleteNode);
            patientsList->listSize--;

        }
        else
        temp = temp->next;
    }
    if(temp->next == NULL)
        patientsList->tail = temp;
}


void copyNode(struct Node *O_node, struct Node *N_node) {

    strcpy(N_node->fName, O_node->fName);
    strcpy(N_node->lName, O_node->lName);
    strcpy(N_node->DateOfBirth, O_node->DateOfBirth);
    strcpy(N_node->nutritionStatus, O_node->nutritionStatus);
    N_node->height = O_node->height;
    N_node->weight = O_node->weight;

}

struct Node* getNodeAddress(List* myList, int position){

    struct Node *temp;
    temp = myList->head;

    for (int i = 0; i < position; ++i) {

        temp = temp->next;

    }
    return temp;
}