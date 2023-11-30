# Parsing *Points
Sports Scores/Betting backend application that retrieves the latest scores and odds of games, leagues, or sporting events.

* Have you ever wanted to see the live scores for your favorite sport/team but didn’t want to parse through multiple websites? 
* Wanted to find the best betting odds on a game/event regardless of the bookmaker? 
* Maybe you want to store previous bets to see how successful you are at sport betting.

Parsing *Points has all the features a sport fan needs to place bets and see live scores.

## Features
* Retrieves all in-season(sports)
* Retrieves live and completed scores for the past 3 days.
* Retrieves betting odds from multiple bookmakers and stores in table to reference odds/scores later.
* Allows users to place bets and give a prediction for potential earnings.
* Allows users to save their bets/predictions as a file(BetSlip) to print and take to their favorite sporting event. 

# Build Process
## Installation & Necessary Libraries
The Parsing *Points utilizes various files, functions and libraries to return the Sports Data into a readable format for the user.
Please complete the following steps to successfully run the program.

Clone Git Repository in to desired directory.
```bash
git clone https://github.com/L-sampson/Hackathon-Project.git
```

Obtain an API KEY from [The Odds-API](https://the-odds-api.com/)

 Install the necessary Libraries:
 1. Libcurl library to make HTTP GET Requests from The Odds-API.[Libcurl](https://curl.se/libcurl/c/libcurl.html)
 2. Nlohmann/Json library to parse through JSON objects in c++. [JSON for Modern C++](https://json.nlohmann.me/)
 3. Postgresql pqxx library to connect and store parsed data into SQL tables.[libpqxx](https://pqxx.org/libpqxx/)

## File Tree Structure
The SERVER directory that contains two subdirectories: Logic and Data.
* The Data directory contains files that handle curl requests, URL endpoints and SQL queries functions.
* The Logic directory contains files that handle the parsing through the data based on URL endpoints, placing wagers, and print the BetSlip.
File Tree:
```html
    └── Hackathon-Project
     ├── Server
     │   └── data
     │       ├── curl_request.hpp
     |       ├── curl_request.cpp       
     │       ├── option_url.hpp
     |       ├── option_url.cpp
     │       ├── sql_queries.hpp
     │       └── sql_queries.cpp
     │   └── logic
     │       ├── get_function.hpp
     │       ├── get_functions.cpp
     │       ├── get_bet.cpp
     │       └── get_bet.hpp
     ├── bets.txt
     └── main.cpp
```
## Complie & Run
Complile using the appropriate flags:
```bash
g++ main.cpp -o API ./data/*.cpp ./logic/*.cpp -I./logic -I./data -lcurl -lpqxx
```
Run as Postgres User:
```bash
postgres@penguin:/path/to/directory
```

## Usage
main.cpp 
```c++
#include<iostream>
#include "get_function.hpp"
#include "sql_queries.hpp"
#include "curl_request.hpp"
#include "option_url.hpp"

int main()
{
    //Reads API Key
    OptionURL choice = envFile();
    string option = choice.option;
    string url = choice.url;
    const char *view = url.c_str();

    if (option == "Sports")
    {
        //Returns avaiable sports
        getSports(view);
    }
    else if (option == "Odds")
    {
        //Returns Avaiable Odds
        getOdds(view);
    }
    else if (option == "Scores")
    {
        //Returns Avaiable Scores
        getScores(view);
    }

    return 0;
}
```
Prints Betting Slip
```C++
Betting Slip
Good Luck
***************************************
Team: Detroit Pistons, Wager: 500, Predicted Earnings: 1225
Total Wager Amount: $500
Total Predicted Earnings: $1225
Remaining Deposit: $0
Parsing *Points 🏈🏀🏒⚽🏏⛳🥊🥋
Founded 2023
```
# License
MIT License
Copyright (c)2023 Lavon Sampson, III

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 


