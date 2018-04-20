//
//
//
//  Matthew Emerson
//
//	Random Map Generator
//
//
//	The purpose of this program is to randomly generate maps that
//	could be used in a strtegy like game. WASD will move the camera around. 
//	Q and E will zoom the camera and holding L will allow you to see the 
//	initial number values that were generated for each tile.
//
//
//
//
//
//




#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <stack>
#include <queue>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>


int BOARDSIZE = 300;
float PI = 3.1415;
int TILESIZE = 64;

sf::RenderWindow window;

sf::View mainView;

sf::Clock gameClock;

sf::Texture grass;


//Keeping track of the type of tile
enum Tile {
	WATER,
	GRASS,
	ROCK
};

//Positions to be put in arrays
struct Pos {
	int x;
	int y;
};

//Structure to store the state of the buttons
//to give smooth movement
struct Buttons {

	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	bool escape = false;
	bool zoomIn = false;
	bool zoomOut = false;
	bool debug = false;

};


//Flood Fill Modified from version found at:
//http://www.pracspedia.com/CG/floodfill.html

int floodFill(int x, int y, Tile type, std::vector < Pos > *pos, std::vector <std::vector < Tile > > tiles){


		Pos tmp;
		tmp.x = x;
		tmp.y = y;

		pos->push_back( tmp);


		bool vr = false;
		bool vl = false;
		bool vu = false;
		bool vd = false;


		//std::cout << "Here's one\n";

		for(int z = 0; z < pos->size(); z++){


			if((*pos)[z].x == x + 1 && (*pos)[z].y == y){
				vr = true;
			}
			
			if((*pos)[z].x == x - 1 && (*pos)[z].y == y){
				vl = true;
			}
			
			if((*pos)[z].x == x && (*pos)[z].y == y + 1){
				vd = true;
			}

			if((*pos)[z].x == x && (*pos)[z].y == y -1){
				vu = true;
			}



		}


	
		
		if(x < BOARDSIZE-1)
		if(tiles[x+1][y] == type && !vr)
			floodFill(x+1,y, type, pos, tiles);

		
		if(x > 0)
		if(tiles[x-1][y] == type && !vl)
			floodFill(x-1,y, type, pos, tiles);

		
		if(y < BOARDSIZE -1)
		if(tiles[x][y+1] == type && !vd)
			floodFill(x, y+1, type, pos, tiles);

		
		if(y > 0)
		if(tiles[x][y-1] == type && !vu)	
			floodFill(x,y-1, type, pos, tiles);

		return pos->size();
}



//Function to remove small bodies of water and also every other large body
void removeWater(std::vector< std::vector <Tile> > &type){


	std::vector <Pos> visited;
	std::vector <Pos> newVisit;



	Tile water = WATER;


	for (int y = 0; y < BOARDSIZE; y++){
		for (int x = 0; x < BOARDSIZE; x++){
		
			bool beenThere = false;


			for(int z = 0; z < visited.size(); z++){

				if(visited[z].x == x && visited[z].y == y){
					beenThere = true;
				}
			}


			if(!beenThere)
			if (type[x][y] == WATER){

				bool beenThere2 = false;


				for(int z = 0; z < visited.size(); z++){

					if(visited[z].x == x && visited[z].y == y){
						beenThere2 = true;
						break;
					}

				}


				if(!beenThere2){

					int num;

					floodFill(x, y, water, &newVisit,  type );


					num = newVisit.size();

					if(num <= 50){

						for(int z = 0; z < newVisit.size(); z++){

							type[newVisit[z].x][newVisit[z].y] = GRASS;

						}
					}

					if (rand()%3){

						for(int z = 0; z < newVisit.size(); z++){

							type[newVisit[z].x][newVisit[z].y] = GRASS;

						}

					}
				
					visited.insert(visited.end(), newVisit.begin(), newVisit.end());
					newVisit.clear();

				}
			}


		}
	}




}

//Creates a river starting at a ranoom side and running until it hits another edge
void makeRiver(std::vector < std::vector < Tile> > & type){



	int direction;
	int shortDir;

	direction = rand()%4;


	int startX;
	int startY;

	//0 - right, 1 - down, 2 - left, 3 - up



	if(direction == 0){
		startX = 1;
		startY = (rand()%BOARDSIZE-5)+5;

	}

	else if(direction == 1){
		startY = 1;
		startX = (rand()%BOARDSIZE-5)+5;
	}

	else if(direction == 2){
		startX = BOARDSIZE-2;
		startY = (rand()%BOARDSIZE-5)+5;
	}

	else if(direction == 3){
		startY = BOARDSIZE-2;
		startX = (rand()%BOARDSIZE-5)+5;
	}

	int inc = 0;

	while(startX >= 1 && startX < BOARDSIZE-1 && startY >= 1 && startY < BOARDSIZE-1){


		type[startX-1][startY-1] = WATER;
		type[startX-1][startY] = WATER;
		type[startX-1][startY+1] = WATER;
		type[startX][startY-1] = WATER;
		type[startX][startY+1] = WATER;
		type[startX+1][startY-1] = WATER;
		type[startX+1][startY] = WATER;
		type[startX+1][startY+1] = WATER;


		if(!(rand()%3))
			inc = (rand()%3)-1;



		if(direction == 0){
			startX += 1;
			startY += inc;

		}

		else if(direction == 1){
			startY += 1;
			startX += inc;
		}

		else if(direction == 2){
			startX -= 1;
			startY += inc;
		}

		else if(direction == 3){
			startY -= 1;
			startX += inc;
		}	





	}



}

//Drunken walk that gets smoothed out later
//Raises value of heightmap at each step
void makeBoardMountains( std::vector< std::vector <int> >  &br){

	int mountains = (rand()%10)+3;

	for(int mount = 0; mount < mountains; mount++){

		int tiles = (rand()%10000)+50;
		int startX = rand() % BOARDSIZE;
		int startY = rand() % BOARDSIZE;


		for(int count = 0; count < tiles; count++){

			int move = rand()%4;

			if(move == 0){

				if(startX == 0){
					startX += 1;
				}

				else{
					startX -= 1;
				}
			}
			
			else if(move == 1){
				if(startX == BOARDSIZE-1){
					startX -= 1;
				}

				else{
					startX += 1;
				}
			}
			
			else if(move == 2){
				if(startY == 0){
					startY += 1;
				}

				else{
					startY -= 1;
				}	
			}

			else if(move == 3){
				if(startY == BOARDSIZE-1){
					startY -= 1;
				}

				else{
					startY += 1;
				}
			}


			br[startX][startY] += 100;

		}


	}


}

//Initial Number generation
void makeBoardHeights( std::vector< std::vector <int> >  &br){


 //If you are divinding by 360 degrees, use either 

	//TileValues tval;

	std::vector< std::vector <int> > vals;

	for(int x = 0; x < BOARDSIZE; x++){

		std::vector <int> row;
		row.resize(BOARDSIZE);
		vals.push_back(row);
	}


	for (int y = 0; y < BOARDSIZE; y++){
		for(int x = 0; x < BOARDSIZE; x++){

			br[x][y] = rand()%255;

		}
	}

	/*int mountains = (rand()%3)+1;

	for(int mount = 0; mount < mountains; mount++){


		int sediment = ((rand()%10000000)*50)+1000;

		int depositX = (rand()%(BOARDSIZE-2))+1;
		int depositY = (rand()%(BOARDSIZE-2))+1;

		//for(int iters = 0; iters < (sediment/50); iters++){

		br[depositX][depositY] += sediment;

		//	shake(br, depositX, depositY);


		//}


	} */

	makeBoardMountains(br);


	for(int times = 0; times < 7; times++)
		for (int y = 0; y < BOARDSIZE; y++){
			for(int x = 0; x < BOARDSIZE; x++){


				if(x == 0 && y == 0){
					int tmp = 0;

					tmp += br[x][y];
					tmp += br[x][y+1];
					tmp += br[x+1][y];
					tmp += br[x+1][y+1];

					br[x][y] = tmp/4;
				}

				else if (x == BOARDSIZE-1 && y == BOARDSIZE-1){
					int tmp = 0;

					tmp += br[x][y];
					tmp += br[x][y-1];
					tmp += br[x-1][y-1];
					tmp += br[x-1][y];

					br[x][y] = tmp/4;
				}

				else if (x == 0 && y == BOARDSIZE -1){
					int tmp = 0;

					tmp += br[x][y];
					tmp += br[x][y-1];
					tmp += br[x+1][y-1];
					tmp += br[x+1][y];


					br[x][y] = tmp/4;
				}

				else if(x == BOARDSIZE-1 && y == 0){
					int tmp = 0;

					tmp += br[x][y];
					tmp += br[x][y+1];
					tmp += br[x-1][y];
					tmp += br[x-1][y+1];

					br[x][y] = tmp/4;
				}

				else if(x == 0){
					int tmp = 0;

					tmp += br[x][y];
					tmp += br[x][y+1];
					tmp += br[x][y-1];
					tmp += br[x+1][y-1];
					tmp += br[x+1][y];
					tmp += br[x+1][y+1];


					br[x][y] = tmp/6;
				}

				else if(y == 0){
					int tmp = 0;

					tmp += br[x][y];
					tmp += br[x][y+1];
					tmp += br[x+1][y];
					tmp += br[x+1][y+1];
					tmp += br[x-1][y];
					tmp += br[x-1][y+1];

					br[x][y] = tmp/6;
				}

				else if(x == BOARDSIZE-1){
					int tmp = 0;

					tmp += br[x][y];
					tmp += br[x][y+1];
					tmp += br[x][y-1];
					tmp += br[x-1][y-1];
					tmp += br[x-1][y];
					tmp += br[x-1][y+1];

					br[x][y] = tmp/6;
				}

				else if(y == BOARDSIZE-1){
					int tmp = 0;

					tmp += br[x][y];
					tmp += br[x][y-1];
					tmp += br[x+1][y-1];
					tmp += br[x+1][y];
					tmp += br[x-1][y-1];
					tmp += br[x-1][y];

					br[x][y] = tmp/6;
				}

				else{
					int tmp = 0;

					tmp += br[x][y];
					tmp += br[x][y+1];
					tmp += br[x][y-1];
					tmp += br[x+1][y-1];
					tmp += br[x+1][y];
					tmp += br[x+1][y+1];
					tmp += br[x-1][y-1];
					tmp += br[x-1][y];
					tmp += br[x-1][y+1];

					br[x][y] = tmp/9;



				}



			}
		}

	//br = vals;


}

//Assign terrain types based on heightmap
void makeBoardType( std::vector< std::vector <int> > height, std::vector < std::vector < Tile> > &type ){


	for(int y = 0; y < BOARDSIZE; y++){
		for(int x = 0; x < BOARDSIZE; x++){

			if(height[x][y] >= 160){
				type[x][y] = ROCK;
			}

			else if (height[x][y] < 160 && height[x][y] > 115){
				type[x][y] = GRASS;
			}

			else if (height[x][y] <= 115){
				type[x][y] = WATER;
			}


		}
	}


}



//Populate vertex arrays with verticies and textures
void makeBoardTiles( std::vector< std::vector <Tile> > type, sf::VertexArray &verts, sf::VertexArray &bg, sf::VertexArray &waterTiles ){


	for(int y = 0; y < BOARDSIZE; y++){
		for(int x = 0; x < BOARDSIZE; x++){


			sf::Color color(255, 255, 255, 255);

			sf::Vertex vert;

			int texX;
			int texY;

			int alpha1;
			int alpha2;
			int alpha3;
			int alpha4;

			texX = 0;
			texY = 32;

			vert.position = sf::Vector2f(x*TILESIZE, y*TILESIZE);
			vert.color = (color);
			vert.texCoords = sf::Vector2f(texX, texY);
			bg.append(vert);

			vert.position = sf::Vector2f(x*TILESIZE+TILESIZE, y*TILESIZE);
			vert.color = (color);
			vert.texCoords = sf::Vector2f(texX+32, texY);
			bg.append(vert);



			vert.position = sf::Vector2f(x*TILESIZE+TILESIZE, y*TILESIZE+TILESIZE);
			vert.color = (color);
			vert.texCoords = sf::Vector2f(texX + 32, texY + 32);
			bg.append(vert);


			vert.position = sf::Vector2f(x*TILESIZE+TILESIZE, y*TILESIZE+TILESIZE);
			vert.color = (color);
			vert.texCoords = sf::Vector2f(texX +32, texY +32);
			bg.append(vert);



			vert.position = sf::Vector2f(x*TILESIZE, y*TILESIZE+TILESIZE);
			vert.color = (color);
			vert.texCoords = sf::Vector2f(texX, texY + 32);
			bg.append(vert);


			vert.position = sf::Vector2f(x*TILESIZE, y*TILESIZE);
			vert.color = (color);
			vert.texCoords = sf::Vector2f(texX, texY);
			bg.append(vert);




			if(type[x][y] == WATER){
				//color = sf::Color::Blue;
				texX = 3*32;
				texY = 32*12;


				if(x > 0 && x < BOARDSIZE-1 && y > 0 && y < BOARDSIZE-1){


					//EDGES
					if(type[x-1][y] == GRASS && type[x+1][y] == WATER && type [x][y-1] == WATER && type[x][y+1] == WATER){
						alpha1 = 20;
						alpha2 = 255;
						alpha3 = 255;
						alpha4 = 20;

					}

					else if(type[x-1][y] == WATER && type[x+1][y] == GRASS && type [x][y-1] == WATER && type[x][y+1] == WATER){
						alpha1 = 255;
						alpha2 = 20;
						alpha3 = 20;
						alpha4 = 255;

					}

					else if(type[x-1][y] == WATER && type[x+1][y] == WATER && type [x][y-1] == GRASS && type[x][y+1] == WATER){
						alpha1 = 20;
						alpha2 = 20;
						alpha3 = 255;
						alpha4 = 255;

					}

					else if(type[x-1][y] == WATER && type[x+1][y] == WATER && type [x][y-1] == WATER && type[x][y+1] == GRASS){
						alpha1 = 255;
						alpha2 = 255;
						alpha3 = 20;
						alpha4 = 20;

					}





					///CORNERS
					else if(type[x-1][y] == GRASS && type[x+1][y] == WATER && type [x][y-1] == GRASS && type[x][y+1] == WATER){
						alpha1 = 20;
						alpha2 = 20;
						alpha3 = 255;
						alpha4 = 20;

					}

					else if(type[x-1][y] == GRASS && type[x+1][y] == WATER && type [x][y-1] == WATER && type[x][y+1] == GRASS){
						alpha1 = 20;
						alpha2 = 255;
						alpha3 = 20;
						alpha4 = 20;

					}

					else if(type[x-1][y] == WATER && type[x+1][y] == GRASS && type [x][y-1] == GRASS && type[x][y+1] == WATER){
						alpha1 = 20;
						alpha2 = 20;
						alpha3 = 20;
						alpha4 = 255;

					}

					else if(type[x-1][y] == WATER && type[x+1][y] == GRASS && type [x][y-1] == WATER && type[x][y+1] == GRASS){
						alpha1 = 255;
						alpha2 = 20;
						alpha3 = 20;
						alpha4 = 20;

					}


					//INSIIDE CORNERS
			
					else if(type[x-1][y] == WATER && type[x+1][y] == WATER && type [x][y-1] == WATER && type[x][y+1] == WATER &&  type[x-1][y-1] == GRASS){
						alpha1 = 50;
						alpha2 = 255;
						alpha3 = 255;
						alpha4 = 255;

					}

					else if(type[x-1][y] == WATER && type[x+1][y] == WATER && type [x][y-1] == WATER && type[x][y+1] == WATER && type[x-1][y+1] == GRASS){
						alpha1 = 255;
						alpha2 = 255;
						alpha3 = 255;
						alpha4 = 50;

					}

					else if(type[x-1][y] == WATER && type[x+1][y] == WATER && type [x][y-1] == WATER && type[x][y+1] == WATER && type[x+1][y-1] == GRASS){
						alpha1 = 255;
						alpha2 = 50;
						alpha3 = 255;
						alpha4 = 255;

					}

					else if(type[x-1][y] == WATER && type[x+1][y] == WATER && type [x][y-1] == WATER && type[x][y+1] == WATER && type[x+1][y+1] == GRASS){
						alpha1 = 255;
						alpha2 = 255;
						alpha3 = 50;
						alpha4 = 255;

					}


					//lone peices

					else if(type[x-1][y] == GRASS && type[x+1][y] == GRASS && type [x][y-1] == GRASS && type[x][y+1] == WATER){
						alpha1 = 50;
						alpha2 = 50;
						alpha3 = 50;
						alpha4 = 50;

					}

					else if(type[x-1][y] == GRASS && type[x+1][y] == GRASS && type [x][y-1] == WATER && type[x][y+1] == GRASS){
						alpha1 = 50;
						alpha2 = 50;
						alpha3 = 50;
						alpha4 = 50;

					}

					else if(type[x-1][y] == GRASS && type[x+1][y] == WATER && type [x][y-1] == GRASS && type[x][y+1] == GRASS){
						alpha1 = 50;
						alpha2 = 50;
						alpha3 = 50;
						alpha4 = 50;

					}

					else if(type[x-1][y] == WATER && type[x+1][y] == GRASS && type [x][y-1] == GRASS && type[x][y+1] == GRASS){
						alpha1 = 50;
						alpha2 = 50;
						alpha3 = 50;
						alpha4 = 50;

					}



					else{
						alpha1 = 255;
						alpha2 = 255;
						alpha3 = 255;
						alpha4 = 255;

					}




				}

			}

			else if (type[x][y] == GRASS){
				//color = sf::Color::Green;
				texX = 0;
				texY = 32;


				int tmp = rand()%50;

				if(tmp == 0){
					texX = 32;
				}



				alpha1 = 255;
				alpha2 = 255;
				alpha3 = 255;
				alpha4 = 255;
			}

			else if (type[x][y] == ROCK){
				//color = sf::Color::Red;
				texX = 32*7;
				texY = 0;

				alpha1 = 255;
				alpha2 = 255;
				alpha3 = 255;
				alpha4 = 255;

				if(x > 0 && x < BOARDSIZE-1 && y > 0 && y < BOARDSIZE-1){

					if(type[x][y-1] != ROCK || type[x-1][y] != ROCK){
						texX = 32*6;
					}

					else if (type[x][y+1] != ROCK || type[x+1][y] != ROCK){
						texX = 32*8;
					}

				}

			}


			if(type[x][y] == ROCK || type[x][y] == GRASS){
				vert.position = sf::Vector2f(x*TILESIZE, y*TILESIZE);
				vert.color = (color);
				vert.color.a = alpha1;
				vert.texCoords = sf::Vector2f(texX, texY);
				verts.append(vert);

				vert.position = sf::Vector2f(x*TILESIZE+TILESIZE, y*TILESIZE);
				vert.color = (color);
				vert.color.a = alpha2;
				vert.texCoords = sf::Vector2f(texX+32, texY);
				verts.append(vert);



				vert.position = sf::Vector2f(x*TILESIZE+TILESIZE, y*TILESIZE+TILESIZE);
				vert.color = (color);
				vert.color.a = alpha3;
				vert.texCoords = sf::Vector2f(texX + 32, texY + 32);
				verts.append(vert);


				vert.position = sf::Vector2f(x*TILESIZE+TILESIZE, y*TILESIZE+TILESIZE);
				vert.color = (color);
				vert.color.a = alpha3;
				vert.texCoords = sf::Vector2f(texX +32, texY +32);
				verts.append(vert);



				vert.position = sf::Vector2f(x*TILESIZE, y*TILESIZE+TILESIZE);
				vert.color = (color);
				vert.color.a = alpha4;
				vert.texCoords = sf::Vector2f(texX, texY + 32);
				verts.append(vert);


				vert.position = sf::Vector2f(x*TILESIZE, y*TILESIZE);
				vert.color = (color);
				vert.color.a = alpha1;
				vert.texCoords = sf::Vector2f(texX, texY);
				verts.append(vert);
			}

			else {
				vert.position = sf::Vector2f(x*TILESIZE, y*TILESIZE);
				vert.color = (color);
				vert.color.a = alpha1;
				vert.texCoords = sf::Vector2f(texX, texY);
				waterTiles.append(vert);

				vert.position = sf::Vector2f(x*TILESIZE+TILESIZE, y*TILESIZE);
				vert.color = (color);
				vert.color.a = alpha2;
				vert.texCoords = sf::Vector2f(texX+32, texY);
				waterTiles.append(vert);



				vert.position = sf::Vector2f(x*TILESIZE+TILESIZE, y*TILESIZE+TILESIZE);
				vert.color = (color);
				vert.color.a = alpha3;
				vert.texCoords = sf::Vector2f(texX + 32, texY + 32);
				waterTiles.append(vert);


				vert.position = sf::Vector2f(x*TILESIZE+TILESIZE, y*TILESIZE+TILESIZE);
				vert.color = (color);
				vert.color.a = alpha3;
				vert.texCoords = sf::Vector2f(texX +32, texY +32);
				waterTiles.append(vert);



				vert.position = sf::Vector2f(x*TILESIZE, y*TILESIZE+TILESIZE);
				vert.color = (color);
				vert.color.a = alpha4;
				vert.texCoords = sf::Vector2f(texX, texY + 32);
				waterTiles.append(vert);


				vert.position = sf::Vector2f(x*TILESIZE, y*TILESIZE);
				vert.color = (color);
				vert.color.a = alpha1;
				vert.texCoords = sf::Vector2f(texX, texY);
				waterTiles.append(vert);
			}


		}
	}


}

//Make tress grow in real time
void updateTrees( std::vector < std::vector <Tile> >  &type, std::vector< std::vector <int> > &objects, sf::VertexArray &va){

	for(int y = 0; y < BOARDSIZE; y++){
		for(int x = 0; x < BOARDSIZE; x++){
		
			if( objects[x][y] == 1 && rand()%50 == 0){

				int friends = 0;

				for(int y2 =  y-15; y2 <= y+15; y2++){
					for(int x2 =  x-15; x2 <= x+15; x2++){

						if(y2 > y -2 && y2 < y + 2 && x2 > x -2 && x2 < x + 2){
							if(objects[x2][y2] == 1){

								friends++;

							}
						}


						if( (abs(y-y2) == 15 || abs(x-x2) == 15) && rand()%20000 == 0)
						if(x2 >= 15 && x2 < BOARDSIZE-15 && y2 >= 15 && y2 < BOARDSIZE-15)
						if(type[x2][y2] == GRASS && objects[x2][y2] == 0){

							objects[x2][y2] = 1;

							sf::Color color(255, 255, 255, 255);
							sf::Vertex vert;

							int texX;
							int texY;

							if(objects[x][y] == 1){
								//color = sf::Color::Blue;
								texX = 32*(rand()%2)*6;
								texY = 32*6;
							

								vert.position = sf::Vector2f(x2*TILESIZE, y2*TILESIZE);
								vert.color = (color);
								vert.texCoords = sf::Vector2f(texX, texY);
								//verts[x][y].texCoords();
								//std::cout << x*TILESIZE << "  " << y*TILESIZE << ',';
								va.append(vert);

								vert.position = sf::Vector2f(x2*TILESIZE+TILESIZE, y2*TILESIZE);
								vert.color = (color);
								vert.texCoords = sf::Vector2f(texX+32, texY);
								//verts[x][y].texCoords();
								//std::cout << x*TILESIZE+TILESIZE << "  " << y*TILESIZE << ',';
								va.append(vert);



								vert.position = sf::Vector2f(x2*TILESIZE+TILESIZE, y2*TILESIZE+TILESIZE);
								vert.color = (color);
								vert.texCoords = sf::Vector2f(texX + 32, texY + 32);

								//verts[x][y].texCoords();
								//std::cout << x*TILESIZE+TILESIZE << "  " << y*TILESIZE+TILESIZE << ',';
								va.append(vert);


								vert.position = sf::Vector2f(x2*TILESIZE+TILESIZE, y2*TILESIZE+TILESIZE);
								vert.color = (color);
								vert.texCoords = sf::Vector2f(texX +32, texY +32);

								//verts[x][y].texCoords();
								//std::cout << x*TILESIZE+TILESIZE << "  " << y*TILESIZE+TILESIZE << ',';
								va.append(vert);



								vert.position = sf::Vector2f(x2*TILESIZE, y2*TILESIZE+TILESIZE);
								vert.color = (color);
								vert.texCoords = sf::Vector2f(texX, texY + 32);

								//verts[x][y].texCoords();
								//std::cout << x*TILESIZE << "  " << y*TILESIZE+TILESIZE << ',';
								va.append(vert);


								vert.position = sf::Vector2f(x2*TILESIZE, y2*TILESIZE);
								vert.color = (color);
								vert.texCoords = sf::Vector2f(texX, texY);

								//verts[x][y].texCoords();
								//std::cout << x*TILESIZE << "  " << y*TILESIZE << std::endl;;
								va.append(vert);
							}

						}


					}
				}

			}


		}
	}

}

//Make the initial forests
void makeTrees( std::vector < std::vector <Tile> >  &type, std::vector< std::vector <int> > &objects){

		

	int woods = (rand()%4)+5;


	for (int y = 0; y < BOARDSIZE; y++){
		for (int x = 0; x < BOARDSIZE; x++){

			objects[x][y] = 0;

		}

	}


	while (woods > 0){

		woods--;

		int time = (rand()%500)+1000;

		bool isGrass = false;


		int tmpx;
		int tmpy;

		while(!isGrass){

			tmpx = rand()%BOARDSIZE;
			tmpy = rand()%BOARDSIZE;


			if( type[tmpx][tmpy] == GRASS){

				isGrass = true;
			}
		}

		std::queue <Pos> valid;

		Pos tmp;
		tmp.x = tmpx;
		tmp.y = tmpy;

		valid.push(tmp);


		while(valid.size() > 0 && time > 0){

			time--;

			Pos front;

			front = valid.front();

			valid.pop();

			objects[front.x][front.y] = 1;



			for(int y = front.y-15; y<=front.y+15; y++){
				for(int x = front.x-15; x<=front.x+15; x++){


					if( (abs(front.y-y) == 15 || abs(front.x-x) == 15) && rand()%60 == 0)
					if(x >= 15 && x < BOARDSIZE-15 && y >= 15 && y < BOARDSIZE-15)
					if(type[x][y] == GRASS && objects[x][y] == 0){

						objects[x][y] = 0;


						tmp.x = x;
						tmp.y = y;

						valid.push(tmp);
					}


				}
			}

			//valid.push(front);


		}

	}



}

//Assign verticies to vertex array for layer containing trees. 
void makeObjects(std::vector < std::vector <int> > objects, sf::VertexArray &va){


	for (int y = 0; y < BOARDSIZE; y++){
		for (int x = 0; x < BOARDSIZE; x++){

			sf::Color color(255, 255, 255, 255);
			sf::Vertex vert;

			int texX;
			int texY;

			if(objects[x][y] == 1){
				//color = sf::Color::Blue;
				texX = 32*(rand()%2)*6;
				texY = 32*6;
			

				vert.position = sf::Vector2f(x*TILESIZE, y*TILESIZE);
				vert.color = (color);
				vert.texCoords = sf::Vector2f(texX, texY);
				//verts[x][y].texCoords();
				//std::cout << x*TILESIZE << "  " << y*TILESIZE << ',';
				va.append(vert);

				vert.position = sf::Vector2f(x*TILESIZE+TILESIZE, y*TILESIZE);
				vert.color = (color);
				vert.texCoords = sf::Vector2f(texX+32, texY);
				//verts[x][y].texCoords();
				//std::cout << x*TILESIZE+TILESIZE << "  " << y*TILESIZE << ',';
				va.append(vert);



				vert.position = sf::Vector2f(x*TILESIZE+TILESIZE, y*TILESIZE+TILESIZE);
				vert.color = (color);
				vert.texCoords = sf::Vector2f(texX + 32, texY + 32);

				//verts[x][y].texCoords();
				//std::cout << x*TILESIZE+TILESIZE << "  " << y*TILESIZE+TILESIZE << ',';
				va.append(vert);


				vert.position = sf::Vector2f(x*TILESIZE+TILESIZE, y*TILESIZE+TILESIZE);
				vert.color = (color);
				vert.texCoords = sf::Vector2f(texX +32, texY +32);

				//verts[x][y].texCoords();
				//std::cout << x*TILESIZE+TILESIZE << "  " << y*TILESIZE+TILESIZE << ',';
				va.append(vert);



				vert.position = sf::Vector2f(x*TILESIZE, y*TILESIZE+TILESIZE);
				vert.color = (color);
				vert.texCoords = sf::Vector2f(texX, texY + 32);

				//verts[x][y].texCoords();
				//std::cout << x*TILESIZE << "  " << y*TILESIZE+TILESIZE << ',';
				va.append(vert);


				vert.position = sf::Vector2f(x*TILESIZE, y*TILESIZE);
				vert.color = (color);
				vert.texCoords = sf::Vector2f(texX, texY);

				//verts[x][y].texCoords();
				//std::cout << x*TILESIZE << "  " << y*TILESIZE << std::endl;;
				va.append(vert);

			}

		}

	}


}


//Uses sin function to asjust color values of all verticies to create a day/night effect
void updateDayTime(float time, sf::VertexArray &va){

	float tmp = time/10*M_PI;


	int r =    floor( ( (sin(tmp)+1)/2)*181);
	int g =    floor( ( (sin(tmp)+1)/2)*181);
	int b =    floor( ( (sin(tmp)+1)/2)*117);

	if(r > 161){
		r = 161;
	}	

	if(g > 161){
		g = 161;
	}	

	if(b  > 97){
		b = 97;
	}


	//if(per == 0){
		for(int x = 0; x < va.getVertexCount(); x++){

			int alpha = va[x].color.a;

			va[x].color = sf::Color(r+94, g+94, b+158, alpha);
		}
	//}



}


//Changes texture on water to create animation
//Water tiles are side by side in .png image
//So we can just adjust the x value. 
void updateWater(sf::VertexArray &va, int state){
	for (int y = 0; y < va.getVertexCount(); y++){

		if(state == 0){

			va[y].texCoords.x -= 64;
		}

		else{
			va[y].texCoords.x += 32;
		}
	}


}





int main(int argc, char* argv[]){


	int seed = 123;

	if(argc > 1){

		for(int x = 1; x < argc; x+=2){
			if( argv[x] == "-s"){
				seed = atoi(argv[x+1]);
			}

			/*else if( strcmp(argv[x], "-b") == 0){
				BOARDSIZE = atoi(argv[x+1]);
			}*/
		}

	}

	srand(seed);


	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

	window.create(sf::VideoMode(desktop.width, desktop.height), "My Game Window");

	mainView.reset(sf::FloatRect(0, 0, desktop.width, desktop.height));

	window.setView(mainView);

	window.setKeyRepeatEnabled(false);


	int time = 0;

	float realtime = 0;
	int timeInDay = 60;

	Buttons buttons;

	sf::Event event;


	//LOAD ASSETS**************************************

	sf::RenderStates states;

	//sf::Texture grass;

	if(!grass.loadFromFile("grounds.png")){

		std::cout << "Cant Open Texture File" << std::endl;

		exit(1);
	}

	//sf::Sprite test;

	//test.setTexture(grass);

	//test.setPosition(0,0);



	states.texture = &grass;


	sf::Font  mainFont;

	if(!mainFont.loadFromFile("Square.ttf")){


		exit(1);
	}

	sf::Text txt;

	txt.setFont(mainFont);

	txt.setFillColor(sf::Color( 255, 255, 255, 255));

	txt.setString("Hello World ");

	txt.setPosition(10,10);

	int count = 0;



	//float board[BOARDSIZE][BOARDSIZE];

	//sf::Text boardTxt[BOARDSIZE][BOARDSIZE];



	std::vector< std::vector <int> > board;

	std::vector < std::vector < sf::Text> > boardTxt;

	std::vector < std::vector < Tile> > boardType;

	sf::VertexArray tiles(sf::Triangles);

	sf::VertexArray waterTiles(sf::Triangles);

	sf::VertexArray bgGrass(sf::Triangles);



	for(int x = 0; x < BOARDSIZE; x++){

		std::vector <int> row;
		row.resize(BOARDSIZE);
		board.push_back(row);
	}

	for(int x = 0; x < BOARDSIZE; x++){

		std::vector <sf::Text> row;
		row.resize(BOARDSIZE);
		boardTxt.push_back(row);
	}

	for(int x = 0; x < BOARDSIZE; x++){

		std::vector <Tile> row;
		row.resize(BOARDSIZE);
		boardType.push_back(row);
	}


	//Object Layer for trees
	std::vector < std::vector < int> > objectLayer;

	sf::VertexArray objects(sf::Triangles);

	//tiles.resize(BOARDSIZE*BOARDSIZE);

	for(int x = 0; x < BOARDSIZE; x++){

		std::vector <int> row;
		row.resize(BOARDSIZE);
		objectLayer.push_back(row);
	}





	makeBoardHeights(board);

	//makeBoardMountains(board);


	for(int x = 0; x < BOARDSIZE; x++){
		for(int y = 0; y < BOARDSIZE; y++){

			boardTxt[x][y].setFont(mainFont);
			boardTxt[x][y].setCharacterSize(16);
			boardTxt[x][y].setFillColor(sf::Color::Black);
			boardTxt[x][y].setString(std::to_string(board[x][y]));
			boardTxt[x][y].setPosition(x*TILESIZE, y*TILESIZE);

		}
	}


	makeBoardType(board, boardType);


	removeWater(boardType);

	makeRiver(boardType);


	makeTrees(boardType, objectLayer );

	makeBoardTiles(boardType, tiles, bgGrass, waterTiles);

	makeObjects(objectLayer, objects);

	std::cout << tiles.getVertexCount() << std::endl;

	int waterState = 0;

	int counter = 0;


	//Begin Main Loop
	while(window.isOpen()){

		counter++;

		sf::Time delta = gameClock.getElapsedTime();

		gameClock.restart();

		float dt = delta.asSeconds();

		realtime += dt;

		if(counter == 10 ){

			counter = 0;

			if (waterState == 2){
				waterState = 0;
			}

			else{
				waterState++;
			}

			updateWater(waterTiles, waterState);


		}

		updateTrees(boardType, objectLayer, objects );
		//makeObjects(objectLayer, objects);
		



		//time = floor(realtime);
		updateDayTime(realtime, objects);
		updateDayTime(realtime, tiles);
		updateDayTime(realtime, bgGrass);
		updateDayTime(realtime, waterTiles);



		//React To Events
		while(window.pollEvent(event)){


			//Window Close
			if(event.type == sf::Event::Closed){
				window.close();
			}

			if(event.type == sf::Event::KeyPressed){

				if(event.key.code == sf::Keyboard::W){
					buttons.up = true;
				}

				else if(event.key.code == sf::Keyboard::A){
					buttons.left = true;
				}

				else if(event.key.code == sf::Keyboard::S){
					buttons.down = true;
				}

				else if(event.key.code == sf::Keyboard::D){
					buttons.right = true;
				}

				else if(event.key.code == sf::Keyboard::Escape){
					buttons.escape = true;
				}

				else if(event.key.code == sf::Keyboard::Q){
					buttons.zoomOut = true;
				}

				else if(event.key.code == sf::Keyboard::E){
					buttons.zoomIn = true;
				}

				else if(event.key.code == sf::Keyboard::L){
					buttons.debug = true;
				}



			}

			else if(event.type == sf::Event::KeyReleased){

				if(event.key.code == sf::Keyboard::W){
					buttons.up = false;
				}

				else if(event.key.code == sf::Keyboard::A){
					buttons.left = false;
				}

				else if(event.key.code == sf::Keyboard::S){
					buttons.down = false;
				}

				else if(event.key.code == sf::Keyboard::D){
					buttons.right = false;
				}

				else if(event.key.code == sf::Keyboard::Escape){
					buttons.escape = false;
				}

				else if(event.key.code == sf::Keyboard::Q){
					buttons.zoomOut = false;
				}

				else if(event.key.code == sf::Keyboard::E){
					buttons.zoomIn = false;
				}

				else if(event.key.code == sf::Keyboard::L){
					buttons.debug = false;
				}

			}



		}



		if(buttons.left){
			mainView.move(-1000*dt, 0);
		}
		
		else if(buttons.right){
			mainView.move(1000*dt, 0);
		}
		
		if(buttons.up){
			mainView.move(0, -1000*dt);
		}

		else if(buttons.down){
			mainView.move(0, 1000*dt);
		}

		if(buttons.zoomIn){
			mainView.zoom(0.9);
		}

		else if (buttons.zoomOut){
			mainView.zoom(1.1);
		}



		window.setView(mainView);


		if(count > 1000){
			txt.setString("I Changed");
		}


		window.clear();



		window.draw(bgGrass, &grass);

		window.draw(tiles, &grass);

		window.draw(waterTiles, &grass);


		window.draw(objects, &grass);


		//window.draw(txt);

		if(buttons.debug)
		for(int x = 0; x < BOARDSIZE; x++){
			for(int y = 0; y < BOARDSIZE; y++){

				window.draw(boardTxt[x][y]);

			}
		}

		//window.draw(test);



		window.display();


		count++;



	}

}
