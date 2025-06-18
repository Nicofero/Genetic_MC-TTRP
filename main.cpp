#include "utils.hpp"
#include "memetic_core.hpp"
#include <omp.h>

using namespace std;

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
    std::ofstream outFile("metrics/real_problems.csv", std::ios::app);

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

    return 0;
}