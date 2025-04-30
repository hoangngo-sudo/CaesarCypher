#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/caesar_dec.h"
#include "include/caesar_enc.h"
#include "include/subst_dec.h"
#include "include/subst_enc.h"
#include "utils.h"

using namespace std;

// Initialize random number generator in .cpp file for ODR reasons
std::mt19937 Random::rng;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/**
 * Print instructions for using the program.
 */
void printMenu();

/**
 * Character rotation function for Caesar cipher
 */
char rot(char c, int amount);

/**
 * String rotation function for Caesar cipher
 */
string rot(const string& line, int amount);

/**
 * Runs the Caesar cipher encryption routine
 */
void caesarEncryptCommand();

/**
 * Rotate strings by specified amount
 */
void rot(vector<string>& strings, int amount);

/**
 * Remove non-letter characters and convert to uppercase
 */
string clean(const string& s);

/**
 * Split string into vector of words
 */
vector<string> splitBySpaces(const string& s);

/**
 * Join words with spaces between them
 */
string joinWithSpaces(const vector<string>& words);

/**
 * Count words that appear in dictionary
 */
int numWordsIn(const vector<string>& words, const vector<string>& dict);

/**
 * Runs the Caesar cipher decryption routine
 */
void caesarDecryptCommand(const vector<string>& dict);

/**
 * Apply substitution cipher to string
 */
string applySubstCipher(const vector<char>& cipher, const string& s);

/**
 * Run random substitution cipher encryption
 */
void applyRandSubstCipherCommand();

/**
 * Score string using quadgram statistics
 */
double scoreString(const QuadgramScorer& scorer, const string& s);

/**
 * Run English-ness scoring command
 */
void computeEnglishnessCommand(const QuadgramScorer& scorer);

/**
 * Creates and returns a QuadgramScorer using data from english_quadgrams.txt
 */
QuadgramScorer scorer();

/**
 * Decrypt text using substitution cipher
 */
vector<char> decryptSubstCipher(const QuadgramScorer& scorer,
                                const string& ciphertext);

/**
 * Run substitution cipher decryption command
 */
void decryptSubstCipherCommand(const QuadgramScorer& scorer);

/**
 * Run substitution cipher file decryption
 */
void decryptSubstCipherFileCommand(const QuadgramScorer& scorer);

int main() {
  Random::seed(time(NULL));
  string command;

  // load dictionary
  vector<string> dict;
  ifstream file("dictionary.txt");
  if (file.is_open()) {
    string word;
    while (getline(file, word)) {
      dict.push_back(word);
    }
    file.close();
  } else {
    return 1;
  }

  QuadgramScorer scoring = scorer();

  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;

  do {
    printMenu();
    cout << endl << "Enter a command (case does not matter): ";

    // Use getline for all user input to avoid needing to handle
    // input buffer issues relating to using both >> and getline
    getline(cin, command);
    cout << endl;

    if (command == "R" || command == "r") {
      string seed_str;
      cout << "Enter a non-negative integer to seed the random number "
              "generator: ";
      getline(cin, seed_str);
      Random::seed(stoi(seed_str));
    } else if (command == "C" || command == "c") {
      caesarEncryptCommand();
    } else if (command == "D" || command == "d") {
      caesarDecryptCommand(dict);
    } else if (command == "A" || command == "a") {
      applyRandSubstCipherCommand();
    } else if (command == "E" || command == "e") {
      computeEnglishnessCommand(scoring);
    } else if (command == "S" || command == "s") {
      decryptSubstCipherCommand(scoring);
    } else if (command == "F" || command == "f") {
      decryptSubstCipherFileCommand(scoring);
    }

    cout << endl;

  } while (!(command == "x" || command == "X") && !cin.eof());

  return 0;
}

void printMenu() {
  cout << "Ciphers Menu" << endl;
  cout << "------------" << endl;
  cout << "C - Encrypt with Caesar Cipher" << endl;
  cout << "D - Decrypt Caesar Cipher" << endl;
  cout << "E - Compute English-ness Score" << endl;
  cout << "A - Apply Random Substitution Cipher" << endl;
  cout << "S - Decrypt Substitution Cipher from Console" << endl;
  cout << "F - Decrypt Substitution Cipher from File" << endl;
  cout << "R - Set Random Seed for Testing" << endl;
  cout << "X - Exit Program" << endl;
}

#pragma region CaesarEnc

char rot(char c, int amount) {
  // Convert character to number (0-25) using ALPHABET.find()
  int pos = ALPHABET.find(c);
  // Add rotation and wrap around using modulo
  pos = (pos + amount) % 26;
  // Convert back to character using ALPHABET.at()
  return ALPHABET.at(pos);
}

string rot(const string& line, int amount) {
  string result;
  // For each character in the input line…
  for (char c : line) {
    // If the character is a letter (isalpha), then convert it to uppercase
    // (toupper), rotate it by the given amount, and append it to a result
    // string.
    if (isalpha(c)) {
      char capitalized = toupper(c);
      result += rot(capitalized, amount);
      // Otherwise, if the character is a space (isspace), append it to a result
      // string.
    } else if (isspace(c)) {
      result += c;
    }
  }
  return result;
}

void caesarEncryptCommand() {
  // get the text to encrypt
  string txt;
  getline(cin, txt);

  // get the rotation amount
  string rotation;
  getline(cin, rotation);
  int amount = stoi(rotation);

  // perform the encryption
  string enc = rot(txt, amount);
  cout << enc << endl;
}

#pragma endregion CaesarEnc

#pragma region CaesarDec

void rot(vector<string>& strings, int amount) {
  for (string& s : strings) {
    s = rot(s, amount);
  }
}

string clean(const string& s) {
  string result;
  for (char c : s) {
    if (isalpha(c)) {
      result += toupper(c);
    }
  }
  return result;
}

// Function to split the input string into a vector og words based on spaces
vector<string> splitBySpaces(const string& s) {
  vector<string> words;
  string current_word;

  for (char c : s) {
    if (isspace(c)) {
      if (!current_word.empty()) {
        words.push_back(current_word);
        current_word.clear();
      }
    } else {
      current_word += c;
    }
  }

  if (!current_word.empty()) {
    words.push_back(current_word);
  }
  return words;
}

// Function to join a vector of words into a single string with spaces between them.
string joinWithSpaces(const vector<string>& words) {
  string result;
  for (size_t i = 0; i < words.size(); i++) {
    result += words[i];
    if (i < words.size() - 1) {
      result += " ";
    }
  }
  return result;
}

// Function to counts the number of words in the input vector that match words in the dict vector
int numWordsIn(const vector<string>& words, const vector<string>& dict) {
  int count = 0;
  for (const string& word : words) {
    for (const string& dict_word : dict) {
      if (word == dict_word) {
        count++;
        break;
      }
    }
  }
  return count;
}

// Function to decrypt a Caesar cipher by trying all possible rotations and checking against a dict
void caesarDecryptCommand(const vector<string>& dict) {
  // Read the text to decrypt
  string text;
  getline(cin, text);

  // Break into words and clean
  vector<string> words = splitBySpaces(text);
  for (string& word : words) {
    word = clean(word);
  }

  int decryptionsOuput = 0;
  // Try all possible rotations (0-25)
  for (int i = 0; i < 26; i++) {
    // Make a copy of words to rotate
    vector<string> rotated_words = words;
    rot(rotated_words, i);

    // Check if more than half the words are in dictionary
    if (numWordsIn(rotated_words, dict) > rotated_words.size() / 2) {
      cout << joinWithSpaces(rotated_words) << endl;
      decryptionsOuput++;
    }
  }

  if (decryptionsOuput == 0) {
    cout << "No good decryptions found" << endl;
  }
}

#pragma endregion CaesarDec

#pragma region SubstEnc

// Function to apply a substitution cipher to a given string
string applySubstCipher(const vector<char>& cipher, const string& s) {
  string result;
  for (char c : s) {
    if (isalpha(c)) {
      char capitalized = toupper(c);
      int pos = capitalized - 'A'; // uses the position of the uppercase character 
                                   // in the alphabet to find the corresponding 
                                   // character in the cipher
      result += cipher[pos];
    } else {
      result += c; // appends non abc characters unchanged
    }
  }
  return result;
}

// Function to apply a random substitution cipher to a string read from std input
void applyRandSubstCipherCommand() {
  string text;
  getline(cin, text);
  vector<char> cipher = genRandomSubstCipher();
  string enc = applySubstCipher(cipher, text);
  cout << enc << endl;
}

#pragma endregion SubstEnc

#pragma region SubstDec

// Function to initialise and return a QuadgramScorer object
QuadgramScorer scorer() {
  vector<string> quadgrams;  // create vector to store quadgrams

  vector<int> counts;  // create vector to store counts of the occurence
                       // corresponding quadgram

  ifstream file("english_quadgrams.txt");  // Open the file using ifstream
  if (!file.is_open()) {
    exit(1);  // exit if the file cannot be opened
  }
  string line;
  while (getline(file, line)) {
    size_t comma_pos = line.find(
        ',');  // find the comma position to seperate the counts and quadgram
    string quadgram = line.substr(0, comma_pos);   // extract the quadgram
    int count = stoi(line.substr(comma_pos + 1));  // extract and convert counts to int num

    quadgrams.push_back(quadgram);  // store quadgram in vetor
    counts.push_back(count);        // store counts in vector
  }
  return QuadgramScorer(quadgrams, counts);  // Return a QuadgramScorer object
                                             // initialised with the quadgrams and counts
}

// Function to score a given string based on quadgram frequencies
double scoreString(const QuadgramScorer& scorer, const string& s) {
  double scores = 0.0;

  // Clean the input string
  string cleanedString;
  for (char c : s) {
    if (isalpha(c)) {
      cleanedString += toupper(c);
    }
  }

  // Return a very low score if the string is less than 4
  if (cleanedString.length() < 4) {
    return -1000.0;
  }

  for (size_t i = 0; i <= cleanedString.length() - 4;
       i++) {  // iterate over each possible quadgram in a string
    string quadgram = cleanedString.substr(i, 4);
    scores += scorer.getScore(quadgram);  // add its score to the total score if valid
  }

  return scores;
}

void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  string text;
  getline(cin, text);

  // Clean and convert to uppercase
  string cleanText;
  for (char c : text) {
    if (isalpha(c)) {
      cleanText += toupper(c);
    }
  }

  // Calculate score
  double score = scoreString(
      scorer, cleanText);  // Calculate the Englishness score of the text
  cout << score << endl;   // Output the score
}

// Function to decrypt a substitution cipher using hill climbing algorithm
vector<char> decryptSubstCipher(const QuadgramScorer& scorer,
                                const string& ciphertext) {
  // Hill Climbing algorithm to improve on a random starting cipher:
  // *   Start with a random substitution cipher key.
  // *   While fewer than 1000 trials in a row have not resulted in a
  // replacement…
  // *        Randomly swap 2 letters to create a new key
  // *             If the new key gets a higher score…
  // *             Replace the old key with the new key.
  // *        Output the result of the decryption.
  vector<char> bestCipherKey = genRandomSubstCipher();
  string bestDescryption = applySubstCipher(bestCipherKey, ciphertext);
  double bestScore = scoreString(scorer, bestDescryption);

  int trials = 0;
  while (trials < 1000) {
    vector<char> newCipherKey = bestCipherKey;

    int random_letter_pos1 = Random::randInt(25);
    int random_letter_pos2;
    do {
      random_letter_pos2 = Random::randInt(25);
    } while (random_letter_pos2 == random_letter_pos1);

    swap(newCipherKey[random_letter_pos1], newCipherKey[random_letter_pos2]);

    string newDescryption = applySubstCipher(newCipherKey, ciphertext);
    double newScore = scoreString(scorer, newDescryption);

    if (newScore > bestScore) {
      bestCipherKey = newCipherKey;
      bestScore = newScore;
      trials = 0;
    } else {
      trials++;
    }
  }

  return bestCipherKey;
}

// This function reads a ciphertext from standard input,
// * attempts to decrypt it using a hill climbing algorithm multiple times,
// * and outputs the best decryption found.
void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  // Read the ciphertext from standard input
  string text;
  getline(cin, text);

  vector<char> bestCipherKey;
  string bestDescryption;
  double bestScore = -INFINITY;

  // run algorithm 25 times
  for (int i = 0; i < 25; i++) {
    // Decrypt the text using the decryptSubstCipher function
    vector<char> cipherKey = decryptSubstCipher(scorer, text);

    // Apply the obtained cipher key to decrypt the text
    string descryption = applySubstCipher(cipherKey, text);

    // Calculate the score of the decrypted text using the QuadgramScorer
    double score = scoreString(scorer, descryption);

    // Update the best score, cipher key, and decryption if the current score is
    // better
    if (score > bestScore) {
      bestScore = score;
      bestCipherKey = cipherKey;
      bestDescryption = descryption;
    }
  }

  cout << bestDescryption << endl;
}

void decryptSubstCipherFileCommand(const QuadgramScorer& scorer) {
  string input, output;
  cout << "Enter input filename: ";
  getline(cin, input);
  cout << "Enter output filename: ";
  getline(cin, output);

  ifstream infile(input);
  if (!infile.is_open()) {
    return;
  }

  ofstream outfile(output);
  if (!outfile.is_open()) {
    return;
  }

  string ciphertext;
  string line;

  // Read the entire file into a single string
  while (getline(infile, line)) {
    ciphertext += line + "\n";
  }

  vector<char> bestCipherKey;
  string bestDescryption;
  double bestScore = -INFINITY;

  // run algorithm 25 times to get the best result
  for (int i = 0; i < 25; i++) {
    // Decrypt the text using the decryptSubstCipher
    vector<char> cipherKey = decryptSubstCipher(scorer, ciphertext);

    // Apply the obtained cipher key using applySubstCipher
    string decryption = applySubstCipher(cipherKey, ciphertext);

    // Calculate the score of the decrypted text using the QuadgramScorer
    double score = scoreString(scorer, decryption);

    if (score > bestScore) {
      bestScore = score;
      bestCipherKey = cipherKey;
      bestDescryption = decryption;
    }
  }

  outfile << bestDescryption;

  infile.close();
  outfile.close();
  cout << "Decryption complete." << endl;
}

#pragma endregion SubstDec
