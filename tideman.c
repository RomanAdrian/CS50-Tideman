#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool cycle_detection(int start, int end);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i; // the voter's ith preference
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++; // updates preferences - one array of preferences over the other array of preferences
        }
    }

    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    int x = 0;

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (preferences[i][j] > 0 && preferences[i][j] > preferences[j][i]) // compares preferences
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++; // populate pair count
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int a, b, swapped;
    pair temp;

    for (int i = 0; i < pair_count - 1; i++)
    {
        swapped = i;

        for (int j = i + 1; j < pair_count; j++)
        {
            a = preferences[pairs[swapped].winner][pairs[swapped].loser];
            b = preferences[pairs[j].winner][pairs[j].loser];
            if (b > a)
            {
                swapped = j;
            }
        }

        if (swapped != i)
        {
            temp = pairs[i];
            pairs[i] = pairs[swapped];
            pairs[swapped] = temp;
        }
    }

    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{

    for (int i = 0; i < pair_count; i++) // get the pairs with the highest strenght of victory starting at pairs[0]
    {
        {
            if (!cycle_detection(pairs[i].winner, pairs[i].loser)) // if cycle_detection returns false ( no cycle will be created using cycle_detection())
            {
                locked[pairs[i].winner][pairs[i].loser] = true; // set locked [.winner][.loser] to true, drawing a line
            }
        }
    }
    return;
}
bool cycle_detection(int start, int end) // int start = pairs[i].winner & int end pairs[i].loser
{
    // base condition for recursion to check if() a backedge is found for the current ending node & starting node

    if (locked[end][start] == true) // if it finds an edge between the starting node (candidate) and the ending node (candidate)
    {
        return true; // indicates there exists an edge
    }

    //recursive case that will track all visited nodes to check for backedge between any 2 nodes in the graph

    for (int i = 0; i < candidate_count; i++)
    {
        if ((locked[i][start]) == true) // if a backedge is found between the starting node (candidate at [i]) and the ending node
        {
            // check the cycle_detection() function then calls itself, it will pass the previous "end node" from the line above as the new "start node", with "Locked(i)[]" as the new "start" parameter

            if (cycle_detection(i, end) == true)
            {
                return true;
            }
        }
    }
    return false; // false if no edges found between 2 nodes
}

// Print the winner of the election
void print_winner(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        bool winner = false;

        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[i][j] == false) // check next candidate [i]
            {
                winner = true;
            }
            if (locked[i][j] == winner || locked[j][i] == winner)
            {
                printf("%s\n", candidates[i]);
            }
        }
    }
    return;
}