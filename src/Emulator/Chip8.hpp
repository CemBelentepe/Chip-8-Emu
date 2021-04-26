#pragma once
#include "../Engine/GameObject.hpp"

class Chip8 : public GameObject
{
public:
    explicit Chip8(const std::string& filename);
    ~Chip8() = default;

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window, float deltaTime) override;

private:
    void setPixel(size_t i, size_t j, sf::Color color);
    void xorPixel(size_t i, size_t j);
    bool getPixel(size_t i, size_t j);
    void displaySprite(size_t i, size_t j, int h);
    std::string putHex(unsigned int n);
    void cycle();
    void setKeys();

private:
    static const size_t width = 64;
    static const size_t height = 32;
    sf::Image screen;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::RectangleShape border;
    sf::Text display1;
    sf::Text display2;
    sf::Font font;
	float timer;

    std::vector<uint16_t> stack;
    uint8_t memory[0x1000] = {0};
    uint8_t V[16] = {0};
    uint16_t I = 0;
    uint16_t pc = 0x200;
    uint8_t keyState[16] = {0};
    uint8_t delayTimer = 0;
    uint8_t soundTimer = 0;
    bool drawFlag = true;
    uint8_t chip8_fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    sf::Keyboard::Key keyList[16] = {
        sf::Keyboard::Num1,
        sf::Keyboard::Num2,
        sf::Keyboard::Num3,
        sf::Keyboard::Num4,
        sf::Keyboard::Q,
        sf::Keyboard::W,
        sf::Keyboard::E,
        sf::Keyboard::R,
        sf::Keyboard::A,
        sf::Keyboard::S,
        sf::Keyboard::D,
        sf::Keyboard::F,
        sf::Keyboard::Z,
        sf::Keyboard::X,
        sf::Keyboard::C,
        sf::Keyboard::V};
};