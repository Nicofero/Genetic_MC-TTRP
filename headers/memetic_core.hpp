#pragma once  // Prevent multiple inclusion
#include "utils.hpp"

// PMX function for the crossover
std::vector<std::vector<int>> PMX (std::vector<int> &parent1, std::vector<int>& parent2);

// GVR Crossover
Solution GVRX(Solution &parent1, Solution &parent2, Matrix &costMatrix);

// Function to generate a random inversion mutation
void inversionMutation(Solution &route);

// Objective function: Maybe for the subtours check if the next node after a VC is a TC and add that to the subtour instead 
float objectiveFunction (Solution &sol, Instance &probl, Matrix &costMatrix);

// Function to generate a random symmetric matrix
Matrix randomSymMatrix(int size);


/*      LOCAL SEARCH METHODS
* The local search methods will be:
*   - Relocate
*   - 2-OPT   - 
*   - Maybe some method for the subtours
*/

// Relocate atomic function (called by the local search if a better move is found)
void relocate (Solution &sol,int element, int route, int position);

// Forceful relocation. Forces to relocate a route even if it doesnt improve the solution
bool forceful_relocation(Solution &sol,Instance &inst);

// Ultra forceful relocation. Forces to relocate a route even if it doesnt improve the solution
bool ultra_forceful_relocation(Solution &sol,Instance &inst);

// Relocation local search. Returns the route where insertion happened
// We supose that every solution that is going through local search is feasible
int relocationLocal (Solution &sol,Instance &inst, Matrix &cost_matrix);

// 2-OPT atomic function
void opt2 (std::vector<int> &route,int edge1,int edge2);

// 2-OPT Local search
void opt2Local (Solution &sol,int pos,Instance &probl, Matrix &costMatrix);

// Something to change type of route (NOT IMPLEMENTED YET)
void routeOpt ();

// Tournament selection (4 participants)
int tournamentSelection(std::vector<Solution> &candidates,Instance &probl, Matrix &costMatrix);

// Function to generate a random permutation of integers from 1 to size
std::vector<int> generate_random_permutation(int size);

// Function to extract a solution from a predecessor vector
Solution extract_solution (std::vector<int> &predecessor, std::vector<int> &route);

// Function to split a route into a feasible solution
Solution ssplit (std::vector<int> &route, Instance &inst, Matrix &cost_matrix);

// Wrapper for the memetic loop
Solution ssplit (Solution &route, Instance &inst, Matrix &cost_matrix);

// Generates a random population with length nodes and size number of individuals 
std::vector<Solution> randomPopulation (int size, int length, Instance &inst, Matrix &cost_matrix);

// Function to select parents for crossover
void selectParents(int &p1, int &p2, Instance &probl, Matrix &costMatrix, std::vector<Solution> &pop);

// Function to draw a progress bar in the console
void drawProgressBar(const std::string& name, int value, int max_value, int bar_width = 40);

// Function to update the progress bar
void updateProgress(int alpha, int beta, int max_value_alpha, int max_value_beta);

// Memetic loop function
Solution memeticLoop(int size, Instance &probl, Matrix &costMatrix, int maxiter=10000,float pc=0.8, float pm=0.4, float pls=0.3, bool verbose=false);