#include "Player.hpp"

// constructors and destructors
Player::Player()
{
}

Player::Player(float x, float y, int i, sf::Texture *ballTexture)
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

	this->friction = 0.98f;
	if (i == 8) {
		this->player.setTexture(ballTexture, true);
		// this->player.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(128, 128)));
	}
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
		this->speed = len / 15.f;
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
		this->friction = 0.98f;
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

		// normal from b -> a (this->delta and this->len were set by checkCollisionWith)
		sf::Vector2f normal = this->delta / this->len;

		// masses (use inverse mass so immovable objects can be mass = INF or inv = 0)
		float mA = a->getR() * a->getR();
		float mB = b->getR() * b->getR();
		float invA = (mA > 0.f) ? (1.f / mA) : 0.f;
		float invB = (mB > 0.f) ? (1.f / mB) : 0.f;

		// If both are immovable, nothing to do
		if (invA + invB <= 0.f)
			return;

		// current full velocities
		sf::Vector2f va = a->getStep() * a->getSpeed();
		sf::Vector2f vb = b->getStep() * b->getSpeed();

		// relative velocity along normal
		float relVelAlongNormal = (va.x - vb.x) * normal.x + (va.y - vb.y) * normal.y;

		// penetration depth
		float overlap = a->getR() + b->getR() - this->len;
		if (overlap > 0.f) {
			// correct positions weighted by inverse mass (lighter object moves more)
			float totalInv = invA + invB;
			float aMove = (totalInv > 0.f) ? (invA / totalInv) : 0.f;
			float bMove = (totalInv > 0.f) ? (invB / totalInv) : 0.f;

			// move them apart
			pa += normal * (overlap * aMove);
			pb -= normal * (overlap * bMove);

			// write back corrected centers (adjust if setNewPos expects top-left)
			a->setNewPos(pa.x - a->getR(), pa.y - a->getR());
			b->setNewPos(pb.x - b->getR(), pb.y - b->getR());
		}

		// if moving apart or tangent, we've already corrected penetration, so skip impulse
		if (relVelAlongNormal >= 0.f)
			return;

		// coefficient of restitution (elasticity) -- pick something or get per-object
		// e = 1.0 for perfectly elastic; smaller for bouncier or inelastic collisions.
		float e = 1.0f;
		// optionally: e = std::min(a->getRestitution(), b->getRestitution());

		// impulse scalar
		float totalInv = invA + invB; // we already checked > 0
		float j = (-(1.f + e) * relVelAlongNormal) / totalInv;

		// apply impulse along normal
		sf::Vector2f impulse = normal * j;
		sf::Vector2f va_after = va + impulse * invA;
		sf::Vector2f vb_after = vb - impulse * invB;

		// keep tangential components implicitly (we used full velocities)
		// convert to dir + speed and write back
		float speedA = std::hypotf(va_after.x, va_after.y);
		float speedB = std::hypotf(vb_after.x, vb_after.y);

		sf::Vector2f dirA = (speedA > 1e-6f) ? (va_after / speedA) : sf::Vector2f(0.f, 0.f);
		sf::Vector2f dirB = (speedB > 1e-6f) ? (vb_after / speedB) : sf::Vector2f(0.f, 0.f);

		a->setNewStep(dirA);
		a->setNewSpeed(speedA);
		b->setNewStep(dirB);
		b->setNewSpeed(speedB);

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
