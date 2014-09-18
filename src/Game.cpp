#include "Game.h"

Game::Game()
{
	state = MENU;
	current_character_id = 0;
	current_level_id = 0;
	attention_tile = sf::Vector2f(0, 0);
	opponent = NULL;
}

bool Game::isWindowOpen()
{
	return window.isOpen();
}

void Game::init()
{
	srand(time(NULL));

	music.openFromFile("data/music.wav");
	music.setLoop(true);

	font.loadFromFile("data/fixedsys.ttf");

	level_list.push_back(Level("data/level0.txt"));
	level_list.push_back(Level("data/level1.txt"));

	ifstream file;
	int x, y, loc, dest;
	std::string name;
	int temp;

	file.open("data/characters.txt");
	while(file >> x >> y >> loc >> name) character_list.push_back(Character(sf::Vector2f(x, y), loc, name));
	file.close();
	Character* cchar = &character_list[current_character_id];
	camera.setHandles(cchar->getPosHandle(), cchar->getShiftHandle());

	file.open("data/settings.txt");
	file >> temp;
	current_level_id = temp;
	file >> temp;
	current_character_id = temp;
	file.close();

	file.open("data/doors.txt");
	while(file >> x >> y >> loc >> dest) door_list.push_back(Door(sf::Vector2f(x,y), loc, dest));
	file.close();

	main_menu.addElement("New game");
	main_menu.addElement("Options");
	main_menu.addElement("Exit");

	window.create(sf::VideoMode(GAME_WIDTH, GAME_HEIGHT + 100), "A role playing game...");

	window.setKeyRepeatEnabled(false);
	//window.setVerticalSyncEnabled(true);

	texture.loadFromFile("data/sheet.png");

	const int number_of_sprites = 24;

	int sprite_rects[number_of_sprites] =
	{
		5,
	   	5,
	   	5,
	   	5,
	   	4
	};

	for(int i = 0; i < number_of_sprites; i++)
	{
		for(int j = 0; j < sprite_rects[i]; j++)
		{
			sf::Sprite sprite;
			sprite.setTexture(texture);
			sprite.setTextureRect(sf::IntRect(TILE_SIZE * j, TILE_SIZE * i, TILE_SIZE, TILE_SIZE));
			sprites.push_back(sprite);
		}
	}

	summon_type_list.push_back(SummonType("Zomobon", 100));
	summon_type_list.push_back(SummonType("Kerodia", 120));
	summon_type_list.push_back(SummonType("Anegont", 114));
	summon_type_list.push_back(SummonType("Sanegoar", 130));
	summon_type_list.push_back(SummonType("Elestrio", 125));

	character_list[0].addSummon(Summon(summon_type_list[3], 5));
	character_list[0].addSummon(Summon(summon_type_list[2], 6));
	character_list[1].addSummon(Summon(summon_type_list[4], 8));
}


void Game::doLogic()
{
	sf::Event event;

	while(window.pollEvent(event))
	{
		if(event.type == sf::Event::Closed) window.close();
		if(event.type == sf::Event::KeyPressed)
		{
			if(state == GAME)
			{
				if(event.key.code == sf::Keyboard::Escape)
				{
					pause();
				}
				else if(event.key.code == sf::Keyboard::Z)
				{
					interact();
				}
			}
			else if(state == MENU)
			{
				if(event.key.code == sf::Keyboard::Escape)
				{
					resume();
				}
				else if(event.key.code == sf::Keyboard::Up)
				{
					main_menu.goUp();
				}
				else if(event.key.code == sf::Keyboard::Down)
				{
					main_menu.goDown();
				}
				else if(event.key.code == sf::Keyboard::Return)
				{
					switch(main_menu.getCurrentElementID())
					{
						case 0:
							state = GAME;
							restart();
							break;
						case 1:
							//options menu
							break;
						case 2:
							window.close();
							break;
					}
				}
			}	

		}
	}

	if(state == GAME)
	{
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			moveCharacter(current_character_id, LEFT);
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			moveCharacter(current_character_id, RIGHT);
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			moveCharacter(current_character_id, UP);
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			moveCharacter(current_character_id, DOWN);
		}
	}

	if(state == GAME)
	{
		int random_dir = rand()%4;
		moveCharacter(1, (Direction)random_dir);

		attention_tile.x = character_list[current_character_id].getX();
		attention_tile.y = character_list[current_character_id].getY();
		switch(character_list[current_character_id].getWalkingDirection())
		{
			case LEFT: attention_tile.x--; break;
			case RIGHT: attention_tile.x++; break;
			case UP: attention_tile.y--; break;
			case DOWN: attention_tile.y++; break;
		}
		for(int i = 0; i < (int)character_list.size(); i++)
		{
			Character* cchar = &character_list[i];
			cchar->onLoop();
			for(int j = 0; j < (int)door_list.size(); j++)
			{
				if(cchar->canTP() == false) continue;
				Door* cdoor = &door_list[j];
				if(cchar->getLevelID() == cdoor->getLevelID() && cchar->getX() == cdoor->getX() && cchar->getY() == cdoor->getY() && cchar->getNewX() == cdoor->getX() && cchar->getNewY() == cdoor->getY())
				{
					int dest_id = cdoor->getDestinationID();
					Door* ddoor = &door_list[dest_id];
					sf::Vector2f dest(ddoor->getX(), ddoor->getY());
					int dest_level = ddoor->getLevelID();
					cchar->teleport(dest, dest_level);
					if(current_character_id == i) current_level_id = cchar->getLevelID();
				}
			}
		}
	}
	else if(state == MENU)
	{

	}
	else if(state == BATTLE)
	{


	}
}

void Game::draw()
{
	window.clear();
	if(state == GAME)
	{
		sf::Vector2f cam(camera.getOffsetX(), camera.getOffsetY());
		for(int i = 0; i < level_list[current_level_id].getH(); i++) for(int j = 0; j < level_list[current_level_id].getW(); j++)
		{
			char tile = level_list[current_level_id].getTile(sf::Vector2f(j, i));
			if(tile == 'o')
			{
				sf::Sprite grass_sprite = sprites[GRASS];
				grass_sprite.setPosition(sf::Vector2f(j * TILE_SIZE - cam.x, i * TILE_SIZE - cam.y));
				window.draw(grass_sprite);
			}
			if(tile == 't')
			{
				sf::Sprite tree_sprite = sprites[TREE];
				tree_sprite.setPosition(sf::Vector2f(j * TILE_SIZE - cam.x, i * TILE_SIZE - cam.y));
				window.draw(tree_sprite);
			}
		}
		for(int i = 0; i < (int)door_list.size(); i++)
		{
			if(door_list[i].getLevelID() == current_level_id)
			{
				sf::Sprite door_sprite = sprites[DOOR];
				door_sprite.setPosition(sf::Vector2f(door_list[i].getX() * TILE_SIZE - cam.x, door_list[i].getY() * TILE_SIZE - cam.y));
				window.draw(door_sprite);
			}
		}
		for(int i = 0; i < (int)character_list.size(); i++)
		{
			if(character_list[i].getLevelID() == current_level_id)
			{
				Character c_char = character_list[i];
				sf::Sprite character_sprite = sprites[CHARACTER+c_char.getWalkingDirection() * 5 + c_char.getAnimStep()];
				sf::Vector2f real_pos = sf::Vector2f(c_char.getX() * TILE_SIZE + c_char.getShiftX(), c_char.getY() * TILE_SIZE + c_char.getShiftY());
				character_sprite.setPosition(sf::Vector2f(real_pos.x - cam.x, real_pos.y - cam.y));
				window.draw(character_sprite);
			}
		}
	}
	else if(state == MENU)
	{
		for(int i = 0; i < main_menu.getSize(); i++)
		{
			sf::Text text;
			text.setFont(font);
			text.setString((main_menu.getCurrentElementID() == i) ? ("* " + main_menu.getElement(i)) : (" " + main_menu.getElement(i)));
			text.setCharacterSize(16);
			text.setColor(sf::Color(20, 100, 255));
			text.setPosition(sf::Vector2f(120, 120 + 30 * i));
			window.draw(text);
		}
	}
	else if(state == BATTLE)
	{
		sf::Color summon_name_color(20, 100, 255), hp_color(51, 132, 33), hp_num_color(40, 40, 140), lvl_color(200, 23, 200);
		int font_size = 16, hp_font_size = 12;

		sf::Text summon1(character_list[current_character_id].getSummon(0)->getName(), font, font_size);
		summon1.setPosition(sf::Vector2f(100, GAME_HEIGHT - 100));
		summon1.setColor(summon_name_color);
		window.draw(summon1);

		sf::Text summon2(opponent->getSummon(0)->getName(), font, font_size);
		summon2.setColor(summon_name_color);
		summon2.setPosition(sf::Vector2f(GAME_WIDTH - 220, 100));
		window.draw(summon2);

		sf::RectangleShape hpbar1(sf::Vector2f(100, 7));
		hpbar1.setPosition(sf::Vector2f(100, GAME_HEIGHT - 100 + 30));
		hpbar1.setFillColor(hp_color);
		window.draw(hpbar1);

		sf::RectangleShape hpbar2(sf::Vector2f(100, 7));
		hpbar2.setPosition(sf::Vector2f(GAME_WIDTH - 220, 100 + 30));
		hpbar2.setFillColor(hp_color);
		window.draw(hpbar2);

		stringstream ss1, ss2;

		ss1 << character_list[current_character_id].getSummon(0)->getHP() << "/" << character_list[current_character_id].getSummon(0)->getMaxHP();
		sf::Text hp_num1(ss1.str(), font, hp_font_size);
		hp_num1.setColor(hp_num_color);
		hp_num1.setPosition(sf::Vector2f(100 + 100 + 20, GAME_HEIGHT - 100 + 25));
		window.draw(hp_num1);

		ss2 << opponent->getSummon(0)->getHP() << "/" << opponent->getSummon(0)->getMaxHP();
		sf::Text hp_num2(ss2.str(), font, hp_font_size);
		hp_num2.setColor(hp_num_color);
		hp_num2.setPosition(sf::Vector2f(GAME_WIDTH - 220 + 100 + 20, 100 + 25));
		window.draw(hp_num2);

		stringstream ss3, ss4;

		ss3 << "Lvl " << character_list[current_character_id].getSummon(0)->getLevel();
		sf::Text sum_lvl1(ss3.str(), font, font_size);
		sum_lvl1.setColor(lvl_color);
		sum_lvl1.setPosition(sf::Vector2f(100 + 100, GAME_HEIGHT - 100));
		window.draw(sum_lvl1);

		ss4 << "Lvl " << opponent->getSummon(0)->getLevel();
		sf::Text sum_lvl2(ss4.str(), font, font_size);
		sum_lvl2.setColor(lvl_color);
		sum_lvl2.setPosition(sf::Vector2f(GAME_WIDTH - 220 + 100, 100));
		window.draw(sum_lvl2);

	}
	sf::RectangleShape interface(sf::Vector2f(GAME_WIDTH, 100));
	interface.setPosition(sf::Vector2f(0, GAME_HEIGHT));
	interface.setFillColor(sf::Color(20, 20, 20));
	window.draw(interface);
	sf::Text text;
	text.setFont(font);
	text.setString(message);
	text.setCharacterSize(16);
	text.setColor(sf::Color(20, 100, 255));
	text.setPosition(sf::Vector2f(15, GAME_HEIGHT + 15));
	window.draw(text);
	window.display();
}

void Game::exit(){}

void Game::restart()
{
	music.stop();
	music.play();

	ifstream file;
	int x, y, loc, dest;
	std::string name;
	int temp;

	character_list.clear();
	file.open("data/characters.txt");
	while(file >> x >> y >> loc >> name) character_list.push_back(Character(sf::Vector2f(x, y), loc, name));
	file.close();
	Character* cchar = &character_list[current_character_id];
	camera.setHandles(cchar->getPosHandle(), cchar->getShiftHandle());

	file.open("data/settings.txt");
	file >> temp;
	current_level_id = temp;
	file >> temp;
	current_character_id = temp;
	file.close();

	character_list[0].addSummon(Summon(summon_type_list[3], 5));
	character_list[0].addSummon(Summon(summon_type_list[2], 6));
	character_list[1].addSummon(Summon(summon_type_list[4], 8));
	
	message = "";
	opponent = NULL;
}

void Game::moveCharacter(int character_id, Direction dir)
{
	Character* cchar = &character_list[character_id];
	if(cchar->isWalking()) return;
	cchar->setWalkingDirection(dir);
	sf::Vector2f new_pos(cchar->getX(), cchar->getY());
	if(dir == LEFT) new_pos.x--;
	else if(dir == RIGHT) new_pos.x++;
	else if(dir == UP) new_pos.y--;
	else if(dir == DOWN) new_pos.y++;
	Level* lvl = &level_list[cchar->getLevelID()];
	if(lvl->getTile(new_pos) != 'o') return;
	if(new_pos.x < 0 || new_pos.x >= lvl->getW()) return;
	if(new_pos.y < 0 || new_pos.y >= lvl->getH()) return;
	for(int i = 0; i < (int)character_list.size(); i++)
	{
		if(i == character_id) continue;
		if(cchar->getLevelID() == character_list[i].getLevelID() && new_pos.x == character_list[i].getX() && new_pos.y == character_list[i].getY()) return;
		if(cchar->getLevelID() == character_list[i].getLevelID() && new_pos.x == character_list[i].getNewX() && new_pos.y == character_list[i].getNewY()) return;
	}
	cchar->move(sf::Vector2f(new_pos.x, new_pos.y));
}

void Game::interact()
{
	for(int i = 0; i < (int)character_list.size(); i++)
	{
		if(i == current_character_id) continue;
		if(character_list[i].getX() == attention_tile.x && character_list[i].getY() == attention_tile.y)
		{
			/*
			message = character_list[i].getName() + ": " + character_list[i].getMessage();
			*/
			Character* npc = &character_list[i];
			
			if(npc->isWalking() == false)
			{
				switch(character_list[current_character_id].getWalkingDirection())
				{
					case LEFT: npc->setWalkingDirection(RIGHT); break;
					case RIGHT: npc->setWalkingDirection(LEFT); break;
					case DOWN: npc->setWalkingDirection(UP); break;
					case UP: npc->setWalkingDirection(DOWN); break;
				}	
			}

			engageInBattle(npc);
		}
	}
}

void Game::engageInBattle(Character* npc)
{
	state = BATTLE;
	opponent = npc;
}

void Game::pause()
{
	state = MENU;
	for(int i = 0; i < (int)character_list.size(); i++)
	{
		character_list[i].pauseTimers();
	}
}

void Game::resume()
{
	state = GAME;
	for(int i = 0; i < (int)character_list.size(); i++)
	{
		character_list[i].resumeTimers();
	}
}
