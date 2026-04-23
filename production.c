int revert_status(const char *machine, const char *compId)
{
    // Try to acquire the file lock for exclusive access


    // Read all data into memory
    Row rows[MAX_ROWS];
    int cnt = read_csv(FPATH, rows);

    int found_rejected = 0;
    // Iterate and update the status for all parameters of the target component
    for (int i = 0; i < cnt; i++)
    {
        // Match by machine ID and component ID (case-insensitive comparison is recommended here)
        // Assuming string comparison helpers are not in the scope of revert_status
        if (!strcmp(str_to_lower(rows[i].machine), str_to_lower(machine)) && !strcmp(str_to_lower(rows[i].comp_id), str_to_lower(compId)))
        {
            // IMPORTANT: Only revert if it is currently REJECTED
            if (rows[i].st == REJECTED) 
            {
                rows[i].st = PENDING; // Reset to PENDING (0)
                found_rejected++;
            }
        }
    }

    if (found_rejected == 0)
    {
        printf("\t\n--- NOTE ---\nComponent not found or not currently REJECTED. Status not reset.\n");
        return 0;
    }

    // Write the modified data back to the file using atomic replacement
    if (!write_csv(rows, cnt))
    {
        printf("\t\n--- ERROR ---\nWrite failed. Status NOT reset.\n");
        return 0;
    }

    return 1; // Success
}

void production()
{
    int option;
    
    // l4: Main Production Menu label 
    l4:printf("\tYOU HAVE ENTERED THE PRODUCTION DEPARTMENT\n");
    
    // l2: Sub-menu label (used for looping the menu prompt)
    l2:printf("\tMENU:\n\t1.Define a new component\n\t2.View status\n\t3.Rejected Components\n\t4.Exit production department\n");
    
    // l6: Option selection label (used for invalid input retry)
    l6:printf("\tEnter your option:");
    scanf("%d",&option); // Read the user's choice

    // New component definition
    if(option==1)
    {
        clearPage();
        printf("\tRedirecting...");
        Sleep(1000);
        clearPage();
        printf("\tDefine new component:\n");
        
        // Variables for new component data
        char m_id[50],comp_id[50],comp_name[50],para[50],unit[10];
        // Variables for checking against existing file data
        char fm_id[50],fcomp_id[50]; 
        int n,status,no_units;
        float val,tolerance;
        
        getchar();
        
        printf("\tEnter the machine id:");
        scanf("%[^\n]s",m_id); // Read machine ID
        getchar();
        
        printf("\tEnter the component's id:");
        scanf("%[^\n]s",comp_id); // Read component ID
        getchar();
        
        // Open the parameters file for reading and appending
        f1=fopen(FPATH,"r+");
        
        // Loop through the file to check for duplicate Machine ID and Component ID
        // %*[^,],%*[^,],%*f,%*f,%*[^,],%*d,%*d%*c skips the middle fields
        while(fscanf(f1,"%[^,],%[^,],%*[^,],%*[^,],%*f,%*f,%*[^,],%*d,%*d%*c",fm_id,fcomp_id)==2)
        {
            // Compare the input IDs (converted to lower case for case-insensitivity)
            if(strcmp(str_to_lower(comp_id),str_to_lower(fcomp_id))==0 && strcmp(str_to_lower(m_id),str_to_lower(fm_id))==0)
            {
                printf("\t\nDuplicate entry!Try again with a different id.\n");
                fclose(f1);
                Sleep(2500);
                clearPage();
                goto l4; // Go back to the main production menu
            }
        }
        
        // If no duplicate found, continue data entry
        printf("\tEnter the component's name:");
        scanf("%[^\n]",comp_name);
        getchar();
        
        printf("\tEnter the number of units to be produced:");
        scanf("%d",&no_units);
        
        printf("\tEnter the number of parameters:");
        scanf("%d",&n); // Number of dimensions/parameters to define
        // Loop for entering multiple parameters
        for(int i=1;i<=n;i++)
        {
            char modify;
            l5:printf("\t\n"); // Loop label for modifying the current parameter
            getchar();
            printf("\tEnter the parameter %d:",i);
            scanf("%[^\n]s",para); // Read parameter name 
            
            printf("\tEnter the dimension/value of the parameter:");
            scanf("%f",&val); // Read expected value
            
            printf("\tEnter the tolerance allowed:");
            scanf("%f",&tolerance); // Read tolerance
            getchar();
            printf("\tEnter the unit:");
            scanf("%[^\n]s",unit); // Read unit 
            getchar();
            // Confirmation
            printf("\tYou entered: %s %.4f %s\n",para,val,unit);
            printf("\tDo you want to modify? (Y/n)");
            
            scanf("%c",&modify);
            
            if(modify=='Y' || modify=='y')
            {
                goto l5; // Go back to re-enter this parameter
            }
            
            // Write the new component parameter to the file (status is 0, meaning PENDING)
            fprintf(f1,"%s,%s,%s,%s,%f,%f,%s,%d,%d\n",m_id,comp_id,comp_name,para,val,tolerance,unit,no_units,0);
        }
        
        fclose(f1);
        
        // Success messages and redirection
        clearPage();
        printf("\tAdding new component...");
        Sleep(3000);
        printf("\t\nComponent added successfully!");
        Sleep(2000);
        clearPage();
        printf("\tGoing back to the production department menu...");
        Sleep(2000);
        clearPage();
        goto l4; // Return to main production menu
        
    }
    // Option 2: View Status
    else if(option==2)
    {
        int status,no_units,flag=0;
        char comp_id[20],machine_id[20],parameter[50],unit[20];
        float value,tolerance;
        char fcomp_id[20],fmachine_id[20],comp_name[50];
        
        clearPage();
        printf("\tRedirecting to status page...");
        Sleep(2000);
        clearPage();
        printf("\tStatus menu:\n");
        
        f1=fopen(FPATH,"r"); // Open file for reading
        
        printf("\tEnter the machine id:");
        getchar();
        scanf("%[^\n]s",machine_id); // Read input machine ID
        getchar();
        
        printf("\tEnter the component id:");     
        scanf("%[^\n]s",comp_id); // Read input component ID

        // Loop through all records in the file
        while(fscanf(f1,"%[^,],%[^,],%[^,],%[^,],%f,%f,%[^,],%d,%d",fmachine_id,fcomp_id,comp_name,parameter,&value,&tolerance,unit,&no_units,&status)==9)
        {
            fscanf(f1,"%*c");
            // Check if the input IDs match the file record IDs 
            if(strcmp(str_to_lower(comp_id),str_to_lower(fcomp_id))==0 && strcmp(str_to_lower(fmachine_id),str_to_lower(machine_id))==0)
            {
                flag=1; // Component found
                printf("\t\nComponent id:%s\nMachine id:%s\n",comp_id,machine_id);
                
                // Display the status based on the numeric value
                if(status==0)
                {
                    printf("\tStatus: Yet to be checked\n"); // PENDING
                }
                else if(status==1)
                {
                    printf("\tStatus: Passed\n"); // APPROVED
                }
                else
                {
                    printf("\tStatus: Failed\n"); // REJECTED
                }
                break; // Exit the loop once the status is found (assuming we only need to show one status for the ID combination)
            }
        }
        
        if(flag==0)
        {
            printf("\tNo such component found!\n");
        }
        
        char ch;
        getchar(); // Consume newline
        printf("\t\nPress any key to continue...");
        scanf("%c",&ch); // Wait for a key press
         
        fclose(f1);
        clearPage();
        printf("\tGoing back to production department menu...");
        Sleep(2000);
        clearPage();
        goto l4; // Return to main production menu

    }
    //OPTION 3 to revert status
    else if(option==3)
    {
        char m_id_rev[50], comp_id_rev[50];
        char ch_corr;
        clearPage();
        printf("\t--- RETURNED COMPONENTS MANAGEMENT ---\n");
        
        getchar();
        printf("\tEnter Machine ID of the component: ");
        scanf("%[^\n]", m_id_rev);
        getchar(); // Consume newline
        
        printf("\tEnter Component ID to reset status: ");
        scanf("%[^\n]", comp_id_rev);
        getchar(); // Consume newline
        
        printf("\t\nHas the issue with component ID '%s' from machine ID '%s' been corrected (Y/N)? ", comp_id_rev, m_id_rev);
        scanf(" %c", &ch_corr); // Note the space before %c to skip any leftover whitespace
        
        if (ch_corr == 'Y' || ch_corr == 'y')
        {
            // Reset status from REJECTED (2) to PENDING (0)
            printf("\t\nAttempting to reset status to PENDING (0)...\n");
            
            if (revert_status(m_id_rev, comp_id_rev))
            {
                printf("\tStatus successfully reset! Component will now be treated as a fresh lot.\n");
            }
            // Error messages handled inside revert_status function
        }
        else
        {
            printf("\tStatus NOT reset. Component remains REJECTED.\n");
        }
        
        Sleep(2000);
        clearPage();
        printf("\tReturning to the production department menu...");
        Sleep(2000);
        clearPage();
        goto l4;
    }
    // Option 4: To exit
    else if(option==4)
    {
        clearPage();
        printf("\tExiting...");
        Sleep(1000);
        clearPage();
        printf("\tGoing back to home page...");
        Sleep(1000);
        clearPage();
        // returns to the 'goto l7' in main()
    }   
    else // Invalid input
    {
        printf("\tInvalid input!Try again!\n");
        goto l6; // Go back to the option prompt
    }
}