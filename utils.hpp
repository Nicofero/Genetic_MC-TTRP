#pragma once  // Prevent multiple inclusion
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <array>
#include <cstdlib>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <limits>
#include <atomic>
#include <chrono>

//Regular text
#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"

// RNG setting
extern std::mt19937 rng;

class Client{
    private:
        int id;
        int type;
        float x;
        float y;        
        std::vector<float> demand;
    public:
        Client(int id, int type, float x, float y, std::vector<float> demand);
        Client();
        int getId();
        int getType();
        float getX();
        float getY();
        std::vector<float> getDemand();
        void setId(int id);
        void setType(int type);
        void setX(float x);
        void setY(float y);
        void setDemand(std::vector<float> demand);
};

class Matrix {
private:
    int rows, cols;
    std::vector<float> data;

public:
    Matrix(int r, int c);
    Matrix();
    float& at(int i, int j);
    void print() const;
};

class Instance {

    private:
        float truck_c;                      // Truck capacity
        float trailer_c;                    // Trailer capacity
        float truck_comp_c;                 // Truck compartment capacity
        float trailer_comp_c;               // Trailer compartment capacity
        int truck_N;                        // Max number of trucks
        int trailer_N;                      // Max number of trailers
        int nClients;                       // Number of clients
        int nLoads;                         // Number of loads
        Client depot;                       // Depot client
        std::vector<Client> clients;        // Client list
        std::vector<float> allDemand;       // All demand vector
    public:
        Instance(std::string filename,int mode = 0);
        Instance() {};
        void print();
        Matrix from_csv(std::string filename);
        Client getDepot();
        std::vector<Client> getVc_clients();
        std::vector<Client> getTc_clients();
        std::vector<Client> getClients(){return clients;};
        int getnClients();
        int getTruck_N();
        int getTrailer_N();
        float getTruck_comp_c();
        float getTrailer_comp_c();
        int getnLoads();
        int gettruck_c();
        int gettrailer_c();
        std::vector<float> getAllDemand();
        int getNCompartmentsTruck ();
        int getNCompartmentsTrailer ();
};

class Solution {
    private:
        std::vector<vector<int>> routes;        
    public:
        Solution();
        void addRoute(std::vector<int> route);
        std::vector<vector<int>> &getRoutes();
        std::vector<int> findElement(int elem);
        void print(bool in_line=false, std::ostream& out = std::cout);
        std::vector<int> toSingleRoute ();                
        int trailers_needed(Instance &probl);   // Gets trailers needed for the solution        
        int trucks_needed();     // Gets trucks needed for the solution
        bool isFeasible(Instance &probl,bool ignore_tn = true);
        std::vector<int> to_1D_array();
        // Public variable
        float cost;
};

void print_vector(const std::vector<int> &veect);