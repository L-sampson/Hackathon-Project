#include <iostream>
#include <curl/curl.h>
#include <fstream>
#include <string.h>
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <pqxx/prepared_statement.hxx>
#include <vector>
#include <map>

#define base_url "https://api.the-odds-api.com/v4/sports/?apiKey="
#define score_url "https://api.the-odds-api.com/v4/sports/"
#define odds_url "https://api.the-odds-api.com/v4/sports/upcoming/odds/?apiKey="

using namespace std;
using namespace nlohmann;

struct OptionURL
{
    string option;
    string url;
};

OptionURL envFile()
{

    ifstream envFile("../apiKey.env");
    // Check if the file is open
    if (!envFile.is_open())
    {
        return {"Error opening .env file"};
    }
    // Read the API key from the file
    string apiKey;
    getline(envFile, apiKey);
    if (apiKey.empty())
    {
        return {"API key not found in .env file"};
    }
    string base = base_url;
    string score = score_url;
    string oddLink = odds_url;
    string option;

    map<string, string> scores = {
        {"NBA", "basketball_nba"},
        {"NFL", "americanfootball_nfl"},
        {"MLB", "baseball_mlb"},
        {"NHL", "icehockey_nhl"},
        {"MMA", "mma_mixed_martial_arts"},
        {"MLS", "soccer_usa_mls"}};

    cout << "Welcome to Parsing *Points\n";
    cout << "What sports data would you like to find today?\nChoose from the following options\n";
    cout << "Sports, Odds, Scores: ";
    cout << "Please type in you selection now\n";

    string url;
    string terminator = "\0";
    getline(cin, option);
    if (option == "Sports")
    {
        base.append(apiKey);
        base.append(terminator);
        url = base;
        // cout<<url<<endl;
    }
    else if (option == "Odds")
    {
        oddLink.append(apiKey);
        oddLink.append(terminator);
        url = oddLink;
        // cout<<url<<endl;
    }
    else if (option == "Scores")
    {
        cout << "\nWhat sport would you like to see the latest scores?\n";
        for (auto it = scores.begin(); it != scores.end(); it++)
        {
            cout << it->first << endl;
        }
        string sport_score;
        getline(cin, sport_score);
        if (scores.find(sport_score) != scores.end())
        {
            sport_score = scores.find(sport_score)->second;
            score += sport_score + "/scores/?apiKey=" + apiKey + terminator;
            url = score;
        }
        else
        {
            cerr << "Could not find sport";
        }
    }
    else
    {
        cerr << "Error: ";
    }
    envFile.close();
    return {option, url};
}

void parse(string &readBuff)
{
    auto p = json::parse(readBuff);
    json pdata = p;

    for (int i = 0; i < pdata.size(); i++)
    {
        string home = pdata[i]["home_team"];
        string away = pdata[i]["away_team"];
        string sport = pdata[i]["sport_title"];
        cout << sport << endl;
        cout << home << " vs " << away << endl;
        for (auto &bookmaker : pdata[i]["bookmakers"])
        {
            cout << bookmaker["title"] << endl;
            for (auto &market : bookmaker["markets"])
            {
                cout << market["last_update"] << endl;
                for (auto &outcome : market["outcomes"])
                {
                    cout << outcome["name"] << " : " << outcome["price"] << endl;
                }
            }
        }
        cout << "\n";
    }
}
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

CURLcode httpRequest(const char *view, string &response)
{
    CURL *curl = curl_easy_init();
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_URL, view);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        cout << "Error downloading web page: " << curl_easy_strerror(res) << endl;
        curl_easy_cleanup(curl);
    }
    curl_easy_cleanup(curl);
    cout << "\nSuccess\n";
    return res;
}

// GET sports methods
void getSports(const char *aSports)
{
    string response;
    CURLcode res = httpRequest(aSports, response);
    if (res == CURLE_OK)
    {
        map<string, string> sports;
        vector<bool> active;
        string status = "Season status: ";
        json league_data = json::parse(response);
        for (int i = 0; i < league_data.size(); i++)
        {
            sports["League Name"] = league_data[i]["title"];
            sports["League Key"] = league_data[i]["key"];
            sports["Sport Group"] = league_data[i]["group"];
            sports["Description"] = league_data[i]["description"];
            active.push_back(league_data[i]["active"]);
            for (auto it = sports.begin(); it != sports.end(); it++)
            {
                cout << it->first << " : " << it->second << endl;
            }
            if (active[i])
            {
                cout << status << "Active";
                cout << endl;
            }
            else
            {
                cout << status << "Inactive";
                cout << endl;
            }
            cout << endl;
            pqxx::connection conn("dbname = sports user = postgres");

        if (conn.is_open())
        {
            cout << "Database opened: " << conn.dbname() << endl;
            pqxx::work l_table(conn);

            // string insertQuery = "INSERT INTO leagues (LeagueName, LeagueKey, SportsType, Description) Values ";
            string insertQuery = "INSERT INTO leagues (LeagueName, LeagueKey, SportsType, Description) VALUES ( "
            + l_table.quote(sports["League Name"]) + ", "
            + l_table.quote(sports["League Key"]) + ", "
            + l_table.quote(sports["Sport Group"]) + ", "
            + l_table.quote(sports["Description"]) + ")";

            l_table.exec(insertQuery);
            l_table.commit();
            cout<<"Data Entered"<<endl;
            conn.disconnect();

            // for (const string &name : title)
            // {
            //     insertQuery += "('" + name + "'),";
            // }
            // insertQuery.pop_back();

            
        }
        else
        {
            cerr << "Can't open database" << endl;
        }
        }

        
        
    }
}

// GET Odds Endpoint
void getOdds(string url)
{
    cout << "\nWhat country would you like to search odds for?\n";
    cout << "Enter one of the following options:\n";
    map<string, string> regions = {
        {"United States", "us"},
        {"United Kingdom", "uk"},
        {"Europen Union", "eu"},
        {"Australia", "au"}};
    cout << "\nCountries : Market\n";
    for (auto itr = regions.begin(); itr != regions.end(); itr++)
    {
        cout << itr->first << endl;
    }
    string region;
    string newReigon;
    getline(cin, region);

    if (regions.find(region) != regions.end())
    {
        newReigon = "&regions=";
        string choice = regions.find(region)->second;
        newReigon.append(choice);
    }
    else
    {
        cout << "Could not find market\n";
    }

    map<string, string> markets = {
        {"Money Line", "h2h"},
        {"Points", "spreads"},
        {"Over/Under", "totals"},
        {"Futures", "outrights"}};

    cout << "\nWhat betting options would you like to bet on?\n";
    for (auto itr = markets.begin(); itr != markets.end(); itr++)
    {
        cout << itr->first << endl;
    }

    string market;
    string newMarket;
    getline(cin, market);

    if (markets.find(market) != markets.end())
    {
        newMarket = "&markets=";
        string choice = markets.find(market)->second;
        newMarket.append(choice);
    }
    else
    {
        cout << "Could not find market\n";
    }
    string end = newReigon.append(newMarket);
    url.append(end);
    string terminator = "\0";
    url.append(terminator);
    // converting into char*
    const char *odds = url.c_str();
    string response;
    CURLcode res = httpRequest(odds, response);
    if (res == CURLE_OK)
    {
        parse(response);
    }
}

// GET Scores
void getScores(const char *aScores)
{
    string response;
    CURLcode res = httpRequest(aScores, response);
    if (res == CURLE_OK)
    {
        // Create a Map that links the scores and teams.
        json score_data = json::parse(response);
        for (int i = 0; i < score_data.size(); i++)
        {
            map<string, string> game_data; // Create a new map for each game

            // Extract data from the current game
            json current_game = score_data[i];
            game_data["League"] = current_game["sport_title"];
            game_data["Start Time"] = current_game["commence_time"];
            game_data["Home Team"] = current_game["home_team"];
            game_data["Away Team"] = current_game["away_team"];
            game_data["ID(event)"] = current_game["id"];

            vector<pair<string, string>> scoreboard;
            json game_scores = score_data[i]["scores"];
            for (int i = 0; i < game_scores.size(); i++)
            {

                pair<string, string> score(game_scores[i]["name"], game_scores[i]["score"]);
                scoreboard.push_back(score);
                // cout<<game_scores[i]["name"]<<" : "<<game_scores[i]["score"]<<endl;
                // game_data["Scoreboard"] = game_scores[i]["name"] + game_scores[i]["score"];
            }
            // Print the game information
            cout << "Game Information for Game " << i + 1 << ":" << endl;
            for (auto it = game_data.begin(); it != game_data.end(); it++)
            {
                cout << it->first << ": " << it->second << endl;
            }
            cout << "Scoreboard: " << endl;
            for (auto &board : scoreboard)
            {
                cout << board.first << " : " << board.second << endl;
            }
            cout << endl;
        }
    }
}
/*Stretch Goals:
Get Historical Odds
Get Event Odds
refactor to make code more DRY!!!*/

int main()
{
    OptionURL choice = envFile();
    string option = choice.option;
    string url = choice.url;
    const char *view = url.c_str();

    if (option == "Sports")
    {
        getSports(view);
    }
    else if (option == "Odds")
    {
        getOdds(view);
    }
    else if (option == "Scores")
    {
        getScores(view);
    }

    return 0;
}