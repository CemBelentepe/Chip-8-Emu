#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include "GameObject.hpp"

class Level
{
protected:
	std::vector<std::unique_ptr<GameObject>> gameObjects;
	std::vector<std::unique_ptr<GameObject>> toAdd;
	std::unordered_map<std::string, sf::Texture> resources;

public:
	virtual ~Level()
	{
	}
	virtual void update(float deltaTime)
	{
		for (auto& go : toAdd)
			gameObjects.push_back(std::move(go));
		toAdd.clear();

		for (int i = gameObjects.size() - 1; i >= 0; i--)
		{
			if (!gameObjects[i]->isAlive())
			{
				gameObjects.erase(gameObjects.begin() + i);
			}
		}

		for (auto& go : gameObjects)
			go->update(deltaTime);
	}
	virtual void render(sf::RenderWindow& window, float deltaTime)
	{
		for (auto& go : gameObjects)
			go->render(window, deltaTime);
	}

	template <typename T, typename... Args>
	T* addGameObject(Args&&... args)
	{
		toAdd.push_back(std::make_unique<T>(std::forward<Args>(args)...));
		return (T*)toAdd.back().get();
	}

	virtual sf::Texture& getTexture(const std::string& str)
	{
		return resources[str];
	}

	std::vector<GameObject*> getGameObjectsWithTag(int tag)
	{
		std::vector<GameObject*> ret;
		for (auto& go : gameObjects)
		{
			if (go->tag == tag)
				ret.push_back(go.get());
		}
		return ret;
	}

	std::vector<GameObject*> getGameObjectsWithTags(std::vector<int> tagList)
	{
		std::vector<GameObject*> ret;
		for (auto& go : gameObjects)
		{
			for (auto& tag : tagList)
			{
				if (go->tag == tag)
					ret.push_back(go.get());
			}
		}
		return ret;
	}
};
