#include "headers.txt"

FILE *f1; // Global file pointer used for file operations in this module

// To convert string to lower case for case-insensitive comparisons
char* str_to_lower(char str[])
{
    
    for(int i=0;str[i];i++)
    {
    
        str[i]=tolower(str[i]);
    }
    return str;
}

// Include the code for the other modules directly into this file for compilation

#include "quality.c"
#include "production.c"
#include "admin.c"

// Function to check login credentials against the employee list
// Returns 1 for Production, 2 for Quality, and 3 for failure
int login_check(char id[],char pass[])
{
    // Variables to hold the credentials and department read from the file
    char f_id[50],f_pass[50],dept[50],line[100],*token;

    // Open the CSV file containing employee credentials
    f1=fopen("H:\\My Drive\\C_txt_files\\employee_list.csv","r");
    
    // Read the file line by line, parsing three comma-separated values: ID, Password, and Department
    // %*c consumes the newline character after the department field
    while(fscanf(f1,"%[^,],%[^,],%[^\n]%*c",f_id,f_pass,dept)==3)
    { 
        // Compare the entered ID and password with the records read from the file
        if(strcmp(id,f_id)==0 && strcmp(pass,f_pass)==0)
        {
            // If credentials match, check the user's department
            if(strcmp("production",dept)==0)
            {
                fclose(f1); // Close file and return 1 for Production
                return 1;
            }
            else if(strcmp("quality",dept)==0)
            {
                // If it's quality return 2
                fclose(f1); // Close file and return 2 for Quality
                return 2;
            }
            else if(strcmp("admin",dept)==0)
            {
                // If it's ADMIN return 3
                fclose(f1); // Close file and return 2 for Quality
                return 3;
            }
            else
            {
                return 0;
            }
        }
    }
    fclose(f1); // Close the file after the loop finishes
    return 4;   // If no match was found after checking all records, return 3 (Failure)
}

// Main function: the entry point of the program
void main()
{
    clearPage(); // Clear the screen upon starting
    char id[50],pass[50];
    int check,n;
    
    // l7: Home Page Menu label (used with goto for looping back after a department exit)
    l7:printf("\tHOME PAGE\n");
    printf("\tMENU\n\t1.Login\n\t2.Exit program");

    // l3: Choice input label (used with goto for invalid input retry)
    l3:printf("\n\tEnter your choice:");
    scanf("%d",&n); // Read the user's menu choice
    getchar();      // Consume the newline character left by scanf

    if(n==1) // Option 1: Login
    {
        clearPage();
        printf("\tEntering login page...");
        Sleep(2500); // Pause for 2.5 seconds (gives user time to read the message)
        clearPage();
        printf("\tLOGIN PAGE:\n");
        
        // l1: Login prompt label (used with goto for incorrect login retry)
        l1:printf("\tEnter your id:");
        scanf("%[^\n]s",id); // Read ID (allows spaces until a newline)
        getchar();           // Consume the newline
        printf("\tEnter your password:");
        scanf("%[^\n]s",pass); // Read Password (allows spaces until a newline)
        
        // Check the entered credentials
        check=login_check(id,pass); 

        if(check==1)
        {
            // Production dept login successful
            clearPage();
            printf("\tLogin successful!\nEntering production department..");
            Sleep(3000); // Pause for 3 seconds
            clearPage();
            production(); // Call the production module function
            goto l7;      // Go back to the Home Menu
        }
        else if(check==2)
        {
            // Quality dept login successful
            clearPage();
            printf("\tLogin successful!\nEntering quality department..");
            Sleep(3000); // Pause for 3 seconds
            clearPage();
            quality(); // Call the quality module function
            goto l7;   // Go back to the Home Menu 
        }
        else if(check==3)
        {
            // admin login successful
            clearPage();
            printf("\tLogin successful!\nEntering Admin..");
            Sleep(3000); // Pause for 3 seconds
            clearPage();
            admin(); // Call the admin module function
            goto l7;// Go back to the Home Menu 
        }
        else
        {
            // Invalid id or pass
            printf("\tWrong login id or password!\nTry again!\n\n");
            getchar();
            goto l1;   // Go back to the login prompt
        }
    }
    else if(n==2) // Option 2: Exit program
    {
        clearPage();
        printf("\tExiting program...");
        Sleep(3000); // Pause for 3 seconds before ending
    }
    else // Invalid menu choice
    {
        printf("\tInvalid choice!Try again!\n");
        goto l3; // Go back to the menu choice prompt
    }
}