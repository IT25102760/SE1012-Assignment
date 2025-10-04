/*
 * SE1012 Assignment - Part 3: Multi-Player Tic-Tac-Toe
 * Name: [IT25102760]
 * Date: [03.10.2025]
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

// Player structure to manage player information
typedef struct {
    char symbol;         // Player's symbol (X, O, or Z)
    bool isComputer;     // true if computer, false if human
    char name[30];       // Player's name for display
} Player;

// Function prototypes
char** initializeBoard(int n);
void displayBoard(char** board, int n);
void freeBoard(char** board, int n);
void setupPlayers(Player* players);
bool getPlayerMove(char** board, int n, Player player);
bool getComputerMove(char** board, int n, Player player);
bool validateMove(char** board, int n, int row, int col);
bool checkWin(char** board, int n, char symbol);
bool checkDraw(char** board, int n);
void logGameState(char** board, int n, Player player, int moveNum, const char* filename);
void clearLogFile(const char* filename);
void displayPlayerInfo(Player* players);


int main() {
    int n;
    char** board;
    Player players[3];  // Array to hold three players
    int currentPlayerIndex = 0;
    int moveCount = 0;
    bool gameWon = false;
    bool gameDraw = false;
    
    // Seed random number generator for computer moves
    srand(time(NULL));
    
    // Display welcome message
    printf("   TIC-TAC-TOE     \n");
    
    // Setup the three players
    setupPlayers(players);
    
    // Display player configuration
    displayPlayerInfo(players);
    
    // Getting Board size from user
    printf("\nEnter board size (3-10): ");
    
    //scanning for correct input
    while (scanf("%d", &n) != 1 || n < 3 || n > 10) {
        printf("Invalid input! Please enter a number between 3 and 10: ");
        while (getchar() != '\n'); // Clear input buffer
    }
    
    // Initialize the game board
    board = initializeBoard(n);
    
    // Clear previous log file
    clearLogFile("game_log.txt");
    
    printf("\nGame started! %s goes first.\n", players[0].name);
    printf("Enter moves as: row column (both starting from 1)\n\n");
    
    // Main game loop - three players take turns in cyclic order
    while (!gameWon && !gameDraw) {
        Player currentPlayer = players[currentPlayerIndex];
        
        // Display current board
        displayBoard(board, n);
        
        // Show whose turn it is
        printf("\n%s's turn (Symbol: %c):\n", currentPlayer.name, currentPlayer.symbol);
        
        bool moveMade = false;
        
        // Get move based on player type (human or computer)
        if (currentPlayer.isComputer) {
            moveMade = getComputerMove(board, n, currentPlayer);
        } else {
            moveMade = getPlayerMove(board, n, currentPlayer);
        }
        
        if (moveMade) {
            moveCount++;
            
            // Log the game state to file
            logGameState(board, n, currentPlayer, moveCount, "game_log.txt");
            
            // Check if current player won
            if (checkWin(board, n, currentPlayer.symbol)) {
                displayBoard(board, n);
                printf("\n %s WINS! \n", currentPlayer.name);
                gameWon = true;
            }
            // Check for draw
            else if (checkDraw(board, n)) {
                displayBoard(board, n);
                printf("\n GAME DRAW! \n");
                gameDraw = true;
            }
            else {
                // Move to next player in cyclic order (0 -> 1 -> 2 -> 0)
                currentPlayerIndex = (currentPlayerIndex + 1) % 3;
            }
        }
    }
    
    // Clean up memory
    freeBoard(board, n);
    
    printf("\nThank you for playing!\n");
    printf("Game log saved to 'game_log.txt'\n");
    return 0;
}

/*
 * Setup the three players
 */
void setupPlayers(Player* players) {
    int computerCount;
    char symbols[] = {'X', 'O', 'Z'};
    
    printf("Configure Three Players:\n");
    printf("How many computer players? (0, 1, or 2): ");
    
    // Validating input  must have at least one user
    while (scanf("%d", &computerCount) != 1 || computerCount < 0 || computerCount > 2) {
        printf("Invalid! Please enter 0, 1, or 2: ");
        while (getchar() != '\n');
    }
    
    printf("\n");
    
    // Setup all three players
    for (int i = 0; i < 3; i++) {
        players[i].symbol = symbols[i];
        
        if (i < (3 - computerCount)) {
	       // User	player
            sprintf(players[i].name, "Player %d", i + 1);
            players[i].isComputer = false;
            printf("%s will be USER (Symbol: %c)\n", players[i].name, players[i].symbol);
        } else {
            // Computer player
            int compNum = i - (3 - computerCount) + 1;
            sprintf(players[i].name, "Computer %d", compNum);
            players[i].isComputer = true;
            printf("%s will be COMPUTER (Symbol: %c)\n", players[i].name, players[i].symbol);
        }
    }
}

/*
 * Display player information in a formatted way
 */
void displayPlayerInfo(Player* players) {
	printf("\n PLAYERS LIST \n");
    for (int i = 0; i < 3; i++) {
        printf("  %s (Symbol: %c)  %s\n", 
               players[i].name, 
               players[i].symbol,
               players[i].isComputer ? "Computer" : "User");
    }
}

/*
 * Initialize the game board with empty cells
 * Returns: 2D array (dynamically allocated)
 */
char** initializeBoard(int n) {
    char** board = (char**)malloc(n * sizeof(char*));
    
    for (int i = 0; i < n; i++) {
        board[i] = (char*)malloc(n * sizeof(char));
        for (int j = 0; j < n; j++) {
            board[i][j] = ' '; // Empty cell
        }
    }
    
    return board;
}

/*
 * Display the current state of the board
 */
void displayBoard(char** board, int n) {
    printf("\n");
    
    // Print column numbers
    printf("   ");
    for (int i = 1; i <= n; i++) {
        printf(" %2d ", i);
    }
    printf("\n");
    
    // Print top border
    printf("   ");
    for (int i = 0; i < n; i++) {
        printf("----");
    }
    printf("-\n");
    
    // Print each row
    for (int i = 0; i < n; i++) {
        printf("%2d ", i + 1); // Row number
        
        for (int j = 0; j < n; j++) {
            printf("| %c ", board[i][j]);
        }
        printf("|\n");
        
        // Print row separator
        printf("   ");
        for (int j = 0; j < n; j++) {
            printf("----");
        }
        printf("-\n");
    }
}

/*
 * Free the dynamically allocated board memory
 */
void freeBoard(char** board, int n) {
    for (int i = 0; i < n; i++) {
        free(board[i]);
    }
    free(board);
}

/*
 * Get and validate human player move
 * Returns: true if move was successful, false otherwise
 */
bool getPlayerMove(char** board, int n, Player player) {
    int row, col;
    
    printf("Enter row and column (1-%d): ", n);
    
    // Read input
    if (scanf("%d %d", &row, &col) != 2) {
        printf("Invalid input! Please enter two numbers.\n");
        while (getchar() != '\n'); // Clear input buffer
        return false;
    }
    
    // Convert to 0-indexed
    row--;
    col--;
    
    // Validate move
    if (validateMove(board, n, row, col)) {
        board[row][col] = player.symbol;
        printf("%s placed '%c' at (%d, %d)\n", 
               player.name, player.symbol, row + 1, col + 1);
        return true;
    } else {
        printf("Invalid move! Try again.\n");
        return false;
    }
}

/*
 * Generate and execute a random computer move
 * Returns: true if move was successful
 */
bool getComputerMove(char** board, int n, Player player) {
    int row, col;
    int attempts = 0;
    int maxAttempts = n * n * 2; // Safety limit
    
    printf("%s is thinking...\n", player.name);
    
    // Find a random empty cell
    do {
        row = rand() % n;  // Random row
        col = rand() % n;  // Random column
        attempts++;
        
        if (attempts > maxAttempts) {
            printf("Error: Could not find valid move.\n");
            return false;
        }
        
    } while (board[row][col] != ' '); // Keep trying until empty cell found
    
    // Make the move
    board[row][col] = player.symbol;
    printf("%s placed '%c' at position (%d, %d)\n", 
           player.name, player.symbol, row + 1, col + 1);
    
    return true;
}

/*
 * Validate if a move is legal
 * Returns: true if move is valid, false otherwise
 */
bool validateMove(char** board, int n, int row, int col) {
    // Check if position is within bounds
    if (row < 0 || row >= n || col < 0 || col >= n) {
        printf("Position out of bounds!\n");
        return false;
    }
    
    // Check if cell is already occupied
    if (board[row][col] != ' ') {
        printf("Cell already occupied!\n");
        return false;
    }
    
    return true;
}

/*
 * Check if a player has won
 * Works for three-player game
 * Returns: true if player has won, false otherwise
 */
bool checkWin(char** board, int n, char symbol) {
    // Check all rows
    for (int i = 0; i < n; i++) {
        bool rowWin = true;
        for (int j = 0; j < n; j++) {
            if (board[i][j] != symbol) {
                rowWin = false;
                break;
            }
        }
        if (rowWin) return true;
    }
    
    // Check all columns
    for (int j = 0; j < n; j++) {
        bool colWin = true;
        for (int i = 0; i < n; i++) {
            if (board[i][j] != symbol) {
                colWin = false;
                break;
            }
        }
        if (colWin) return true;
    }
    
    // Check main diagonal (top-left to bottom-right)
    bool diagWin = true;
    for (int i = 0; i < n; i++) {
        if (board[i][i] != symbol) {
            diagWin = false;
            break;
        }
    }
    if (diagWin) return true;
    
    // Check anti-diagonal (top-right to bottom-left)
    diagWin = true;
    for (int i = 0; i < n; i++) {
        if (board[i][n - 1 - i] != symbol) {
            diagWin = false;
            break;
        }
    }
    if (diagWin) return true;
    
    return false;
}

/*
 * Check if the game is a draw (board full with no winner)
 * Returns: true if draw, false otherwise
 */
bool checkDraw(char** board, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j] == ' ') {
                return false; // Found empty cell
            }
        }
    }
    return true; // Board is completely full
}

/*
 * Log the current game state to a file
 * Records each player's move for review
 */
void logGameState(char** board, int n, Player player, int moveNum, const char* filename) {
    FILE* file = fopen(filename, "a");
    
    if (file == NULL) {
        printf("Warning: Could not open log file.\n");
        return;
    }
    
    // Write move information
    fprintf(file, " Move %d: %s (%c) \n", moveNum, player.name, player.symbol);
    
    // Write board state
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fprintf(file, "| %c ", board[i][j]);
        }
        fprintf(file, "|\n");
    }
    fprintf(file, "\n");
    
    fclose(file);
}

/*
 * Clear the log file at the start of a new game
 */
void clearLogFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file != NULL) {
        fprintf(file, " NEW GAME: TIC-TAC-TOE \n");
        fclose(file);
    }
}
