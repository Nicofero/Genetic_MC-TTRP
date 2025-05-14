#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>

class Event {
    public:
        std::string name;
        int id;
        struct tm date;
        std::string description;
        std::string location;    
        Event(int id, std::string name, std::string description, std::string location, std::string date){
            this->id = id;
            this->name = name;
            this->description = description;
            this->location = location;
            struct tm date_string;
            strptime(date.c_str(), "%d/%m/%Y %H:%M:%S", &date_string);
            this->date = date_string;
        };
        Event(){};
};

//ID,Name,Description,Location,Date
//1,Meeting,Project meeting,Office,2023-10-01 10:00:00
class Agenda {
    public:
        std::vector<Event> events;
        Agenda(std::string filename){
            events.reserve(100);
            std::ifstream file;
            std::string line;

            file.open(filename);
            if(file.is_open()){
                while(getline(file,line)){
                    Event event;
                    std::string date_str;
                    std::stringstream ss(line);
                    std::getline(ss, line, ',');
                    event.id = std::stoi(line);
                    std::getline(ss, event.name, ',');
                    std::getline(ss, event.description, ',');
                    std::getline(ss, event.location, ',');
                    std::getline(ss, date_str, ',');
                    
                    // Parse the date string
                    strptime(date_str.c_str(), "%d/%m/%Y %H:%M:%S", &event.date);
                    
                    events.push_back(event);
                }
            }else{
                std::cerr << "Error opening file" << std::endl;
            }
        }
        void addEvent(Event event);
        void removeEvent(int id);
        void updateEvent(int id, Event event);
        void displayEvent(int id);
        void displayAllEvents();
        void saveToFile(std::string filename);
};

int main(){

    Agenda agenda("events.txt");
    Event new_event;
    std::string date_str;  
    struct tm date_string;  
    int id;
    // Menu
    int choice=0;
    while(choice != 6){
        std::cout << "====================" << std::endl;
        std::cout << "Agenda Menu" << std::endl;
        std::cout << "1. Add Event" << std::endl;
        std::cout << "2. Remove Event" << std::endl;
        std::cout << "3. Update Event" << std::endl;
        std::cout << "4. Display Event" << std::endl;
        std::cout << "5. Display All Events" << std::endl;
        std::cout << "6. Save and exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore(); // Ignore the newline character left in the buffer

        switch (choice){
        case 1:        
            std::cout << "Enter event name: ";
            std::getline(std::cin, new_event.name);
            std::cout << "Enter event description: ";
            std::getline(std::cin, new_event.description);
            std::cout << "Enter event location: ";
            std::getline(std::cin, new_event.location);
            std::cout << "Enter event date (dd/mm/yyyy hh:mm:ss): ";
            std::getline(std::cin, date_str);           
            strptime(date_str.c_str(), "%d/%m/%Y %H:%M:%S", &date_string);
            new_event.date = date_string;
            new_event.id = agenda.events.size() + 1; // Simple ID assignment
            agenda.addEvent(new_event);
            std::cout << "Event added successfully!" << std::endl;
            break;
        
        case 2:            
            std::cout << "Enter event ID to remove: ";
            std::cin >> id;
            agenda.removeEvent(id);
            std::cout << "Event removed successfully!" << std::endl;
            break;

        case 3:
            std::cout << "Enter event ID to update: ";
            std::cin >> id;
            std::cin.ignore(); // Ignore the newline character left in the buffer
            std::cout << "Enter new event name: ";
            std::getline(std::cin, new_event.name);
            std::cout << "Enter new event description: ";
            std::getline(std::cin, new_event.description);
            std::cout << "Enter new event location: ";
            std::getline(std::cin, new_event.location);
            std::cout << "Enter new event date (dd/mm/yyyy hh:mm:ss): ";
            std::getline(std::cin, date_str);       
            strptime(date_str.c_str(), "%d/%m/%Y %H:%M:%S", &date_string);
            new_event.date = date_string;
            agenda.updateEvent(id, new_event);
            std::cout << "Event updated successfully!" << std::endl;
            break;

        case 4:
            std::cout << "Enter event ID to display: ";
            std::cin >> id;
            std::cin.ignore();
            agenda.displayEvent(id);
            break;

        case 5:
            agenda.displayAllEvents();
            break;

        case 6:
            agenda.saveToFile("events.txt");
            std::cout << "Events saved to file. Exiting..." << std::endl;
            break;

        default:
            agenda.saveToFile("events.txt");
            std::cout << "Events saved to file. Exiting..." << std::endl;
            break;
        }
    }

    return 0;
}

void Agenda::addEvent(Event event){
    events.push_back(event);
}

void Agenda::removeEvent(int id){
    events.erase(events.begin() + id - 1);
    std::cout << "Event removed successfully!" << std::endl;
}

void Agenda::updateEvent(int id, Event event){
    events[id - 1] = event;
}

void Agenda::displayEvent(int id){
    if(id > 0 && id <= events.size()){
        Event event = events[id - 1];
        std::cout << "\n" << std::endl;
        std::cout << "Event ID: " << event.id << std::endl;
        std::cout << "Name: " << event.name << std::endl;
        std::cout << "Description: " << event.description << std::endl;
        std::cout << "Location: " << event.location << std::endl;
        char date_string[50];
        strftime(date_string,50,"%d/%m/%Y %H:%M:%S",&event.date);
        std::cout << "Date: " << date_string << std::endl;
    }else{
        std::cout << "Event not found!" << std::endl;
    }
}

void Agenda::displayAllEvents(){
    for(Event event: events){
        std::cout << "Event ID: " << event.id << std::endl;
        std::cout << "Name: " << event.name << std::endl;
        std::cout << "Description: " << event.description << std::endl;
        std::cout << "Location: " << event.location << std::endl;
        char date_string[50];
        strftime(date_string,50,"%d/%m/%Y %H:%M:%S",&event.date);
        std::cout << "Date: " << date_string << std::endl;
    }
    if(events.empty()){
        std::cout << "No events found!" << std::endl;
    }
    
}

void Agenda::saveToFile(std::string filename){
    std::ofstream file;
    char date_string[50];
    file.open(filename);
    if(file.is_open()){
        for(Event event: events){
            strftime(date_string,50,"%d/%m/%Y %H:%M:%S",&event.date);
            file << event.id << "," << event.name << "," << event.description << "," << event.location << "," 
                 << date_string << std::endl;
        }
    }else{
        std::cerr << "Error opening file" << std::endl;
    }
}