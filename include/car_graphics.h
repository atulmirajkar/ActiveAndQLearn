#ifndef ATUL_CAR_GRAPHICS
#define ATUL_CAR_GRAPHICS

#include <SFML/Graphics.hpp>
#include<iostream>
#include<stdio.h>
#include<fstream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>    


using namespace std;

#define MYCARGROUNDTRUTH 0
#define OTHERCARGROUNDTRUTH 1
#define EGGGROUNDTRUTH 2
#define MAX_NUMBER_OF_MENU_ITEMS 3

class DrawableSprite{
	
	sf::Sprite sprite;
	sf::Texture texture;
	int x_pos;
	int y_pos;
	//can have values
	/*
	  
	  #define MYCARGROUNDTRUTH 0
	  #define OTHERCARGROUNDTRUTH 1
	  #define EGGGROUNDTRUTH 2
	 */
	int groundTruth;
public:
	friend class Simulation;
	friend class QTableDrive;
	friend class Menu;
	DrawableSprite();
	void setTextureAndPos(string imagePath,int x, int y);
	void setTexture(string imagePath);
	void createRandomPos();
	void setPosition(int x, int y);
};


class Menu{
private:
	
	int selectedItemIndex;
	DrawableSprite  menu[MAX_NUMBER_OF_MENU_ITEMS];
	string pauseButtonImage;
	string eggButtonImage;
	string carButtonImage;
	
public:
	friend class Simulation;
	Menu();
	
	void setMenu(string imageBasePath,float width,float height);
	void draw(sf::RenderWindow &);
	void pauseFrame();
	void isEgg();
	void isOtherCar();
		
};
/*
class InfoText{
private:
	sf::Font font;
	sf::Text text;
public:
	InfoText();
	InfoText(string);
	void setString(string inputString);
	friend class Simulation;

};
*/

class Simulation{
	int window_h;
	int window_w;
	int frameRate;
	string roadImage;
	string myCarImage;
	string otherCarImage;
	string eggImage;
	//Main window
	sf::RenderWindow window;
	//road sprite and texture
	DrawableSprite roadDSprite;

	//My car sprite and texture
	DrawableSprite myCarDSprite; 
	

	//common Drawable Sprite vector
	vector<DrawableSprite> dSpriteVec;

	//menu sprites
	Menu menuDSprites;
	
	//images base folder
	string imagesFolder;

	//density with which cars and eggs to be added
	float carDensity;

	//step size
	int carStepSize;
	
	//textsize
	int textSize;
	
	vector<string> infoTextVec;
	
	int currTextPosX;
	int currTextPosY;
	
	bool pauseWindow;
	bool stepWindow;
public:
	friend class QTableDrive;
	Simulation();

	void initializeWindow();
	void updateWindow();
	void deleteOutOfBound(vector<DrawableSprite> & dSpriteVec);
	DrawableSprite getOtherDSpriteWithPD(DrawableSprite otherCarDSprite,DrawableSprite eggDSprite);
	void pollEvent(sf::Event event);
	void drawCurrentView();
	void updateCurrentView();
	//DrawableSprite createRandomOtherCar(float);
	void displayInfoText(sf::RenderWindow & window);

};

#endif
