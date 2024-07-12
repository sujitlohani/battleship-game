#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <thread>

using namespace std;

// Function to clear the screen
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Function to pause for a few seconds
void pause(int seconds) {
    this_thread::sleep_for(chrono::seconds(seconds));
}

// Ship class definition
class Ship {
private:
    int size;
    vector<pair<int, int>> coordinates;
    int hits;

public:
    Ship(int size) : size(size), hits(0) {}

    void placeShip(const vector<pair<int, int>>& coords) {
        coordinates = coords;
    }

    bool checkHit(int x, int y) {
        for (auto& coord : coordinates) {
            if (coord.first == x && coord.second == y) {
                hits++;
                return true;
            }
        }
        return false;
    }

    bool isSunk() const {
        return hits >= size;
    }

    int getSize() const {
        return size;
    }
};

// Board class definition
class Board {
private:
    vector<vector<char>> grid;
    int size;

public:
    Board(int size) : size(size) {
        grid.resize(size, vector<char>(size, '~'));
    }

    bool placeShip(Ship& ship, const vector<pair<int, int>>& coords) {
        for (const auto& coord : coords) {
            if (coord.first >= size || coord.second >= size || grid[coord.first][coord.second] != '~') {
                return false;
            }
        }
        ship.placeShip(coords);
        for (const auto& coord : coords) {
            grid[coord.first][coord.second] = 'S';
        }
        return true;
    }

    bool checkHit(int x, int y) {
        if (grid[x][y] == 'S') {
            grid[x][y] = 'H';
            return true;
        } else if (grid[x][y] == '~') {
            grid[x][y] = 'M';
        }
        return false;
    }

    void display() const {
        for (const auto& row : grid) {
            for (const auto& cell : row) {
                cout << cell << ' ';
            }
            cout << endl;
        }
    }

    int getSize() const {
        return size;
    }
};

// Player class definition
class Player {
private:
    string name;
    Board ownBoard;
    Board opponentBoard;

public:
    Player(const string& name, int boardSize)
        : name(name), ownBoard(boardSize), opponentBoard(boardSize) {}

    string getName() const {
        return name;
    }

    void placeShips(vector<Ship>& ships) {
        for (auto& ship : ships) {
            bool placed = false;
            while (!placed) {
                vector<pair<int, int>> coords;
                int x = rand() % ownBoard.getSize();
                int y = rand() % ownBoard.getSize();
                bool horizontal = rand() % 2;

                if (horizontal) {
                    if (x + ship.getSize() > ownBoard.getSize()) continue;
                    for (int i = 0; i < ship.getSize(); ++i) {
                        coords.push_back({x + i, y});
                    }
                } else {
                    if (y + ship.getSize() > ownBoard.getSize()) continue;
                    for (int i = 0; i < ship.getSize(); ++i) {
                        coords.push_back({x, y + i});
                    }
                }

                placed = ownBoard.placeShip(ship, coords);
            }
        }
    }

    bool takeShot(Player& opponent, int x, int y) {
        return opponent.ownBoard.checkHit(x, y);
    }

    bool hasLost(const vector<Ship>& ships) const {
        for (const auto& ship : ships) {
            if (!ship.isSunk()) {
                return false;
            }
        }
        return true;
    }

    void displayBoards() const {
        cout << name << "'s Board:\n";
        ownBoard.display();
        cout << name << "'s View of Opponent's Board:\n";
        opponentBoard.display();
    }

    void markOpponentBoard(int x, int y, bool hit) {
        if (hit) {
            opponentBoard.checkHit(x, y); // Updates the display board with 'H'
        } else {
            opponentBoard.checkHit(x, y); // Updates the display board with 'M'
        }
    }
};

// Game class definition
class Game {
private:
    Player player1;
    Player player2;
    vector<Ship> ships1;
    vector<Ship> ships2;

public:
    Game(const string& name1, const string& name2, int boardSize)
        : player1(name1, boardSize), player2(name2, boardSize) {
        ships1.push_back(Ship(5)); // Carrier
        ships1.push_back(Ship(4)); // Battleship
        ships1.push_back(Ship(3)); // Cruiser
        ships1.push_back(Ship(3)); // Submarine
        ships1.push_back(Ship(2)); // Destroyer

        ships2.push_back(Ship(5)); // Carrier
        ships2.push_back(Ship(4)); // Battleship
        ships2.push_back(Ship(3)); // Cruiser
        ships2.push_back(Ship(3)); // Submarine
        ships2.push_back(Ship(2)); // Destroyer
    }

    void initialize() {
        cout << "Placing ships for " << player1.getName() << "...\n";
        player1.placeShips(ships1);
        cout << "Placing ships for " << player2.getName() << "...\n";
        player2.placeShips(ships2);
    }

    void start() {
        bool gameOver = false;
        while (!gameOver) {
            clearScreen();
            player1.displayBoards();
            int x, y;
            cout << player1.getName() << "'s turn. Enter coordinates to shoot (x y): ";
            cin >> x >> y;
            bool hit = player1.takeShot(player2, x, y);
            if (hit) {
                cout << "Hit!\n";
                player1.markOpponentBoard(x, y, true);
            } else {
                cout << "Miss.\n";
                player1.markOpponentBoard(x, y, false);
            }
            pause(2); 
            gameOver = player2.hasLost(ships2);
            if (gameOver) {
                clearScreen();
                cout << player1.getName() << " wins!\n";
                break;
            }

            clearScreen();
            player2.displayBoards();
            cout << player2.getName() << "'s turn. Enter coordinates to shoot (x y): ";
            cin >> x >> y;
            hit = player2.takeShot(player1, x, y);
            if (hit) {
                cout << "Hit!\n";
                player2.markOpponentBoard(x, y, true);
            } else {
                cout << "Miss.\n";
                player2.markOpponentBoard(x, y, false);
            }
            pause(2); // Pause for 2 seconds to show hit/miss message
            gameOver = player1.hasLost(ships1);
            if (gameOver) {
                clearScreen();
                cout << player2.getName() << " wins!\n";
                break;
            }
        }
    }
};

// Main function
int main() {
    srand(time(0));
    string player1Name, player2Name;
    cout << "Enter name for Player 1: ";
    cin >> player1Name;
    cout << "Enter name for Player 2: ";
    cin >> player2Name;

    Game game(player1Name, player2Name, 10); // Assuming a 10x10 board
    game.initialize();
    game.start();

    return 0;
}
