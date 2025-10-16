#include "Player.hpp"

// constructors and destructors
Player::Player()
{
}

Player::Player(float x, float y, int i)
{
	float r = 20.f;
	this->player.setOutlineColor(sf::Color::White);
	
	if (i == 8) {
		r = r / 4 * 3;
		this->player.setFillColor(sf::Color::White);
		this->player.setOutlineColor(sf::Color::Black);
	}
	else if (i % 2 == 0)
		this->player.setFillColor(sf::Color::Red);
	else
		this->player.setFillColor(sf::Color::Blue);
	
	this->player.setPosition(sf::Vector2f(x - r, y - r));
	this->player.setOutlineThickness(-2.5f);
	
	this->player.setRadius(r);

	this->index = i;
	
	this->isMoving = false;
	this->isAiming = false;
	this->inGoal = false;
	this->isResettingPos = false;
	this->ballInGoal = false;

	this->step = {0.f, 0.f};
	this->speed = 0.f;

	this->friction = 0.95f;
}

Player::~Player()
{
}

// functions
void Player::drawPlayer(sf::RenderTarget &target, bool firstTeamPlaying, bool playIsOn, bool someoneSelected, Player *selected) {
	if (someoneSelected) {
		if (this == selected) {
			this->player.setOutlineColor(sf::Color::Black);
			this->player.setOutlineThickness(-5.f);
		}
		else if (this != selected && this->getColor() != sf::Color::White){
			this->player.setOutlineColor(sf::Color::White);
			this->player.setOutlineThickness(-2.5f);
		}
	}
	else {
		if (firstTeamPlaying && this->getIndex() % 2 == 0 && this->getColor() != sf::Color::White && !playIsOn) {
			if (!this->isAiming) {
				this->player.setOutlineColor(sf::Color::Black);
				this->player.setOutlineThickness(-5.f);
			}
		}
		else if (!firstTeamPlaying && this->getIndex() % 2 != 0 && !playIsOn) {
			if (!this->isAiming) {
				this->player.setOutlineColor(sf::Color::Black);
				this->player.setOutlineThickness(-5.f);
			}
		}
		else if (this->getColor() != sf::Color::White) {
			this->player.setOutlineColor(sf::Color::White);
			this->player.setOutlineThickness(-2.5f);
		}
	}
	target.draw(this->player);
}

bool Player::getClick(sf::Vector2f mouseLocalPosition) {
	return this->player.getGlobalBounds().contains(mouseLocalPosition) && this->player.getFillColor() != sf::Color::White;
}

void Player::aim(sf::Vector2f mouseLocalPosition, float speed)
{
	this->delta = (this->getPlayerCenterPos() - mouseLocalPosition);
	this->len = std::hypotf(this->delta.x, this->delta.y);
	if (this->len < this->getR() * 2.f)
		this->isAiming = false;
	else
		this->isAiming = true;
	this->step = delta / len;
	if (speed == 0)
		this->speed = len / 10;
	else
		this->speed = speed;
	if (this->speed > 50)
		this->speed = 50;
}

bool Player::checkIfAimingOrSelecting()
{
	if (this->len < this->getR() * 5)
		return false;
	return true;
}

void Player::playerMoves()
{
	this->isAiming = false;
	if (this->speed > 0.5f) {
		this->player.move(this->step * this->speed);
		if (this->ballInGoal)
			this->friction = 0.5f;
		this->speed *= this->friction;
	}
	else {
		this->speed = 0;
		this->isMoving = false;
		this->friction = 0.95f;
		// this->setNewPos(this->player.getGlobalBounds().position.x, this->player.getGlobalBounds().position.y);
	}
}

bool Player::isInGoal(sf::FloatRect goal, std::string side)
{
	if (this->getPlayerCenterPos().y - this->getR() > goal.position.y && this->getPlayerCenterPos().y + this->getR() < goal.position.y + goal.size.y) {
		if (side == "l") {
			if (this->player.getPosition().x < goal.position.x + goal.size.x)
				return true;
		}
		else if (side == "r") {
			if (this->player.getPosition().x + this->getR() * 2 > goal.position.x)
				return true;
		}
		else {
			return true;
		}
	}
	return false;
}

bool Player::checkCollisionWith(Player *b)
{
	Player *a = this;
	this->delta = a->getPlayerCenterPos() - b->getPlayerCenterPos();
	this->len = std::hypotf(this->delta.x, this->delta.y);
	float minDist = a->getR() + b->getR();
	if (this->len <= 0.f) {
		this->delta = sf::Vector2f(1.f, 0.f);
		this->len = 1e-6f;
	}
	if (this->len < minDist)
		return true;
	return false;
}

void Player::handleCollisionWith(Player *b)
{
	Player *a = this;
	sf::Vector2f pa = a->getPlayerCenterPos();
	sf::Vector2f pb = b->getPlayerCenterPos();
	sf::Vector2f normal = this->delta / this->len; // from b -> a

	// compute full velocities
	sf::Vector2f va = a->getStep() * a->getSpeed();
	sf::Vector2f vb = b->getStep() * b->getSpeed();

	// relative velocity along normal
	float relVelAlongNormal = (va.x - vb.x) * normal.x + (va.y - vb.y) * normal.y;

	// if moving apart (or exactly tangent), skip impulse (but still separate if overlapped)
	if (relVelAlongNormal >= 0.f) {
		// still fix penetration so they are not stuck
		float overlap = a->getR() + b->getR() - this->len;
		pa += normal * (overlap * 0.5f);
		pb -= normal * (overlap * 0.5f);

		// write back corrected centers (adjust if setNewPos expects top-left)
		a->setNewPos(pa.x - a->getR(), pa.y - a->getR());
		b->setNewPos(pb.x - b->getR(), pb.y - b->getR());

		return ;
	}

	// --- resolve penetration (move each by half overlap) ---
	float overlap = a->getR() + b->getR() - this->len;
	pa += normal * (overlap * 0.5f);
	pb -= normal * (overlap * 0.5f);
	a->setNewPos(pa.x - a->getR(), pa.y - a->getR());
	b->setNewPos(pb.x - b->getR(), pb.y - b->getR());

	// --- decompose velocities into normal/tangent ---
	float va_n = va.x * normal.x + va.y * normal.y;
	float vb_n = vb.x * normal.x + vb.y * normal.y;
	sf::Vector2f va_t = va - normal * va_n;
	sf::Vector2f vb_t = vb - normal * vb_n;

	// --- for equal masses, swap normal components (elastic) ---
	float va_n_after = vb_n;
	float vb_n_after = va_n;

	sf::Vector2f va_after = va_t + normal * va_n_after;
	sf::Vector2f vb_after = vb_t + normal * vb_n_after;

	// --- convert to direction + speed and write back ---
	float speedA = std::hypotf(va_after.x, va_after.y);
	float speedB = std::hypotf(vb_after.x, vb_after.y);

	sf::Vector2f dirA = (speedA > 1e-6f) ? (va_after / speedA) : sf::Vector2f(0.f, 0.f);
	sf::Vector2f dirB = (speedB > 1e-6f) ? (vb_after / speedB) : sf::Vector2f(0.f, 0.f);

	a->setNewStep(dirA);
	a->setNewSpeed(speedA);
	b->setNewStep(dirB);
	if (b->getIndex() == 8)
		b->setNewSpeed(speedB * 1.5);
	else
		b->setNewSpeed(speedB);

	// set moving flags if they gained speed (tweak threshold to taste)
	if (speedA > 0.01f) a->setIsMoving(true);
	if (speedB > 0.01f) b->setIsMoving(true);
}

sf::Vector2f Player::rotateVector(sf::Vector2f v, float angle)
{
	return {v.x * std::cos(angle) - v.y * std::sin(angle), v.x * std::sin(angle) + v.y * std::cos(angle)};
}

void Player::drawAimingShapes(sf::RenderTarget &target, sf::Vector2f mousePos)
{
	// aiming triangle
	sf::Vector2f pCenter = this->getPlayerCenterPos();
		// angle between len and r
	float angle = std::acos(this->getR() / this->len);
		// rotating the step by +- angle to get the correct r directions for the tangent points
	sf::Vector2f t1 = pCenter - this->rotateVector(step, angle) * this->getR();
	sf::Vector2f t2 = pCenter - this->rotateVector(step, -angle) * this->getR();
	sf::VertexArray triangle(sf::PrimitiveType::Triangles, 3);
	sf::Color triColor(50, 220, 50, 200);
	triangle[0] = {mousePos, triColor};
	triangle[1] = {t1, triColor};
	triangle[2] = {t2, triColor};
	target.draw(triangle);

	// trajectory rectangle
	sf::Vector2f opposite = -this->step;
	sf::Vector2f perp = {-opposite.y, opposite.x};
	sf::Vector2f v1 = pCenter + perp * this->getR();
	sf::Vector2f v2 = pCenter - perp * this->getR();
	sf::Vector2f v3 = v2 + step * 200.f;
	sf::Vector2f v4 = v1 + step * 200.f;
	sf::VertexArray rectangle(sf::PrimitiveType::Triangles, 6);
	sf::Color rectColor(255, 255, 255, 150);
	rectangle[0] = {v1, rectColor};
	rectangle[1] = {v2, rectColor};
	rectangle[2] = {v3, rectColor};
	rectangle[3] = {v1, rectColor};
	rectangle[4] = {v3, rectColor};
	rectangle[5] = {v4, rectColor};
	target.draw(rectangle);
}

void Player::resetPos()
{
	if (this->xAfterResetting > this->player.getPosition().x) {
		if (this->xAfterResetting - this->player.getPosition().x > 1) {
			this->player.move({1.f, 0.f});
		}
		else {
			this->player.setPosition({this->xAfterResetting, this->player.getPosition().y});
			this->isResettingPos = false;
			this->inGoal = false;
		}
	}
	else if (this->xAfterResetting < this->player.getPosition().x) {
		if (this->player.getPosition().x - this->xAfterResetting > 1) {
			this->player.move({-1.f, 0.f});
		}
		else {
			this->player.setPosition({this->xAfterResetting, this->player.getPosition().y});
			this->isResettingPos = false;
			this->inGoal = false;
		}
	}
}

void Player::setIsMoving(bool value)
{
	this->isMoving = value;
}

const bool Player::getIsMoving() const
{
	return this->isMoving;
}

const sf::Vector2f Player::getPlayerCenterPos() const
{
	auto pos = this->player.getPosition();
	pos.x += this->player.getRadius();
	pos.y += this->player.getRadius();
	return pos;
}

void Player::changeStepDir(float x, float y)
{
	this->step.x *= x;
	this->step.y *= y;
}

void Player::setNewStep(sf::Vector2f new_step)
{
	this->step = new_step;
}

void Player::setNewPos(float x, float y)
{
	this->player.setPosition({x, y});
}

const float Player::getR() const
{
	return this->player.getRadius();
}

const float Player::getSpeed() const
{
	return this->speed;
}

void Player::setNewSpeed(float new_speed)
{
	this->speed = new_speed;
	if (this->speed > 50)
		this->speed = 50;
}

const sf::Vector2f Player::getStep() const
{
	return this->step;
}

sf::Color Player::getColor()
{
	return this->player.getFillColor();
}

const int Player::getIndex() const
{
	return this->index;
}

const bool Player::getIsAiming() const
{
	return this->isAiming;
}

void Player::setInGoal(bool value)
{
	this->inGoal = value;
}

const bool Player::getInGoal() const
{
	return this->inGoal;
}

void Player::setIsResettingPos(bool value)
{
	this->isResettingPos = value;
}

const bool Player::getIsResettingPos() const
{
	return this->isResettingPos;
}

void Player::setXAfterResetting(float value)
{
	this->xAfterResetting = value;
}

void Player::setBallInGoal(bool value)
{
	this->ballInGoal = value;
}

const bool Player::getBallInGoal() const
{
	return this->ballInGoal;
}
