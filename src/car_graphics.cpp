#include "car_graphics.h"
#include "Utility.h"

Menu::Menu()
{
	this->pauseButtonImage = "pausebutton.png";
	this->carButtonImage = "stepbutton.png";
	this->eggButtonImage = "resumebutton.png";

}


void Menu::setMenu(string imageBasePath,float width,float height)
{
	
	menu[0].setTextureAndPos(imageBasePath + "/" + pauseButtonImage,150+20, height / (MAX_NUMBER_OF_MENU_ITEMS+1) * 1 -25);
	menu[1].setTextureAndPos(imageBasePath + "/" + carButtonImage,150+20, height / (MAX_NUMBER_OF_MENU_ITEMS+1) * 2 -25);
	menu[2].setTextureAndPos(imageBasePath + "/" + eggButtonImage,150+20, height / (MAX_NUMBER_OF_MENU_ITEMS+1) * 3 -25);
}

void Menu::draw(sf::RenderWindow & window)
{
	for(int i=0;i<MAX_NUMBER_OF_MENU_ITEMS;i++)
	{
		window.draw(menu[i].sprite);
	}
}

DrawableSprite::DrawableSprite()
{
	x_pos = -1;
	y_pos = -1;
	groundTruth = -1;
}

void DrawableSprite::setTextureAndPos(string imagePath,int x, int y)
{
	if(!texture.loadFromFile(imagePath.c_str()))
	{
		cout<<"Error loading image"<<endl;
		return;
	}

	sprite.setTexture(texture);
        x_pos = x;
        y_pos = y;
	sprite.setPosition(x_pos,y_pos);

}

void DrawableSprite::setTexture(string imagePath)
{
	if(!texture.loadFromFile(imagePath.c_str()))
	{
		cout<<"Error loading image"<<endl;
		return;
	}
	sprite.setTexture(texture);

}

void DrawableSprite::setPosition(int x, int y)
{
	x_pos = x;
        y_pos = y;
	sprite.setPosition(x,y);
}

Simulation::Simulation()
{
	this->window_h = 400;
	this->window_w = 1000;
	this->roadImage = "car_path.png";
	this->myCarImage = "my_car.png";
	this->otherCarImage = "other_car.png";
	this->eggImage = "egg.png";
	
	textSize = 20;
	currTextPosX = 300;
	currTextPosY = 25;
}


void Simulation::initializeWindow()
{
	window.create(sf::VideoMode(window_w, window_h), "Car Driving Simulation");
	//set up road sprite
	roadDSprite.setTextureAndPos(imagesFolder + "/" + roadImage,0,0);
	//set up my car sprite
        myCarDSprite.setTextureAndPos(imagesFolder + "/" + myCarImage,65,360);


	//set frame rate to 5fps
	window.setFramerateLimit(frameRate);
	
	//create menus
	menuDSprites.setMenu(imagesFolder,window_w, window_h);	
}

void DrawableSprite::createRandomPos()
{
	
	//srand(seed + time(NULL));
	int laneNum = rand() % 3 + 1;
        setPosition(laneNum*30 + 5,0);

}


void Simulation::deleteOutOfBound(vector<DrawableSprite> &  dSpriteVec)
{
	for(vector<DrawableSprite>::iterator it = dSpriteVec.begin();it!=dSpriteVec.end();)
	{
		if(it->y_pos >= (window_h-carStepSize))
		{
			it = dSpriteVec.erase(it);
		}
		else
		{
			it++;
		}
	}
}

DrawableSprite Simulation::getOtherDSpriteWithPD(DrawableSprite otherCarDSprite,DrawableSprite eggDSprite)
{
	  float pd = rand()%100;
	  
	  if(pd < (carDensity*100))
	  {					
		
		  otherCarDSprite.createRandomPos(); 
		  //set groundtruth
		  otherCarDSprite.groundTruth = OTHERCARGROUNDTRUTH;	     
		  return otherCarDSprite;
	  }
	  else
	  {
		  eggDSprite.createRandomPos();
		  //set groundtruth
		  eggDSprite.groundTruth = EGGGROUNDTRUTH;
		  return eggDSprite;
	  }
	  
}


void Simulation::pollEvent(sf::Event event)
{
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window.close();

		if(event.type == sf::Event::MouseButtonPressed)
		{
			switch (event.key.code)
			{
			case sf::Mouse::Left:
				for(int i=0;i<MAX_NUMBER_OF_MENU_ITEMS;i++)
				{
					if(menuDSprites.menu[i].sprite.getGlobalBounds().contains(sf::Mouse::getPosition(window).x,sf::Mouse::getPosition(window).y))
					{
						//call respective menu functions
						if(i==0)
						{
							cout<<"pause"<<endl;
							pauseWindow = true;
						}
						else if(i==1)
						{
							cout<<"step"<<endl;
							stepWindow = true;
						}
						else if(i==2)
						{
							cout<<"resume"<<endl;
							pauseWindow = false;

						}
					}
						
				}
				break;
			default:
				break;
			}
		}
	}

}

void Simulation::drawCurrentView()
{
	window.clear();
		
	window.draw(roadDSprite.sprite);
	window.draw(myCarDSprite.sprite);
		

		
	for(int i=0;i<dSpriteVec.size();i++)
	{
		
		window.draw(dSpriteVec[i].sprite);
	}
		
	menuDSprites.draw(window);
        displayInfoText(window);
	window.display();

}

void Simulation::updateCurrentView()
{
	deleteOutOfBound(dSpriteVec);
	for(int i=0;i<dSpriteVec.size();i++)
	{
		
		dSpriteVec[i].setPosition(dSpriteVec[i].x_pos,dSpriteVec[i].y_pos + carStepSize);
	}
        
	myCarDSprite.setPosition(myCarDSprite.x_pos,myCarDSprite.y_pos);

	//clear text
	infoTextVec.clear();
		
}

void Simulation::displayInfoText(sf::RenderWindow & window)
{
	sf::Font font;
 	if(!font.loadFromFile("fonts/Ubuntu-L.ttf"))
	{
		cout<<"Font file not found";
		return;
	}
	int xpos = currTextPosX;
	int ypos = currTextPosY;
	for(int i=0;i<infoTextVec.size();i++)
	{
		sf::Text text;
		text.setString(infoTextVec[i].c_str());
	        text.setPosition(xpos,ypos);
		text.setFont(font);
		text.setColor(sf::Color::White);
		text.setCharacterSize(textSize);
			
		ypos = ypos + textSize + 5; 
		window.draw(text);
	}
}

void Simulation::updateWindow()
{

	sf::Clock clock;
	//other Car sprite
	DrawableSprite otherCarDSprite;
	otherCarDSprite.setTexture(imagesFolder + "/" + otherCarImage);


	//egg sprite
	DrawableSprite eggDSprite;
	eggDSprite.setTexture(imagesFolder + "/" + eggImage);
	
	int seed = 1;
	while (window.isOpen())
	{
		seed++;

		//disperse cars and eggs with some probability distribution
		
	        int pd = rand()%2;
		if(pd)
		{
			if(dSpriteVec.size()==0)
			{
				DrawableSprite tempOtherCarDSprite = otherCarDSprite;
				otherCarDSprite.createRandomPos(); 
				//otherCarDSpriteVec.push_back(otherCarDSprite);
				dSpriteVec.push_back(otherCarDSprite);
				otherCarDSprite = tempOtherCarDSprite;

			}
		}
		else
		{
			if(dSpriteVec.size()==0)
			{
				DrawableSprite tempEggDSprite = eggDSprite;
				eggDSprite.createRandomPos();
				dSpriteVec.push_back(eggDSprite);
				eggDSprite = tempEggDSprite;
			}
		}
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if(event.type == sf::Event::MouseButtonPressed)
			{
				switch (event.key.code)
				{
				case sf::Mouse::Left:
					for(int i=0;i<MAX_NUMBER_OF_MENU_ITEMS;i++)
					{
						if(menuDSprites.menu[i].sprite.getGlobalBounds().contains(sf::Mouse::getPosition(window).x,sf::Mouse::getPosition(window).y))
						{
							//call respective menu functions
							if(i==0)
							{
								cout<<"pause"<<endl;
							}
							else if(i==1)
							{
								cout<<"car"<<endl;
							}
							else if(i==2)
							{
								cout<<"egg"<<endl;
							}
						}
						
					}
					break;
				default:
					break;
				}
			}
		}

		window.clear();
		
		window.draw(roadDSprite.sprite);
		window.draw(myCarDSprite.sprite);
		

		
		for(int i=0;i<dSpriteVec.size();i++)
		{
		
			window.draw(dSpriteVec[i].sprite);
		}
		
		menuDSprites.draw(window);

		window.display();
		

		
		for(int i=0;i<dSpriteVec.size();i++)
		{
		
		        dSpriteVec[i].setPosition(dSpriteVec[i].x_pos,dSpriteVec[i].y_pos + 10);
		}
        

		deleteOutOfBound(dSpriteVec);
		//deleteOutOfBound(eggDSpriteVec);
		
		sf::Time time = clock.getElapsedTime();
		//cout<<1.0f/time.asSeconds()<<endl;
		clock.restart().asSeconds();
	}
	

}
/*
InfoText::InfoText()
{
	if(!font.loadFromFile("fonts/Ubuntu-L.ttf"))
	{
		cout<<"Font file not found";
		return;
	}
	
	text.setFont(font);
	text.setColor(sf::Color::White);
	text.setCharacterSize(20);
}
InfoText::InfoText(string inputString)
{
	if(!font.loadFromFile("fonts/Ubuntu-L.ttf"))
	{
		cout<<"Font file not found";
		return;
	}
	
	text.setFont(font);
	text.setColor(sf::Color::White);
	text.setCharacterSize(20);
	
	text.setString(inputString);
}

void InfoText::setString(string inputString)
{
	text.setString(inputString);
		
}
*/
/*
int main()
{
	
	Simulation simuObj;
	simuObj.initializeConfig();
	simuObj.initializeWindow();
	simuObj.updateWindow();
	return 0;
}
*/
