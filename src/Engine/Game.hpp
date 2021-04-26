#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Level;
class GameObject;
class Game
{
private:
    static Game* instance;

    sf::RenderWindow window;
    Level* level;
    Level* nextLevel;
    bool willExit;

    Game(unsigned int width, unsigned int height, const char* title);

public:
    Game(const Game&) = delete;
    void operator=(const Game&) = delete;

    static void run();
    static void exit();

    static sf::Vector2i getMousePosition();
    static void attachLevel(Level* level);

    static sf::Texture& getTexture(const std::string& str);
    static const Level* getLevel();
    static std::vector<GameObject*> getGameObjectsWithTag(int tag);
    static std::vector<GameObject*> getGameObjectsWithTags(std::vector<int> tagList);

    static const sf::RenderWindow& getWindow();
    static Game* getInstance(unsigned int width, unsigned int height, const char* title);
    static void free();
};
