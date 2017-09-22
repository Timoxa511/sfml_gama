#define _USE_MATH_DEFINES

//#include <txLIb.h>
// #define _TX_NOINIT
#pragma GCC diagnostic ignored "-Weffc++"

#include <sfml/graphics.hpp>
#include <windows.h>
#include <math.h>
#include <assert.h>

//{Structs&Functions__PROTOTYPES-----------------------------------------------
//-----------------------------------------------------------------------------

typedef sf::Vector2 <float> Vector;

//-----------------------------------------------------------------------------

struct Object
	{
	Vector pos_;
	Vector v_;
	sf::Texture tex_;
	sf::Sprite sprite_;
	//--------------------
	Object ();
	Object (const Vector &pos,
            const sf::Texture &tex,
            const sf::IntRect &rectangle);
    virtual ~Object () {};

	virtual void logic ();
	virtual void update();
	virtual void draw  ();
	};

struct      Mc      : Object
	{
	int rotation_;
	//--------------------
    Mc (const Vector &pos,
        const sf::Texture &tex,
        const sf::IntRect &rectangle,
          int rotation = 0);

    virtual void logic () override;
    virtual void draw  () override;
    virtual void update() override;

	void control();
	};

struct StaticObject : Object
    {
    sf::CircleShape shape_;
    int rotationSpeed_;
    float radius_;
    //-------------------
    StaticObject (const Vector &pos,
                  const sf::CircleShape &tex,
                    int rotationSpeed);

    virtual void logic () override;
	virtual void update() override;
	virtual void draw  () override;
    };

struct     Map      : Object
    {
    std::vector<StaticObject> Objects_;
    //-------------------
    Map (const Vector &size = Vector(800, 600),
         const Vector &pos = Vector(0, 0),
         const sf::Texture &tex = sf::Texture());

    //virtual void logic () override;
	virtual void update() override;
	virtual void draw  () override;

	void generate();
	void add(const StaticObject& object);
    };

struct Engine
    {
    std::vector <Object*> Objects_;
    //-------------------
    Engine ();

    void add (Object* pls_serve_me_senpai);
    void work  ();
    };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//------------------virtual void draw () override;-----------------------------------------------------------

Object* CheckCollision(Map& map, Vector pos, float radius);
void check_for_events ();
void temp_game_proc ();
void lowing_game_speed ();
void SpawnTheStars(Map& map, Vector starSizeRange, int nStars);
Vector vNormalize (Vector longV);
const sf::IntRect TheWholeTextureRect (const sf::Texture &tex);
double hypot (const Vector& v1, const Vector& v2);

bool operator == (const Vector& lvalue, const sf::Vector2u& rvalue);
bool operator == (const sf::Vector2u& rvalue, const Vector& lvalue);
bool operator == (const sf::IntRect& lvalue, const sf::IntRect& rvalue);
//}
//-----------------------------------------------------------------------------

//{GlobalVars------------------------------------------------------------------

sf::RenderWindow* Window = nullptr;
float VMAX = 15;
const float EPS = 0.000001f;
Vector Weight (1,1);
const float DEGREESINRADIAN = (float) (180/M_PI);


Vector  cameraPos (0, 0);
Vector  cameraSiz (800, 600);
sf::Vector2<int> mapSize (2000, 2000);

//}
//-----------------------------------------------------------------------------


//===================================================================================================


//{Classes::-------------------------------------------------------------------

//{Object::--------------------------------------------------------------------

Object::Object (const Vector &pos,
                const sf::Texture &tex,
                const sf::IntRect &rectangle) :
	pos_(pos),
	tex_(tex),
	sprite_(tex, rectangle)
	{
	sprite_.setOrigin(float (rectangle.width/2) , float (rectangle.height/2) );
	}

//-----------------------------------------------------------------------------

Object::Object () :
    pos_(Vector(500, 500)),
    tex_(),
	sprite_()
	{
	}

//=============================================================================

void Object::logic()
    {}

//-----------------------------------------------------------------------------

void Object::update()
	{
	pos_ += v_;
	sprite_.setPosition (pos_);
	}

//-----------------------------------------------------------------------------

void Object::draw()
	{

    sf::Sprite fakeSprite (sprite_);
    fakeSprite.move (-cameraPos.x, -cameraPos.y);
	Window->draw(fakeSprite);
	}

//}----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//{StaticObject::--------------------------------------------------------------

StaticObject::StaticObject (const Vector &pos,
                            const sf::CircleShape &shape,
                              int rotationSpeed) :
    Object (pos, sf::Texture(), shape.getTextureRect() ),
    shape_(shape),
    rotationSpeed_(rotationSpeed),
    radius_(shape.getRadius())
    {
    auto rect = shape_.getLocalBounds();
    shape_.setOrigin(rect.width/2, rect.height/2);
    }

//=============================================================================

void StaticObject::logic ()
    {}

//-----------------------------------------------------------------------------

void StaticObject::update()
    {
    shape_.rotate(float (rotationSpeed_*0.2));
    shape_.setPosition (pos_);
    }

//-----------------------------------------------------------------------------

void StaticObject::draw()
	{

    sf::CircleShape fakeShape (shape_);
    fakeShape.move (-cameraPos.x, -cameraPos.y);

	Window->draw(fakeShape);
	}

//}
//-----------------------------------------------------------------------------


//{Mc::------------------------------------------------------------------------

Mc::Mc  (const Vector &pos,
         const sf::Texture &tex,
         const sf::IntRect &rectangle,
           int rotation) :
	Object (pos, tex, rectangle),
	rotation_ (rotation)
	{}

//=============================================================================

void Mc::control()
	{

	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Left))  v_.x -= 5;
	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Right)) v_.x += 5;
	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Up))	  v_.y -= 5;
	if (sf::Keyboard::isKeyPressed (sf::Keyboard::Down))  v_.y += 5;

    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Space))  v_ = Vector (0, 0);

	if (v_.x >  VMAX) v_.x =  VMAX;
	if (v_.y >  VMAX) v_.y =  VMAX;
	if (v_.x < -VMAX) v_.x = -VMAX;
	if (v_.y < -VMAX) v_.y = -VMAX;

    v_*= float (0.9);
    // if (!(!v.x && !v.y))         if (!!v.x || !!v.y)

    if (fabs (v_.x) > EPS || fabs (v_.y) > EPS)
        {
        Vector NormalV = vNormalize(v_);
        rotation_ = int (
                        (NormalV.y >= 0)?
                         DEGREESINRADIAN * (acos ( NormalV.x) ) :
                         DEGREESINRADIAN * (acos (-NormalV.x) ) + 180
                        );
        }

	}

//-----------------------------------------------------------------------------
void Mc::logic()
    {
    control();
    }

//-----------------------------------------------------------------------------
void Mc::draw()
    {
    sprite_.setRotation(float(rotation_));

    Window->draw(sprite_);
    }

//-----------------------------------------------------------------------------

void Mc::update()
	{
	cameraPos += v_;
	sprite_.setPosition (pos_);
	}

//}
//-----------------------------------------------------------------------------


//{Map::-----------------------------------------------------------------------

Map::Map (const Vector &size,
          const Vector &pos,
          const sf::Texture &tex) :
    Object()
    {

    if (tex_.getSize() == Vector(0, 0)) tex_.create( (unsigned int) size.x,
                                                     (unsigned int) size.y );
    sprite_ = sf::Sprite(tex, TheWholeTextureRect (tex) );
    }

//=============================================================================

void Map::draw()
    {

    Window->draw(sprite_);
    for (auto& Object : Objects_)
        {
        //todo drawing objcz in rect optimization
        Object.draw();
        }
    }

//-----------------------------------------------------------------------------

void Map::add(const StaticObject& object)
    {
    Objects_.push_back(object);
    }

//-----------------------------------------------------------------------------

void Map::update()
    {
    for (auto& Object : Objects_)
        {

        Object.update();
        }
    }

//}
//-----------------------------------------------------------------------------


//{Engine-----------------------------------------------------------------------

Engine::Engine ()
    {}

//-----------------------------------------------------------------------------

void Engine::add (Object* p_obj)
    {
    assert(p_obj);
    Objects_.push_back(p_obj);
    }

//-----------------------------------------------------------------------------

void Engine::work  ()
    {
    lowing_game_speed();
    check_for_events();

    Window->clear();

    for (auto& p_obj : Objects_)
        {
        p_obj->update();
        p_obj->logic();
        p_obj->draw();
        }
    Window->display();
    }

//}
//-----------------------------------------------------------------------------


//}
//-----------------------------------------------------------------------------

//{Functions::-----------------------------------------------------------------


int main()
	{
	srand(unsigned (time(nullptr)));
	sf::RenderWindow window (sf::VideoMode (800, 600), "okoshe4ko");
	Window = &window;

	temp_game_proc ();
	}

//-----------------------------------------------------------------------------

void temp_game_proc ()
	{

	sf::Texture txtr;       txtr.loadFromFile ("image1.png");
	Mc player(Vector(cameraSiz.x/2, cameraSiz.y/2), txtr, sf::IntRect (0, 0, txtr.getSize().x, txtr.getSize().y));

    Map map;
    SpawnTheStars(map, Vector(5, 20), 135);

    Engine engine;
    engine.add (&map);
    engine.add (&player);


	while (Window->isOpen() )
		{
		engine.work();
		}

	}

//-----------------------------------------------------------------------------
void SpawnTheStars(Map& map, Vector starSizeRange, int nStars)
    {
    for (int i = 0, limit = 0; i < nStars && limit < nStars+100; i++, limit++)
        {
        int randR = rand()%100/4+20;

        Vector randPos = Vector (rand() % (mapSize.x - 2*randR) + randR, rand() % (mapSize.y - 2*randR) + randR);
        sf::CircleShape shape (randR, rand()%2*2 + 5);

        StaticObject meteor (randPos, shape, (rand()%3*4-3)*3 );

        if (CheckCollision (map, meteor.pos_, meteor.radius_)) { i--; continue; }

        map.add (meteor);
        }
    }

//-----------------------------------------------------------------------------
Object* CheckCollision(Map& map, Vector pos, float radius)
    {
    for (auto& object : map.Objects_)
        {
        if (hypot(object.pos_, pos) < object.radius_+radius) return &object;
        }
    return NULL;
    }

//-----------------------------------------------------------------------------

const sf::IntRect TheWholeTextureRect (const sf::Texture &tex)
    {

    auto size2u = tex.getSize();
    Vector size2 (size2u);
    return sf::IntRect (0, 0, size2.x, size2.y);
    }

//-----------------------------------------------------------------------------





//{service_func----------------------------------------------------------------

void check_for_events()
	{
	sf::Event event;
	while (Window->pollEvent (event) )
		if (event.type == sf::Event::Closed) Window->close();
	}

//-----------------------------------------------------------------------------

void lowing_game_speed()
    {
    Window->setFramerateLimit(15);
    //Sleep(100);
    }

//-----------------------------------------------------------------------------

double hypot (const Vector& v1, const Vector& v2)
    {
    return hypot(v1.x - v2.x, v1.y - v2.y);
    }

//-----------------------------------------------------------------------------

Vector vNormalize(Vector longV)
    {
    if (longV.x == 0 && longV.y == 0) return Vector (0, 1);
    return longV / hypot (longV.x, longV.y);
    }

//-----------------------------------------------------------------------------

//}
//-----------------------------------------------------------------------------


//{operators-------------------------------------------------------------------

bool operator == (const Vector& lvalue, const sf::Vector2u& rvalue)
    {
    return ( lvalue.x == rvalue.x &&
             lvalue.y == rvalue.y );
    }

//-----------------------------------------------------------------------------

bool operator == (const sf::Vector2u& rvalue, const Vector& lvalue)
    {
    return lvalue == rvalue;
    }

//-----------------------------------------------------------------------------

bool operator == (const sf::IntRect& lvalue, const sf::IntRect& rvalue)
    {
    return ( lvalue.top    == rvalue.top   &&
             lvalue.left   == rvalue.left  &&
             lvalue.width  == rvalue.width &&
             lvalue.height == rvalue.height );
    }

//}
//-----------------------------------------------------------------------------



//}----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


















