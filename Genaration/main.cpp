#include <iostream>
#include <string>
#include "tinyxml.h"
#include <opencv.hpp>

using namespace cv;
using namespace std;

struct object{
	object * next = NULL;
	int x, y, w, h;
} * ob;
struct frame{
	char time[30];
	object * basket = NULL, * ball = NULL, * referee = NULL, * playerList = NULL;
	frame * next = NULL;
} * frameList;

int main(){
	char filepath[100];
	char type[10];
	int frameNum = 0, n;
	int mode = -1;
	frame * frameNow = frameList = new frame;
	object * playerNow;
	Mat img(1200, 1600, CV_8UC3, Scalar(0, 0, 0)), black(1200, 1600, CV_8UC3, Scalar(0, 0, 0));
	Rect r;
	char imgName[20];

	cout << "Input the path of the XML:" << endl;
	cin >> filepath;

	TiXmlDocument doc(filepath);
	bool loadOkay = doc.LoadFile();
	if (!loadOkay) {
		printf("Could not load test file %s. Error='%s'. Exiting.\n", filepath, doc.ErrorDesc());
		system("pause");
		exit(1);
	}

	TiXmlElement * tracks = doc.FirstChildElement("tracks");
	TiXmlElement * frameXML = NULL;
	if (tracks){
		frameXML = tracks->FirstChildElement("frame");
		cout << "Success to catch the tracks." << endl;
	}
	else
	{
		cout << "Fail to catch the tracks!" << endl;
		system("pause");
		exit(1);
	}


	while (frameXML){
		frameNum++;
		frameNow->next = new frame;
		frameNow = frameNow->next;
		strcpy_s(frameNow->time, frameXML->FirstChildElement("time")->GetText());
		playerNow = frameNow->playerList = new object;
		cout << "Frame " << frameNum << ":" << endl;

		TiXmlElement * track = frameXML->FirstChildElement("track");
		while (track){
			TiXmlElement * position = track->FirstChildElement("position");
			ob = new object;
			ob->x = atoi(position->FirstChildElement("x")->GetText());
			ob->y = atoi(position->FirstChildElement("y")->GetText());
			ob->w = atoi(position->FirstChildElement("w")->GetText());
			ob->h = atoi(position->FirstChildElement("h")->GetText());

			strcpy_s(type, track->Attribute("type"));
			cout << type << endl;
			if (strcmp(type, "player") == 0){
				playerNow->next = ob;
				playerNow = playerNow->next;
			}
			else if (strcmp(type, "ball") == 0){
				frameNow->ball = ob;
			}
			else if (strcmp(type, "basket") == 0){
				frameNow->basket = ob;
			}
			else if (strcmp(type, "referee") == 0){
				frameNow->referee = ob;
			};

			track = track->NextSiblingElement("track");
		}
		
		frameXML = frameXML->NextSiblingElement("frame");
		cout << endl;
	}

	cout << "Analysis is done!" << endl;

	while (mode != 4){
		do{
			cout << "Select the output mode:" << endl << "1.only ball 2.only others 3.all 4.quit" << endl;
			cin >> mode;
		} while (mode > 4 || mode < 1);


		if (mode != 4){
			frameNow = frameList->next;
			n = 1;
			img = black.clone();
			while (frameNow){

				if (mode == 3 || mode == 1){
					if (frameNow->ball){
						r.x = frameNow->ball->x;
						r.y = frameNow->ball->y;
						r.width = frameNow->ball->w;
						r.height = frameNow->ball->h;
						rectangle(img, r, Scalar(255, 255, 255), CV_FILLED, 8, 0);
					}
				}
				if (mode == 3 || mode == 2){
					if (frameNow->basket){
						r.x = frameNow->basket->x;
						r.y = frameNow->basket->y;
						r.width = frameNow->basket->w;
						r.height = frameNow->basket->h;
						rectangle(img, r, Scalar(255, 255, 255), CV_FILLED, 8, 0);
					}
					if (frameNow->referee){
						r.x = frameNow->referee->x;
						r.y = frameNow->referee->y;
						r.width = frameNow->referee->w;
						r.height = frameNow->referee->h;
						rectangle(img, r, Scalar(255, 255, 255), CV_FILLED, 8, 0);
					}
					ob = frameNow->playerList->next;
					while (ob){
						r.x = ob->x;
						r.y = ob->y;
						r.width = ob->w;
						r.height = ob->h;
						rectangle(img, r, Scalar(255, 255, 255), CV_FILLED, 8, 0);
						ob = ob->next;
					}
				}

				_itoa(n, imgName, 10);
				if (mode == 1)
					strcat_s(imgName, "_ball.png");
				else if (mode == 2)
					strcat_s(imgName, "_noball.png");
				else
					strcat_s(imgName, "_all.png");

				imwrite(imgName, img);
				cout << "Output of frame " << n << endl;
				img = black.clone();
				n++;
				frameNow = frameNow->next;
			}

			cout << "Done!" << endl << endl;
		}
	}
	
	
	

	while (frameList){
		if (frameList->ball)
			delete frameList->ball;
		if (frameList->basket)
			delete frameList->basket;
		if (frameList->referee)
			delete frameList->referee;
		while (frameList->playerList){
			ob = frameList->playerList;
			frameList->playerList = frameList->playerList->next;
			delete ob;
		}
		frameNow = frameList;
		frameList = frameList->next;
		delete frameNow;
	}

	system("pause");

	return 0;
}