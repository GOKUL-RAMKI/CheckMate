#include "headers.txt"
// --- ENUMERATION AND STRUCTURES ---
typedef enum
{
    PENDING = 0,
    APPROVED = 1,
    REJECTED = 2
} status;

typedef struct
{
    char machine[bufferSize], comp_id[bufferSize], comp_name[bufferSize], dim[bufferSize], unit[bufferSize], numComp[bufferSize];
    double expected, tol;
    status st;
} Row;


// Converts a string ("0", "1", "2") to the 'status' enumeration
static status stparse(const char *s)
{
    if (strcmp(s, "1") == 0)
    {
        return APPROVED;
    }
    else if (strcmp(s, "2") == 0)
    {
        return REJECTED;
    }
    else
    {
        return PENDING;
    }
}

// Converts the 'status' enumeration back to its string representation ("0", "1", "2")
static const char *ststr(status s)
{
    if (s == APPROVED)
    {
        return "1";
    }
    else if (s == REJECTED)
    {
        return "2";
    }
    else
    {
        return "0";
    }
}

// Parses a comma-separated line (string) into a Row structure
int parse_line(char *line, Row *r)
{
    char *f[10];
    int i = 0;

    for (char *p = strtok(line, ","); p && i < 9; p = strtok(NULL, ","))
    {
        f[i++] = p;
    }
    if (i < 9)
        return 0;

    strncpy(r->machine, f[0], bufferSize - 1);
    r->machine[bufferSize - 1] = 0;
    strncpy(r->comp_id, f[1], bufferSize - 1);
    r->comp_id[bufferSize - 1] = 0;
    strncpy(r->comp_name, f[2], bufferSize - 1);
    r->comp_name[bufferSize - 1] = 0;
    strncpy(r->dim, f[3], bufferSize - 1);
    r->dim[bufferSize - 1] = 0;

    r->expected = atof(f[4]);
    r->tol = atof(f[5]);

    strncpy(r->unit, f[6], bufferSize - 1);
    r->unit[bufferSize - 1] = 0;
    strncpy(r->numComp, f[7], bufferSize - 1);
    r->numComp[bufferSize - 1] = 0;

    char statusStr[bufferSize];
    strncpy(statusStr, f[8], bufferSize - 1);
    statusStr[bufferSize - 1] = 0;

    statusStr[strcspn(statusStr, "\r\n")] = 0;

    r->st = stparse(statusStr);
    return 1;
}

// Reads all data from the CSV file into the array of Row structures
int read_csv(const char *path, Row rows[])
{
    FILE *f = fopen(path, "r");
    if (!f)
        return -1;
    char buffer[1024];
    int n = 0;

    while (fgets(buffer, sizeof(buffer), f) && n < MAX_ROWS)
    {
        char tmp[1024];
        strcpy(tmp, buffer);

        if (parse_line(tmp, &rows[n]))
            n++;
    }
    fclose(f);
    return n;
}

// Writes the array of Row structures back to a file
int write_csv(Row rows[], int n)
{
    FILE *f = fopen(FPATH, "w");
    if (!f)
        return 0;

    for (int i = 0; i < n; i++)
    {
        fprintf(f, "%s,%s,%s,%s,%.10g,%.10g,%s,%s,%s\n",
                rows[i].machine, rows[i].comp_id, rows[i].comp_name, rows[i].dim,
                rows[i].expected, rows[i].tol, rows[i].unit, rows[i].numComp, ststr(rows[i].st));
    }
    if (ferror(f))
    {
        fclose(f);
        return 0;
    }

    fclose(f);
    return 1;
}

// Check if two Row structures have the same unique key
int samekey(const Row *a, const Row *b)
{
    return !strcmp(a->machine, b->machine) && !strcmp(a->comp_id, b->comp_id) && !strcmp(a->dim, b->dim);
}

// Safely reads a floating-point number (double) from standard input
int read_double(double *out)
{
    if (scanf("%lf", out) == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// Updates the inspection status of a specific component in the CSV file
int update_status(const char *machine, const char *compId, status newSt)
{
    const char *res;
    if (newSt == APPROVED) res = "APPROVED";
    else if (newSt == REJECTED) res = "REJECTED";
    else res = "PENDING";

    // Time setup
    time_t raw_time = time(NULL);
    struct tm *local_time_info = localtime(&raw_time);

    // Opening report file
    FILE *op = fopen(TL_FPATH, "a");

    if (op == NULL)
    {
        printf("\tDEBUG: Report file failed to open! (Continuing without log)\n");
        fflush(stdout);
    }
    else
    {
        fprintf(op, "%s, %s, %s, %02d:%02d, %d-%d-%d\n",
                str_to_lower((char*)machine),
                str_to_lower((char*)compId),
                res,
                local_time_info->tm_hour,
                local_time_info->tm_min,
                local_time_info->tm_mday,
                local_time_info->tm_mon + 1,
                local_time_info->tm_year + 1900
        );

        if (ferror(op))
        {
            printf("\tDEBUG: Error encountered during file write operation.\n");
        }

        fclose(op);
        fflush(stdout);
    }

    // Read all data into memory
    Row rows[MAX_ROWS];
    int cnt = read_csv(FPATH, rows);
    if (cnt < 0)
    {
        printf("\n--- ERROR ---\nError reading file. Status NOT saved.\n");
        return 0;
    }

    int found = 0;
    // Iterate and update the status for all parameters of the target component
    for (int i = 0; i < cnt; i++)
    {
        if (!strcmp(rows[i].machine, machine) && !strcmp(rows[i].comp_id, compId))
        {
            if (rows[i].st == PENDING)
            {
                rows[i].st = newSt;
                found++;
            }
        }
    }

    if (found == 0)
    {
        printf("\t\n--- ERROR ---\nComponent not found or not pending. Status NOT saved.\n");
        return 0;
    }

    // Write the modified data back to the file
    if (!write_csv(rows, cnt))
    {
        printf("\t\n--- ERROR ---\nWrite failed. Status NOT saved/reset.\n");
        return 0;
    }

    return 1;
}

// Main function for the Quality Department
int quality()
{
    getchar(); // Consume any leftover newline character
    char chosenMachine[bufferSize], chosenComp[bufferSize];
    char ch;

    while (1) // Loop 1: Select Machine or Quit
    {
        printf("\t\n\n(Q)uit / Enter Machine ID: ");
        if (!fgets(chosenMachine, sizeof(chosenMachine), stdin))
            break;

        chosenMachine[strcspn(chosenMachine, "\r\n")] = 0;

        if (chosenMachine[0] == 'q' || chosenMachine[0] == 'Q')
            break;
        if (chosenMachine[0] == 0)
            continue;

        Row rows[MAX_ROWS];
        int cnt = read_csv(FPATH, rows);
        if (cnt < 0)
        {
            printf("\tError opening file\n");
            continue;
        }
        if (cnt == 0)
        {
            printf("\tFile is empty or has only a header.\n");
            continue;
        }

        printf("\t\nPending Components in machine %s:\n", chosenMachine);
        printf("\t--------------------------------------------------\n");
        printf("\t|%14s| %-20s| %-12s|\n", "Component ID", "Name", "Dimensions");
        printf("\t--------------------------------------------------\n");

        int any = 0;
        // List all PENDING components for the chosen machine
        for (int i = 0; i < cnt; i++)
        {
            char machineIdLower[bufferSize];
            strcpy(machineIdLower, rows[i].machine);
            str_to_lower(machineIdLower);

            char chosenMachineLower[bufferSize];
            strcpy(chosenMachineLower, chosenMachine);
            str_to_lower(chosenMachineLower);

            if (rows[i].st == PENDING && !strcmp(machineIdLower, chosenMachineLower))
            {
                any = 1;
                printf("\t|%14s| %-20s| %-12s|\n", rows[i].comp_id, rows[i].comp_name, rows[i].dim);
            }
        }
        printf("\t--------------------------------------------------\n");

        if (!any)
        {
            printf("\tNo pending components for machine %s\n", chosenMachine);
            continue;
        }

        // Prompt for Component ID to inspect
        printf("\tEnter component id to inspect: ");
        if (!scanf("%s", chosenComp))
            break;
        chosenComp[strcspn(chosenComp, "\r\n")] = 0;
        str_to_lower(chosenComp);

        int foundComp = 0;
        // Verify the chosen component is pending for the chosen machine
        for (int i = 0; i < cnt; i++)
        {
            char compIdLower[bufferSize];
            strcpy(compIdLower, rows[i].comp_id);
            str_to_lower(compIdLower);

            char chosenCompLower[bufferSize];
            strcpy(chosenCompLower, chosenComp);
            str_to_lower(chosenCompLower);

            char machineIdLower[bufferSize];
            strcpy(machineIdLower, rows[i].machine);
            str_to_lower(machineIdLower);

            char chosenMachineLower[bufferSize];
            strcpy(chosenMachineLower, chosenMachine);
            str_to_lower(chosenMachineLower);

            if (!strcmp(chosenCompLower, compIdLower) && rows[i].st == PENDING && !strcmp(machineIdLower, chosenMachineLower))
            {
                foundComp = 1;
                break;
            }
        }

        if (!foundComp)
        {
            printf("\tComponent not found or not pending.\n");
            continue;
        }

        int continueWithComponent = 1;

        while (continueWithComponent) // Loop 2: Sample collection for the chosen component
        {
            int sampleCount = 0;
            int continueSampling = 1;

            while (continueSampling) // Loop 3: Inspection of a single sample
            {
                sampleCount++;
                printf("\n\t--- Sample %d ---\n", sampleCount);

                Row dimRows[MAX_ROWS];
                int dimCount = 0;

                // Collect all PENDING parameter rows for the specific Machine/Component
                for (int i = 0; i < cnt; i++)
                {
                    char compIdLower[bufferSize];
                    strcpy(compIdLower, rows[i].comp_id);
                    str_to_lower(compIdLower);

                    char chosenCompLower[bufferSize];
                    strcpy(chosenCompLower, chosenComp);
                    str_to_lower(chosenCompLower);

                    char machineIdLower[bufferSize];
                    strcpy(machineIdLower, rows[i].machine);
                    str_to_lower(machineIdLower);

                    char chosenMachineLower[bufferSize];
                    strcpy(chosenMachineLower, chosenMachine);
                    str_to_lower(chosenMachineLower);

                    if (!strcmp(chosenCompLower, compIdLower) && rows[i].st == PENDING && !strcmp(machineIdLower, chosenMachineLower))
                    {
                        dimRows[dimCount++] = rows[i];
                    }
                }

                if (dimCount == 0)
                {
                    printf("\tComponent is no longer pending. Exiting sample collection.\n");
                    break;
                }

                int allPassed = 1;

                // Loop 4: Enter measured value for each dimension/parameter
                for (int d = 0; d < dimCount; d++)
                {
                    double obs;
                    printf("\tEnter Observed %s of %s in %s: ", dimRows[d].dim, dimRows[d].comp_name, dimRows[d].unit);

                    if (!read_double(&obs))
                    {
                        while (getchar() != '\n' && !feof(stdin) && !ferror(stdin))
                            ;
                        printf("\tInvalid input. Sample check aborted.\n");
                        allPassed = -1;
                        break;
                    }

                    double diff = fabs(obs - dimRows[d].expected);

                    if (diff > dimRows[d].tol + EPSILON)
                    {
                        allPassed = 0;
                        printf("\t[FAIL] %s: measured %f\n", dimRows[d].dim, obs);
                    }
                    else
                    {
                        printf("\t[PASS] %s: measured %f\n", dimRows[d].dim, obs);
                    }
                }

                if (allPassed == -1)
                {
                    break;
                }

                if (allPassed == 0) // Sample FAILED (REJECTED)
                {
                    printf("\t\nSample %d: REJECTED\n", sampleCount);

                    if (update_status(dimRows[0].machine, dimRows[0].comp_id, REJECTED))
                    {
                        printf("\tStatus saved successfully.\n");
                    }

                    printf("\t\nNext machine (M) or Quit (Q)? ");
                    if (scanf(" %c", &ch) != 1)
                        ch = 'm';
                    while (getchar() != '\n');

                    if (ch == 'm' || ch == 'M')
                    {
                        fflush(stdout);
                        clearPage();
                        continueSampling = 0;
                        continueWithComponent = 0;
                    }
                    else if (ch == 'q' || ch == 'Q')
                    {
                        fflush(stdout);
                        clearPage();
                        return 0;
                    }
                    else
                    {
                        continueSampling = 0;
                        continueWithComponent = 0;
                    }
                }
                else // Sample PASSED
                {
                    printf("\t\nSample %d: ALL DIMENSIONS APPROVED\n", sampleCount);

                    printf("\tCheck another sample of this component (Y/n)? ");
                    if (scanf(" %c", &ch) != 1)
                        ch = 'n';
                    while (getchar() != '\n');

                    if (ch == 'y' || ch == 'Y')
                    {
                        continueSampling = 1;
                    }
                    else
                    {
                        if (update_status(dimRows[0].machine, dimRows[0].comp_id, APPROVED))
                        {
                            printf("\tStatus saved successfully.\n");
                        }

                        printf("\t\nNext machine (M) or Quit (Q)? ");
                        if (scanf(" %c", &ch) != 1)
                            ch = 'm';
                        while (getchar() != '\n');

                        if (ch == 'm' || ch == 'M')
                        {
                            fflush(stdout);
                            clearPage();
                            continueSampling = 0;
                            continueWithComponent = 0;
                        }
                        else if (ch == 'q' || ch == 'Q')
                        {
                            fflush(stdout);
                            clearPage();
                            return 0;
                        }
                        else
                        {
                            printf("\tInvalid input. Returning to component selection...\n");
                            continueSampling = 0;
                            continueWithComponent = 0;
                        }
                    }
                }
            }
        }
    }

    printf("\t\nExiting quality control system.\n");
    return 0;
}