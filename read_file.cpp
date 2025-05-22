#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <array>
#include <cstdlib>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <random>
using namespace std;


// RNG setting
std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));

class Client{
    private:
        int id;
        int type;
        float x;
        float y;        
        std::vector<float> demand;
    public:
        Client(int id, int type, float x, float y, std::vector<float> demand) 
            : id(id), type(type), x(x), y(y), demand(std::move(demand)) {};
        Client() {};
        int getId() { return id; };
        int getType() { return type; };
        float getX() { return x; };
        float getY() { return y; };
        std::vector<float> getDemand() { return demand; };
        void setId(int id) { this->id = id; };
        void setType(int type) { this->type = type; };
        void setX(float x) { this->x = x; };
        void setY(float y) { this->y = y; };
        void setDemand(std::vector<float> demand) { this->demand = demand; };
};

class Instance {

    private:
        float truck_c;
        float trailer_c;
        float truck_comp_c;
        float trailer_comp_c;
        int truck_N;
        int trailer_N;
        int nClients;
        int nLoads;
        Client depot;
        std::vector<Client> clients;
    public:
        Instance(std::string filename,int mode = 0) {
            std::ifstream file(filename);
            int counter = 0;
            int id;
            int type;
            float x;
            float y;
            float d;                  
            if (!file.is_open()) {
                std::cerr << "Error opening file" << std::endl;
                return;
            }

            std::string line;

            while(getline(file, line)){
                // Skip empty lines and comments
                if (line.empty() || line[0] == '#') {
                    continue;
                }
                std::stringstream ss(line);
                std::vector<float> demand;
                if (counter == 0 ) { //Define instance
                    ss >> truck_c >> trailer_c >> nClients >> truck_N >> trailer_N;
                    counter++;
                    if (mode == 1) {
                        counter++;
                        nLoads = 1;
                        truck_comp_c = truck_c;
                        trailer_comp_c = trailer_c;
                    }
                } else if (counter == 1) { // Products and compartments
                    ss >> nLoads >> truck_comp_c >> trailer_comp_c;
                    counter++;
                } else if (counter == 2){ //Depot
                    ss >> id >> x >> y;
                    depot = Client(id, 0, x, y, demand);
                    counter++;
                } else { // Clients
                    if (mode == 0){
                        ss >> id >> x >> y >> type;        
                        while(ss >> d){
                            demand.push_back(d);
                        }
                    }else{
                        ss >> id >> x >> y >> d >> type;
                        demand.push_back(d);
                    }
                    clients.push_back(Client(id, type, x, y, demand));
                }
            }
            
        };
        Instance() {};
        void print() {
            std::cout << "Truck capacity: " << truck_c << std::endl;
            std::cout << "Trailer capacity: " << trailer_c << std::endl;
            std::cout << "Truck compartment capacity: " << truck_comp_c << std::endl;
            std::cout << "Trailer compartment capacity: " << trailer_comp_c << std::endl;
            std::cout << "Max number of trucks: " << truck_N << std::endl;
            std::cout << "Max number of trailers: " << trailer_N << std::endl;
            std::cout << "VC clients:" << std::endl;
            for (Client client : clients) {
                std::cout << "Client ID: " << client.getId() << std::endl;
                std::cout << "Client type: " << (int)client.getType() << std::endl;
                std::cout << "Client x: " << client.getX() << std::endl;
                std::cout << "Client y: " << client.getY() << std::endl;
                std::cout << "Client demand: ";
                for (float d : client.getDemand()) {
                    std::cout << d << " ";
                }
                std::cout << "\n" << std::endl;
            }
        }
        Client getDepot() { return depot; };
        std::vector<Client> getVc_clients() {
            vector<Client> vc_clients;
            for(Client client: clients){
                if(client.getType()==0){
                    vc_clients.push_back(client);
                }
            }
            return vc_clients;
        };
        std::vector<Client> getTc_clients() {
            vector<Client> tc_clients;
            for(Client client: clients){
                if(client.getType()==1){
                    tc_clients.push_back(client);
                }
            }
            return tc_clients;};
        vector<Client> getClients(){return clients;};
        int getnClients() {return nClients;};
        int getTruck_N() { return truck_N; };
        int getTrailer_N() { return trailer_N; };
        float getTruck_comp_c() { return truck_comp_c; };
        float getTrailer_comp_c() { return trailer_comp_c; };
        int getnLoads() { return nLoads; };
        int gettruck_c() { return truck_c; };
        int gettrailer_c() { return trailer_c; };
        std::vector<float> getAllDemand(){
            std::vector<float> allDemand;
            int i;
            float demand;
            for(i=0;i<nLoads;i++){
                demand = 0;
                for(Client client: clients){
                    demand+= client.getDemand()[i];
                }
                allDemand.push_back(demand);
            }
            return allDemand;
        }
};

// Simple matrix class
class Matrix {
private:
    int rows, cols;
    std::vector<float> data;

public:
    Matrix(int r, int c) : rows(r), cols(c), data(r * c) {}
    Matrix() {};

    float& at(int i, int j) {
        return data[i * cols + j];
    }

    void print() const {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                std::cout << data[i * cols + j] << " ";
            }
            std::cout << std::endl;
        }
    }
};

// Distance function
float distance(Client a, Client b) {
    return std::sqrt(pow(a.getX() - b.getX(), 2) + pow(a.getY() - b.getY(), 2));
}

Matrix distanceMatrix(Instance instance) {
    int n = instance.getnClients();
    Matrix dist(n+1, n+1);
    // distance from depot to clients
    for(Client client: instance.getVc_clients()){
        dist.at(0, client.getId()) = distance(instance.getDepot(), client);
        dist.at(client.getId(), 0) = dist.at(0, client.getId());
    }
    for(Client client: instance.getTc_clients()){
        dist.at(0, client.getId()) = distance(instance.getDepot(), client);
        dist.at(client.getId(), 0) = dist.at(0, client.getId());
    }
    // distance from clients to clients
    for(Client client1: instance.getVc_clients()){
        for(Client client2: instance.getVc_clients()){
            if(client1.getId() != client2.getId()){
                dist.at(client1.getId(), client2.getId()) = distance(client1, client2);
                dist.at(client2.getId(), client1.getId()) = dist.at(client1.getId(), client2.getId());
            }
        }
    }
    for(Client client1: instance.getTc_clients()){
        for(Client client2: instance.getTc_clients()){
            if(client1.getId() != client2.getId()){
                dist.at(client1.getId(), client2.getId()) = distance(client1, client2);
                dist.at(client2.getId(), client1.getId()) = dist.at(client1.getId(), client2.getId());
            }
        }
    }
    // distance from VC clients to TC clients
    for(Client client1: instance.getVc_clients()){
        for(Client client2: instance.getTc_clients()){
            dist.at(client1.getId(), client2.getId()) = distance(client1, client2);
            dist.at(client2.getId(), client1.getId()) = dist.at(client1.getId(), client2.getId());
        }
    }
    return dist;
}

// Probably the solution encoding class
class Solution {
    private:
        std::vector<vector<int>> routes;
    public:
        Solution(){};
        void addRoute(vector<int> route) {
            routes.push_back(route);
        }
        vector<vector<int>> &getRoutes(){return routes;};
        void print(){
            int i = 0;
            for(vector<int> route:routes){
                i++;
                cout << "Route " << i << ": ";
                for(int elem: route) cout << elem << " ";
                cout << endl;
            }
        }
};

// PMX function for the crossover
std::vector<std::vector<int>> PMX (std::vector<int> &parent1, std::vector<int>& parent2){
    int size = parent1.size();
    std::vector<int> child1(size, -1);
    std::vector<int> child2(size, -1);

    // Random number generation setup    
    std::uniform_int_distribution<int> dist(1, size-1);

    int start = dist(rng);
    int end = dist(rng);
    if (start > end) std::swap(start, end);

    // std::cout << "Start: " << start << "; End: " << end << std::endl;

    // Step 1: Copy crossover segment
    for (int i = start; i <= end; ++i) {
        child1[i] = parent2[i];
        child2[i] = parent1[i];
    }
    // std::cout << "Child 1: ";
    // for (int i=0;i<size;i++) std::cout << child1[i] << " "; 
    // std::cout << "\nChild 2: ";
    // for (int i=0;i<size;i++) std::cout << child2[i] << " "; 
    // std::cout << std::endl;
    // Helper function to resolve mapping conflicts
    auto fill_child = [&](std::vector<int>& child, const std::vector<int>& parent, 
                          const std::vector<int>& segment, const std::vector<int>& mappedSegment) {
        for (int i = 0; i < size; ++i) {
            if (i >= start && i <= end) continue;

            int gene = parent[i];
            while (std::find(segment.begin(), segment.end(), gene) != segment.end()) {
                auto it = std::find(segment.begin(), segment.end(), gene);
                int index = static_cast<int>(std::distance(segment.begin(), it));
                gene = mappedSegment[index];
                // std::cout << "Start: " << start << "; End: " << end << std::endl;
            }
            child[i] = gene;
        }
    };

    // Build mappings from crossover segment
    std::vector<int> seg1(parent1.begin() + start, parent1.begin() + end + 1);
    std::vector<int> seg2(parent2.begin() + start, parent2.begin() + end + 1);

    // Step 2: Fill in the rest of the children
    fill_child(child1, parent1, seg2, seg1);
    fill_child(child2, parent2, seg1, seg2);

    return {child1, child2};
}

// GVR Crossover
Solution GVRX(Solution &parent1, Solution &parent2, Matrix &costMatrix){
    int nRoutes2 = parent2.getRoutes().size();
    Solution child;
    // Random number generation setup    
    std::uniform_int_distribution<int> dist1(0, nRoutes2-1);

    // Select the sub-route to be changed
    int n = dist1(rng);
    vector<int> route = parent2.getRoutes()[n];
    int size = route.size();
    uniform_int_distribution<int> dist2(0,route.size()-1);
    int m = dist2(rng);
    vector<int> subroute;
    vector<int> modRoute;

    for (int i = m;i<size;i++){
        subroute.push_back(route[i]);
    }

    cout << "The subroute is: ";
    for(int elem:subroute) cout << elem << " ";
    cout << endl;

    float min = 100000000.;
    int i=0,elem_route,element;
    //Select the closest client to the first client of the sub-route
    for(vector<int> route: parent1.getRoutes()){
        for(int elem: route){
            cout << "Cost from s_0 to " << elem << ": " << costMatrix.at(subroute[0],elem) << endl;
            if (costMatrix.at(subroute[0],elem)<min && find(subroute.begin(),subroute.end(),elem) == subroute.end()){
                min = costMatrix.at(subroute[0],elem);
                element = elem;
                elem_route = i;
            }
        }
        i++;
    }
    cout << "The min is: " << min << endl;
    cout << "The min is get at: " << element << endl;
    cout << "In the route: " << elem_route << endl;

    // Elimination of duplicates and insertion of the new subroute
    auto begin = subroute.begin(),end = subroute.end();
    for (vector<int> route: parent1.getRoutes()){
        vector<int> new_route;
        for (int elem: route){
            if (find(begin,end,elem) == end){
                new_route.push_back(elem);
            }
        }
        if (!new_route.empty()){ 
            if (find(new_route.begin(),new_route.end(),element) != new_route.end()){
                new_route.insert(find(new_route.begin(),new_route.end(),element)+1,subroute.begin(),subroute.end());
            }
            child.addRoute(new_route);            
        }
    }
    return child;
}

// Function to generate a random inversion mutation
void inversionMutation(Solution &route) {
    int nRoutes = route.getRoutes().size();

    // Random number generation setup    
    std::uniform_int_distribution<int> dist1(0, nRoutes-1);

    // Select the sub-route to be changed
    int n = dist1(rng);
    vector<int>& rt = route.getRoutes()[n];
    int size = rt.size();
    uniform_int_distribution<int> dist2(0,rt.size()-1);
    int m = dist2(rng);

    vector<int> subroute;

    // Get the subroute to be inverted
    for (int i = m;i<size;i++){
        subroute.push_back(rt[i]);
    }
    cout << "The subroute is: ";
    for (int elem: subroute) cout << elem << " ";
    cout << endl;

    // Inversion and reinsertion of the subroute
    reverse(subroute.begin(),subroute.end());
    rt.resize(m);
    rt.insert(rt.end(),subroute.begin(),subroute.end());
}

float objectiveFunction (Solution &sol, Instance &probl, Matrix &costMatrix){
    float cost = 0.;
    int i,rtype,parking;
    for(vector<int> route: sol.getRoutes()){
        cost+=costMatrix.at(0,route[0]);
        parking = 0;
        rtype = probl.getClients()[route[0]-1].getType();
        // cout << "Añadimos el coste entre el deposito y el cliente " << route[0] << endl;
        for(i=1;i<int(route.size());i++){            
            if (probl.getClients()[route[i]-1].getType() == 1 && rtype == 0 && parking == 0){
                parking = route[i-1];
                // cout << "Entramos en una subruta" << endl;
            }
            if (parking == 0){
                cost+= costMatrix.at(route[i-1],route[i]);
                // cout << "Añadimos el coste entre el cliente " << route[i-1] << " y el cliente " << route[i] << endl;
            } else{
                if (probl.getClients()[route[i]-1].getType() == 0 && rtype == 0){
                    cost+= costMatrix.at(parking,route[i-1]);
                    // cout << "Volvemos al parking " << parking << " desde el cliente " << route[i-1] << endl;
                    cost+= costMatrix.at(parking,route[i]);
                    // cout << "Se acabo la subruta, añadimos el coste entre el cliente " << parking << " y el cliente " << route[i] << endl;
                    parking = 0;
                }else{
                    cost+= costMatrix.at(route[i-1],route[i]);
                    // cout << "Añadimos el coste entre el cliente " << route[i-1] << " y el cliente " << route[i] << " en la subruta con parking " << parking << endl;
                }
            }
        }
        if (parking != 0){
            cost+= costMatrix.at(parking,route.back());
            // cout << "Volvemos al parking " << parking << " desde el cliente " << route.back() << endl;
            // cout << "Volvemos al deposito desde el cliente " << parking << endl;
            cost+=costMatrix.at(0,parking);
        }else{
            // cout << "Volvemos al deposito desde el cliente " << route.back() << endl;
            cost+=costMatrix.at(0,route.back());
        }
    }
    return cost;
}

// Function to generate a random symmetric matrix
Matrix randomSymMatrix(int size){
    Matrix mat(size+1,size+1);
    uniform_real_distribution<float> dist(10.,30.);

    for(int i=0;i<size+1;i++){
        for(int j=0;j<i;j++){
            // if (j==i) mat.at(i,j) = 0;
            mat.at(i,j) = dist(rng);
            mat.at(j,i) = mat.at(i,j);
        }
    }
    return mat;
}


int main(int argc, char* argv[]) {
    // Initialize the instance
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    std::cout << "Reading file: " << filename << std::endl;
    std::string title = filename.substr(filename.find("/")+1,filename.size());
    title.erase(0,title.find("_")+1);
    title = title.substr(0,title.find("_"));
    int mode = (title == "TTRP");
    // std::cout << mode << std::endl;

    Instance instance(filename,mode);
    // instance.print();
    Matrix distance_matrix;
    distance_matrix = distanceMatrix(instance);

    std::cout << instance.getAllDemand()[0] << std::endl;
    std::cout << "Needed trucks: " << instance.getAllDemand()[0]/instance.gettruck_c() << std::endl;

    // std::vector<int> p1 = {1,6,3,4,5,2,9,7,8};
    // std::vector<int> p2 = {4,3,1,2,6,5,8,9,7};

    // std::vector<std::vector<int>> a = PMX(p1,p2);

    // std::cout << "Child 1: ";
    // for (int i=0;i<9;i++) std::cout << a[0][i] << " "; 
    // std::cout << "\nChild 2: ";
    // for (int i=0;i<9;i++) std::cout << a[1][i] << " "; 
    // std::cout << std::endl;

    // GVR Crossover
    Solution sol1,sol2;

    sol1.addRoute({1,2,3});
    sol1.addRoute({4,5});
    sol1.addRoute({6,7,8});
    cout << "test" << endl;
    sol2.addRoute({1,8});
    sol2.addRoute({3,4,5,2});
    sol2.addRoute({6,7});

    Matrix mat = randomSymMatrix(8);

    Solution child = GVRX(sol1,sol2,distance_matrix);

    child.print();

    inversionMutation(child);

    distance_matrix.print();

    child.print();

    float cost = objectiveFunction(child,instance,distance_matrix);

    cout << "The cost of the trip is: " << cost << endl;

    return 0;
}