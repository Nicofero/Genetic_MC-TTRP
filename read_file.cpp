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
        std::vector<Client> vc_clients;
        std::vector<Client> tc_clients;
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
                    if(type == 0){
                        vc_clients.push_back(Client(id, type, x, y, demand));
                    }else{
                        tc_clients.push_back(Client(id, type, x, y, demand));
                    }
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
            for (Client client : vc_clients) {
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
            std::cout << "TC clients:" << std::endl;
            for (Client client : tc_clients) {
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
        std::vector<Client> getVc_clients() { return vc_clients; };
        std::vector<Client> getTc_clients() { return tc_clients; };
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
                for(Client client: vc_clients){
                    demand+= client.getDemand()[i];
                }
                for(Client client: tc_clients){
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
        std::vector<int> solution;
        std::vector<float> capacity;
        int nCompTruck;
        int nCompTrailer;
        int nEjes;
        int nTruck;
        int nTrailer;
        int nClients;
        int nLoads;
    public:
        Solution(std::vector<int> solution) : solution(std::move(solution)) {};
        Solution(int nClients, int nTruck, int nTrailer, float truck_c, float trailer_c, float truck_comp_c, float trailer_comp_c, int nLoads){
            int nCompTruck = int(truck_c / truck_comp_c);
            int nCompTrailer = int(trailer_c / trailer_comp_c);
            int nEjes = (nClients+1)*(nClients+1);

            // This is a cromosome with the following structure:
            // [x_{ij}^{kr}] + [y_{ij}^{klv}] + [U] + [V]
            solution.resize((nEjes*nTruck*nTrailer)+(nEjes*nTruck*(nClients+1))+(nCompTruck*nTruck*nLoads*nClients)+(nCompTrailer*nTrailer*nLoads*nClients));
            // [ZT] + [ZL]
            capacity.resize((nCompTruck*nTruck*nLoads*nClients)+(nCompTrailer*nTrailer*nLoads*nClients));
            this->nCompTruck = nCompTruck;
            this->nCompTrailer = nCompTrailer;
            this->nEjes = nEjes;
            this->nTruck = nTruck;
            this->nTrailer = nTrailer;
            this->nClients = nClients;
            this->nLoads = nLoads;
        };
        Solution() {};
        std::vector<int> getSolution() { return solution; };
        std::vector<int> getx() {
            std::vector<int> x;
            for (int i = 0; i < nEjes*nTruck*nTrailer; i++) {
                x.push_back(solution[i]);
            }
            return x;
        }
        std::vector<int> gety() {
            std::vector<int> y;
            for (int i = nEjes*nTruck*nTrailer; i < nEjes*nTruck*nTrailer+(nEjes*nTruck*(nClients+1)); i++) {
                y.push_back(solution[i]);
            }
            return y;
        }
        std::vector<int> getU() {
            std::vector<int> U;
            for (int i = nEjes*nTruck*nTrailer+(nEjes*nTruck*(nClients+1)); i < nEjes*nTruck*nTrailer+(nEjes*nTruck*(nClients+1))+(nCompTruck*nTruck*nLoads*nClients); i++) {
                U.push_back(solution[i]);
            }
            return U;
        }
        std::vector<int> getV() {
            std::vector<int> V;
            for (int i = nEjes*nTruck*nTrailer+(nEjes*nTruck*(nClients+1))+(nCompTruck*nTruck*nLoads*nClients); i < nEjes*nTruck*nTrailer+(nEjes*nTruck*(nClients+1))+(nCompTruck*nTruck*nLoads*nClients)+(nCompTrailer*nTrailer*nLoads*nClients); i++) {
                V.push_back(solution[i]);
            }
            return V;
        }        
        std::vector<float> getCapacity() { return capacity; };
        std::vector<float> getZT() {
            std::vector<float> ZT;
            for (int i = 0; i < nCompTruck*nTruck*nLoads*nClients; i++) {
                ZT.push_back(capacity[i]);
            }
            return ZT;
        }
        std::vector<float> getZL() {
            std::vector<float> ZL;
            for (int i = nCompTruck*nTruck*nLoads*nClients; i < nCompTruck*nTruck*nLoads*nClients+(nCompTrailer*nTrailer*nLoads*nClients); i++) {
                ZL.push_back(capacity[i]);
            }
            return ZL;
        }
        void print() {
            for (int i : solution) {
                std::cout << i << " ";
            }
            std::cout << std::endl;
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

    std::cout << "Start: " << start << "; End: " << end << std::endl;

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
                std::cout << "Start: " << start << "; End: " << end << std::endl;
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

    std::vector<int> p1 = {1,6,3,4,5,2,9,7,8};
    std::vector<int> p2 = {4,3,1,2,6,5,8,9,7};

    std::vector<std::vector<int>> a = PMX(p1,p2);

    std::cout << "Child 1: ";
    for (int i=0;i<9;i++) std::cout << a[0][i] << " "; 
    std::cout << "\nChild 2: ";
    for (int i=0;i<9;i++) std::cout << a[1][i] << " "; 
    std::cout << std::endl;

    // std::cout << "Distance matrix:" << std::endl;
    // distance_matrix.print();
    // Initialize the solution
    // Solution solution(instance.getnClients(), instance.getTruck_N(), instance.getTrailer_N(), instance.gettruck_c(), instance.gettrailer_c(), instance.getTruck_comp_c(), instance.getTrailer_comp_c(), instance.getnLoads());
    // std::cout << "Size of solution:" << solution.getSolution().size() + solution.getCapacity().size() << std::endl;
    // std::cout << "Percentage of x:" << float(solution.getx().size())/float(solution.getSolution().size()) << std::endl;
    // std::cout << "Percentage of y:" << float(solution.gety().size())/float(solution.getSolution().size()) << std::endl;
    // std::cout << "Percentage of U:" << float(solution.getU().size())/float(solution.getSolution().size()) << std::endl;
    // std::cout << "Percentage of V:" << float(solution.getV().size())/float(solution.getSolution().size()) << std::endl;
    // std::cout << "Size of capacity:" << solution.getCapacity().size() << std::endl;
    // std::cout << "Percentage of ZT:" << float(solution.getZT().size())/float(solution.getCapacity().size()) << std::endl;
    // std::cout << "Percentage of ZL:" << float(solution.getZL().size())/float(solution.getCapacity().size()) << std::endl;
    return 0;
}