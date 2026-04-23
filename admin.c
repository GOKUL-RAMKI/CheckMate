int timeline()
{
    char choice;
    // Define variables to hold the five fields
    char machine[15], cmpId[15], action[15], time[10], date[20]; // Increased buffer sizes for safety/display

    // Open the report file for reading
    FILE *tl = fopen(TL_FPATH, "r");

    if (tl == NULL)
    {
        printf("\t\n--- ERROR ---\nCould not open timeline file at %s.\n", TL_FPATH);
        return 0;
    }

    fscanf(tl, "%[^,], %[^,], %[^,], %[^,], %[^\n]\n",
           machine, cmpId, action, time, date);

    printf("\n\t------------------------------- TIME LINE ---------------------------------\n");
    printf("\t| %-12s | %-12s | %-12s | %-8s | %-15s |\n", "MACHINE ID", "COMPONENT ID", "ACTION", "TIME", "DATE");

    while (fscanf(tl, "%[^,], %[^,], %[^,], %[^,], %[^\n]\n",
                  machine, cmpId, action, time, date) == 5)
    {
        printf("\t---------------------------------------------------------------------------\n");
        // Print the columns in the order: machine, cmpId, action, time, date
        printf("\t| %-12s | %-12s | %-12s | %-8s | %-15s |\n",
               machine, cmpId, action, time, date);
    }

    printf("\t---------------------------------------------------------------------------\n");
    getchar();
    printf("\n\tDO YOU WANT TO SAVE THE TIMELINE (Y/n): ");
    scanf("%c", &choice);

    if (choice == 'y' || choice == 'Y')
    {
        char userPath[100],savePrompt[100];
        printf("\tEnter Directory (with FILE NAME and .csv extension): ");
        scanf("%s",userPath);
        strcat(savePrompt,cpy);
        strcat(savePrompt," D:\\PROGRAMS\\C_Programs\\package\\timeline.csv ");
        strcat(savePrompt,userPath);
        system(savePrompt);
    }
    int quit;
    printf("\tENTER 1 to exit: ");
    scanf("%d",&quit);
    if(quit==1)
    {
        return 0;
    }
    fclose(tl);
}

int report()
{
    char choice;
    // Define variables to hold the five fields
    Row report;
    char *res;

    // Open the report file for reading
    FILE *tl = fopen(FPATH, "r");

    if (tl == NULL)
    {
        printf("\t\n--- ERROR ---\nCould not open report file at %s.\n", FPATH);
        return 0;
    }

    printf("\n------------------------------------------------------------------------------ REPORT ----------------------------------------------------------------------------------\n");
    printf("| %-12s | %-12s | %-20s | %-20s | %-20s | %-10s | %-16s | %-20s | %-10s |\n", "MACHINE ID", "COMPONENT ID", "COMPONENT NAME", "DIMENSION","EXPECTED PARAMETER","TOLERANCE","MEASURING UNIT","NUM OF COMPONENTS","STATUS");

    while (fscanf(tl, "%[^,],%[^,],%[^,],%[^,],%lf,%lf,%[^,],%[^,],%d\n",
           &report.machine, &report.comp_id, &report.comp_name, &report.dim, &report.expected, &report.tol, &report.unit, &report.numComp, &report.st)==9)
    {
        if(report.st==APPROVED)
        {
            res="APPROVED";
        }
        else if(report.st==PENDING)
        {
            res="PENDING";
        }
        else if(report.st==REJECTED)
        {
            res="REJECTED";
        }
        printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

        // Print the columns in the order
        printf("\t| %-12s | %-12s | %-20s | %-20s | %20lf | %10lf | %16s | %20s | %10s |\n",
            report.machine, report.comp_id, report.comp_name, report.dim, report.expected, report.tol, report.unit, report.numComp, res);
        
    }

    printf("\t------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    getchar();
    printf("\t\nDO YOU WANT TO SAVE THE REPORT (Y/n): ");
    scanf("%c", &choice);

    if (choice == 'y' || choice == 'Y')
    {
        char userPath[100],savePrompt[100];
        printf("\tEnter Directory (with FILE NAME and .csv extension): ");
        scanf("%s",userPath);
        strcat(savePrompt,cpy);
        strcat(savePrompt," D:\\PROGRAMS\\C_Programs\\package\\production_parameters.csv ");
        strcat(savePrompt,userPath);
        system(savePrompt);
    }
    int quit;
    printf("\tENTER 1 to exit: ");
    scanf("%d",&quit);
    if(quit==1)
    {
        return 0;
    }
    fclose(tl);
}

void admin()
{
    int ch;
    menu:printf("\tMENU\n\t1. Time line\n\t2. Report\n\t3. Exit\n");
    printf("\tEnter your choice: ");

    // DANGER: Using scanf without checking return value or clearing buffer
    if (scanf("%d", &ch) != 1)
    {
        printf("\tInvalid input.\n");
        // Clear buffer on failure, then return or loop
        while (getchar() != '\n' && !feof(stdin));
        return;
    }
    clearPage();
    //
    if (ch == 1) // Time line
    {
        int ret=timeline();
        clearPage();
        if(!ret)
            goto menu;
    }
    else if (ch == 2) // Report
    {
        int ret=report();
        clearPage();
        if(!ret)
            goto menu;
    }
    else if (ch==3) // Exit
    {
        clearPage();
        return;
    }
}