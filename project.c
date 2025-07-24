
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

typedef struct {
    char name[MAX];
    char location[MAX];
    int family_size;
    int has_injured;
    int has_vulnerable;
    int priority;
} Victim;

typedef struct {
    char item_name[MAX];
    int quantity;
} Resource;

void victim_register() {
    FILE *fp = fopen("victims.txt", "a");
    if (!fp) {
        printf("File error.\n");
        return;
    }

    Victim v;
    printf("Enter victim name: ");
    fgets(v.name, MAX, stdin);
    v.name[strcspn(v.name, "\n")] = 0;

    printf("Enter location: ");
    fgets(v.location, MAX, stdin);
    v.location[strcspn(v.location, "\n")] = 0;

    printf("Enter family size: ");
    scanf("%d", &v.family_size);

    printf("Any injured members? (1-Yes, 0-No): ");
    scanf("%d", &v.has_injured);

    printf("Any elderly or children? (1-Yes, 0-No): ");
    scanf("%d", &v.has_vulnerable);
    getchar(); 

    v.priority = v.has_injured * 2 + v.has_vulnerable + (v.family_size > 4);

    fprintf(fp, "%s|%s|%d|%d|%d|%d\n", v.name, v.location, v.family_size,
            v.has_injured, v.has_vulnerable, v.priority);
    fclose(fp);
    printf("Victim registered.\n");
}

void supply_input() {
    FILE *fp = fopen("resources.txt", "a");
    if (!fp) {
        printf("File error.\n");
        return;
    }

    Resource r;
    printf("Enter resource item name: ");
    fgets(r.item_name, MAX, stdin);
    r.item_name[strcspn(r.item_name, "\n")] = 0;

    printf("Enter quantity: ");
    scanf("%d", &r.quantity);
    getchar(); 

    fprintf(fp, "%s|%d\n", r.item_name, r.quantity);
    fclose(fp);
    printf("Resource added.\n");
}

void generate_report() {
    FILE *fp = fopen("allocation_log.txt", "r");
    if (!fp) {
        printf("No allocation log found.\n");
        return;
    }

    char line[256];
    printf("\n--- Allocation Report ---\n");
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
}

void allocate_resources() {
    FILE *vf = fopen("victims.txt", "r");
    FILE *rf = fopen("resources.txt", "r");
    FILE *log = fopen("allocation_log.txt", "a");

    if (!vf || !rf || !log) {
        printf("Missing file(s).\n");
        if (vf) fclose(vf);
        if (rf) fclose(rf);
        if (log) fclose(log);
        return;
    }

    Victim victims[MAX];
    Resource resources[MAX];
    int vcount = 0, rcount = 0;

    while (vcount < MAX &&
           fscanf(vf, " %[^|]|%[^|]|%d|%d|%d|%d\n", victims[vcount].name,
                  victims[vcount].location, &victims[vcount].family_size,
                  &victims[vcount].has_injured, &victims[vcount].has_vulnerable,
                  &victims[vcount].priority) == 6) {
        vcount++;
    }

    while (rcount < MAX &&
           fscanf(rf, " %[^|]|%d\n", resources[rcount].item_name,
                  &resources[rcount].quantity) == 2) {
        rcount++;
    }

    for (int i = 0; i < vcount - 1; i++) {
        for (int j = i + 1; j < vcount; j++) {
            if (victims[j].priority > victims[i].priority) {
                Victim temp = victims[i];
                victims[i] = victims[j];
                victims[j] = temp;
            }
        }
    }

    for (int i = 0; i < vcount; i++) {
        for (int j = 0; j < rcount; j++) {
            int need = victims[i].family_size;
            int give = (resources[j].quantity >= need) ? need : resources[j].quantity;
            if (give > 0) {
                fprintf(log, "%s|%s|%d\n", victims[i].name, resources[j].item_name, give);
                resources[j].quantity -= give;
            }
        }
    }

    fclose(vf);
    fclose(rf);
    fclose(log);
    printf("Resources allocated.\n");
}

int main() {
    int choice;
    while (1) {
        printf("\n--- Offline Disaster Resource Allocator ---\n");
        printf("1. Register Victim\n");
        printf("2. Input Resources\n");
        printf("3. Allocate Resources\n");
        printf("4. Generate Report\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar(); 

        switch (choice) {
            case 1:
                victim_register();
                break;
            case 2:
                supply_input();
                break;
            case 3:
                allocate_resources();
                break;
            case 4:
                generate_report();
                break;
            case 5:
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
}
