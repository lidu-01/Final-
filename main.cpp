#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>         // For using time functions
#include <algorithm>
#include <cstdlib>      // For general utilities
#include <random>       // For random number generators
using namespace std;
// Flashcard structure representing each flashcard's data
struct Flashcard {
    std::string front;       // The word/question
    std::string back;        // The translation/answer
    int correctCount;        // Number of times answered correctly
    time_t lastReview;       // Last review timestamp
};
// LinguaCards class: handles flashcard management and review
class LinguaCards {
private:
    std::vector<Flashcard> cards;   // List of flashcards 
    const std::string filename = "flashcards.txt";    // File to store cards
// load flasgcards from file (front, back, correctcount, lastReview)
    void loadCards() {
        std::ifstream file(filename);
        if (!file.is_open()) return;    // if file doesn't exist, skip

        std::string front, back;
        int count;
        time_t last;
    // Read each card's data: front, back, correctCount, lastReview
        while (std::getline(file, front) && std::getline(file, back) && file >> count >> last) {
            file.ignore(); // Clear newline
            cards.push_back({front, back, count, last});
        }
        file.close();
    }
// Saves current flashcards to file pon exit or update
    void saveCards() {
        std::ofstream file(filename);
        for (const auto& card : cards) {
            file << card.front << "\n"
                 << card.back << "\n"
                 << card.correctCount << " "
                 << card.lastReview << "\n";
        }
        file.close();
    }

public:
// Constructor: automatically load cards when app starts 
    LinguaCards() {
        loadCards();
    }
// Destructor: automaticall save cards when app ends
    ~LinguaCards() {
        saveCards();
    }
// Add a new flashcard to the list
    void addCard(const std::string& front, const std::string& back) {
        cards.push_back({front, back, 0, time(nullptr)});
        saveCards();
    }
// Review flashcrads using spaced repetition logic 
    void reviewCards() {
        if (cards.empty()) {
            std::cout << "No flashcards available. Add some first!\n";
            return;
        }

        std::vector<Flashcard> dueCards;
        time_t now = time(nullptr);
        // Select cards that are either not mastered or due after 1 day
        for (const auto& card : cards) {
            // Simple spaced repetition: review if not answered correctly 3 times or 1 day has passed
            if (card.correctCount < 3 || difftime(now, card.lastReview) >= 86400) {
                dueCards.push_back(card);
            }
        }

        if (dueCards.empty()) {
            std::cout << "No cards due for review!\n";
            return;
        }
       // Shuffle cards randomly for varied practice  
        std::random_shuffle(dueCards.begin(), dueCards.end());
        // Review each due card
        for (auto& card : dueCards) {
            std::cout << "Front: " << card.front << "\nEnter answer: ";
            std::string answer;
            std::getline(std::cin, answer);

            if (answer == card.back) {
                std::cout << "Correct!\n";
                // Update the original card in the list 
                for (auto& c : cards) {
                    if (c.front == card.front) {
                        c.correctCount++;
                        c.lastReview = time(nullptr);
                    }
                }
            } else {
                std::cout << "Incorrect. Answer: " << card.back << "\n";
                // Penalize and update last review time
                for (auto& c : cards) {
                    if (c.front == card.front) {
                        c.correctCount = std::max(0, c.correctCount - 1);
                        c.lastReview = time(nullptr);
                    }
                }
            }
        }
        saveCards();   // Save updates after reviewing 
    }
// Display flashcard statistics to the user
    void showStats() {
        if (cards.empty()) {
            std::cout << "No cards available.\n";
            return;
        }

        int total = cards.size();
        int mastered = 0;
        // Count cards mastered (answer correctly 3+ times)
        for (const auto& card : cards) {
            if (card.correctCount >= 3) mastered++;
        }
        std::cout << "Total cards: " << total << "\n";
        std::cout << "Mastered cards: " << mastered << " (" << (mastered * 100 / total) << "%)\n";
    }
};
// Main function: entry point for the application 
int main() {
    LinguaCards app;
    std::string choice;
    // Menu loop
    while (true) {
        std::cout << "\nLinguaCards Menu:\n"
                  << "1. Add flashcard\n"
                  << "2. Review flashcards\n"
                  << "3. Show stats\n"
                  << "4. Exit\n"
                  << "Choose an option: ";
        std::getline(std::cin, choice);

        if (choice == "1") {
            std::string front, back;
            std::cout << "Enter front (word): ";
            std::getline(std::cin, front);
            std::cout << "Enter back (translation): ";
            std::getline(std::cin, back);
            app.addCard(front, back);
            std::cout << "Card added!\n";
        } else if (choice == "2") {
            app.reviewCards();
        } else if (choice == "3") {
            app.showStats();
        } else if (choice == "4") {
            break; // Exist the program 
        } else {
            std::cout << "Invalid option. Try again.\n";
        }
    }

    return 0;
}