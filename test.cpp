#include <iostream>
#include <vector>
using namespace std;

int main() {

    // Printing hello world using cout
    cout << "Hello, World!" << endl;
    vector<int> a = {1,2,3,4};
    for (int elem:a){
        cout << elem << endl;
    }

    return 0;
}