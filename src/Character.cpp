#include "Character.h"

Character::Character(sf::Vector2f pos, int level_id, std::string name)
{
	this->pos = pos;
	this->level_id = level_id;
	this->name = name;
	new_pos = sf::Vector2f(0, 0);
	shift = sf::Vector2f(0, 0);
	walking = false;
	walk_clock.restart();
	walk_dir = DOWN;
	block_tp = false;
	anim_step = 0;
	anim_clock.restart();
	message = "Hi there!";
}

int Character::getX()
{
	return pos.x;
}

int Character::getY()
{
	return pos.y;
}

int Character::getShiftX()
{
	return shift.x;
}

int Character::getShiftY()
{
	return shift.y;
}

sf::Vector2f* Character::getPosHandle()
{
	return &pos;
}

sf::Vector2f* Character::getShiftHandle()
{
	return &shift;
}

int Character::getNewX()
{
	return new_pos.x;
}

int Character::getNewY()
{
	return new_pos.y;
}

bool Character::isWalking()
{
	return walking;
}

Direction Character::getWalkingDirection()
{
	return walk_dir;
}

void Character::setWalkingDirection(Direction dir)
{
	walk_dir = dir;
}

int Character::getAnimStep()
{
	return anim_step;
}

std::string Character::getMessage()
{
	return message;
}

std::string Character::getName()
{
	return name;
}

bool Character::canTP()
{
	return !block_tp;
}

void Character::disableTP()
{
	block_tp = true;
}

void Character::onLoop()
{
	if(walking)
	{
		sf::Time time = walk_clock.getElapsedTime();
		float multiplier = (float)time.asMilliseconds()/WALK_TIME;
		if(anim_clock.getElapsedTime() >= sf::milliseconds(60))
		{
			anim_step = anim_step%4+1;
			anim_clock.restart();
		}


		shift = sf::Vector2f(0, 0);
		if(walk_dir == LEFT) shift.x = -TILE_SIZE*multiplier;
		if(walk_dir == RIGHT) shift.x = TILE_SIZE*multiplier;
		if(walk_dir == UP) shift.y = -TILE_SIZE*multiplier;
		if(walk_dir == DOWN) shift.y = TILE_SIZE*multiplier;

		if(time >= sf::milliseconds(WALK_TIME))
		{
			walking = false;
			multiplier = 0;
			pos = new_pos;
			shift = sf::Vector2f(0, 0);
			anim_step = 0;
			anim_clock.restart();
		}
	}
}

void Character::move(sf::Vector2f new_pos)
{
	walking = true;
	this->new_pos = new_pos;
	block_tp = false;

	walk_clock.restart();
}

void Character::teleport(sf::Vector2f pos, int level_id)
{
	this->pos = pos;
	this->new_pos = pos;
	this->level_id = level_id;
}

