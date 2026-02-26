// Bank-account program with nominee details and account holder age
#include <stdio.h>
#include <stdlib.h>

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    
    char firstName[10];   
    int age;              // NEW: age of account holder
    double balance;       

    // NEW: nominee details
    char nomineeName[25];
    char nomineeRelation[15];
};

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);

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

        // create 100 empty records
        struct clientData blank = {0, "", "", 0, 0.0, "", ""};

        for (int i = 0; i < 100; i++)
        {
            fwrite(&blank, sizeof(struct clientData), 1, cfPtr);
        }

        rewind(cfPtr);
    }

    while ((choice = enterChoice()) != 5)
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
    int result;

    struct clientData client = {0, "", "", 0, 0.0, "", ""};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
    }
    else
    {
        rewind(readPtr);

        fprintf(writePtr,
                "%-6s%-16s%-11s%-5s%-10s%-20s%-15s\n",
                "Acct", "Last Name", "First Name",
                "Age", "Balance", "Nominee Name", "Relation");

        while (!feof(readPtr))
        {
            result = fread(&client, sizeof(struct clientData), 1, readPtr);

            if (result != 0 && client.acctNum != 0)
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

        fclose(writePtr);
    }
}

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;

    struct clientData client = {0, "", "", 0, 0.0, "", ""};

    printf("Enter account to update (1 - 100): ");
    scanf("%d", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

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

        printf("Enter charge (+) or payment (-): ");
        scanf("%lf", &transaction);

        client.balance += transaction;

        fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    }
}

// delete record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0, 0.0, "", ""};
    unsigned int accountNum;

    printf("Enter account number to delete (1 - 100): ");
    scanf("%d", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account %d does not exist.\n", accountNum);
    }
    else
    {
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    }
}

// create new record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0, 0.0, "", ""};
    unsigned int accountNum;

    printf("Enter new account number (1 - 100): ");
    scanf("%d", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account #%d already contains information.\n", client.acctNum);
    }
    else
    {
        printf("Enter lastname firstname age balance nomineeName relation\n? ");

        scanf("%14s%9s%d%lf%24s%14s",
              client.lastName,
              client.firstName,
              &client.age,
              &client.balance,
              client.nomineeName,
              client.nomineeRelation);

        client.acctNum = accountNum;

        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    }
}

// menu
unsigned int enterChoice(void)
{
    unsigned int menuChoice;

    printf("\nEnter your choice\n"
           "1 - store formatted text file (accounts.txt)\n"
           "2 - update an account\n"
           "3 - add a new account\n"
           "4 - delete an account\n"
           "5 - end program\n? ");

    scanf("%u", &menuChoice);
    return menuChoice;
}