/**
 * @file RNG.cpp
 * @author Juan Cocina (juancocina1337@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-07
 */

#include <iostream>
#include <vector>
#include <unordered_map> //might use this for lookup
#include <random> //rng
#include <chrono>
#include <string>

// obtain seed from the timer
// typedef std::chrono::high_resolution_clock myClock;
// myClock::time_point beginning = myClock::now();
// myClock::duration d = myClock::now() - beginning;
// unsigned seed = d.count();

// obtain seed from another random number generator 
std::random_device seed_gen;
//random number generator
std::default_random_engine rng(seed_gen());
std::uniform_int_distribution<int> distribution(0,6); // will gen notes from 0 to 6 (notes in a scale)
std::uniform_int_distribution<int> typeDistribution(0,2); // for types (note tie rest)

/* variables to be used later */
// Keys stored in a hashmap for quick lookup O(1)
std::unordered_map<int, std::string> keys = {{1,"C/B#"}, {2,"C#/Db"}, {3,"D"}, {4,"D#/Eb"}, 
                                             {5,"E/Eb"}, {6,"F/E#"}, {7,"F#/Gb"}, {8,"G"}, 
                                             {9,"G#/Ab"}, {10,"A" "A#/Bb"}, {11,"B/Cb"}};

// go back to change to vectors instead of hashmaps
std::unordered_map<std::string, std::vector<std::string>> majors = {
    { "C-Major", {"C", "D", "E","F","G","A", "B"} },
    { "D-Major", {"D", "E", "F#", "G", "A", "B", "C#"} },
    { "E-Major", {"E", "F#", "G#", "A", "B", "C#", "D#"} },
    { "F-Major", {"F", "G", "A", "Bb", "C", "D", "E"} },
    { "G-Major", {"G", "A", "B", "C", "D", "E", "F"} },
    { "A-Major", {"A", "B", "C#", "D", "E", "F#", "G#"} },
    { "B-Major", {"B", "C#", "D#", "E", "F#", "G#", "A#"} },

    { "D-Flat-Major", {"Db", "Eb", "F", "Gb", "Ab", "Bb", "C"} },
    { "C-Sharp-Major", {"C#", "D#", "E#", "F#", "G#", "A#", "B#"} },
    { "G-Flat-Major", {"Gb", "Ab", "Bb", "Cb", "Db", "Eb", "F"} },
    { "F-Sharp-Major", { "F#", "G#", "A#", "B", "C#", "D#", "E#"} },
    { "A-Flat-Major", {"Ab", "Bb", "C", "Db", "Eb", "F", "G"} },
    { "B-Flat-Major", {"Bb", "C", "D", "Eb", "F", "G", "A"} }
};
/* 
    UPDATED to use a vector inside an unorderd_map...
    A vectors lookup is constant using its indexes.
*/
std::unordered_map<std::string, std::vector<std::string>> minors = {
    { "A-Minor", {"A", "B", "C", "D", "E", "F", "G"} },
    { "E-Minor", {"E", "F#", "G", "A", "B", "C", "D"} },
    { "B-Minor", {"B", "C#", "D", "E", "F#", "G", "A"}},
    { "F-Sharp-Minor", {"F#", "G#", "A", "B", "C#", "D", "E"} },
    { "C-Sharp-Minor", {"C#", "D#", "E", "F#", "G#", "A", "B"} },

    { "G-Sharp-Minor", {"G#", "A#", "B", "C#", "D#", "E", "F#"} },
    { "A-Flat-Minor", {"Ab", "Bb", "Cb", "Db", "Eb", "Fb", "Gb"} },

    { "D-Sharp-Minor", {"D#", "E#", "F#", "G#", "A#", "B", "C#"} },
    { "E-Flat-Minor", {"Eb", "F", "Gb", "Ab", "Bb", "Cb", "Db"} },

    { "A-Sharp-Minor", {"A#", "B#", "C#", "D#", "E#", "F#", "G#"} },
    { "B-Flat-Minor", {"Bb", "C", "Db","Eb", "F", "Gb", "Ab"} },

    { "F-Minor-Scale", {"F", "G", "Ab", "Bb", "C", "Db", "Eb"} },
    { "C-Minor-Scale", {"C", "D", "Eb", "F", "G", "Ab", "Bb"} },
    { "G-Minor-Scale", {"G", "A", "Bb", "C", "D", "Eb", "F"} }, 
    { "D-Minor-Scale", {"D", "E", "F", "G", "A", "Bb", "C"} }
};

void introMessage(){
    std::cout << "Welcome to the Random Note Generator!\n";
    std::cout << "Made to be paired with the Behringer TD-3 Analog Bassline Synthesizer.\n\n";

    std::cout << "Here are the available scales... \n\n";
    //for loop to print out major scales
    std::cout << "Major Scales --------------\n";
    for(auto scales: majors){
        std::cout << scales.first << std::endl;
    }
    std::cout << "---------------------------\n\n";

    //for loop to print out minor scales
    std::cout << "Minor Scales --------------\n";
    for(auto scales: minors){
        std::cout << scales.first << std::endl;
    }
    std::cout << "---------------------------\n\n";
}

//generate numbers and push them into the vector...
void generateNumbers(std::vector<int> &numbers, int steps) {
    for(int i = 0; i < steps; i++){
        numbers.push_back(distribution(rng));
    }
}
//generate NOTES / TIES / RESTS
std::string generateType(){
    switch(typeDistribution(rng)){
        case 0: return "";
        case 1: return "*";
        case 2: return "-";
    }
    return "ERROR at generateType()"; // probably not the best way to do this...
}

//print major notes
void printMajorNotes(std::vector<int> numbers, std::string input) {
    std::cout << "Your Notes Are: (* means tie, - means rest)\n";
    for(int i = 0; i < numbers.size(); i++){
        std::cout << majors[input][numbers[i]] << generateType() << " ";
    }
}
//print minor notes
void printMinorNotes(std::vector<int> numbers, std::string input) {
    std::cout << "Your Notes Are: (* means tie, - means rest)\n";
    for(int i = 0; i < numbers.size(); i++){
        std::cout << minors[input][numbers[i]] << generateType() << " ";
    }
}
// I'm sure I could blend the above two functions into one using if statements but I don't really want to


int main() {
    std::string input = "";
    int steps = 0;
    bool validInput = false;
    bool validSteps = false;
    std::vector<int> numbers;

    //print intro message
    introMessage();
    
    //taker user input and check its validity..
    std::cout << "Please Enter Your Desired Scale (as printed above): \n";
    while(!validInput){
        std::cin >> input;
        if(minors.count(input) || majors.count(input)){
            validInput = true;
        }
        else if(input == "close" || input == "Close"){
            std::cout << "Thank You, Goodbye!\n";
            return 0;   // not sure if this is the best course of action...
        }
        else{
            std::cout << "Please Enter A Valid Input or Enter 'Close' to exit the program..." << std::endl;
        }
    }
    //ask for the amount of steps and check its validity
    std::cout << "How many steps would you like (1-16)?\n";
    while(!validSteps){
        std::cin >> steps;
        if(steps < 0 || steps > 16){
            validSteps = false;
            std::cout << "Please Enter A Valid Input or Enter '00' to exit the program...\n";
        }else if(steps == 00) {
            std::cout << "Thank You, Goodbye!\n";
            return 0;
        }else validSteps = true;
    }
    std::cout << "\nYou chose the key of: " << input << " with " << steps << " steps..." << std::endl;
    
    //call random number generator based off input (TO-OD)
    generateNumbers(numbers, steps);

    //find the scale in our major or minor unordered_maps, then print out the NOTES
    if(majors.count(input)) printMajorNotes(numbers, input);
    else if(minors.count(input)) printMinorNotes(numbers, input);
    else { std::cout << "An error has occurd\n"; return 1; }


    std::cout << "\n\nThank You, Goodbye!\n";
    system("PAUSE");
    return 0;
}