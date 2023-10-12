#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include "Player.cpp"
#include "Tile.cpp"

using namespace std;
using namespace chrono;

void AddToLeaderboard(int rows, int cols, std::string time, std::string player, std::vector<Player>& Players, std::vector<std::string>& originals) {
    std::string numberForm = time.substr(0, 2) + time.substr(3, 2);
    int inputForm = std::stoi(numberForm);
    for (int i = 0; i < 5; i++) {
        std::string Form = Players[i].time_str.substr(0, 2) + Players[i].time_str.substr(3, 2);
        int Form1 = std::stoi(Form);
        if (inputForm < Form1) {
            Player NewPlayer(time, player + '*');
            auto iter = Players.begin() + i;
            Players.insert(iter, NewPlayer);
            Players.pop_back();
            Players.erase(Players.begin() + 4);
            auto iter2 = originals.begin() + i;
            originals.insert(iter2, (time + ',' + player));
            originals.pop_back();
            std::ofstream outputFile("files/leaderboard.txt");
            for (const auto& line : originals) {
                outputFile << line << std::endl;
            }
            outputFile.close();
            break;
        }
    }
}
void SetText(sf::Text& text, float x, float y) {
    sf::FloatRect textRectangl = text.getLocalBounds();
    text.setOrigin(textRectangl.left +  textRectangl.width / 2.0f, textRectangl.top  + textRectangl.height / 2.0f);
    text.setPosition(sf::Vector2f(x, y));
}
void DrawDigit(sf::RenderWindow&  game_window, sf::Sprite& digits_sprite, int num) {
    digits_sprite.setTextureRect(sf::IntRect(21 * num, 0 , 21, 32));
    game_window.draw(digits_sprite);
}
void RevealTiles(Tile& tile, int& tiles) {
    if (tile.value == 0 && !tile.isRevealed && !tile.isFlagged) {
        tile.isRevealed = true;
        tiles++;
        for (int i = 0; i < tile.adjacents.size(); i++) {
            if (!tile.adjacents[i]->isMine) {
                RevealTiles(*tile.adjacents[i], tiles);
            }
        }
    }
    else {
        for (int i = 0; i < tile.adjacents.size(); i++) {
            if (!tile.isRevealed && !tile.isFlagged) {
                tile.isRevealed = true;
                tiles++;

            }
        }
    }
}
bool CheckWon(vector<vector<Tile>>& grid, int rows, int cols) {
    int count = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (!grid[i][j].isRevealed && !grid[i][j].isMine) {
                count++;
            }
        }
    }
    return count == 0;
}
int main(){
    // setup welcome window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Minesweeper");
    int width = 800;
    int height = 600;
    std::string name;
    sf::Font font;
    font.loadFromFile("files/font.ttf");
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
            if (event.type == sf::Event::TextEntered) {
                if (name.length() != 0) {
                    if (event.text.unicode == 13) {
                        window.close();
                    }
                    if (event.text.unicode == 8) {
                        name.erase(name.size() - 1);
                    }
                }
                if (std::isalpha(event.key.code)) {
                    if (name.length() == 10) {

                    }
                    else {
                        if (name.length() == 0) {
                            if (event.text.unicode >= 'a' && event.text.unicode <= 'z') {
                                name = name + static_cast<char>(std::toupper(static_cast<char>(event.text.unicode)));
                            }
                            if (event.text.unicode >= 'A' && event.text.unicode <= 'Z') {
                                name = name + static_cast<char>(event.text.unicode);
                            }
                        }
                        else {
                            if (event.text.unicode >= 'A' && event.text.unicode <= 'Z') {
                                name = name + static_cast<char>(std::tolower(static_cast<char>(event.text.unicode)));
                            }
                            else {
                                if (event.text.unicode >= 'a' && event.text.unicode <= 'z') {
                                    name = name + static_cast<char>(event.text.unicode);
                                }
                            }
                        }
                    }
                }
            }
        }
        window.clear(sf::Color::Blue);
        // Setup
        sf::Text text("Welcome to Minesweeper!", font, 24);
        SetText(text, width / 2.0f, height / 2.0f - 150);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold | sf::Text::Underlined);
        window.draw(text);
        sf::Text text2("Enter Your Name:", font, 20);
        SetText(text2, width / 2.0f, height / 2.0f - 75);
        text2.setFillColor(sf::Color::White);
        text2.setStyle(sf::Text::Bold);
        window.draw(text2);

        // Input Name
        sf::Text text3(name + "|", font, 18);
        SetText(text3, width / 2.0f, height / 2.0f - 45);
        text3.setFillColor(sf::Color::Yellow);
        text3.setStyle(sf::Text::Bold);
        window.draw(text3);
        window.display();
    }
    std::ifstream board("files/board_config.cfg");
    std::string line;
    int cols, rows, mines;
    getline(board, line);
    cols = std::stoi(line);
    getline(board, line);
    rows = std::stoi(line);
    getline(board, line);
    mines = std::stoi(line);
    int startMineCount = mines;
    int numTiles = cols * rows;
    int width2 = cols * 32;
    int height2 = (rows * 32) + 100;

    // Leaderboard setup
    int leaderboardHeight = rows * 16 + 50;
    int leaderboardLength = cols * 16;
    sf::Text textLdrbrd("LEADERBOARD", font, 20);
    textLdrbrd.setStyle(sf::Text::Bold | sf::Text::Underlined);
    textLdrbrd.setFillColor(sf::Color::White);
    SetText(textLdrbrd, leaderboardLength / 2.0f, leaderboardHeight / 2.0f - 120);
    fstream leaderboardFile("files/leaderboard.txt");
    // make leaderboard text
    std::string tempLine;
    std::vector <string> leaderboardTexts;
    std::vector <string> originalLines;
    while (getline(leaderboardFile, tempLine)) {
        originalLines.push_back(tempLine);
        string token;
        int index = 0;
        while ((index = (int) tempLine.find(',')) != string::npos) {
            token = tempLine.substr(0, index);
            leaderboardTexts.push_back(token);
            tempLine.erase(0, static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(index) + 1);
        }
        leaderboardTexts.push_back(tempLine);
    }
    string t1 = "1.\t" + leaderboardTexts[0] + "\t" + leaderboardTexts[1] + "\n\n" +  "2.\t" + leaderboardTexts[2] + "\t" + leaderboardTexts[3] + "\n\n" + "3.\t" + leaderboardTexts[4] + "\t" + leaderboardTexts[5] + "\n\n" + "4.\t" + leaderboardTexts[6] + "\t" + leaderboardTexts[7] + "\n\n" + "5.\t" + leaderboardTexts[8] + "\t" + leaderboardTexts[9] + "\n\n";
    sf::Text textleaderbrd1(t1, font, 18);
    SetText(textleaderbrd1, leaderboardLength / 2.0f, leaderboardHeight / 2.0f + 20);
    textleaderbrd1.setStyle(sf::Text::Bold);
    vector<string> times;
    times.push_back(leaderboardTexts[0]);
    times.push_back(leaderboardTexts[2]);
    times.push_back((leaderboardTexts[4]));
    times.push_back((leaderboardTexts[6]));
    times.push_back((leaderboardTexts[8]));
    vector<Player> Players;
    Player player1(leaderboardTexts[1], leaderboardTexts[0]);
    Players.push_back(player1);
    Player player2(leaderboardTexts[3], leaderboardTexts[2]);
    Players.push_back(player2);
    Player player3(leaderboardTexts[5], leaderboardTexts[4]);
    Players.push_back(player3);
    Player player4(leaderboardTexts[7], leaderboardTexts[6]);
    Players.push_back(player4);
    Player player5(leaderboardTexts[9], leaderboardTexts[8]);
    Players.push_back(player2);
    sf::Text text4("Game Window", font, 24);
    SetText(text4, width / 2.0f, height / 2.0f - 45);
    leaderboardFile.close();

    // load the textures
    sf::Texture* win = new sf::Texture;
    (*win).loadFromFile("files/images/face_win.png");
    sf::Sprite winSpr(*win);
    winSpr.setPosition(((cols / 2.0f) * 32.0f) - 32, (rows + 0.5f) * 32);
    sf::Texture* happy = new sf::Texture;
    (*happy).loadFromFile("files/images/face_happy.png");
    sf::Sprite happySpr(*happy);
    happySpr.setPosition(((cols / 2.0f) * 32.0f) - 32, (rows + 0.5f) * 32);
    sf::Texture* lose = new sf::Texture;
    (*lose).loadFromFile("files/images/face_lose.png");
    sf::Sprite loseSpr(*lose);
    loseSpr.setPosition(((cols / 2.0f) * 32.0f) - 32, (rows + 0.5f) * 32);
    sf::Texture* mine = new sf::Texture;
    (*mine).loadFromFile("files/images/mine.png");
    sf::Sprite mineSprite(*mine);
    sf::Texture* hiddenTile = new sf::Texture;
    (*hiddenTile).loadFromFile("files/images/tile_hidden.png");
    sf::Sprite hiddenTileSprite(*hiddenTile);
    sf::Texture revealedTileTexture;
    revealedTileTexture.loadFromFile("files/images/tile_revealed.png");
    sf::Sprite revealedTileSprite(revealedTileTexture);
    sf::Texture num1;
    num1.loadFromFile("files/images/number_1.png");
    sf::Sprite* num1Sprite = new sf::Sprite(num1);
    sf::Texture num2;
    num2.loadFromFile("files/images/number_2.png");
    sf::Sprite* num2Sprite = new sf::Sprite(num2);
    sf::Texture num3;
    num3.loadFromFile("files/images/number_3.png");
    sf::Sprite* num3Sprite = new sf::Sprite(num3);
    sf::Texture num4;
    num4.loadFromFile("files/images/number_4.png");
    sf::Sprite* num4Sprite = new sf::Sprite(num4);
    sf::Texture num5;
    num5.loadFromFile("files/images/number_5.png");
    sf::Sprite* num5Sprite = new sf::Sprite(num5);
    sf::Texture num6;
    num6.loadFromFile("files/images/number_6.png");
    sf::Sprite* num6Sprite = new sf::Sprite(num6);
    sf::Texture num7;
    num7.loadFromFile("files/images/number_7.png");
    sf::Sprite* num7Sprite = new sf::Sprite(num7);
    sf::Texture num8;
    num8.loadFromFile("files/images/number_8.png");
    sf::Sprite* num8Sprite = new sf::Sprite(num8);
    sf::Texture debug;
    debug.loadFromFile("files/images/debug.png");
    sf::Sprite debugSprite(debug);
    debugSprite.setPosition((cols * 32) -304.0f, (rows + 0.5f) * 32);
    sf::Texture pause;
    pause.loadFromFile("files/images/pause.png");
    sf::Sprite pauseSpr(pause);
    pauseSpr.setPosition((cols * 32) - 240.0f, (rows + 0.5f) * 32);
    sf::Texture play;
    play.loadFromFile("files/images/play.png");
    sf::Sprite playSpr(play);
    playSpr.setPosition((cols * 32) - 240.0f, (rows + 0.5f) * 32);
    sf::Texture leaderboard;
    leaderboard.loadFromFile("files/images/leaderboard.png");
    sf::Sprite leaderboardSprite(leaderboard);
    leaderboardSprite.setPosition((cols * 32) - 176.0f, (rows + 0.5f) * 32);
    sf::Texture flag;
    flag.loadFromFile("files/images/flag.png");
    sf::Sprite flagSprite(flag);
    sf::Texture digits;
    digits.loadFromFile("files/images/digits.png");
    sf::Sprite digitsSprite(digits);

    //  digits for thr clock
    sf::Sprite timer1(digits);
    sf::Sprite timer2(digits);
    sf::Sprite timer3(digits);
    sf::Sprite timer4(digits);

    timer1.setPosition((cols * 32) - 97.0f, 32 * (rows + 0.5f) + 16);
    timer2.setPosition((cols * 32) - 97.0f + 21, 32 * (rows + 0.5f) + 16);
    timer3.setPosition((cols * 32) - 54.0f, 32 * (rows + 0.5f) + 16);
    timer4.setPosition((cols * 32) - 54.0f + 21, 32 * (rows + 0.5f) + 16);

    //  digits for counter
    int firstMn = mines / 10;
    int secondMn = mines % 10;
    int thirdMn = 0;
    sf::Sprite counter1(digits);
    sf::Sprite counter2(digits);
    sf::Sprite counter3(digits);
    counter1.setPosition(12, 32 * (rows + 0.5f) + 16);
    counter2.setPosition(33, 32 * (rows + 0.5f) + 16);
    counter3.setPosition(33 + 21, 32 * (rows + 0.5f) + 16);
    std::vector<std::vector<Tile>>  grid;
    std::vector<Tile> tilesV;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Tile tempTile(*hiddenTile, j, i);
            tilesV.push_back(tempTile);
        }
        grid.push_back(tilesV);
        tilesV.clear();
    }
    // mines
    for (int i = 0; i < mines; i++) {
        int rand_num = std::rand() % rows;
        int rand_num1 = std::rand() % cols;
        grid[rand_num][rand_num1].isMine = true;
        grid[rand_num][rand_num1].value = 11;
    }
    bool debugging = false;

    // clock
    int clockNum1 = 0;
    int clockNum2 = 0;
    int clockNum3 = 0;
    int clockNum4 = 0;
    sf::Sprite button;
    button.setPosition((cols * 32.0f) -240, (rows +0.5f) * 32);
    button.setTexture(pause);
    bool paused = false;
    int timePassed = 0;
    int gameTime = 0;
    auto startTime = high_resolution_clock::now();
    auto pauseTimer = high_resolution_clock::now();

    int count = 0;
    // adjacent tiles
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            count++;
            Tile& tile = grid[i][j];
            tile.AdjacentTiles(grid, i, j, rows, cols);
            //grid[i][j].Give_Value(); broken??
        }
    }
    bool lead;
    bool won;
    bool lost;
    int numTilesRevealed = 0;
    int numWon = 0;
    sf::RenderWindow window2(sf::VideoMode(width2, height2), "Minesweeper");
    while (window2.isOpen()) {
        sf::Event event;
        while (window2.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                // Get the bounding rectangle of the sprite
                sf::FloatRect pauseBounds = pauseSpr.getGlobalBounds();
                sf::FloatRect debugBounds = debugSprite.getGlobalBounds();
                sf::FloatRect leaderboardBounds = leaderboardSprite.getGlobalBounds();
                sf::FloatRect HappyBounds = happySpr.getGlobalBounds();
                // Check if the mouse click is within the sprite bounds

                // restarts the game to beginning
                if (HappyBounds.contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y))) {
                    // clear grid
                    grid.clear();
                    mines = startMineCount;
                    // make new grid
                    for (int i = 0; i < rows; i++) {
                        for (int j = 0; j < cols; j++) {
                            Tile this_tile(*hiddenTile, j, i);
                            tilesV.push_back(this_tile);
                        }
                        grid.push_back(tilesV);
                        tilesV.clear();
                    }
                    // input mines
                    for (int i = 0; i < startMineCount; i++) {
                        int random1 = std::rand() % rows;
                        int random2 = std::rand() % cols;
                        grid[random1][random2].isMine = true;
                        grid[random1][random2].value = 11;
                    }
                    debugging = false;
                    // GIVES NEIGHBOR TILES
                    for (int i = 0; i < rows; i++) {
                        for (int j = 0; j < cols; j++) {
                            count++;
                            Tile& tile = grid[i][j];
                            tile.AdjacentTiles(grid, i, j, rows, cols);
                        }
                    }
                    numWon = 0;
                    won = false;
                    paused = false;
                    lost = false;
                    lead = false;
                    int numRevealedTiles = 0;
                    startTime = high_resolution_clock::now();
                    pauseTimer = high_resolution_clock::now();
                }
                if (pauseBounds.contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)) && !lost && !won) {
                    if (!paused) {
                        pauseTimer = high_resolution_clock::now();
                        for (int i = 0; i < rows; i++) {
                            for (int j = 0; j < cols; j++) {
                                grid[i][j].SetTexture(revealedTileTexture);
                            }
                        }
                        button.setTexture(play);
                        paused = true;
                    }
                    else if (!lead) {
                        auto resume = high_resolution_clock::now();
                        for (int i = 0; i < rows; i++) {
                            for (int j = 0; j < cols; j++) {
                                if (!grid[i][j].isRevealed) {
                                    grid[i][j].SetTexture(*hiddenTile);
                                }
                            }
                        }
                        button.setTexture(pause);
                        paused = false;
                        auto time_elapsed = duration_cast<seconds>(resume - pauseTimer);
                        timePassed += (int) time_elapsed.count();
                    }
                }
                if (debugBounds.contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)) && !paused && !lost) {
                    if (!debugging) {
                        debugging = true;
                    }
                    else {
                        debugging = false;
                    }
                }
                if (leaderboardBounds.contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y))) {
                    if (!paused) {
                        paused = true;
                        lead = true;
                        pauseTimer = high_resolution_clock::now();
                        for (int i = 0; i < rows; i++) {
                            for (int j = 0; j < cols; j++) {
                                grid[i][j].SetTexture(revealedTileTexture);
                            }
                        }
                        sf::RenderWindow window3(sf::VideoMode(leaderboardLength, leaderboardHeight), "Leaderboard");
                        while (window3.isOpen()) {
                            sf::Event event2;
                            while (window3.pollEvent(event2)) {
                                if (event2.type == sf::Event::Closed) {
                                    window3.close();
                                    auto resume = high_resolution_clock::now();
                                    for (int i = 0; i < rows; i++) {
                                        for (int j = 0; j < cols; j++) {
                                            if (!grid[i][j].isRevealed) {
                                                grid[i][j].SetTexture(*hiddenTile);
                                            }
                                        }
                                    }

                                    auto timeElapsed = duration_cast<seconds>(resume - pauseTimer);
                                    timePassed += (int) timeElapsed.count();
                                    paused = false;
                                    lead = false;
                                }
                            }
                            window3.clear(sf::Color::Blue);
                            window3.draw(textLdrbrd);
                            if (!won) window3.draw(textleaderbrd1);
                            if (won) {
                                fstream leaderboardFile("files/leaderboard.txt");
                                // make leaderboard text
                                string line;
                                vector <string> file_texts;
                                vector <string> lines;  // is this broken??
                                while (getline(leaderboardFile, line)) {
                                    lines.push_back(line);
                                    int index = 0;
                                    while ((index = (int) line.find(',')) != string::npos) {
                                        lines.push_back(line.substr(0, index));
                                        line.erase(0, static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(index) + 1);
                                    }
                                    file_texts.push_back(line);
                                }
                                string t2 = "1.\t"  + file_texts[0] +  "\t" + file_texts[1] +  "\n\n" + "2.\t"  + file_texts[2] + "\t" + file_texts[3] + "\n\n" + "3.\t" + file_texts[4] + "\t" + file_texts[5] + "\n\n" + "4.\t" + file_texts[6] + "\t" + file_texts[7] + "\n\n" + "5.\t" + file_texts[8] + "\t" + file_texts[9] + "\n\n";
                                sf::Text textL2(t2, font, 18);
                                SetText(textL2, leaderboardLength / 2.0f, leaderboardHeight / 2.0f + 20);
                                textleaderbrd1.setStyle(sf::Text::Bold);
                                window3.draw(textL2);
                            }
                            window3.display();
                            lead = true;
                            paused = true;
                        }
                        paused = false;
                        lead = false;
                    }
                }
                // checks if tile is clicked
                for (int i = 0; i < rows; i++) {
                    for (int j = 0; j < cols; j++) {
                        sf::FloatRect tileBounds = grid[i][j].image.getGlobalBounds();
                        if (tileBounds.contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)) && !paused && !lead) {
                            if (!grid[i][j].isRevealed && !grid[i][j].isFlagged && !paused && !lead && !grid[i][j].isMine && !lost && !won) {
                                RevealTiles(grid[i][j], numTilesRevealed);
                                int target = rows * cols - startMineCount;
                            }
                            //   if they lose the game
                            else if (!grid[i][j].isRevealed && grid[i][j].isMine && !paused && !lead && !grid[i][j].isFlagged) {
                                window2.draw(loseSpr);
                                debugging = true;
                                lost = true;
                                numTilesRevealed = 0;
                            }
                        }
                    }
                }
                won = CheckWon(grid, rows, cols);
            }
            if (event.mouseButton.button == sf::Mouse::Right&& event.type == sf::Event::MouseButtonPressed) {
                for (int i = 0; i < rows; i++) {
                    for (int j = 0; j < cols; j++) {
                        sf::FloatRect tileBounds = grid[i][j].image.getGlobalBounds();
                        if (tileBounds.contains(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)) && !paused) {
                            if (!grid[i][j].isFlagged && !grid[i][j].isRevealed) {
                                grid[i][j].isFlagged = true;
                                mines--;
                            }
                            else if (grid[i][j].isRevealed) {
                                continue;
                            }
                            else if (!grid[i][j].isRevealed && grid[i][j].isFlagged) {
                                grid[i][j].isFlagged = false;
                                mines++;
                            }
                        }
                    }
                }
            }
        }
        if (!paused && !won && !lost) {
            auto ending = high_resolution_clock::now();
            auto elapsed = duration_cast<seconds>(ending - startTime).count();
            elapsed = elapsed - timePassed;
            clockNum1 = (int) (elapsed / 60) / 10;
            clockNum2 = (elapsed / 60) % 10;
            clockNum3 = (elapsed % 60) / 10;
            clockNum4 = (elapsed % 60) % 10;
        }
        window2.clear(sf::Color::White);
        if (!won && !lost) window2.draw(happySpr);
        else if (lost) window2.draw(loseSpr);
        else if (won) window2.draw(winSpr);
        window2.draw(debugSprite);
        window2.draw(button);
        window2.draw(leaderboardSprite);

        // clock
        DrawDigit(window2, timer1, clockNum1);
        DrawDigit(window2, timer2, clockNum2);
        DrawDigit(window2, timer3, clockNum3);
        DrawDigit(window2, timer4, clockNum4);
        // counter
        firstMn = abs(mines) / 10;
        secondMn = abs(mines) % 10;
        if (mines < 0) thirdMn = 10; 
            else  thirdMn = 0;
        DrawDigit(window2, counter2, firstMn);
        DrawDigit(window2, counter3, secondMn);
        DrawDigit(window2, counter1, thirdMn);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (debugging && !paused) {
                    if (grid[i][j].isMine) {
                        grid[i][j].DrawMine(window2, mineSprite, revealedTileSprite);
                    }
                    else if (grid[i][j].isFlagged) {
                        grid[i][j].DrawFlag(window2, flagSprite, hiddenTileSprite);
                    }
                    else if (grid[i][j].isRevealed && !grid[i][j].isMine) {
                        if (grid[i][j].value == 0) 
                            grid[i][j].DrawBlank(window2, revealedTileSprite);
                        else if (grid[i][j].value == 1) 
                            grid[i][j].DrawRevealed(window2, *num1Sprite, revealedTileSprite);
                        else if (grid[i][j].value == 2) 
                            grid[i][j].DrawRevealed(window2, *num2Sprite, revealedTileSprite);
                        else if (grid[i][j].value == 3) 
                            grid[i][j].DrawRevealed(window2, *num3Sprite, revealedTileSprite);
                        else if (grid[i][j].value == 4) 
                            grid[i][j].DrawRevealed(window2, *num4Sprite, revealedTileSprite);
                        else if (grid[i][j].value == 5) 
                            grid[i][j].DrawRevealed(window2, *num5Sprite, revealedTileSprite);
                        else if (grid[i][j].value == 6) 
                            grid[i][j].DrawRevealed(window2, *num6Sprite, revealedTileSprite);
                        else if (grid[i][j].value == 7) 
                            grid[i][j].DrawRevealed(window2, *num7Sprite, revealedTileSprite);
                        else if (grid[i][j].value == 8) 
                            grid[i][j].DrawRevealed(window2, *num8Sprite, revealedTileSprite);
                    }
                    else if (grid[i][j].isRevealed && grid[i][j].isMine) {
                        grid[i][j].DrawRevealed(window2, mineSprite, revealedTileSprite);

                    }
                    else {
                        sf::Sprite this_sprite(grid[i][j].image);
                        window2.draw(this_sprite);

                    }
                }
                else if (paused)
                    grid[i][j].DrawBlank(window2, revealedTileSprite);
                else if (grid[i][j].isFlagged && !grid[i][j].isMine && !grid[i][j].isRevealed)
                    grid[i][j].DrawFlag(window2, flagSprite, hiddenTileSprite);
                else if (grid[i][j].isFlagged && grid[i][j].isMine && !grid[i][j].isRevealed && won)
                    grid[i][j].DrawFlag(window2, flagSprite, hiddenTileSprite);
                else if (grid[i][j].isRevealed && !grid[i][j].isMine) {
                    if (grid[i][j].value == 0) { grid[i][j].DrawBlank(window2, revealedTileSprite); }
                    else if (grid[i][j].value == 1) 
                        grid[i][j].DrawRevealed(window2, *num1Sprite, revealedTileSprite);
                    else if (grid[i][j].value == 2)
                        grid[i][j].DrawRevealed(window2, *num2Sprite, revealedTileSprite);
                    else if (grid[i][j].value == 3)
                        grid[i][j].DrawRevealed(window2, *num3Sprite, revealedTileSprite);
                    else if (grid[i][j].value == 4)
                        grid[i][j].DrawRevealed(window2, *num4Sprite, revealedTileSprite);
                    else if (grid[i][j].value == 5)
                        grid[i][j].DrawRevealed(window2, *num5Sprite, revealedTileSprite);
                    else if (grid[i][j].value == 6)
                        grid[i][j].DrawRevealed(window2, *num6Sprite, revealedTileSprite);
                    else if (grid[i][j].value == 7)
                        grid[i][j].DrawRevealed(window2, *num7Sprite, revealedTileSprite);
                    else if (grid[i][j].value == 8)
                        grid[i][j].DrawRevealed(window2, *num8Sprite, revealedTileSprite);
                }
                else if (grid[i][j].isRevealed && grid[i][j].isMine && !grid[i][j].isFlagged && won) {
                    grid[i][j].DrawFlagMine(window2, flagSprite, mineSprite, revealedTileSprite);
                }
                else if (grid[i][j].isRevealed && grid[i][j].isMine && grid[i][j].isFlagged) {
                    grid[i][j].DrawFlagMine(window2, flagSprite, mineSprite, revealedTileSprite);
                }
                else {
                    sf::Sprite this_sprite(grid[i][j].image);
                    window2.draw(this_sprite);
                }
            }
        }
        if (won) {
            numWon++;
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    if (grid[i][j].isMine) {
                        grid[i][j].DrawFlag(window2, flagSprite, hiddenTileSprite);
                    }
                }
            }
            if (numWon==1) {
                int clockNum;
                string clock_num_str = to_string(clockNum1) + to_string(clockNum2) + to_string(clockNum3) + to_string(clockNum4);
                clockNum = stoi(clock_num_str);
                string finishTime;
                if (clockNum/10 == 0) {
                    finishTime = "00:0"  + to_string(clockNum);
                }
                else if (clockNum / 10 < 10) {
                    finishTime = "00:" +  to_string(clockNum);
                }
                else if (clockNum / 100 < 10) {
                    string x = to_string(clockNum);
                    finishTime = "0" + x.substr(0, 1)  + ":" + x.substr(1);
                }
                else if (clockNum / 1000 < 10) {
                    string x = to_string(clockNum);
                    finishTime = x.substr(0, 1) + x.substr(1, 1) + ":" + x.substr(2);
                }
                else
                    finishTime = to_string(clockNum);
                std::cout << clockNum << endl;
                std::cout << finishTime;
                AddToLeaderboard(rows, cols, finishTime, name, Players, originalLines);
            }
        } else if (lost) {
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    if (grid[i][j].isMine && grid[i][j].isFlagged) {
                        grid[i][j].DrawFlagMine(window2, flagSprite, mineSprite, revealedTileSprite);
                    }
                }
            }
        } else if (!paused) {
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    if (grid[i][j].isMine && grid[i][j].isFlagged) {
                        grid[i][j].DrawFlag(window2, flagSprite, hiddenTileSprite);
                    }
                }
            }
        }
        window2.display();
    }
    return 0;
}