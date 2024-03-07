#include <iostream>

enum Comparison { INCREASE, DECREASE, SAME };

Comparison compareValues(int newValue, int previousValue) {
    if (newValue > previousValue) {
        return INCREASE;
    } else if (newValue < previousValue) {
        return DECREASE;
    } else {
        return SAME;
    }
}

int selector(int options[], int length) {
    int option_index = 0;
    int option_choice;

    while (true) {
        // Simulating rot.value() increase or decrease
        char input;
        std::cout << "Enter '+' for increase or '-' for decrease (or any other key to quit): ";
        std::cin >> input;

        if (input == '+') {
            option_index++;
            if (option_index >= length) {
                option_index = 0;
            }
            std::cout << "Value increased." << std::endl;
        } else if (input == '-') {
            option_index--;
            if (option_index < 0) {
                option_index = length - 1;
            }
            std::cout << "Value decreased." << std::endl;
        } else {
            break; // Exit the loop if any other key is pressed
        }

        option_choice = options[option_index];
        std::cout << "Current option choice: " << option_choice << std::endl;
    }

    return option_index;
}