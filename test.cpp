#include <sfml/graphics.hpp>
#include <windows.h>

//{structs&functions__PROTOTYPES-----------------------------------------------

typedef sf::Vector2 <float> Vector;

//-----------------------------------------------------------------------------

struct Object
	{
	Vector pos_;
	Vector v_;
	sf::Sprite sprite_;
	//--------------------
	Object (const Vector &pos,
            const sf::Texture &tex,
            const sf::IntRect &rectangle);

	virtual void logic();
	virtual void update();
	virtual void draw();
	};

struct Mc : Object
	{
	int rotation;
	//--------------------
    Mc (const Vector &pos,
        const sf::Texture &tex,
        const sf::IntRect &rectangle,
          int rotation);

    virtual void logic() override;

	void control();
	};

//-----------------------------------------------------------------------------

void check_for_events();
void temp_game_proc ();
void lowing_speed();





//}
//-----------------------------------------------------------------------------

//{GlobalVars------------------------------------------------------------------

sf::RenderWindow* Window = nullptr;

//}
//-----------------------------------------------------------------------------


//===================================================================================================


//-----------------------------------------------------------------------------
//{Mc::

Mc::Mc  (const Vector &pos, const sf::Texture &tex, const sf::IntRect &rectangle, int rotation) :
	Object (pos, tex, rectangle)
	{}

//=============================================================================

void Mc::control()
	{
	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Left))  v_.x -= 5;
	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Right)) v_.x += 5;
	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Up))	  v_.y -= 5;
	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Down))  v_.y += 5;
	}

//-----------------------------------------------------------------------------
void Mc::logic()
    {
    control();
    }

//-----------------------------------------------------------------------------

//}
//-----------------------------------------------------------------------------

//{Object::--------------------------------------------------------------------

Object::Object (const Vector &pos, const sf::Texture &tex, const sf::IntRect &rectangle) :
	pos_(pos),
	sprite_(tex, rectangle)
	{
	sprite_.setOrigin(rectangle.width/2, rectangle.height/2);
	}

//=============================================================================

void Object::logic()
    {

    }

//-----------------------------------------------------------------------------

void Object::update()
	{
	pos_ += v_;
	sprite_.setPosition (pos_);
	}

//-----------------------------------------------------------------------------

void Object::draw()
	{
	Window->draw(sprite_);
	}

//}----------------------------------------------------------------------------
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
//{NonClassFunctions::


int main()
	{
	sf::RenderWindow window (sf::VideoMode (800, 600), "okoshe4o");
	Window = &window;

	temp_game_proc ();
	}

//-----------------------------------------------------------------------------

void temp_game_proc ()
	{
	sf::Texture txtr;
	txtr.loadFromFile ("image1.png");


	Mc player(Vector(500, 500), txtr, sf::IntRect (0, 0, txtr.getSize().x, txtr.getSize().y));
	while (Window->isOpen() )
		{
		check_for_events();

		player.update();
		player.logic();

		Window->clear();
		player.draw();
		Window->display();
        lowing_speed();
		}

	}

//-----------------------------------------------------------------------------

void check_for_events()
	{
	sf::Event event;
	while (Window->pollEvent (event) )
		if (event.type == sf::Event::Closed) Window->close();
	}

//-----------------------------------------------------------------------------

void lowing_speed()
    {
    Window->setFramerateLimit(10);
    //Sleep(100);
    }

//-----------------------------------------------------------------------------

//}-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


















