#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define SIZE 3

char board[SIZE][SIZE];
int scores[2] = {0, 0};
int last_r, last_c;
bool game_over = false;
int winner = -1; // -1: none, 0: P1, 1: P2, 2: Tie
int moves_count = 0;

// Semaphores
sem_t sem_p1, sem_p2, sem_mod_trigger, sem_mod_done;

void reset_board() {
    for(int i=0; i<SIZE; i++)
        for(int j=0; j<SIZE; j++) board[i][j] = ' ';
    game_over = false;
    winner = -1;
    moves_count = 0;
}

void print_board() {
    printf("\n-------------\n");
    for(int i=0; i<SIZE; i++) {
        printf("| %c | %c | %c |\n", board[i][0], board[i][1], board[i][2]);
        printf("-------------\n");
    }
}

// DFS to check for win from the last played position
int dfs(int r, int c, int dr, int dc, char symbol) {
    if (r < 0 || r >= SIZE || c < 0 || c >= SIZE || board[r][c] != symbol) return 0;
    return 1 + dfs(r + dr, c + dc, dr, dc, symbol);
}

bool check_winner(int r, int c) {
    char s = board[r][c];
    
    // directions = {up, right, diagonal up, diagonal down}
    int directions[4][2] = {{0,1}, {1,0}, {1,1}, {1,-1}};

    for(int i=0; i<4; i++) {
        // check directions and their inverse
        if (dfs(r, c, directions[i][0], directions[i][1], s) + 
            dfs(r, c, -directions[i][0], -directions[i][1], s) - 1 >= 3) return true;
    }
    return false;
}

void* player_thread(void* arg) {
    int id = *(int*)arg;
    char symbol = (id == 0) ? 'X' : 'O';
    sem_t *my_sem = (id == 0) ? &sem_p1 : &sem_p2;
    sem_t *other_sem = (id == 0) ? &sem_p2 : &sem_p1;

    while(true) {
        sem_wait(my_sem);
        if (game_over) continue; // Skip if moderator ended game

        // Find random valid move
        int r, c;
        do {
            r = rand() % SIZE;
            c = rand() % SIZE;
        } while(board[r][c] != ' ');

        board[r][c] = symbol;
        last_r = r; last_c = c;
        moves_count++;
        printf("Player %d (%c) plays: [%d, %d]\n", id+1, symbol, r, c);

        sem_post(&sem_mod_trigger); // Wake moderator
        sem_wait(&sem_mod_done);    // Wait for evaluation

        if (!game_over) {
            sem_post(other_sem); // Hand over turn
        }
    }
    return NULL;
}

void* moderator_thread(void* arg) {
    int next_starter = rand() % 2;

    while(true) {
        // Initialize Round
        reset_board();
        printf("\n=== NEW ROUND STARTING (Player %d starts) ===\n", next_starter + 1);
        if(next_starter == 0) sem_post(&sem_p1); else sem_post(&sem_p2);

        while(!game_over) {
            sem_wait(&sem_mod_trigger);
            
            if(check_winner(last_r, last_c)) {
                winner = (board[last_r][last_c] == 'X') ? 0 : 1;
                game_over = true;
            } else if (moves_count == 9) {
                winner = 2; // Tie
                game_over = true;
            }
            sem_post(&sem_mod_done);
        }

        print_board();
        if(winner == 2) {
            printf("RESULT: It's a Tie!\n");
            next_starter = rand() % 2;
        } else {
            printf("RESULT: Player %d Wins!\n", winner + 1);
            scores[winner]++;
            next_starter = (winner == 0) ? 1 : 0; // Loser starts next
        }
        printf("SCORES: P1: %d | P2: %d\n", scores[0], scores[1]);
        
        sleep(5); // Wait 5 seconds before reset
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_t p1, p2, mod;
    int id1 = 0, id2 = 1;

    sem_init(&sem_p1, 0, 0);
    sem_init(&sem_p2, 0, 0);
    sem_init(&sem_mod_trigger, 0, 0);
    sem_init(&sem_mod_done, 0, 0);

    pthread_create(&mod, NULL, moderator_thread, NULL);
    pthread_create(&p1, NULL, player_thread, &id1);
    pthread_create(&p2, NULL, player_thread, &id2);

    pthread_join(mod, NULL); // Keep main alive
    return 0;
}