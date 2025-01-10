//pc3656
//Doodlebugs and ants simulation.

#include <iostream>
#include <vector>
#include <time.h>
using namespace std;

//Prototype for a random number generator.
int generateRandom(int min, int max);

//Prototypes for the different classes.
class Simulation;
class Organism;
class Doodlebug;
class Ant;

//Chars to identify the different critters.
const char DOODLEBUG('X'), ANT('o');

//Int for movements
const int LEFT(0), RIGHT(1), UP(2), DOWN(3);

//Size of the World.
const int WORLD_SIZE_X(20), WORLD_SIZE_Y(20);

//Amount of critters.
const int AMOUNT_DOODLEBUGS(5), AMOUNT_ANTS(100);

//Breed and starve time for each species.
const int BREED_DOODLEBUG(8), STARVE_DOODLEBUG(3), BREED_ANTS(3);

//The engine of the simulation.
class Simulation {
    //Friends classes of the simulation
    friend class Organism;
    friend class Doodlebug;
    friend class Ant;
    
    private:
        //2D array of the size of the world.
        Organism* world[WORLD_SIZE_X][WORLD_SIZE_Y];
        
        //List to keep track of the different organisms.
        vector<Doodlebug*> doodlebugList;
        vector<Ant*> antList;
        int frameCount;
    
    public:
        //Constructor
        Simulation();
        ~Simulation();
        //The three functions that run the simulation.
        void setup();
        void forward();
        void display();
};

//Abstract class: Organism.
class Organism {
    private:
        vector<int> validMoves(int x, int y) const;
        int x, y;
    public:
        Simulation* currentSimulation;
        //Constructor
        Organism(Simulation* currentSimulation, int x, int y) : currentSimulation(currentSimulation), x(x), y(y), breedCount(0), didMove(false) {}
        
        //Make these variables private later and write accessors and setters for them.
        
        int breedCount;
        bool didMove;
        int getX() const {return x;}
        int getY() const {return y;}
        void setX(int n) {x = n;}
        void setY(int n) {y = n;}
        virtual void move();
        virtual void setDidMove() {didMove = false;}
        virtual void breed(int breedTime);
        //Pure virtual function        
        virtual char returnType() = 0;
};

//Doodlebug class based on the Organism class.
class Doodlebug: public Organism {
    private:
        int starveCount;
        vector<int> nearbyAnts(int x, int y) const;
    public:
        Doodlebug(Simulation* currentSimulation, int x, int y) : Organism(currentSimulation, x, y), starveCount(0) {}
        void chase();
        char returnType() {return DOODLEBUG; }

};

//Ant class based on the Organism class.
class Ant: public Organism {
    private:
    public:
        Ant(Simulation* currentSimulation, int x, int y) : Organism(currentSimulation, x, y) {}
        char returnType() {return ANT; }
        
};

//SIMULATION CLASS FUNCTIONS
//Constructor for the simulation class.
Simulation::Simulation() {
    frameCount = 0;
    for(int x = 0; x < WORLD_SIZE_X; x++) {
        for(int y = 0; y < WORLD_SIZE_Y; y++) {
            world[x][y] = nullptr;
        }
    }
}

//Initialization of the simulation.
void Simulation::setup() {
    //Keep track of the amount of each types of critters.
    int doodlebugs(0), ants(0);
    //Populate with doodlebugs.
    while(doodlebugs < AMOUNT_DOODLEBUGS) {
        int x = generateRandom(0, WORLD_SIZE_X - 1);
        int y = generateRandom(0, WORLD_SIZE_Y - 1);
        if(world[x][y] == nullptr) {
            Doodlebug* babyDoodlebug = new Doodlebug(this, x, y);
            world[x][y] = babyDoodlebug;
            doodlebugList.push_back(babyDoodlebug);
            doodlebugs ++;
        }
    }
    //Populate with ants.
    while(ants < AMOUNT_ANTS) {
        int x = generateRandom(0, WORLD_SIZE_X - 1);
        int y = generateRandom(0, WORLD_SIZE_Y - 1);
        if(world[x][y] == nullptr) {
            Ant* babyAnt = new Ant(this, x, y);
            world[x][y] = babyAnt;
            antList.push_back(babyAnt);
            ants ++;
        }
    }
}

//Function to display the world.
void Simulation::display() {
    //Clear terminal.
    cout << "\033[2J\033[H";
    cout.flush();
    //Iterate over the world and display the right symbol according to the critter present at each position.
    for(int y = 0; y < WORLD_SIZE_Y; y++) {
        for (int x = 0; x < WORLD_SIZE_X; x++) {
            if(world[x][y] == nullptr) {
                cout << '-';
            }
            else if (world[x][y] -> returnType() == ANT) {
                cout << 'x';
            }
            else {
                cout << 'O';    
            } 
        }
        cout << endl;
    }
}

//Function to take a time step forward.
void Simulation::forward() {
    //Reset the didMove flag for everybody
    for(int i = 0; i < doodlebugList.size(); i++) {
        doodlebugList[i]->setDidMove();
    }
    for(int i = 0; i < antList.size(); i++) {
        antList[i]->setDidMove();
    }
    //The doodlebugs chase their preys
    for(int i = 0; i < doodlebugList.size(); i++) {
        doodlebugList[i]->chase();
    }
    //Anyone who did not move yet moves
    for(int i = 0; i < doodlebugList.size(); i++) {
        doodlebugList[i]->breed(BREED_DOODLEBUG);
        doodlebugList[i]->move();
    }
    for(int i = 0; i < antList.size(); i++) {
        antList[i]->breed(BREED_ANTS);
        antList[i]->move();
    } 
}

//Destructor for the simulation at the end of the program.
Simulation::~Simulation() {
    for (auto doodlebug : doodlebugList) {
        delete doodlebug;
    }
    for (auto ant : antList) {
        delete ant;
    }
}


//ORGANISM FUNCTIONS
//Function that returns a vector of valid moves.
vector<int> Organism::validMoves(int x, int y) const{
    vector<int> moves;
    //left
    if(x - 1 > 0 && currentSimulation -> world[x-1][y] == nullptr) {
        moves.push_back(LEFT);
    }
    //right
    if(x + 1 < WORLD_SIZE_X && currentSimulation -> world[x+1][y] == nullptr) {
        moves.push_back(RIGHT);
    }
    //up
    if(y - 1 > 0 && currentSimulation -> world[x][y-1] == nullptr) {
        moves.push_back(UP);
    }
    //down
    if(y + 1 < WORLD_SIZE_Y && currentSimulation -> world[x][y+1] == nullptr) {
        moves.push_back(DOWN);
    }
    return moves;
}

//Function that move the organism to a valid location.
void Organism::move() {
    vector<int> moves = validMoves(x, y);
    if(moves.size() > 0 && didMove == false) {
        int direction = moves[generateRandom(0, moves.size() - 1)];
        int newX(x), newY(y);
        switch (direction)
        {
        case LEFT:
            newX --;
            break;

        case RIGHT:
            newX ++;
            break;

        case UP:
            newY --;
            break;

        case DOWN:
            newY ++;
            break;
        
        default:
            break;
        }
        currentSimulation -> world[newX][newY] = this;
        currentSimulation -> world[x][y] = nullptr;
        x = newX;
        y = newY;
        didMove = true;
    }
}

void Organism::breed(int breedTime) {
    //When it is time to make a baby
    if (breedCount >= breedTime) {
        vector<int> moves = validMoves(x, y);

        //If there is space to make a baby
        if (moves.size() > 0) {
            //Pick a direction from the vector of possible directions.
            int direction = moves[generateRandom(0, moves.size() - 1)];
            int newX = x, newY = y;
            switch (direction) {
                case LEFT:  
                    newX--; 
                    break;
                case RIGHT: 
                    newX++; 
                    break;
                case UP:    
                    newY--; 
                    break;
                case DOWN:  
                    newY++; 
                    break;
            }

            //Create the new organism based on the type.
            if (returnType() == DOODLEBUG) {
                Doodlebug* baby = new Doodlebug(currentSimulation, newX, newY);
                currentSimulation->world[newX][newY] = baby;
                currentSimulation->doodlebugList.push_back(baby);
            } else if (returnType() == ANT) {
                Ant* baby = new Ant(currentSimulation, newX, newY);
                currentSimulation->world[newX][newY] = baby;
                currentSimulation->antList.push_back(baby);
            }
            //Reset the breedCount
            breedCount = 0;
        }
    } 
    //Otherwise increment the breedCount.
    else breedCount++;
}


//DOODLEBUG FUNCTION
//Function that returns a vector of nearby ants.
vector<int> Doodlebug::nearbyAnts(int x, int y) const {
    vector<int>ants;
    //left
    if(x - 1 > 0 && currentSimulation -> world[x-1][y] != nullptr) {
        if(currentSimulation -> world[x-1][y]->returnType() == ANT) {
            ants.push_back(LEFT);
        }
    }
    //right
    if(x + 1 < WORLD_SIZE_X && currentSimulation -> world[x+1][y] != nullptr) {
        if(currentSimulation -> world[x+1][y]->returnType() == ANT) {
            ants.push_back(RIGHT);
        }
    }
    //up
    if(y - 1 > 0 && currentSimulation -> world[x][y-1] != nullptr) {
        if(currentSimulation -> world[x][y-1]->returnType() == ANT) {
            ants.push_back(UP);
        }
    }
    //down
    if(y + 1 < WORLD_SIZE_Y && currentSimulation -> world[x][y+1] != nullptr) {
        if(currentSimulation -> world[x][y+1]->returnType() == ANT) {
            ants.push_back(DOWN);
        }
    }
    return ants;
} 

//Function that moves a doodlebug to a nearby ant and eats it.
void Doodlebug::chase() {
    vector<int>ants = nearbyAnts(getX(), getY());
    if(ants.size() > 0 && didMove == false) {
        
        int direction = ants[generateRandom(0, ants.size() - 1)];
        int newX(getX()), newY(getY());
        
        switch (direction)
        {
        case LEFT:
            newX --;
            if(newX < 0) {
                newX = 0;
            }
            break;

        case RIGHT:
            newX ++;
            if(newX > WORLD_SIZE_X) {
                newX = WORLD_SIZE_X;
            }
            break;

        case UP:
            newY --;
            if(newY < 0) {
                newY = 0;
            }
            break;

        case DOWN:
            newY ++;
            if(newY > WORLD_SIZE_Y) {
                newY = WORLD_SIZE_Y;
            }
            break;
        
        default:
            break;
        }

        //Find an delete the ant in the list of ants.
        Ant* preyAnt = dynamic_cast<Ant*>(currentSimulation->world[newX][newY]);

        if (preyAnt != nullptr) {
            for (size_t i = 0; i < currentSimulation->antList.size(); ++i) {
                if (currentSimulation->antList[i] == preyAnt) {
                    //Remove it from the antList.
                    currentSimulation->antList.erase(currentSimulation->antList.begin() + i);
                    break;
                }
            }
            //Delete the ant
            delete preyAnt;
        }

        //delete currentSimulation->world[newX][newY];
        currentSimulation->world[newX][newY] = this;
        currentSimulation->world[getX()][getY()] = nullptr;
        setX(newX);
        setY(newY);
        didMove = true;
    }

    else {
        starveCount ++;
    }

    //Starvation
    if (starveCount >= STARVE_DOODLEBUG) {
        //Remove doodlebug from the world and the doodlebug list.
        currentSimulation->world[getX()][getY()] = nullptr;
        for (size_t i = 0; i < currentSimulation->doodlebugList.size(); ++i) {
            if (currentSimulation->doodlebugList[i] == this) {
                currentSimulation->doodlebugList.erase(currentSimulation->doodlebugList.begin() + i);
                break;
            }
        }
        //Free the memory
        delete this;
    }
}

//Main function.
int main () {
    char input('f');
    srand(time(0));
    Simulation s;
    s.setup();
    s.display();

    while(true) {
        input = cin.get();
        if(input == '\n') {
            s.forward();
            s.display();
        }
        else {
            break;
        }
        
    }

    return 0;
}

//Function to generate random number.
int generateRandom(int min, int max) {
    return rand() % (max - min + 1) + min;
}