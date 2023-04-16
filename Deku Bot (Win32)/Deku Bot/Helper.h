#pragma once
#include <iostream>

// Gets the color of the AI from the user
// Returns an integer; 1 -> White | -1 -> Black
int getUserColor()
{
    // Catches user input
    std::string input;
    char flagSelection;

    // Prompt the user for a color
    std::cout << "AI Color (w/b): " << std::endl;
    std::cin >> input;

    // Convert first character to lowercase
    flagSelection = input[0];
    flagSelection = std::tolower(flagSelection);

    // Check for invalid input
    while (flagSelection != 'w' && flagSelection != 'b')
    {
        // Prompt the user again
        std::cout << "Invalid Input; Please Enter 'w' or 'b': ";
        std::cin >> input;

        // Convert first character to lowercase
        flagSelection = input[0];
        flagSelection = std::tolower(flagSelection);
    }

    // Return the appropriate flag
    return (flagSelection == 'b') ? -1 : 1;
}

// Gets the maximum ammount of time the AI is allowed to search for from the user
// Returns a float representing a time duration in minutes
float getSearchTime()
{
    // Prompt the user for a duration
    std::string input;
    std::cout << "How long may the AI search in minutes? (Negative numbers allow for no limit): ";
    std::cin >> input;

    // Convert the input to a float
    float inputTime = std::stof(input);

    // Check if user chose a dangerous duration
    if (inputTime < 0)
    {
        // Confirm the user's choice
        std::cout << "WARNING: You have selected no limit to the search time. This may have unknown concequences " << std::endl
            << "regarding the application, results, or total time spent computing. If you would like to cap" << std::endl
            << "the search, please enter a positive integer now." << std::endl;

        // Prompt the user again
        std::cin >> input;
        inputTime = std::stof(input);

        // Set the input time to a maximum number
        if (inputTime < 0)
            inputTime = INT32_MAX / 60000;
    }

    return inputTime;
}