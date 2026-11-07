#include <stdio.h>
#include <string.h>

/* Programmer:Amaan Masood */
int readInt(int min, int max){
    char line[100];
    int num;
    while (1) {
        if (fgets(line, sizeof(line), stdin) == NULL) continue;
        if (sscanf(line, "%d", &num) == 1 && num >= min && num <= max) {
            return num;
        }
        printf("Invalid input, enter a number from %d to %d: ", min, max);
    }
}

void readString(char buf[], int size) {
    if (fgets(buf, size, stdin) != NULL) {
        buf[strcspn(buf, "\n")] = '\0';
    }
}
/* Amaan's code ends */

/* Programmer : Manahil Rehan */
// Structure for habits
struct Habit
{
    int id;
    char name[50];
    int completedToday;
};

// Global variables
struct Habit habits[50];
int nextHabitId = 1;
int habitCount=0;

//fucntion for addding habits
void addHabit(){
    //check whether to do list has reached its limit or not
    if (habitCount == 50)
    {
        printf("Habits list is full!\n");
        return;
    }
    
    char name[50];
    
    //reading the name input using readstring fucntion
    readString(name, 50);

    //checking if the user entered anthing or not
    if (name[0] == '\0')
    {
        printf("Enter a habit!\n");
        return;
    }

    //adding the habits updating the id and completion status
    strcpy(habits[habitCount].name, name);
    habits[habitCount].id = nextHabitId;
    habits[habitCount].completedToday = 0;

    //incrementing the number of habits and next habit's id
    habitCount++;
    nextHabitId++;
}

void viewHabits(){
    if (habitCount == 0)
    {
        printf("No habits yet!\n");
        return;
    }
    for (int i = 0; i < habitCount; i++)
    {
        printf("ID: %d\n", habits[i].id);
        printf("Name: %s\n", habits[i].name);
        if (habits[i].completedToday == 0)
        {
            printf("Habit not completed\n");
        }
        else
        {
            printf("Habit completed\n");
        }
        
    }
    
    
}
/* Manahil's code ends */

//tenporaray main fucntion to call the other fucntions for testing 
int main(){
    //calling a fucntion to add a habit
    printf("Enter a Habit !: \n");
    addHabit();
    printf("\nHabit added successfully!");

    //calling a function to view habits saved
    printf("\n----- Your Habits -----\n");
    viewHabits();
    return 0;
}