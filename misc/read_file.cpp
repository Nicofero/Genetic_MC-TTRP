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
#include <limits>
#include <omp.h>
#include <atomic>
#include <chrono>
using namespace std;

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

class Instance {

    private:
        float truck_c;  // Truck capacity
        float trailer_c;    // Trailer capacity
        float truck_comp_c; // Truck compartment capacity
        float trailer_comp_c;  // Trailer compartment capacity
        int truck_N;
        int trailer_N;
        int nClients;
        int nLoads;
        Client depot;
        std::vector<Client> clients;
        std::vector<float> allDemand;
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
                exit(EXIT_FAILURE);
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
        Matrix from_csv(string filename){
            std::ifstream file(filename);            
            string line;
            getline(file,line);
            getline(file,line);
            std::stringstream ss(line);
            vector<float> demand;
            string item;
            std::getline(ss, item, ','); truck_c = std::stoi(item);
            std::getline(ss, item, ','); trailer_c = std::stoi(item);
            std::getline(ss, item, ','); nClients = std::stoi(item);
            std::getline(ss, item, ','); truck_N = std::stoi(item);
            std::getline(ss, item, ','); trailer_N = std::stoi(item);
            std::getline(ss, item, ','); nLoads = std::stoi(item);
            std::getline(ss, item, ','); truck_comp_c = std::stoi(item);
            std::getline(ss, item, ','); trailer_comp_c = std::stoi(item);
            std::cout << "Parsed line: " << line << std::endl;

            Matrix cost_mat(nClients+1,nClients+1);
            getline(file,line);
            int i=0,j=0;
            while(getline(file,line)){
                std::stringstream ss2(line);
                Client client;            
                demand = vector<float>();
                j=-1;
                while (getline(ss2, item,',')) {
                    int value = std::stoi(item);
                    if (j>=0 && j<nClients+1){
                        cost_mat.at(i,j) = value;
                    }else if (j>nClients+1){
                        demand.push_back(float(value));
                    }else if (j==nClients+1){
                        client.setType(value);
                    }else{
                        client.setId(value);
                    }
                    j++;
                }
                client.setDemand(demand);
                if(i==0){
                    depot = client;
                }else{
                    clients.push_back(client);
                }
                
                std::cout << "Parsed line " << i << ": " << line << std::endl;
                i++;
            }
            return cost_mat;
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
            if(allDemand.empty()){
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
                this->allDemand = allDemand;
                return allDemand;
            }else{
                return allDemand;
            }

        }
        int getNCompartmentsTruck (){
            return int(truck_c/truck_comp_c);
        }
        int getNCompartmentsTrailer (){
            return int(trailer_c/trailer_comp_c);
         }
};

// Distance function
float distance(Client a, Client b) {
    return std::sqrt(pow(a.getX() - b.getX(), 2) + pow(a.getY() - b.getY(), 2));
}

// Creates distance matrix
Matrix distanceMatrix(Instance instance) {
    int n = instance.getnClients();
    Matrix dist(n+1, n+1);
    // distance from depot to clients
    for(Client client: instance.getClients()){
        dist.at(0, client.getId()) = distance(instance.getDepot(), client);
        dist.at(client.getId(), 0) = dist.at(0, client.getId());
    }
    // distance from clients to clients
    for(Client client1: instance.getClients()){
        for(Client client2: instance.getClients()){
            if(client1.getId() != client2.getId()){
                dist.at(client1.getId(), client2.getId()) = distance(client1, client2);
                dist.at(client2.getId(), client1.getId()) = dist.at(client1.getId(), client2.getId());
            }
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
        vector<int> findElement(int elem){
            vector<int> pos(2);
            for (int i=0;i<int(routes.size());i++){
                for(int j=0;j<int(routes[i].size());j++){
                    if(routes[i][j] == elem){
                        pos[0] = i;
                        pos[1] = j;
                        return pos;
                    }
                }
            }
            return pos;
        };
        void print(bool in_line=false, std::ostream& out = std::cout){
            int i = 0;
            for(vector<int> route:routes){
                i++;
                out << "Route " << i << ": ";
                for(int elem: route) out << elem << " ";
                if (in_line)
                    out << "; ";                    
                else
                    out << endl;
            }
            out << endl;
        }
        vector<int> toSingleRoute (){
            vector<int> route;
            for(vector<int> elem: routes){
                route.insert(route.end(),elem.begin(),elem.end());
            }
            return route;
        }
        float cost;
        // Gets trailers needed for the solution
        int trailers_needed(Instance &probl){
            int trailers=0;
            for(vector<int> route: routes){
                if(probl.getClients()[route[0]-1].getType() == 0) trailers++;
            }
            return trailers;
        }
        // Gets trucks needed for the solution
        int trucks_needed() {return routes.size();}
        bool isFeasible(Instance &probl,bool ignore_tn = true){
            if (trailers_needed(probl)>probl.getTrailer_N() && ignore_tn){
                // cout << "Too many trailers" << endl;
                return false;
            }
            if (trucks_needed()>probl.getTruck_N() && ignore_tn){
                // cout << "Too many trucks" << endl;
                return false;
            }
            // Max compartments for each type of trip
            int max_comp_trailer = probl.getNCompartmentsTrailer(), max_comp_truck = probl.getNCompartmentsTruck();
            
            int route_type, aux_trailer, aux_truck;
            vector<float> demand;
            vector<Client> clients = probl.getClients();
            for(vector<int> rt: routes){
                // Get route type to get                 
                route_type = clients[rt[0]-1].getType();
                switch (route_type){
                    case 0: // VC route
                        aux_trailer = max_comp_trailer;
                        aux_truck = max_comp_truck;
                        for (int elem: rt){
                            demand = clients[elem-1].getDemand();
                            for (float load: demand){
                                if (clients[elem-1].getType() == 0){
                                    aux_trailer-=ceil(load/probl.getTrailer_comp_c());
                                }else{
                                    aux_truck-=ceil(load/probl.getTruck_comp_c());
                                }

                                if (aux_truck < 0 || aux_trailer < 0){
                                    // if (aux_truck < 0)
                                        // cout << "Too much load in the truck of a VR" << endl;
                                    // else
                                        // cout << "Too much load in the trailer of a VR" << endl;
                                    return false;
                                }
                            }
                        }
                        break;
                    
                    case 1: // TC route
                        aux_truck = max_comp_truck;
                        for (int elem: rt){
                            demand = clients[elem-1].getDemand();
                            for (float load:demand){
                                aux_truck-=ceil(load/probl.getTruck_comp_c());
                                if (aux_truck <0){
                                    // cout << "Too much load in a TR" << endl;
                                    return false;
                                }
                            }
                        }
                        break;
                    default:
                        cout << RED << "YOU SHALL NEVER BE HERE" << WHT << endl;
                        return false;
                        break;
                }
            }
            return true;
        }
        vector<int> to_1D_array(){
            vector<int> aux;
            for(vector<int> rt: routes){
                aux.insert(aux.end(),rt.begin(),rt.end());
            }
            return aux;
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
    // cout << nRoutes2 << endl;
    std::uniform_int_distribution<int> dist1(0, nRoutes2-1);
    // Select the sub-route to be changed
    int n = dist1(rng);

    vector<int> route = parent2.getRoutes()[n];
    uniform_int_distribution<int> dist2(0,route.size()-1);
    int m = dist2(rng);
    int m2 = dist2(rng);
    if (m > m2) swap(m,m2);
    vector<int> subroute;

    for (int i = m;i<(m2+1);i++){
        subroute.push_back(route[i]);
    }

    // cout << "The subroute is: ";
    // for(int elem:subroute) cout << elem << " ";
    // cout << endl;

    float min = numeric_limits<float>::infinity();
    int i=0,element;
    //Select the closest client to the first client of the sub-route
    for(vector<int> route: parent1.getRoutes()){
        for(int elem: route){
            // cout << "Cost from s_0 to " << elem << ": " << costMatrix.at(subroute[0],elem) << endl;
            if (costMatrix.at(subroute[0],elem)<min && find(subroute.begin(),subroute.end(),elem) == subroute.end()){
                min = costMatrix.at(subroute[0],elem);
                element = elem;
            }
        }
        i++;
    }
    // cout << "The min is: " << min << endl;
    // cout << "The min is get at: " << element << endl;
    // cout << "In the route: " << elem_route << endl;

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
    // if (child.getRoutes().empty()){
    //     cout << "Child empty" << endl;
    //     for (int elem:parent1.getRoutes()[0]) cout << elem << " ";
    //     cout << endl;
    //     for (int elem:parent2.getRoutes()[0]) cout << elem << " ";
    //     cout << endl;
    //     cout << "Subroute" << endl;
    //     for (int elem:subroute) cout << elem << " ";
    //     cout << endl;
    // }
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
    if (size>1) size--;
    uniform_int_distribution<int> dist2(0,size-1);
    int m = dist2(rng);

    // cout << "Reverse the route " << n << " from the position " << m << endl;

    reverse(rt.begin()+m,rt.end());
}

// TODO: Review this function
// TODO: Review this function
// Objective function: Maybe for the subtours check if the next node after a VC is a TC and add that to the subtour instead 
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


/*      LOCAL SEARCH METHODS
* The local search methods will be:
*   - Relocate
*   - 2-OPT   - 
*   - Maybe some method for the subtours
*/

// Relocate atomic function (called by the local search if a better move is found)
void relocate (Solution &sol,int element, int route, int position){

    vector<int> pos = sol.findElement(element);
    sol.getRoutes()[pos[0]].erase(sol.getRoutes()[pos[0]].begin()+pos[1]);    
    sol.getRoutes()[route].insert(sol.getRoutes()[route].begin()+position,element);
    if (sol.getRoutes()[pos[0]].empty()){        
        sol.getRoutes().erase(sol.getRoutes().begin()+pos[0]);
    }
    
}

// Forceful relocation. Forces to relocate a route even if it doesnt improve the solution
bool forceful_relocation(Solution &sol,Instance &inst, Matrix &cost_matrix){
    Solution changes;
    sol.print();
    for(int i=0;i<int(sol.getRoutes().size());i++){
        for (int j=0;j<int(sol.getRoutes().size());j++){
            if(i!=j){
                changes = sol;
                changes.getRoutes()[j].insert(changes.getRoutes()[j].end(),changes.getRoutes()[i].begin(),changes.getRoutes()[i].end());
                changes.getRoutes().erase(changes.getRoutes().begin()+i);
                changes.print();
                if (changes.isFeasible(inst,false)){
                    sol = changes;
                    return true;
                }
                else{
                    cout << "Route " << i << " into route " << j << " doesnt work" << endl;
                }
            }
        }
    }
    return false;
}

bool ultra_forceful_relocation(Solution &sol,Instance &inst, Matrix &cost_matrix){
    Solution changes;
    // sol.print();
    int numRoutes = sol.getRoutes().size();

    for (int i = 0; i < numRoutes; i++) {
        for (int j = 0; j < numRoutes; j++) {
            if (i != j) {
                changes = sol;

                // Try full route relocation
                changes.getRoutes()[j].insert(
                    changes.getRoutes()[j].end(),
                    changes.getRoutes()[i].begin(),
                    changes.getRoutes()[i].end()
                );
                changes.getRoutes().erase(changes.getRoutes().begin() + i);

                // changes.print();
                if (changes.isFeasible(inst, false)) {
                    sol = changes;
                    return true;
                } else {
                    // cout << "Route " << i << " into route " << j << " doesn't work" << endl;
                }

                // --- Try splitting the route into two parts and relocating each part ---
                if (numRoutes > 2) { // Need at least 3 routes to relocate into two others
                    for (int k = 0; k < numRoutes; k++) {
                        if (k != i && k != j) {
                            changes = sol;
                            auto& routeToSplit = changes.getRoutes()[i];
                            size_t mid = routeToSplit.size() / 2;

                            // First half to route j
                            changes.getRoutes()[j].insert(
                                changes.getRoutes()[j].end(),
                                routeToSplit.begin(),
                                routeToSplit.begin() + mid
                            );

                            // Second half to route k
                            changes.getRoutes()[k].insert(
                                changes.getRoutes()[k].end(),
                                routeToSplit.begin() + mid,
                                routeToSplit.end()
                            );

                            // Remove original route
                            changes.getRoutes().erase(changes.getRoutes().begin() + i);

                            // changes.print();
                            if (changes.isFeasible(inst, false)) {
                                sol = changes;
                                return true;
                            } else {
                                // cout << "Split of route " << i << " into routes " << j << " and " << k << " doesn't work" << endl;
                            }
                        }
                    }
                }
            }
        }
    }
    // --- 3. Atomic relocation: LAST RESORT ---
    for (int i = 0; i < int(sol.getRoutes().size()); i++) {
        auto& routeFrom = sol.getRoutes()[i];

        for (int n = 0; n < int(routeFrom.size()); n++) {
            for (int j = 0; j < int(sol.getRoutes().size()); j++) {
                if (i == j) continue;

                for (int pos = 0; pos <= int(sol.getRoutes()[j].size()); pos++) {
                    changes = sol;

                    auto node = routeFrom[n];
                    changes.getRoutes()[j].insert(changes.getRoutes()[j].begin() + pos, node);

                    int i_adj = (j < i) ? i + 1 : i;
                    changes.getRoutes()[i_adj].erase(changes.getRoutes()[i_adj].begin() + n);

                    if (changes.isFeasible(inst, false)) {
                        sol = changes;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

// Relocation local search. Returns the route where insertion happened
// We supose that every solution that is going through local search is feasible
int relocationLocal (Solution &sol,Instance &inst, Matrix &cost_matrix){
    // uniform_int_distribution<int> dist(0,sol.getRoutes().size()-1);
    float best_sol = sol.cost, sol_act;
    Solution changes;
    if (sol.getRoutes().size()>1){
        // Search for a feasible relocation. 
        for(int i=0;i<int(sol.getRoutes().size());i++){
            for (int elem: sol.getRoutes()[i]){
                for (int j=0;j<int(sol.getRoutes().size());j++){
                    if (j!=i){
                        for (int k=0;k<int(sol.getRoutes()[j].size());k++){
                            changes = sol;
                            relocate(changes,elem,j,k);
                            if(changes.isFeasible(inst)){
                                sol_act = objectiveFunction(changes,inst,cost_matrix);
                                if (sol_act < best_sol){
                                    sol = changes;
                                    return j;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return -1;
}

// 2-OPT atomic function
void opt2 (vector<int> &route,int edge1,int edge2){
    // edge1>=1 to not change the type of route
    if (edge1 >= 1 && edge2 < int(route.size()) && edge1 < edge2) {
        reverse(route.begin() + edge1, route.begin() + edge2 + 1);
    }
}

// 2-OPT Local search
void opt2Local (Solution &sol,int pos,Instance &probl, Matrix &costMatrix){
    vector<int> &route = sol.getRoutes()[pos];
    vector<int> best_route;
    float new_distance;
    float best_distance = objectiveFunction(sol,probl,costMatrix);
    bool end;

    // Only if route to be changed is more than one element
    if (route.size()>1){
        do{
            end = true;
            // i=1 to not change the type of route
            for(int i=1;i<int(route.size()-1) && end;i++){
                for(int j=i;j<int(route.size()) && end;j++){
                    opt2(route,i,j);
                    new_distance = objectiveFunction(sol,probl,costMatrix);
                    if (new_distance < best_distance){
                        best_route = route;
                        best_distance = new_distance;
                        end = false;
                    }
                }
            }
        }while (!end);
    }
}

// Something to change type of route
void routeOpt (){
    // TODO:
}


// Tournament selection (4 participants)
int tournamentSelection(vector<Solution> &candidates,Instance &probl, Matrix &costMatrix){
    std::vector<float> scores(4);
    for (int i = 0; i < 4; ++i) {
        scores[i] = objectiveFunction(candidates[i], probl, costMatrix);
    }

    // Determine winner of left and right brackets
    int leftWinner = (scores[0] < scores[1]) ? 0 : 1;
    int rightWinner = (scores[2] < scores[3]) ? 2 : 3;

    // Final match between left and right bracket winners
    int finalWinner = (scores[leftWinner] < scores[rightWinner]) ? leftWinner : rightWinner;

    return finalWinner;
}

std::vector<int> generate_random_permutation(int size) {
    std::vector<int> vec(size);
    for (int i = 0; i < size; ++i) {
        vec[i] = i + 1;
    }
    std::shuffle(vec.begin(), vec.end(), rng);
    return vec;
}

// Deprecated: Receives a permutation of nodes and returns a Solution forming routes
// Solution ssplit (vector<int> &perm){
//     // To be changed
//     int s = perm.size(), rsize;
//     uniform_int_distribution<int> dist1(0, s-1);    
//     Solution sol;
//     vector<int> route;

//     while(s>0){
//         route = vector<int>();
//         rsize = dist1(rng) % s;
//         if(s==1 || rsize==0) rsize = 1;
//         route.insert(route.begin(),perm.begin(),perm.begin()+rsize);
//         perm.erase(perm.begin(),perm.begin()+rsize);
//         s-=rsize;
//         sol.getRoutes().push_back(route);
//     }
//     return sol;

// }

Solution extract_solution (vector<int> &predecessor, vector<int> &route){
    int N = predecessor.size(),j=N,i;
    Solution sol;
    vector<int> aux;

    do{
        i = predecessor[j-1];
        aux = vector<int>();
        for (int k=i;k<j;k++){
            aux.push_back(route[k]);
        }
        if(!aux.empty()){
            sol.getRoutes().push_back(aux);
        }
        j=i;

    }while(i!=0);
    return sol;
}

Solution ssplit (vector<int> &route, Instance &inst, Matrix &cost_matrix){
    int N = route.size(), comp_used_truck, comp_used_trailer, j, route_type, subtour;
    vector<float> varray(N+1);
    vector<int> predecessor(N);
    float cost;
    Solution new_route;
    // Set label V0 to 0
    varray[0] = 0.;
    predecessor[0] = 0;
    // Set each other label Vi to +∞, for 1 ≤ i ≤ n
    for (int i=1;i<N;i++){
        varray[i] = numeric_limits<float>::infinity();
        predecessor[i] = 0;
    }
    varray[N] = numeric_limits<float>::infinity();
    
    // Max compartments for each type of trip
    int max_comp_trailer = inst.getNCompartmentsTrailer(), max_comp_truck = inst.getNCompartmentsTruck();

    for (int i=1;i<=N;i++){
        cost = 0;
        // Set load(p) to 0, for 1 ≤ p ≤ m
        comp_used_trailer = 0;
        comp_used_truck = 0;
        subtour=-1;
        route_type = inst.getClients()[route[i-1]-1].getType();
        // cout << "Start " << i << endl;
        j = i;
        // j-1 is actual node
        do{
            // Update load for route type
            for (float elem:inst.getClients()[route[j-1]-1].getDemand()){
                if (inst.getClients()[route[j-1]-1].getType() == 0 && route_type == 0){
                    comp_used_trailer+= ceil(elem/inst.getTrailer_comp_c());
                }else{
                    comp_used_truck+= ceil(elem/inst.getTruck_comp_c());
                }
            }
            // Subroute check
            if (inst.getClients()[route[j-1]-1].getType() == 1 && route_type == 0 && subtour==-1){
                subtour = route[j-2];
            }
            // Update cost // SHOULD BE DONE
            if (i==j){
                // cout << "Cost act" << endl;
                cost = 2*cost_matrix.at(0,route[j-1]);
                // cout << "Cost act 2" << endl;
            }else{
                // Return from a subtour
                if (subtour > 0 && inst.getClients()[route[j-1]-1].getType() == 0){
                    cost+= cost_matrix.at(route[j-1],subtour) - cost_matrix.at(subtour,0) + cost_matrix.at(0,route[j-1]);
                }else if (subtour > 0 && inst.getClients()[route[j-1]-1].getType() == 1){   // Enter subtour (add return to parking spot, remove from previous)
                    cost+= cost_matrix.at(route[j-2],route[j-1]) - cost_matrix.at(route[j-2],subtour) + cost_matrix.at(subtour,route[j-1]);
                }else{
                    cost+= cost_matrix.at(route[j-2],route[j-1]) - cost_matrix.at(route[j-2],0) + cost_matrix.at(0,route[j-1]);
                }
            }
            
            if (comp_used_truck < max_comp_truck && comp_used_trailer < max_comp_trailer){
                if (varray[i-1]+cost < varray[j]){
                    varray[j] = varray[i-1]+cost;
                    predecessor[j-1] = i-1;
                }
                j++;
            }
            
        }while(j<=N && comp_used_truck < max_comp_truck && comp_used_trailer < max_comp_trailer);
        //         If load(p) ≤ Qp and cost ≤ L then
        //             If Vi−1 + cost < Vj then
        //                 Set Vj = Vi−1 + cost
        //                 Set Pj = i − 1
        //             End if
        //             Increment j by 1
        //         End if
        //     Until j > n or load(p) > Qp
    }

    new_route = extract_solution(predecessor,route);
    bool feas = true;
    // This returns feasible routes but has no regard for maximum vehicles
    while ((new_route.trailers_needed(inst)>inst.getTrailer_N() || new_route.trucks_needed()>inst.getTruck_N()) && feas){
        // Forceful relocation
        feas = ultra_forceful_relocation(new_route,inst,cost_matrix);
        // exit(EXIT_SUCCESS);
        if (!feas){
            exit(EXIT_FAILURE);
        }
    }

    return new_route;
}

// Wrapper for the memetic loop
Solution ssplit (Solution &route, Instance &inst, Matrix &cost_matrix){
    vector<int> route_vector = route.to_1D_array();

    return ssplit(route_vector,inst,cost_matrix);
}

// Generates a random population with length nodes and size number of individuals 
vector<Solution> randomPopulation (int size, int length, Instance &inst, Matrix &cost_matrix){
    vector<Solution> pop;
    vector<vector<int>> seeds;
    Solution aux;

    // Generate random permutations
    for(int i=0; i<size;i++){
        seeds.push_back(generate_random_permutation(length));
    }

    // Divide the individuals into random routes. 
    for(int i=0; i<size;i++){
        pop.push_back(ssplit(seeds[i],inst,cost_matrix));
    }

    return pop;
}

void selectParents(int &p1, int &p2, Instance &probl, Matrix &costMatrix, vector<Solution> &pop){
    vector<Solution> tournament(4);
    uniform_int_distribution<int> dist(0,pop.size()-1);
    
    for(int j=0;j<4;j++){
        tournament[j] = pop[dist(rng)];
    }    
    p1 = tournamentSelection(tournament,probl,costMatrix);
    
    tournament = vector<Solution>(4);
    
    for(int j=0;j<4;j++){
        tournament[j] = pop[dist(rng)];
    }
    p2 = tournamentSelection(tournament,probl,costMatrix);
}

void drawProgressBar(const std::string& name, int value, int max_value, int bar_width = 40) {
    int filled = (value * bar_width) / max_value;
    std::string bar = std::string(filled, '#') + std::string(bar_width - filled, '-');
    std::cout << name << ": [" << bar << "] " << value << "/" << max_value << "\n";
}

void updateProgress(int alpha, int beta, int max_value_alpha, int max_value_beta) {
    // Move the cursor up two lines
    std::cout << "\033[F\033[F"; // ANSI escape codes to move cursor up
    drawProgressBar("Maxiter", alpha, max_value_alpha);
    drawProgressBar("MaxiterNC", beta, max_value_beta);
    std::cout.flush();
}


Solution memeticLoop(int size, Instance &probl, Matrix &costMatrix, int maxiter=10000,float pc=0.8, float pm=0.4, float pls=0.3, bool verbose=false){
    Solution best_sol;
    float best_cost;
    if (size < 4){
        cout << RED << "The population size must be bigger than 4" << WHT << endl;
        return best_sol;
    }
    vector<Solution> pop = randomPopulation(size,probl.getnClients(),probl,costMatrix), ext_pop, tournament(4);
    Solution child;
    int alpha=0, beta=0, maxiternor, nParents; // i, p1, p2, route_local;
    uniform_int_distribution<int> dist(0,size-1);
    uniform_real_distribution<float> distfloat(0,1);
    // Assign costs for the population
    #pragma omp parallel for
    for(Solution& elem:pop){
        elem.cost = objectiveFunction(elem,probl,costMatrix);
    }

    std::sort(pop.begin(), pop.end(), [](const Solution& a, const Solution& b) {
        return a.cost < b.cost;
    });
    best_cost = pop[0].cost;
    best_sol = pop[0];

    // cout << "New best solution: " << best_cost << endl;
    // best_sol.print();

    if (maxiter > 5000) maxiternor = maxiter/100;
    else maxiternor = 50;
    // for(int i=0;i<int(pop.size());i++){
    //     cout << "Child " << i << endl;
    //     pop[i].print();
    // }
    if(verbose){
        drawProgressBar("Maxiter",alpha,maxiter);
        drawProgressBar("MaxiterNC",beta,maxiternor);
    }

    while(alpha < maxiter && beta < maxiternor){
        ext_pop = pop;
        // cout << "New pop done" << endl;
        // Selection and crossover
        nParents = ceil(pc*size)/2;
        // cout << nParents << endl;
        // i = 0;
        // cout << "----> Iteration " << alpha << endl;
        // while(i<nParents){
        //     selectParents(p1,p2,probl,costMatrix,pop);
        //     child = GVRX(pop[p1],pop[p2],costMatrix);
        //     // You can add here a function that checks variety in the population
            
        //     if(!child.getRoutes().empty()){
        //         // Fix the solution
        //         if (!child.isFeasible(probl)){
        //             child = ssplit(child,probl,costMatrix);
        //         }
        //         child.cost = objectiveFunction(child,probl,costMatrix);
        //         ext_pop.push_back(child);
        //         i++;
        //     }
        //     // cout << "New child" << endl;
        // }
        // Parallel version
        std::atomic<int> parallel_i(0);
        #pragma omp parallel
        {
            // Local storage for valid children
            std::vector<Solution> local_ext_pop;

            // Local RNG
            std::mt19937 rng_local(std::random_device{}() + omp_get_thread_num());

            while (true) {
                int current = parallel_i.load();
                if (current > nParents) break;

                // Parent selection and crossover
                int p1, p2;
                selectParents(p1, p2, probl, costMatrix, pop);  // assumes thread-safe or RNG-only
                Solution child = GVRX(pop[p1], pop[p2], costMatrix);

                if (!child.getRoutes().empty()) {
                    if (!child.isFeasible(probl)) {
                        child = ssplit(child, probl, costMatrix);
                    }
                    child.cost = objectiveFunction(child, probl, costMatrix);
                    // Atomically claim a slot if we’re still under limit
                    int idx = parallel_i.fetch_add(1);
                    if (idx < nParents) {
                        local_ext_pop.push_back(child);
                    } else {
                        break;  // We went over the limit — don’t keep adding
                    }
                }
            }

            // Merge thread-local children into global vector
            #pragma omp critical
            ext_pop.insert(ext_pop.end(), local_ext_pop.begin(), local_ext_pop.end());
        }
        // cout << "Crossover done" << endl;

        // Mutation and local search
        // #pragma omp parallel for
        // for(Solution &s: pop){
        //     if (distfloat(rng)<pm){
        //         inversionMutation(s);
        //         if (!s.isFeasible(probl)){
        //             s = ssplit(s,probl,costMatrix);
        //         }
        //         s.cost = objectiveFunction(s,probl,costMatrix);
        //         ext_pop.push_back(s);
        //     }
        //     if (distfloat(rng)<pls){ // Every method here mantains feasibility
        //         // Relocation
        //         route_local = relocationLocal(s,probl,costMatrix);
        //         // 2-OPT
        //         if (route_local==-1) route_local = dist(rng) % s.getRoutes().size();
        //         opt2Local(s,route_local,probl,costMatrix);
        //         s.cost = objectiveFunction(s,probl,costMatrix);
        //         ext_pop.push_back(s);
        //     }
        // }
        #pragma omp parallel
        {
            // Create a thread-local RNG
            std::mt19937 rng_local(std::random_device{}() + omp_get_thread_num());
            std::uniform_real_distribution<float> distfloat(0.0f, 1.0f);
            std::uniform_int_distribution<int> dist;  // You may need to specify bounds later

            // Thread-local vector to collect new solutions
            std::vector<Solution> local_ext_pop;

            // Parallel loop
            #pragma omp for nowait
            for (int i = 0; i < int(pop.size()); ++i) {
                Solution s = pop[i];  // Work on a copy, not the original

                if (distfloat(rng_local) < pm) {
                    inversionMutation(s);
                    if (!s.isFeasible(probl)) {
                        s = ssplit(s, probl, costMatrix);
                    }
                    s.cost = objectiveFunction(s, probl, costMatrix);
                    local_ext_pop.push_back(s);
                }

                if (distfloat(rng_local) < pls) {
                    int route_local = relocationLocal(s, probl, costMatrix);
                    if (route_local == -1)
                        route_local = dist(rng_local) % s.getRoutes().size();
                    opt2Local(s, route_local, probl, costMatrix);
                    s.cost = objectiveFunction(s, probl, costMatrix);
                    local_ext_pop.push_back(s);
                }
            }

            // Merge results into the shared vector
            #pragma omp critical
            ext_pop.insert(ext_pop.end(), local_ext_pop.begin(), local_ext_pop.end());
        }
        // Minima update

        // Sort the extended population
        std::sort(ext_pop.begin(), ext_pop.end(), [](const Solution& a, const Solution& b) {
            return a.cost < b.cost;
        });
        // TODO: Maybe here i can shuffle the second half of the ext_pop to add some randomness to the population
        pop.assign(ext_pop.begin(),ext_pop.begin()+size);
        
        if (pop[0].cost < best_cost){
            best_cost = pop[0].cost;
            best_sol = pop[0];
            beta = 0;
            // cout << "Best cost updated" << endl;
        }else{
            beta++;
        }

        alpha++;

        if (verbose){
            updateProgress(alpha,beta,maxiter,maxiternor);
        }
    }

    return best_sol;
}

// Stupid function for easy print of vectors

void print_vector(const vector<int> &veect){
    for (int elem: veect) cout << elem << " ";
    cout << endl;
}


int main(int argc, char* argv[]) {
    // Initialize the instance
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    std::string filename = argv[1];

    // Code for txt
    std::cout << "Reading file: " << filename << std::endl;
    std::string title = filename.substr(filename.find("/")+1,filename.size());
    title.erase(0,title.find("_")+1);
    title = title.substr(0,title.find("_"));
    int mode = (title == "TTRP");
    std::cout << mode << std::endl;
    // Extension
    std::string ext = filename.substr(filename.find("/")+1,filename.size());
    ext.erase(0,ext.find("_")+1);
    ext.erase(0,ext.find("_")+1);
    ext.erase(0,ext.find(".")+1);

    Matrix distance_matrix;
    Instance instance;

    if (ext == "txt"){    
        instance = Instance(filename,mode);
        // instance.print();
        
        distance_matrix = distanceMatrix(instance);
    } else if (ext == "csv"){
        // Code for CSV        
        distance_matrix = instance.from_csv(filename);
    }
    // instance.print();
    
    // instance.print();
    // instance.print();

    // std::vector<int> p1 = {1,6,3,4,5,2,9,7,8};
    // std::vector<int> p2 = {4,3,1,2,6,5,8,9,7};

    // std::vector<std::vector<int>> a = PMX(p1,p2);

    // std::cout << "Child 1: ";
    // for (int i=0;i<9;i++) std::cout << a[0][i] << " "; 
    // std::cout << "\nChild 2: ";
    // for (int i=0;i<9;i++) std::cout << a[1][i] << " "; 
    // std::cout << std::endl;

    // GVR Crossover
    // Solution sol1,sol2;

    // sol1.addRoute({1,2,3});
    // sol1.addRoute({4,5});
    // sol1.addRoute({6,7,8});
    // cout << "test" << endl;
    // sol2.addRoute({1,8});
    // sol2.addRoute({3,4,5,2});
    // sol2.addRoute({6,7});

    // Matrix mat = randomSymMatrix(8);

    // Solution child = GVRX(sol1,sol2,distance_matrix);

    // cout << "Child after crossover:" << endl;
    // child.print();

    // inversionMutation(child);

    // // distance_matrix.print();
    // cout << "Child after mutation:" << endl;
    // child.print();

    // float cost = objectiveFunction(child,instance,distance_matrix);

    // cout << "The cost of the trip is: " << cost << endl;

    // relocate(child,4,0,0);

    // cout << "Child after relocation:" << endl;
    // child.print();

    std::cout << "Max threads: " << omp_get_max_threads() << "\n";
    int pop_size;
    if (omp_get_max_threads() > 50){
        pop_size = omp_get_max_threads();
    } else{
        pop_size = 4 * omp_get_max_threads();
    }
    std::cout << "Pop size: " << pop_size << "\n";
   
    auto start = chrono::high_resolution_clock::now();
    Solution best_route = memeticLoop(32,instance,distance_matrix,10000,0.9,0.5,0.5,true);
    auto end = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(end-start);

    int timer = duration.count();

    // cout << "\nThe best solution found is:" << endl;
    std::ostringstream ss;    
    best_route.print(true,ss);
    std::string result = ss.str();
    // cout << result << endl;
    float best_cost = objectiveFunction(best_route,instance,distance_matrix);   
    cout << "The cost of this solution is: " << best_cost << endl; 

    // Print to a CSV file
    std::ofstream outFile("real_problems.csv", std::ios::app);

    // Get ID of file
    std::string id = filename.substr(filename.find("/")+1,filename.size());
    id.erase(0,id.find("_")+1);
    id.erase(0,id.find("_")+1);
    id = id.substr(0,id.find("."));

    if (outFile.is_open()) {
        outFile << id << "," << best_cost << "," << float(timer)/1000. << "," << result;
        outFile.close(); // Always close the file
        std::cout << "Data appended successfully.\n";
    } else {
        std::cerr << "Error opening file for appending.\n";
    }

    // if (best_route.isFeasible(instance)) cout << "The route is feasible" << endl;
    // else cout << "The route isn't feasible" << endl;

    // int not_f = 0;
    // vector<int> perm;
    // Solution sol;
    // for (int i=0;i<10000;i++){
        // perm = generate_random_permutation(instance.getnClients());
        // sol = ssplit(perm,instance,distance_matrix);
    //     if (!sol.isFeasible(instance)) not_f++;
    // }
    // cout << "Non feasible solutions: " << not_f << endl;

    return 0;
}