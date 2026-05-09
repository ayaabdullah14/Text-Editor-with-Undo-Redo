// Aya Abdullah ID:1220782 sec 2
// COMP2421 - Data Structures and Algorithms - Project #1
// Doubly Linked List + Radix Sort for Palestinian Districts

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>   // FIX: added missing header for isdigit/isupper/islower

// ── Data Structures ──────────────────────────────────────────────────────────

typedef struct Town {
    char name[50];
    int population;
    struct Town* next;
    struct Town* prev;
} Town;

typedef struct District {
    char district[50];
    int totalpopulation;
    struct Town* head;
    struct District* next;
    struct District* prev;
} District;

// ── Forward Declarations ─────────────────────────────────────────────────────

Town*     createTownNode(char* name, int population);
District* FindDistrict(District* head, char* districtName);
void      InsertTown(District* district, char* townName, int population);
void      InsertDistricts(char* x, char* y, int z, District* head, District* p);
District* readFromFile(const char* filename);
void      DeleteDistrict(District** l, char* nameDistrict);   // FIX: pointer-to-pointer
void      AddDistrict(District** l, char* nameOfDistrict, char* nameOfTown, int population); // FIX: pointer-to-pointer
void      printLoadedInfo(District* head);
void      printDistrict(District* head);
int       GetMax(District* head);
int       GetMin(District* head);
Town*     FindTown(Town* head, char* townName);
void      bubbleSort(District* head);
void      AddTown(District* head, char* districtName, char* townName, int population);
void      deleteTown(District* dis, char* nameTown);
void      DeleteTownList(Town* head);           // FIX: renamed & corrected signature
void      DeleteListDistrict(District* l);
int       GetMaxChar(District* L);
District* sortDistricts(District* L);
void      saveToFile(District* L);
void      ChangeValue(District* L, char* townName, int newPopulation);
int       populationOfPalestine(District* l);
void      printSortInfo(District* head);
void      printMenu();

// ── Main ─────────────────────────────────────────────────────────────────────

int main() {
    District* head = NULL;
    int num;

    do {
        printMenu();
        scanf("%d", &num);

        switch (num) {
        case 1:
            head = NULL;
            head = readFromFile("districts.txt");
            break;

        case 2:
            printLoadedInfo(head);
            break;

        case 3:
            head = sortDistricts(head);
            printf("Districts sorted alphabetically.\n");
            break;

        case 4:
            bubbleSort(head);
            printf("Towns sorted by population.\n");
            break;

        case 5:
            printSortInfo(head);
            break;

        case 6: {
            char nameDistrict[50], nameTown[50];
            int population;
            printf("Enter the name of District: ");
            scanf(" %49[^\n]", nameDistrict);
            printf("Enter the name of Town: ");
            scanf(" %49[^\n]", nameTown);
            printf("Enter the population of Town: ");
            scanf("%d", &population);
            AddDistrict(&head, nameDistrict, nameTown, population);  // FIX: pass address
            bubbleSort(head);
            head = sortDistricts(head);
            break;
        }

        case 7: {
            char nameDist[50], nametown[50];
            int populationTown;
            printf("Enter the name of District to add Town to: ");
            scanf(" %49[^\n]", nameDist);
            printf("Enter the name of Town: ");
            scanf(" %49[^\n]", nametown);
            printf("Enter the population of Town: ");
            scanf("%d", &populationTown);
            AddTown(head, nameDist, nametown, populationTown);
            break;
        }

        case 8: {
            // FIX: also ask for district name so deletion is unambiguous
            char distName[50], town[50];
            printf("Enter the name of District: ");
            scanf(" %49[^\n]", distName);
            printf("Enter the name of Town: ");
            scanf(" %49[^\n]", town);
            District* d = FindDistrict(head, distName);
            if (d != NULL)
                deleteTown(d, town);
            else
                printf("District not found.\n");
            break;
        }

        case 9: {
            char district[50];
            printf("Enter the name of District: ");
            scanf(" %49[^\n]", district);
            DeleteDistrict(&head, district);   // FIX: pass address
            break;
        }

        case 10:
            printf("Total population of Palestine: %d\n", populationOfPalestine(head));
            printf("Max town population: %d\n", GetMax(head));
            printf("Min town population: %d\n", GetMin(head));
            break;

        case 11:
            printDistrict(head);
            break;

        case 12: {
            char t[50];
            int popu;
            printf("Enter the name of Town: ");
            scanf(" %49[^\n]", t);
            printf("Enter the new population of Town: ");
            scanf("%d", &popu);
            ChangeValue(head, t, popu);
            break;
        }

        case 13:
            saveToFile(head);
            break;

        case 14:
            printf("Exit\n");   // FIX: typo "Exsit"
            break;

        default:
            printf("Invalid option. Please try again.\n");
        }

    } while (num != 14);

    return 0;
}

// ── Menu ─────────────────────────────────────────────────────────────────────

void printMenu() {
    printf("\n========= Menu =========\n");
    printf(" Please select an option (1-14):\n\n");  // FIX: was "1-8"
    printf("  1 - Load the input file\n");
    printf("  2 - Print loaded information (unsorted)\n");
    printf("  3 - Sort districts alphabetically (Radix Sort)\n");
    printf("  4 - Sort towns by population (ascending)\n");
    printf("  5 - Print sorted information\n");
    printf("  6 - Add a new district\n");
    printf("  7 - Add a new town to a district\n");
    printf("  8 - Delete a town from a district\n");
    printf("  9 - Delete a complete district\n");
    printf(" 10 - Population stats (total, max, min)\n");
    printf(" 11 - Print districts and total population\n");
    printf(" 12 - Change the population of a town\n");
    printf(" 13 - Save to output file\n");
    printf(" 14 - Exit\n");
    printf("========================\n");
    printf("Choice: ");
}

// ── Town / District Node Creation ────────────────────────────────────────────

Town* createTownNode(char* name, int population) {
    Town* newTown = (Town*)malloc(sizeof(Town));
    if (newTown == NULL) {
        printf("Memory allocation failed for town node.\n");
        return NULL;
    }
    // FIX: trim leading spaces that strtok may leave
    while (*name == ' ') name++;
    char* end = name + strlen(name) - 1;
    while (end > name && (*end == ' ' || *end == '\n' || *end == '\r')) *end-- = '\0';

    strcpy(newTown->name, name);
    newTown->population = population;
    newTown->next = NULL;
    newTown->prev = NULL;
    return newTown;
}

// ── Find ─────────────────────────────────────────────────────────────────────

District* FindDistrict(District* head, char* districtName) {
    // FIX: trim input before comparing
    while (*districtName == ' ') districtName++;
    District* current = head;
    while (current != NULL) {
        // compare trimmed versions
        char tmp[50];
        strcpy(tmp, current->district);
        char* e = tmp + strlen(tmp) - 1;
        while (e > tmp && (*e == ' ' || *e == '\n' || *e == '\r')) *e-- = '\0';
        if (strcmp(tmp, districtName) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

Town* FindTown(Town* head, char* townName) {
    while (*townName == ' ') townName++;
    Town* current = head;
    while (current != NULL) {
        char tmp[50];
        strcpy(tmp, current->name);
        char* e = tmp + strlen(tmp) - 1;
        while (e > tmp && (*e == ' ' || *e == '\n' || *e == '\r')) *e-- = '\0';
        if (strcmp(tmp, townName) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

// ── Insert ───────────────────────────────────────────────────────────────────

void InsertTown(District* district, char* townName, int population) {
    Town* newTown = createTownNode(townName, population);
    if (newTown == NULL) return;
    if (district->head == NULL) {
        district->head = newTown;
    } else {
        Town* currentTown = district->head;
        while (currentTown->next != NULL)
            currentTown = currentTown->next;
        currentTown->next = newTown;
        newTown->prev = currentTown;
    }
    district->totalpopulation += newTown->population;
}

void InsertDistricts(char* x, char* y, int z, District* head, District* p) {
    District* existingDistrict = FindDistrict(head, x);
    if (existingDistrict == NULL) {
        District* newDist = (District*)malloc(sizeof(District));
        if (newDist == NULL) {
            printf("Memory allocation failed for district node.\n");
            return;
        }
        // trim x
        while (*x == ' ') x++;
        char* e = x + strlen(x) - 1;
        while (e > x && (*e == ' ' || *e == '\n' || *e == '\r')) *e-- = '\0';

        strcpy(newDist->district, x);
        newDist->head = NULL;
        newDist->totalpopulation = 0;
        newDist->next = p->next;
        newDist->prev = p;
        p->next = newDist;
        if (newDist->next != NULL)
            newDist->next->prev = newDist;
        InsertTown(newDist, y, z);
    } else {
        InsertTown(existingDistrict, y, z);
    }
}

// ── Read from File ───────────────────────────────────────────────────────────

District* readFromFile(const char* filename) {
    if (filename == NULL) {
        printf("Error: filename is NULL\n");
        return NULL;
    }
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: could not open file '%s'\n", filename);
        return NULL;
    }

    District* head = NULL;
    char line[200];

    while (fgets(line, sizeof(line), file) != NULL) {
        // skip blank lines
        if (line[0] == '\n' || line[0] == '\r') continue;

        char* p0 = strtok(line, "|");
        char* p1 = strtok(NULL, "|");
        char* p2 = strtok(NULL, "|");
        if (p0 == NULL || p1 == NULL || p2 == NULL) continue;

        // trim district name
        while (*p0 == ' ') p0++;
        char* e = p0 + strlen(p0) - 1;
        while (e > p0 && (*e == ' ' || *e == '\n' || *e == '\r')) *e-- = '\0';

        int pop = atoi(p2);

        // FIX: simplified — no double InsertDistricts call
        if (head == NULL) {
            head = (District*)malloc(sizeof(District));
            if (head == NULL) { fclose(file); return NULL; }
            strcpy(head->district, p0);
            head->head = NULL;
            head->totalpopulation = 0;
            head->next = NULL;
            head->prev = NULL;
            InsertTown(head, p1, pop);
        } else {
            District* existing = FindDistrict(head, p0);
            if (existing == NULL) {
                // find last node
                District* last = head;
                while (last->next != NULL) last = last->next;
                District* newDist = (District*)malloc(sizeof(District));
                if (newDist == NULL) { fclose(file); return head; }
                strcpy(newDist->district, p0);
                newDist->head = NULL;
                newDist->totalpopulation = 0;
                newDist->next = NULL;
                newDist->prev = last;
                last->next = newDist;
                InsertTown(newDist, p1, pop);
            } else {
                InsertTown(existing, p1, pop);
            }
        }
    }

    printf("File loaded successfully.\n");
    fclose(file);
    return head;
}

// ── Print ─────────────────────────────────────────────────────────────────────

void printLoadedInfo(District* head) {
    if (head == NULL) { printf("No data loaded.\n"); return; }
    District* d = head;
    while (d != NULL) {
        Town* t = d->head;
        while (t != NULL) {
            printf("%s | %s | %d\n", d->district, t->name, t->population);
            t = t->next;
        }
        d = d->next;
    }
}

void printDistrict(District* head) {
    if (head == NULL) { printf("No data loaded.\n"); return; }
    District* d = head;
    while (d != NULL) {
        printf("%s, Population = %d\n", d->district, d->totalpopulation);
        d = d->next;
    }
}

// FIX: removed erroneous re-sort and re-bubblesort inside the print loop
void printSortInfo(District* head) {
    if (head == NULL) { printf("No data loaded.\n"); return; }
    District* d = head;
    while (d != NULL) {
        printf("%s District, Population = %d\n", d->district, d->totalpopulation);
        Town* t = d->head;
        while (t != NULL) {
            printf("  %s, %d\n", t->name, t->population);
            t = t->next;
        }
        d = d->next;
    }
}

// ── Sort ─────────────────────────────────────────────────────────────────────

// FIX: bubbleSort — guard against NULL next pointer (single-town districts)
void bubbleSort(District* head) {
    District* dis = head;
    while (dis != NULL) {
        if (dis->head == NULL || dis->head->next == NULL) {
            dis = dis->next;
            continue;
        }
        int swapped;
        do {
            swapped = 0;
            Town* current = dis->head;
            while (current != NULL && current->next != NULL) {
                if (current->population > current->next->population) {
                    // swap data only (names + population)
                    int tmpPop = current->population;
                    current->population = current->next->population;
                    current->next->population = tmpPop;

                    char tmpName[50];
                    strcpy(tmpName, current->name);
                    strcpy(current->name, current->next->name);
                    strcpy(current->next->name, tmpName);

                    swapped = 1;
                }
                current = current->next;
            }
        } while (swapped);
        dis = dis->next;
    }
}

int GetMaxChar(District* L) {
    int max = 0;
    District* temp = L;
    while (temp != NULL) {
        int len = (int)strlen(temp->district);
        if (len > max) max = len;
        temp = temp->next;
    }
    return max;
}

District* sortDistricts(District* L) {
    if (L == NULL) return L;

    const int NUM_BUCKETS = 64;
    District* buckets[64];
    District* sorted = NULL;

    int maxLen = GetMaxChar(L);

    for (int pos = maxLen - 1; pos >= 0; pos--) {
        for (int i = 0; i < NUM_BUCKETS; i++) buckets[i] = NULL;

        District* current = L;
        while (current != NULL) {
            int index;
            char x = (pos < (int)strlen(current->district)) ? current->district[pos] : '\0';
            if (isdigit((unsigned char)x))
                index = x - '0';
            else if (isupper((unsigned char)x))
                index = 10 + (x - 'A');
            else if (islower((unsigned char)x))
                index = 10 + (x - 'a');
            else
                index = NUM_BUCKETS - 1;

            District* nextNode = current->next;
            current->next = buckets[index];
            if (buckets[index] != NULL) buckets[index]->prev = current;
            current->prev = NULL;
            buckets[index] = current;
            current = nextNode;
        }

        sorted = NULL;
        for (int i = NUM_BUCKETS - 1; i >= 0; i--) {
            while (buckets[i] != NULL) {
                District* temp = buckets[i];
                buckets[i] = temp->next;
                temp->next = sorted;
                temp->prev = NULL;
                if (sorted != NULL) sorted->prev = temp;
                sorted = temp;
            }
        }
        L = sorted;
    }
    // Fix prev pointers after sort
    if (L != NULL) {
        L->prev = NULL;
        District* cur = L;
        while (cur->next != NULL) {
            cur->next->prev = cur;
            cur = cur->next;
        }
    }
    return L;
}

// ── Add / Delete ─────────────────────────────────────────────────────────────

void AddTown(District* head, char* districtName, char* townName, int population) {
    District* current = FindDistrict(head, districtName);
    if (current != NULL) {
        Town* existingTown = FindTown(current->head, townName);
        if (existingTown == NULL)
            InsertTown(current, townName, population);
        else
            printf("Town already exists in the district.\n");
    } else {
        printf("District does not exist.\n");
    }
}

// FIX: pointer-to-pointer so head can be updated when first element is deleted
void AddDistrict(District** l, char* nameOfDistrict, char* nameOfTown, int population) {
    District* dist = FindDistrict(*l, nameOfDistrict);
    if (dist != NULL) {
        printf("District already exists.\n");
        return;
    }
    dist = (District*)malloc(sizeof(District));
    if (dist == NULL) { printf("Memory allocation failed.\n"); return; }

    // trim
    while (*nameOfDistrict == ' ') nameOfDistrict++;
    char* e = nameOfDistrict + strlen(nameOfDistrict) - 1;
    while (e > nameOfDistrict && (*e == ' ' || *e == '\n' || *e == '\r')) *e-- = '\0';

    strcpy(dist->district, nameOfDistrict);
    dist->head = NULL;
    dist->totalpopulation = 0;
    dist->next = NULL;
    dist->prev = NULL;

    if (*l == NULL) {
        *l = dist;
    } else {
        District* temp = *l;
        while (temp->next != NULL) temp = temp->next;
        temp->next = dist;
        dist->prev = temp;
    }
    InsertTown(dist, nameOfTown, population);
    printf("District '%s' added.\n", dist->district);
}

// FIX: DeleteTownList — correct type; was mixing Town* and District* casts
void DeleteTownList(Town* head) {
    Town* current = head;
    while (current != NULL) {
        Town* temp = current->next;
        free(current);
        current = temp;
    }
}

// FIX: pointer-to-pointer so head can be updated if first district is deleted
void DeleteDistrict(District** l, char* nameDistrict) {
    District* exist = FindDistrict(*l, nameDistrict);
    if (exist == NULL) {
        printf("District '%s' not found.\n", nameDistrict);
        return;
    }
    // Unlink
    if (exist->prev != NULL)
        exist->prev->next = exist->next;
    else
        *l = exist->next;   // FIX: update head if first node deleted

    if (exist->next != NULL)
        exist->next->prev = exist->prev;

    DeleteTownList(exist->head);   // delete all towns first
    free(exist);
    printf("District deleted.\n");
}

void deleteTown(District* dis, char* nameTown) {
    Town* town = FindTown(dis->head, nameTown);
    if (town == NULL) {
        printf("Town '%s' not found.\n", nameTown);
        return;
    }
    if (town->prev != NULL)
        town->prev->next = town->next;
    else
        dis->head = town->next;   // was first town

    if (town->next != NULL)
        town->next->prev = town->prev;

    dis->totalpopulation -= town->population;
    free(town);
    printf("Town deleted.\n");
}

// ── Stats ─────────────────────────────────────────────────────────────────────

int GetMax(District* head) {
    if (head == NULL || head->head == NULL) return 0;
    int max = 0;
    District* current = head;
    while (current != NULL) {
        Town* t = current->head;
        while (t != NULL) {
            if (t->population > max) max = t->population;
            t = t->next;
        }
        current = current->next;
    }
    return max;
}

int GetMin(District* head) {
    if (head == NULL || head->head == NULL) return 0;
    int min = head->head->population;
    District* current = head;
    while (current != NULL) {
        Town* t = current->head;
        while (t != NULL) {
            if (t->population < min) min = t->population;
            t = t->next;
        }
        current = current->next;
    }
    return min;
}

int populationOfPalestine(District* l) {
    int total = 0;
    District* current = l;
    while (current != NULL) {
        total += current->totalpopulation;
        current = current->next;
    }
    return total;
}

// FIX: ChangeValue — update totalpopulation correctly using the difference
void ChangeValue(District* L, char* townName, int newPopulation) {
    District* current = L;
    while (current != NULL) {
        Town* t = FindTown(current->head, townName);
        if (t != NULL) {
            current->totalpopulation -= t->population;   // subtract old
            t->population = newPopulation;
            current->totalpopulation += newPopulation;   // add new
            printf("Population updated.\n");
            return;
        }
        current = current->next;
    }
    printf("Town '%s' not found.\n", townName);
}

// ── Save to File ─────────────────────────────────────────────────────────────

// FIX: was starting from L->next (skipping first district)
// FIX: output format matches spec: "Gaza District, Population = 70000"
// FIX: removed erroneous line that re-added head population on every iteration
void saveToFile(District* L) {
    FILE* out = fopen("sorted_districts.txt", "w");
    if (out == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }
    District* temp = L;   // FIX: start from L, not L->next
    while (temp != NULL) {
        fprintf(out, "%s District, Population = %d\n", temp->district, temp->totalpopulation);
        Town* town = temp->head;
        while (town != NULL) {
            fprintf(out, "%s, %d\n", town->name, town->population);
            town = town->next;
        }
        temp = temp->next;
    }
    fclose(out);
    printf("Saved to sorted_districts.txt\n");
}

void DeleteListDistrict(District* l) {
    District* p = l;
    while (p != NULL) {
        District* temp = p->next;
        DeleteTownList(p->head);
        free(p);
        p = temp;
    }
}