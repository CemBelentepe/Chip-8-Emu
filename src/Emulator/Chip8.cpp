#include "Chip8.hpp"
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

Chip8::Chip8(const std::string& filename)
{
    srand(time(nullptr));

    font.loadFromFile("res/terminus.ttf");

    screen.create(width, height, sf::Color::Black);
    texture.loadFromImage(screen);
    sprite.setTexture(texture);

    float scl = 1.0f * Game::getWindow().getSize().x / width;
    sprite.setScale(scl, scl);
    border.setSize(sf::Vector2f(Game::getWindow().getSize().x - 4, Game::getWindow().getSize().y - scl * height - 8));
    border.setPosition(2, scl * height + 4);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color::White);
    border.setOutlineThickness(4);

    std::ifstream file(filename, std::ios::binary);
    if (file.fail())
    {
        std::cout << "[ERROR] File: " << filename << " does not exist." << std::endl;
        Game::exit();
    }
    file.read((char*)(&memory[0x200]), 0x1000 - 0x200);
    file.close();

    display1.setFont(font);
    display1.setPosition(border.getPosition());
    display1.setFillColor(sf::Color::White);
    display1.setCharacterSize(16);

    display2.setFont(font);
    display2.setPosition(border.getPosition() + sf::Vector2f(60, 0));
    display2.setFillColor(sf::Color::White);
    display2.setCharacterSize(16);

    for (int i = 0; i < 80; i++)
        memory[i] = chip8_fontset[i];
}

void Chip8::update(float deltaTime)
{
    if (timer < 0)
    {
        timer = 1 / 300.0f;
        std::stringstream code1;
        for (size_t i = 0; i < 24; i += 2)
            code1 << putHex(memory[pc + i]) << putHex(memory[pc + i + 1]) << "\n";
        display1.setString(code1.str());

        std::stringstream code2;
        for (size_t i = 24; i < 48; i += 2)
            code2 << putHex(memory[pc + i]) << putHex(memory[pc + i + 1]) << "\n";
        display2.setString(code2.str());

        cycle();
        if (delayTimer > 0)
            delayTimer--;
        if (soundTimer > 0)
        {
            soundTimer--;
            if (soundTimer == 0)
                std::cout << "BEEP!" << std::endl;
        }

        if (drawFlag)
        {
            drawFlag = false;
            texture.loadFromImage(screen);
            sprite.setTexture(texture);
        }

        setKeys();
    }
    timer -= deltaTime;
}

void Chip8::render(sf::RenderWindow& window, float deltaTime)
{
    window.draw(sprite);
    window.draw(border);
    window.draw(display1);
    window.draw(display2);

    float scl = 1.0f * Game::getWindow().getSize().x / width;
    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(40, 40));
    rect.setOutlineThickness(2);
    rect.setOutlineColor(sf::Color::White);
    for (int i = 0; i < 16; i++)
    {
        int x = i % 4;
        int y = i / 4;
        rect.setPosition(x * 40 + 32 * scl, y * 40 + 32 * scl + 12);
        rect.setFillColor(keyState[i] ? sf::Color::Red : sf::Color::Transparent);
        window.draw(rect);
    }
}

void Chip8::setPixel(size_t i, size_t j, sf::Color color)
{
    screen.setPixel(i, j, color);
}

void Chip8::xorPixel(size_t i, size_t j)
{
    sf::Color col = screen.getPixel(i, j);
    if (col.r == 0)
        screen.setPixel(i, j, sf::Color::White);
    else
        screen.setPixel(i, j, sf::Color::Black);
}

bool Chip8::getPixel(size_t i, size_t j)
{
    return screen.getPixel(i, j).r != 0;
}

void Chip8::displaySprite(size_t i, size_t j, int h)
{
    V[15] = 0;
    for (int n = 0; n < h; n++)
    {
        uint8_t row = memory[I + n];
        for (int m = 0; m < 8; m++)
        {
            if (((row >> (7 - m)) & 0b1) > 0)
            {
                V[15] = getPixel(i, j);
                xorPixel(i + m, j + n);
            }
        }
    }
}

std::string Chip8::putHex(unsigned int n)
{
    std::stringstream ss;
    ss << std::hex << std::internal << std::setfill('0') << std::right << std::setw(2) << std::uppercase << n;
    return ss.str();
}

void Chip8::cycle()
{
    uint16_t opcode = memory[pc] << 8 | memory[pc + 1];
    pc += 2;

    switch (opcode & 0xF000)
    {
    case 0x0000:
    {
        if (opcode == 0x00E0)
        {
            screen.create(width, height, sf::Color::Black);
            drawFlag = true;
            return;
        }
        if (opcode == 0x00EE)
        {
            pc = stack.back();
            stack.pop_back();
            return;
        }
        break;
    }
    case 0x1000:
    {
        pc = opcode & 0x0FFF;
        return;
    }
    case 0x2000:
    {
        stack.push_back(pc);
        pc = opcode & 0x0FFF;
        return;
    }
    case 0x3000:
    {
        int x = (opcode & 0x0F00) >> 8;
        int n = opcode & 0x00FF;
        if (V[x] == n)
            pc += 2;
        return;
    }
    case 0x4000:
    {
        int x = (opcode & 0x0F00) >> 8;
        int n = opcode & 0x00FF;
        if (V[x] != n)
            pc += 2;
        return;
    }
    case 0x5000: // 5XY0
    {
        int x = (opcode & 0x0F00) >> 8;
        int y = (opcode & 0x00F0) >> 4;
        if (V[x] == V[y])
            pc += 2;
        return;
    }
    case 0x6000:
    {
        int x = (opcode & 0x0F00) >> 8;
        int n = opcode & 0x00FF;
        V[x] = n;
        return;
    }
    case 0x7000:
    {
        int x = (opcode & 0x0F00) >> 8;
        int n = opcode & 0x00FF;
        V[x] += n;
        return;
    }
    case 0x8000:
    {
        int x = (opcode & 0x0F00) >> 8;
        int y = (opcode & 0x00F0) >> 4;
        int type = opcode & 0x000F;
        switch (type)
        {
        case 0x0:
            V[x] = V[y];
            return;
        case 0x1:
            V[x] |= V[y];
            return;
        case 0x2:
            V[x] &= V[y];
            return;
        case 0x3:
            V[x] ^= V[y];
            return;
        case 0x4:
        {
            uint16_t sum = uint16_t(V[x]) + uint16_t(V[y]);
            V[15] = sum > 0xFF;
            V[x] = sum;
            return;
        }
        case 0x5:
        {
            V[15] = V[y] <= V[x];
            V[x] = V[x] - V[y];
            return;
        }
        case 0x6:
        {
            V[15] = V[x] & 0b1;
            V[x] >>= 1;
            return;
        }
        case 0x7:
        {
            V[15] = V[y] > V[x];
            V[x] = V[y] - V[x];
            return;
        }
        case 0xE:
        {
            V[15] = V[x] & (0b1 << 15);
            V[x] <<= 1;
            return;
        }
        }
        break;
    }
    case 0x9000:
    {
        int x = (opcode & 0x0F00) >> 8;
        int y = (opcode & 0x00F0) >> 4;
        if (V[x] != V[y])
            pc += 2;
        return;
    }
    case 0xA000:
    {
        I = opcode & 0x0FFF;
        return;
    }
    case 0xB000:
    {
        int n = opcode & 0x0FFF;
        pc = V[0] + n;
        return;
    }
    case 0xC000:
    {
        int x = (opcode & 0x0F00) >> 8;
        int n = opcode & 0x00FF;
        V[x] = (rand() >> 8) + n;
        return;
    }
    case 0xD000:
    {
        int x = (opcode & 0x0F00) >> 8;
        int y = (opcode & 0x00F0) >> 4;
        int n = opcode & 0x000F;
        displaySprite(V[x], V[y], n);
        drawFlag = true;
        return;
    }
    case 0xE000:
    {
        int x = (opcode & 0x0F00) >> 8;
        switch (opcode & 0x00FF)
        {
        case 0x9E:
            if (keyState[V[x]])
                pc += 2;
            return;
        case 0xA1:
            if (!keyState[V[x]])
                pc += 2;
            return;
        }
        break;
    }
    case 0xF000:
    {
        int x = (opcode & 0x0F00) >> 8;
        switch (opcode & 0x00FF)
        {
        case 0x07:
            V[x] = delayTimer;
            return;
        case 0x0A:
        {
            int press = -1;
            while (press < 0)
            {
                for (int i = 0; i < 16; i++)
                {
                    if (sf::Keyboard::isKeyPressed(keyList[i]) && (keyState[keyList[i]] & 0b01) == 0b00)
                    {
                        press = i;
                        break;
                    }
                }
            }
            V[x] = press;
            return;
        }
        case 0x15:
        {
            delayTimer = V[x];
            return;
        }
        case 0x18:
        {
            soundTimer = V[x];
            return;
        }
        case 0x1E:
        {
            I += V[x];
            return;
        }
        case 0x29:
        {
            I = V[x] * 5;
            return;
        }
        case 0x33:
        {
            int val = V[x];
            int d3 = val / 100;
            int d2 = (val / 10) % 10;
            int d1 = val % 10;

            memory[I + 0] = d3;
            memory[I + 1] = d2;
            memory[I + 2] = d1;
            return;
        }
        case 0x55:
        {
            for (int i = 0; i <= x; i++)
            {
                memory[I + i] = V[i];
            }
            return;
        }
        case 0x65:
        {
            for (int i = 0; i <= x; i++)
            {
                V[i] = memory[I + i];
            }
            return;
        }
        }
        break;
    }
    }
    std::cout << "[ERROR] Opcode 0x" << putHex(opcode) << " is not implemented!" << std::endl;
}

void Chip8::setKeys()
{
    for (int i = 0; i < 16; i++)
        keyState[i] = sf::Keyboard::isKeyPressed(keyList[i]);
}
