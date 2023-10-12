#include <SFML/Graphics.hpp>
#include <vector>

class Tile {
public:
    int value = 0;
    sf::Sprite image;
    sf::Sprite original;
    std::vector<Tile*> adjacents;
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    int x;
    int y;
    sf::Vector2f position;

    Tile(sf::Texture& t, int x, int y) {
        this->original.setTexture(t);
        this->original.setPosition(this->position);
        this->x = x;
        this->y = y;
        this->position = sf::Vector2f(x * 32.0f, y * 32.0f);
        this->image.setTexture(t);
        this->image.setPosition(this->position);

        isMine = false;
        isRevealed = false;
        isFlagged = false;
    }
    void SetTexture(sf::Texture& set) {
        image.setTexture(set);
    }
    void DrawMine(sf::RenderWindow& gameWindow, sf::Sprite& mine, sf::Sprite& revealed) {
        mine.setPosition(this->position);
        revealed.setPosition(this->position);
        gameWindow.draw(revealed);
        gameWindow.draw(mine);
    }
    void DrawFlagMine(sf::RenderWindow& gameWindow, sf::Sprite& flag, sf::Sprite& mine, sf::Sprite& revealed) {
        flag.setPosition(this->position);
        revealed.setPosition(this->position);
        mine.setPosition(this->position);
        gameWindow.draw(revealed);
        gameWindow.draw(flag);
        gameWindow.draw(mine);
    }
    void DrawFlag(sf::RenderWindow& gameWindow, sf::Sprite& flag, sf::Sprite& hidden) {
        flag.setPosition(this->position);
        hidden.setPosition(this->position);
        gameWindow.draw(hidden);
        gameWindow.draw(flag);
    }
    void DrawRevealed(sf::RenderWindow& gameWindow, sf::Sprite& number, sf::Sprite& reveal) {
        number.setPosition(this->position);
        reveal.setPosition(this->position);
        gameWindow.draw(reveal);
        gameWindow.draw(number);
    }
    void GiveValue() {
        for (int i = 0; i < this->adjacents.size(); i++) {
            if ((adjacents[i])->isMine && !this->isMine) {
                this->value++;
            }
            else if (this->isMine) {
                this->value = 9;
            }
        }
    }
    void DrawBlank(sf::RenderWindow& gameWindow, sf::Sprite& reveal) {
        reveal.setPosition(this->position);
        gameWindow.draw(reveal);

    }
    void AdjacentTiles(std::vector<std::vector<Tile>>& board, int tileX, int tileY, int col, int row) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) 
                    continue; // this is the actual tile we are on
                int nextColumn = tileX + j;
                int nextRow = tileY + i;
                if (nextColumn < 0 || nextColumn >= col || nextRow < 0 || nextRow >= row) 
                    continue;
                this->adjacents.push_back(&board[nextColumn][nextRow]);
                if (board[nextColumn][nextRow].isMine) { this->value++; }
            }
        }
    }
};