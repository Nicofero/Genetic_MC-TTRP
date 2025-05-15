#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <array>

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
        Client depot;
        std::vector<Client> vc_clients;
        std::vector<Client> tc_clients;
    public:
        Instance(std::string filename) {
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
                if (counter == 0) { //Define instance
                    ss >> truck_c >> trailer_c >> nClients >> truck_N >> trailer_N;
                    counter++;
                } else if (counter == 1) { // Products and compartments
                    ss >> *(new int) >> truck_comp_c >> trailer_comp_c;
                    counter++;
                } else if (counter == 2){ //Depot
                    ss >> id >> x >> y;
                    depot = Client(id, 0, x, y, demand);
                    counter++;
                } else { // Clients
                    ss >> id >> x >> y >> type;        
                    while(ss >> d){
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

int main() {
    Instance instance("instances/CHAO_MCTTRP_01.txt");
    instance.print();
    // Example of using the distance function
    Matrix distance_matrix;
    distance_matrix = distanceMatrix(instance);
    std::cout << "Distance matrix:" << std::endl;
    distance_matrix.print();
    return 0;
}