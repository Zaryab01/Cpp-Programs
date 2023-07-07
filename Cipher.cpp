#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <bitset>

using namespace std;

namespace Zaryab{ //Creating own namespace

// Abstract class for all converters
template <typename Z>
class Converter {
public:
    virtual string encode(Z input) = 0;
    virtual Z decode(string input) = 0;
    virtual string get_name() = 0;
};

// Class for Morse code conversion
class MorseConverter : public Converter<string> {
public:
    string encode(string input);
    string decode(string input);
    string get_name() { return "Morse Code"; }
private:
    static const string morseAlphabet[];
};

//Morse dictionary
const string MorseConverter::morseAlphabet[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
    "..-", "...-", ".--", "-..-", "-.--", "--..",
    ".----", "..---", "...--", "....-", ".....",
    "-....", "--...", "---..", "----.", "-----"
};

//Encoding and decoding functions declared outside of class
string MorseConverter::encode(string input) {
    string output;
    for (char c : input) {
        if (isalpha(c)) {
            output += morseAlphabet[toupper(c) - 'A'] + " ";
        }
        else if (isdigit(c)) {
            output += morseAlphabet[c - '0' + 26] + " ";
        }
    }
    return output;
}

string MorseConverter::decode(string input) {
    string output;
    istringstream ss(input);
    string code;
    while (ss >> code) {
        for (int i = 0; i < 36; i++) {
            if (code == morseAlphabet[i]) {
                if (i < 26) {
                    output += static_cast<char>('A' + i);
                }
                else {
                    output += static_cast<char>('0' + i - 26);
                }
                break;
            }
        }
    }
    return output;
}

// Class for binary conversion
class BinaryConverter : public Converter<string> {
public:
    string encode(string input);
    string decode(string input);
    string get_name() { return "Binary"; }
};

//Encoding and decoding functions declared outside of class
string BinaryConverter::encode(string input) {
    string output;
    for (char c : input) {
        output += bitset<8>(c).to_string() + " ";
    }
    return output;
}

string BinaryConverter::decode(string input) {
    string output;
    istringstream ss(input);
    string code;
    while (ss >> code) {
        output += static_cast<char>(bitset<8>(code).to_ulong());
    }
    return output;
}

// Class for Caesar cipher conversion
class CaesarConverter : public Converter<string> {
public:
    string encode(string input);
    string decode(string input);
    string get_name() { return "Caesar Cipher"; }
private:
    static const int alphabetSize = 26;
};

//Encoding and decoding functions declared outside of class
string CaesarConverter::encode(string input) {
    string output;
    int key;
    cout << "Enter key:";
    cin >> key;
    for (char c : input) {
        if (isalpha(c)) {
            char shifted = (toupper(c) - 'A' + key) % alphabetSize + 'A';
            output += shifted;
        } else {
            output += c;
        }
    }
    return output;
}

string CaesarConverter::decode(string input) {
    string output;
    int key;
    for (char c : input) {
        if (isalpha(c)) {
            char shifted = (toupper(c) - 'A' + alphabetSize - key) % alphabetSize + 'A';
            output += shifted;
        } else {
            output += c;
        }
    }
    return output;
}

}

// Class for storing conversion history in a file
class History {
private:
    vector<pair<string, string>> records;
public:
    void add_record(string input, string output, string converter_name);
    void save_history();
    void load_history();

};

void History::add_record(string input, string output, string converter_name) {
    records.push_back(make_pair(input, converter_name + ": " + output));
}

void History::save_history() {
    ofstream file("history.txt");
    if (file.is_open()) {
        for (auto record : records) {
            file << record.first << endl << record.second << endl;
        }
        file.close();
    }
    else {
        cerr << "Error: could not save history" << endl;
    }
}

void History::load_history() {
    ifstream file("history.txt");
    if (file.is_open()) {
        string input, output;
        while (getline(file, input) && getline(file, output)) {
            records.push_back(make_pair(input, output));
        }
        file.close();
    }
}


// Exception class for adding binary conversions
class BinaryAdditionException : public exception {
public:
    const char* what() const throw() {
        return "Binary addition not allowed for non-binary inputs";
    }
};

// Operator overloaded class for binary addition
class BinaryAddition {
public:
    string operator()(string input1, string input2);
};

string BinaryAddition::operator()(string input1, string input2) {
    if (input1.find_first_not_of("01") !=  string::npos || input2.find_first_not_of("01") !=  string::npos) {
        throw BinaryAdditionException();
    }
     bitset<8> bit1( stoi(input1, nullptr, 2));
     bitset<8> bit2( stoi(input2, nullptr, 2));
     bitset<8> sum(bit1.to_ulong() + bit2.to_ulong());
    return sum.to_string();
}


class PageBorder {
public:
  virtual string draw() = 0;
};

class Animation : public PageBorder {
public:
  string draw() {
    string border;
    for (int i = 0; i < 3; i++) {
      border += "____________________\t\t";
      cout << border << "\r" << flush;
      sleep(1); // delay for 1 second
    }
    return border + "\n";
  }
};


int main() {
    // Initialize converters and history
    Zaryab::MorseConverter morseConverter;
    Zaryab::BinaryConverter binaryConverter;
    Zaryab::CaesarConverter caesarConverter;
    History history;
    history.load_history();

    // Main program loop
    while (true) {
        // Ask user for input
        cout << "Enter a string to convert (or type 'exit' to quit): ";
        string input;
        getline(cin, input);
        if (input == "exit") {
            break;
        }

        // Ask user for conversion type
        cout << "Choose conversion type:" << endl;
        cout << "1. Morse Code" << endl;
        cout << "2. Binary" << endl;
        cout << "3. Caesar Cipher" << endl;
        int choice;
        cin >> choice;
        cin.ignore(); // consume newline character

        // Perform conversion
        string output;
        string converter_name;
        Zaryab::Converter<string>* converter;
        switch (choice) {
        case 1:
            converter = &morseConverter;
            output = converter->encode(input);
            break;
        case 2:
            converter = &binaryConverter;
            output = converter->encode(input);
            converter_name = "Binary";
            break;
        case 3:
            converter = &caesarConverter;
            output = converter->encode(input);
            converter_name = "Caesar Cipher";
            break;
        default:
            cout << "Invalid choice" << endl;
            continue;
        }

        // Ask user if they want to add the output to the history
        cout << "Do you want to add the conversion to the history? (y/n) [Case sensitive]: ";
        char add_to_history;
        cin >> add_to_history;
        if (add_to_history == 'y') {
            history.add_record(input, output, converter_name);
        }
        cin.ignore(); // consume newline character

        // Print the output
        cout << "Output: " << output << endl;

        // Ask user if they want to perform binary addition
        if (choice == 2) {
            cout << "Do you want to perform binary addition with the output? (y/n): ";
            char perform_binary_addition;
            cin >> perform_binary_addition;
            if (perform_binary_addition == 'y') {
                string input2;
                cout << "Enter a binary string to add to the output: ";
                cin >> input2;
                BinaryAddition binaryAddition;
				try {
                    
                    string result = binaryAddition(output, input2);
                    cout << "Output after binary addition: " << result << endl;
                }
                catch (BinaryAdditionException e) {
                    cerr << "Error: " << e.what() << endl;
                }
            }
            cin.ignore(); // consume newline character
        }
        
        Animation animation1;
        
        cout << "\n" << animation1.draw() << "\n" << endl;
    }

    // Save history to file
    history.save_history();

    return 0;
}
