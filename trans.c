// Bank-account program with nominee details and account holder age
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants
#define MAX_ACCOUNTS 100
#define LAST_NAME_LEN 15
#define FIRST_NAME_LEN 10
#define NOMINEE_NAME_LEN 25
#define NOMINEE_RELATION_LEN 15
#define INPUT_BUFFER_SIZE 256

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[LAST_NAME_LEN];    
    char firstName[FIRST_NAME_LEN];   
    int age;              // NEW: age of account holder
    double balance;       

    // NEW: nominee details
    char nomineeName[NOMINEE_NAME_LEN];
    char nomineeRelation[NOMINEE_RELATION_LEN];
};

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void searchRecord(FILE *fPtr);

int main(int argc, char *argv[])
{
    FILE *cfPtr;
    unsigned int choice;

    // try opening existing file
    cfPtr = fopen("credit.dat", "rb+");

    // if file does not exist → create automatically
    if (cfPtr == NULL)
    {
        cfPtr = fopen("credit.dat", "wb+");

        if (cfPtr == NULL)
        {
            printf("File could not be created.\n");
            exit(-1);
        }

        // create MAX_ACCOUNTS empty records
        struct clientData blank = {0, "", "", 0, 0.0, "", ""};

        for (int i = 0; i < MAX_ACCOUNTS; i++)
        {
            if (fwrite(&blank, sizeof(struct clientData), 1, cfPtr) != 1)
            {
                printf("Error initializing file.\n");
                fclose(cfPtr);
                exit(-1);
            }
        }

        rewind(cfPtr);
    }

    while ((choice = enterChoice()) != 6)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;
        case 2:
            updateRecord(cfPtr);
            break;
        case 3:
            newRecord(cfPtr);
            break;
        case 4:
            deleteRecord(cfPtr);
            break;
        case 5:
        searchRecord(cfPtr);
            break;
        case 6:
            puts("Ending program.");
            break;
        default:
            puts("Incorrect choice");
        }
    }

    fclose(cfPtr);
    return 0;
}
// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr;
    struct clientData client = {0, "", "", 0, 0.0, "", ""};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
        return;
    }

    rewind(readPtr);

    fprintf(writePtr,
            "%-6s%-16s%-11s%-5s%-10s%-20s%-15s\n",
            "Acct", "Last Name", "First Name",
            "Age", "Balance", "Nominee Name", "Relation");

    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr,
                    "%-6d%-16s%-11s%-5d%-10.2f%-20s%-15s\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.age,
                    client.balance,
                    client.nomineeName,
                    client.nomineeRelation);
        }
    }

    if (ferror(readPtr))
    {
        puts("Error reading from file.");
    }

    fclose(writePtr);
}

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;
    char input[INPUT_BUFFER_SIZE];
    struct clientData client = {0, "", "", 0, 0.0, "", ""};

    while (1)
    {
        printf("Enter account to update (1 - %d): ", MAX_ACCOUNTS);
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            puts("Error reading input.");
            continue;
        }
        if (sscanf(input, "%u", &account) != 1 || account < 1 || account > MAX_ACCOUNTS)
        {
            puts("Invalid account number. Please enter a number between 1 and 100.");
            continue;
        }
        break;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        puts("Error reading record.");
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account #%d has no information.\n", account);
    }
    else
    {
        printf("\n%-6d%-16s%-11s%-5d%-10.2f%-20s%-15s\n",
               client.acctNum,
               client.lastName,
               client.firstName,
               client.age,
               client.balance,
               client.nomineeName,
               client.nomineeRelation);

        while (1)
        {
            printf("Enter charge (+) or payment (-): ");
            if (fgets(input, sizeof(input), stdin) == NULL)
            {
                puts("Error reading input.");
                continue;
            }
            if (sscanf(input, "%lf", &transaction) != 1)
            {
                puts("Invalid transaction amount. Please enter a number.");
                continue;
            }
            break;
        }

        client.balance += transaction;

        fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
        if (fwrite(&client, sizeof(struct clientData), 1, fPtr) != 1)
        {
            puts("Error writing to file.");
        }
    }
}

// delete record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0, 0.0, "", ""};
    unsigned int accountNum;
    char input[INPUT_BUFFER_SIZE];
    char confirm;

    while (1)
    {
        printf("Enter account number to delete (1 - %d): ", MAX_ACCOUNTS);
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            puts("Error reading input.");
            continue;
        }
        if (sscanf(input, "%u", &accountNum) != 1 || accountNum < 1 || accountNum > MAX_ACCOUNTS)
        {
            puts("Invalid account number. Please enter a number between 1 and 100.");
            continue;
        }
        break;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        puts("Error reading record.");
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account %d does not exist.\n", accountNum);
    }
    else
    {
        printf("Account details:\n%-6d%-16s%-11s%-5d%-10.2f%-20s%-15s\n",
               client.acctNum,
               client.lastName,
               client.firstName,
               client.age,
               client.balance,
               client.nomineeName,
               client.nomineeRelation);

        while (1)
        {
            printf("Are you sure you want to delete this account? (y/n): ");
            if (fgets(input, sizeof(input), stdin) == NULL)
            {
                puts("Error reading input.");
                continue;
            }
            if (sscanf(input, " %c", &confirm) != 1)
            {
                puts("Invalid input. Please enter 'y' or 'n'.");
                continue;
            }
            if (confirm == 'y' || confirm == 'Y')
            {
                fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
                if (fwrite(&blankClient, sizeof(struct clientData), 1, fPtr) != 1)
                {
                    puts("Error writing to file.");
                }
                else
                {
                    printf("Account %d deleted.\n", accountNum);
                }
                break;
            }
            else if (confirm == 'n' || confirm == 'N')
            {
                puts("Deletion cancelled.");
                break;
            }
            else
            {
                puts("Please enter 'y' or 'n'.");
            }
        }
    }
}

// create new record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0, 0.0, "", ""};
    unsigned int accountNum;
    char input[INPUT_BUFFER_SIZE];

    while (1)
    {
        printf("Enter new account number (1 - %d): ", MAX_ACCOUNTS);
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            puts("Error reading input.");
            continue;
        }
        if (sscanf(input, "%u", &accountNum) != 1 || accountNum < 1 || accountNum > MAX_ACCOUNTS)
        {
            puts("Invalid account number. Please enter a number between 1 and 100.");
            continue;
        }
        break;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        puts("Error reading record.");
        return;
    }

    if (client.acctNum != 0)
    {
        printf("Account #%d already contains information.\n", client.acctNum);
    }
    else
    {
        printf("Enter details in format: lastname firstname age balance nomineeName nomineeRelation\n? ");

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            puts("Error reading input.");
            return;
        }

        // Parse input safely
        char lastName[LAST_NAME_LEN], firstName[FIRST_NAME_LEN], nomineeName[NOMINEE_NAME_LEN], nomineeRelation[NOMINEE_RELATION_LEN];
        int age;
        double balance;

        if (sscanf(input, "%14s %9s %d %lf %24s %14s",
                   lastName, firstName, &age, &balance, nomineeName, nomineeRelation) != 6)
        {
            puts("Invalid input format. Please enter all fields separated by spaces.");
            return;
        }

        // Validate age and balance
        if (age < 0 || age > 150)
        {
            puts("Invalid age. Please enter an age between 0 and 150.");
            return;
        }

        // Copy to struct
        strcpy(client.lastName, lastName);
        strcpy(client.firstName, firstName);
        client.age = age;
        client.balance = balance;
        strcpy(client.nomineeName, nomineeName);
        strcpy(client.nomineeRelation, nomineeRelation);
        client.acctNum = accountNum;

        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        if (fwrite(&client, sizeof(struct clientData), 1, fPtr) != 1)
        {
            puts("Error writing to file.");
        }
        else
        {
            printf("Account %d created successfully.\n", accountNum);
        }
    }
}

// search record
void searchRecord(FILE *fPtr)
{
    unsigned int account;
    char input[INPUT_BUFFER_SIZE];
    struct clientData client = {0, "", "", 0, 0.0, "", ""};

    while (1)
    {
        printf("Enter account number to search (1 - %d): ", MAX_ACCOUNTS);
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            puts("Error reading input.");
            continue;
        }
        if (sscanf(input, "%u", &account) != 1 || account < 1 || account > MAX_ACCOUNTS)
        {
            puts("Invalid account number. Please enter a number between 1 and 100.");
            continue;
        }
        break;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        puts("Error reading record.");
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account %d not found.\n", account);
    }
    else
    {
        printf("\nAccount Found:\n");
        printf("%-6d%-16s%-11s%-5d%-10.2f%-20s%-15s\n",
               client.acctNum,
               client.lastName,
               client.firstName,
               client.age,
               client.balance,
               client.nomineeName,
               client.nomineeRelation);
    }
}

// menu
unsigned int enterChoice(void)
{
    unsigned int menuChoice;
    char input[INPUT_BUFFER_SIZE];

    while (1)
    {
        printf("\nEnter your choice\n"
               "1 - store formatted text file (accounts.txt)\n"
               "2 - update an account\n"
               "3 - add a new account\n"
               "4 - delete an account\n"
               "5 - search account\n"
               "6 - end program\n? ");

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            puts("Error reading input.");
            continue;
        }

        if (sscanf(input, "%u", &menuChoice) != 1)
        {
            puts("Invalid input. Please enter a number between 1 and 6.");
            continue;
        }

        if (menuChoice >= 1 && menuChoice <= 6)
        {
            return menuChoice;
        }
        else
        {
            puts("Incorrect choice. Please enter a number between 1 and 6.");
        }
    }
}