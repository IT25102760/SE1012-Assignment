/*
 * SE1012 Assignment - Part 2: User vs Computer Tic-Tac-Toe
 * Name: [IT25102760]
 * Date: [02.10.2025]
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Function prototypes
char** initializeBoard(int n);
void displayBoard(char** board, int n);
void freeBoard(char** board, int n);
bool getPlayerMove(char** board, int n, char player);
bool getComputerMove(char** board, int n, char player);
bool validateMove(char** board, int n, int row, int col);
bool checkWin(char** board, int n, char player);
bool checkDraw(char** board, int n);
void logGameState(char** board, int n, char player, int moveNum, const char* filename);
void clearLogFile(const char* filename);

int main() {
    int n;
    char** board;
    char currentPlayer = 'X';
    int moveCount = 0;
    bool gameWon = false;
    bool gameDraw = false;
    
    // Seed random number generator for computer moves
    srand(time(NULL));
    
    // Display welcome message
    printf("    TIC-TAC-TOE     \n");
    
    printf("You are Player X\n");
    printf("Computer is Player O\n\n");
    
    // getting Board size from user
    printf("Enter board size (3-10): "); //ex: if you press 3 you can get 3x3
    
    //scanning for correct input
    while (scanf("%d", &n) != 1 || n < 3 || n > 10) {
        printf("Invalid input! Please enter a number between 3 and 10: ");
        while (getchar() != '\n'); // Clear input buffer
    }
    
    // Initialize the game board
    board = initializeBoard(n);
    
    // Clear previous log file
    clearLogFile("game_log.txt");
    
    printf("\nGame started! You (X) go first.\n");
    printf("Enter moves as: row column (both starting from 1)\n\n");
    
    // Main game loop
    while (!gameWon && !gameDraw) {
        // Display current board
        displayBoard(board, n);
        
        bool moveMade = false;
        
        // Player X (USER) turn
        if (currentPlayer == 'X') {
            printf("\nYour turn (X):\n");
            moveMade = getPlayerMove(board, n, currentPlayer);
        }
        // Player O (Computer) turn
        else {
            printf("\nComputer's turn (O):\n");
            moveMade = getComputerMove(board, n, currentPlayer);
        }
        
        if (moveMade) {
            moveCount++;
            
            // Log the game state to file
            logGameState(board, n, currentPlayer, moveCount, "game_log.txt");
            
            // Check for win
            if (checkWin(board, n, currentPlayer)) {
                displayBoard(board, n);
                if (currentPlayer == 'X') {
                    printf("\n CONGRATULATIONS! YOU WIN! \n");
                } else {
                    printf("\n COMPUTER WINS! BETTER LUCK NEXT TIME! \n");
                }
                gameWon = true;
            }
            // Check for draw
            else if (checkDraw(board, n)) {
                displayBoard(board, n);
                printf("\n GAME DRAW! \n");
                gameDraw = true;
            }
            else {
                // Switch player
                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
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
bool getPlayerMove(char** board, int n, char player) {
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
        board[row][col] = player;
        return true;
    } else {
        printf("Invalid move! Try again.\n");
        return false;
    }
}

/*
 * Generate and execute a random computer move
 * This function makes the computer select a random valid position
 * Returns: true if move was successful
 */
bool getComputerMove(char** board, int n, char player) {
    int row, col;
    int attempts = 0;
    int maxAttempts = n * n * 2; // Safety limit to prevent infinite loop
    
    printf("Computer is thinking...\n");
    
    // Generate random positions until we find an empty cell
    do {
        row = rand() % n;  // Random row (0 to n-1)
        col = rand() % n;  // Random column (0 to n-1)
        attempts++;
        
        // Safety check - should never happen if checkDraw works correctly
        if (attempts > maxAttempts) {
            printf("Error: Could not find valid move.\n");
            return false;
        }
        
    } while (board[row][col] != ' '); // Keep trying until we find empty cell
    
    // Make the move
    board[row][col] = player;
    printf("Computer placed '%c' at position (%d, %d)\n", 
           player, row + 1, col + 1);
    
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
 * Check if the current player has won
 * Returns: true if player has won, false otherwise
 */
bool checkWin(char** board, int n, char player) {
    // Check all rows
    for (int i = 0; i < n; i++) {
        bool rowWin = true;
        for (int j = 0; j < n; j++) {
            if (board[i][j] != player) {
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
            if (board[i][j] != player) {
                colWin = false;
                break;
            }
        }
        if (colWin) return true;
    }
    
    // Check main diagonal (top-left to bottom-right)
    bool diagWin = true;
    for (int i = 0; i < n; i++) {
        if (board[i][i] != player) {
            diagWin = false;
            break;
        }
    }
    if (diagWin) return true;
    
    // Check anti-diagonal (top-right to bottom-left)
    diagWin = true;
    for (int i = 0; i < n; i++) {
        if (board[i][n - 1 - i] != player) {
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
                return false; // Found empty cell, game continues
            }
        }
    }
    return true; // Board is completely full
}

/*
 * Log the current game state to a file
 * Appends each move to the log file for review
 */
void logGameState(char** board, int n, char player, int moveNum, const char* filename) {
    FILE* file = fopen(filename, "a");
    
    if (file == NULL) {
        printf("Warning: Could not open log file.\n");
        return;
    }
    
    // Write move information
    if (player == 'X') {
        fprintf(file, " Move %d: Player (X) \n", moveNum);
    } else {
        fprintf(file, " Move %d: Computer (O) \n", moveNum);
    }
    
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
        fprintf(file, " NEW GAME: USER vs COMPUTER \n");
        fprintf(file, "Player X: Human\n");
        fprintf(file, "Player O: Computer\n\n");
        fclose(file);
    }
}
