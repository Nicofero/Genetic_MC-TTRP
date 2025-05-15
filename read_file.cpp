#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

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
                    ss >> truck_c >> trailer_c >> *(new int) >> truck_N >> trailer_N;
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
};

int main() {
    Instance instance("instances/CHAO_MCTTRP_01.txt");
    instance.print();
    return 0;
}