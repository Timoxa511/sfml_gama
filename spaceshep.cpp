#define _USE_MATH_DEFINES

//#include <txLIb.h>
// #define _TX_NOINIT
#pragma GCC diagnostic ignored "-Weffc++"

#include <sfml/graphics.hpp>
#include <windows.h>
#include <math.h>
#include <assert.h>
#include <string>

//{GlobalVars------------------------------------------------------------------
//-----------------------------------------------------------------------------

typedef sf::Vector2 <float> Vector;

//-----------------------------------------------------------------------------

sf::RenderWindow* Window = nullptr;
float VMAX = 15;
const float EPS = 0.000001f;
Vector Weight (1,1);
const float DEGREESINRADIAN = (float) (180/M_PI);


const sf::Vector2<int> MapSize (800, 800);
Vector  CameraPos (0, 0);
const Vector  CameraSiz (800, 800);

const int NStars = 10;

//}
//-----------------------------------------------------------------------------


//{Structs&Functions__PROTOTYPES-----------------------------------------------


struct Object
	{
	Vector screenPos_;
	Vector mapPos_;
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
//-----------------------------------------------------------------------------

Object* CheckCollision(Map& map, Vector pos, float radius);
void check_for_events ();
void temp_game_proc ();
void lowing_game_speed ();
void SpawnTheStars(Map& map, sf::Vector2 <int> starSizeRange, int nStars);
Vector vNormalize (Vector longV);
const sf::IntRect TheWholeTextureRect (const sf::Texture &tex);
double hypot (const Vector& v1, const Vector& v2);
void WritePlayerCords (const Mc* player);

bool operator == (const Vector& lvalue, const sf::Vector2u& rvalue);
bool operator == (const sf::Vector2u& lvalue, const Vector& rvalue);
bool operator == (const sf::IntRect& lvalue, const sf::IntRect& rvalue);

Vector operator % (const Vector& lvalue, const  sf::Vector2<int>& rvalue);
Vector operator % (const Vector& lvalue, const      Vector&       rvalue);
//float  operator % (const float&  lvalue, const      float&        rvalue);     CYKA XYLU

//{templates-------------------------------------------------------------------

template <typename T, typename U>
sf::Vector2 <T> operator / (const sf::Vector2 <T>& lvalue, const U& rvalue)
    {
    return sf::Vector2 <T> (lvalue.x / rvalue, lvalue.y / rvalue);
    }
/*
template <typename T, typename U>
float  operator % (const T lvalue, const U rvalue)
    {
    return lvalue % rvalue;
    }                                                                     DDA BLYAT'
float operator % (const float& lvalue, const float& rvalue)
    {
    float mlt = 1000000;
    return float ( ((int) lvalue*mlt % (int) rvalue*mlt)/mlt );

    }     */
//}
//-----------------------------------------------------------------------------

//}
//-----------------------------------------------------------------------------


//===================================================================================================


//{Classes::-------------------------------------------------------------------

//{Object::--------------------------------------------------------------------

Object::Object (const Vector &mapPos,
                const sf::Texture &tex,
                const sf::IntRect &rectangle) :
	mapPos_(mapPos),
	screenPos_(mapPos - CameraPos),
	tex_(tex),
	sprite_(tex, rectangle)
	{
	sprite_.setOrigin(float (rectangle.width/2) , float (rectangle.height/2) );
	}

//-----------------------------------------------------------------------------

Object::Object () :
    mapPos_(MapSize/2),
    screenPos_(mapPos_ - CameraPos),
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
	mapPos_ += v_;
	screenPos_ = mapPos_ - CameraPos;
	sprite_.setPosition (screenPos_);
	}

//-----------------------------------------------------------------------------

void Object::draw()
	{

	Window->draw(sprite_);
	}

//}----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//{StaticObject::--------------------------------------------------------------

StaticObject::StaticObject (const Vector &mapPos,
                            const sf::CircleShape &shape,
                              int rotationSpeed) :
    Object (mapPos, sf::Texture(), shape.getTextureRect() ),
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

	screenPos_ = mapPos_ - CameraPos;
    shape_.setPosition (screenPos_);
    }

//-----------------------------------------------------------------------------

void StaticObject::draw()
	{
    double prec = 10000000.0;
    sf::CircleShape fakeShape (shape_);
    fakeShape.setPosition ( (fmod ((fakeShape.getPosition().x + MapSize.x*123456.0) * prec, MapSize.x * prec) )/prec  ,                //todo 4epe3 if
                            (fmod ((fakeShape.getPosition().y + MapSize.y*123456.0) * prec, MapSize.y * prec) )/prec  );

	Window->draw(fakeShape);
	Window->draw(shape_);
	}

//}
//-----------------------------------------------------------------------------


//{Mc::------------------------------------------------------------------------

Mc::Mc  (const Vector &mapPos,
         const sf::Texture &tex,
         const sf::IntRect &rectangle,
           int rotation) :
	Object (mapPos, tex, rectangle),
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
    WritePlayerCords (this);
    }

//-----------------------------------------------------------------------------

void Mc::update()
	{
	mapPos_   += v_;
	CameraPos += v_;

	screenPos_ = CameraSiz/2 + mapPos_ - CameraPos;
	sprite_.setPosition (screenPos_);
	}

//-----------------------------------------------------------------------------

void Mc::draw()
    {
    sprite_.setRotation(float(rotation_));

    Window->draw(sprite_);
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
	sf::RenderWindow window (sf::VideoMode (CameraSiz.x, CameraSiz.y), "okoshe4ko");
	Window = &window;

	temp_game_proc ();
	}

//-----------------------------------------------------------------------------

void temp_game_proc ()
	{



	sf::Texture txtr;       txtr.loadFromFile ("image1.png");
	Mc player(Vector(0, 0), txtr, sf::IntRect (0, 0, txtr.getSize().x, txtr.getSize().y));

    Map map;
    SpawnTheStars(map, sf::Vector2<int>(15, 30), NStars);

    Engine engine;
    engine.add (&map);
    engine.add (&player);


	while (Window->isOpen() )
		{
		engine.work();
		}

	}

//-----------------------------------------------------------------------------
void SpawnTheStars(Map& map, sf::Vector2 <int> starSizeRange, int nStars)
    {
    for (int i = 0, limit = 0; i < nStars && limit < nStars+100; i++, limit++)
        {
        int    randAngleMilt = rand() % 2*2 + 5;
        int    randRotation = (rand() % 3*4 - 3) * 3;
        int    randR = rand() % (starSizeRange.y - starSizeRange.x) + starSizeRange.x;
        Vector randPos = Vector (rand() % (MapSize.x + 1) - (MapSize.x - CameraSiz.x)/2,
                                 rand() % (MapSize.y + 1) - (MapSize.y - CameraSiz.y)/2);
       // sf::Color randCol (rand()%255, rand()%255, rand()%255);
        sf::CircleShape shp (randR, randAngleMilt);
       // shp.setFillColor (randCol);
        StaticObject meteor (randPos, shp,  randRotation);

        if (CheckCollision (map, meteor.mapPos_, meteor.radius_)) { i--; continue; }

        map.add (meteor);
        }
    }

//-----------------------------------------------------------------------------

void RoundMap(Map* p_map, Mc* p_player)
    {


    }

//-----------------------------------------------------------------------------
Object* CheckCollision(Map& map, Vector pos, float radius)
    {
    for (auto& object : map.Objects_)
        {
        if (hypot(object.mapPos_, pos) < object.radius_+radius) return &object;
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

void WritePlayerCords (const Mc* player)
    {
    std::string str_cords = "x:" + std::to_string((int) player->mapPos_.x /*CameraPos.x*/) + "\n" +
                            "y:" + std::to_string((int) player->mapPos_.y /*CameraPos.y*/);
    sf::Font font;
    font.loadFromFile ("C:/windows/fonts/Arial.ttf");

    sf::Text txt_cords (str_cords, font);
    Vector leftDownCornerPos ((float) 5, (float) 0.9*CameraSiz.y);
    txt_cords.setFillColor (sf::Color (170, 60, 60));
    txt_cords.setPosition  (leftDownCornerPos);

    Window->draw (txt_cords);
    }



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

//-----------------------------------------------------------------------------

sf::Vector2<int> operator % (const sf::Vector2<int>& lvalue, const sf::Vector2<int>& rvalue)
    {
    return sf::Vector2<int>(lvalue.x % rvalue.x,
           lvalue.y % rvalue.y);
    }

//-----------------------------------------------------------------------------

Vector operator % (const Vector& lvalue, const sf::Vector2<int>& rvalue)
    {
    float to4nost = 1000000;
    Vector ret (( (int) (lvalue.x * to4nost) % (int) (rvalue.x * to4nost)) / to4nost,
                ( (int) (lvalue.y * to4nost) % (int) (rvalue.y * to4nost)) / to4nost   );
    return   ret;
    }

//-----------------------------------------------------------------------------

Vector operator % (const Vector& lvalue, const Vector& rvalue)
    {
    float to4nost = 10000;
    Vector ret (( (int) (lvalue.x * to4nost) % (int) (rvalue.x * to4nost)) / to4nost,
                ( (int) (lvalue.y * to4nost) % (int) (rvalue.y * to4nost)) / to4nost   );
    return   ret;
    }

//-----------------------------------------------------------------------------
/*float operator % (const float& lvalue, const float& rvalue)
    {
    float mlt = 1000000;
    return float ( ((int) lvalue*mlt % (int) rvalue*mlt)/mlt );

    }                    */
//}
//-----------------------------------------------------------------------------



//}----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


















