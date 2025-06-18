#include "utils.hpp"
#include <ctime>

using namespace std;
// RNG setting
std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));

// Matrix class implementation
Matrix::Matrix(int r, int c) : rows(r), cols(c), data(r * c) {}
Matrix::Matrix() {}

float& Matrix::at(int i, int j) {
    return data[i * cols + j];
}

void Matrix::print() const {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << data[i * cols + j] << " ";
        }
        std::cout << std::endl;
    }
}

// ----------------------------------------------------------------------------------------------

// Client class implementation
Client::Client(int id, int type, float x, float y, std::vector<float> demand) 
    : id(id), type(type), x(x), y(y), demand(std::move(demand)) {}
Client::Client() {};

// Getters and setters for Client class
int Client::getId() { return id; }
int Client::getType() { return type; }
float Client::getX() { return x; }
float Client::getY() { return y; }
std::vector<float> Client::getDemand() { return demand; }
void Client::setId(int id) { this->id = id; }
void Client::setType(int type) { this->type = type; }
void Client::setX(float x) { this->x = x; }
void Client::setY(float y) { this->y = y; }
void Client::setDemand(std::vector<float> demand) { this->demand = demand; }

// ----------------------------------------------------------------------------------------------

// Instance class implementation
Instance::Instance(std::string filename,int mode) {
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

Instance::Instance() {};

void Instance::print() {
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

Matrix Instance::from_csv(string filename){
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

Client Instance::getDepot() { return depot; };
std::vector<Client> Instance::getVc_clients() {
    vector<Client> vc_clients;
    for(Client client: clients){
        if(client.getType()==0){
            vc_clients.push_back(client);
        }
    }
    return vc_clients;
};
std::vector<Client> Instance::getTc_clients() {
    vector<Client> tc_clients;
    for(Client client: clients){
        if(client.getType()==1){
            tc_clients.push_back(client);
        }
    }
    return tc_clients;
};

// Getters for Instance class
vector<Client> Instance::getClients() { return clients; };
int Instance::getnClients() { return nClients; }
int Instance::getTruck_N() { return truck_N; }
int Instance::getTrailer_N() { return trailer_N; }
float Instance::getTruck_comp_c() { return truck_comp_c; }
float Instance::getTrailer_comp_c() { return trailer_comp_c; }
int Instance::getnLoads() { return nLoads; }
int Instance::gettruck_c() { return truck_c; }
int Instance::gettrailer_c() { return trailer_c; }

// Gets all demand vector
std::vector<float> Instance::getAllDemand(){
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

// Gets number of compartments for trucks
int Instance::getNCompartmentsTruck() {
    return int(truck_c / truck_comp_c);
}
int Instance::getNCompartmentsTrailer() {
    return int(trailer_c / trailer_comp_c);
}

// ----------------------------------------------------------------------------------------------

// Solution class implementation
Solution::Solution(){};

void Solution::addRoute(vector<int> route) {
    routes.push_back(route);
}

vector<vector<int>>& Solution::getRoutes(){return routes;};

vector<int> Solution::findElement(int elem){
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

void Solution::print(bool in_line, std::ostream& out){
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

vector<int> Solution::toSingleRoute (){
    vector<int> route;
    for(vector<int> elem: routes){
        route.insert(route.end(),elem.begin(),elem.end());
    }
    return route;
}

int Solution::trailers_needed(Instance &probl){
    int trailers=0;
    for(vector<int> route: routes){
        if(probl.getClients()[route[0]-1].getType() == 0) trailers++;
    }
    return trailers;
}

int Solution::trucks_needed() {return routes.size();}

bool Solution::isFeasible(Instance &probl,bool ignore_tn){
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

vector<int> Solution::to_1D_array(){
    vector<int> aux;
    for(vector<int> rt: routes){
        aux.insert(aux.end(),rt.begin(),rt.end());
    }
    return aux;
}

void print_vector(const vector<int> &veect){
    for (int elem: veect) cout << elem << " ";
    cout << endl;
}