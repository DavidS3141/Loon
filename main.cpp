/*
 * Main.cpp
 *
 *  Created on: 04.02.2016
 *      Author: David
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include "LoonState.h"

using namespace std;

int R,C,A,L,V,B,T,rs,cs;
bool** Lgrid;
vector<pair<int,int> > Lcells;
int**** winds;
vector<pair<int,int> > delVvec;
default_random_engine randomGenerator;

void doAlgo(){
	LoonState currentMoves;
	for(int i=0;i<53;i++){
		currentMoves.evaluate();
		if(currentMoves.getPoints()/(double)T/L>0.5)
			break;
		currentMoves.optimize();
	}
	currentMoves.evaluate();
	currentMoves.shortDump();
}

int main()
{
	ifstream in("loon_r70_c300_a8_radius7_saturation_250.in");
	in >> R >> C >> A;
	in >> L >> V >> B >> T;
	in >> rs >> cs;

	Lgrid=new bool*[R];
	for(int row=0;row<R;++row){
		Lgrid[row]=new bool[C];
		for(int col=0;col<C;++col)
			Lgrid[row][col]=false;
	}

	for(int i=0;i<L;++i){
		int row, col;
		in >> row >> col;
		Lgrid[row][col]=true;
		Lcells.push_back( make_pair(row,col));

	}

	winds=new int***[R];
	for(int row=0;row<R;++row){
		winds[row]=new int**[C];
		for(int col=0;col<C;++col){
			winds[row][col]=new int*[A];
			for(int alt=0;alt<A;++alt)
				winds[row][col][alt]=new int[2];
		}
	}

	for(int alt=0;alt<A;++alt){
		for(int row=0;row<R;++row){
			for(int col=0;col<C;++col){
				in >> winds[row][col][alt][0] >> winds[row][col][alt][1];
			}
		}
	}
	in.close();

	for(int delR=-V;delR<=V;++delR){
		for(int delC=-V;delC<=V;++delC){
			if(delR*delR+delC*delC<=V*V)
				delVvec.push_back(make_pair(delR,delC) );
		}
	}

	doAlgo();
}
