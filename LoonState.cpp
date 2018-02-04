/*
 * LoonState.cpp
 *
 *  Created on: 04.02.2016
 *      Author: David
 */

#include "LoonState.h"

#include <fstream>
#include <cassert>
#include <vector>
#include <random>
#include <iostream>
#include <exception>

using namespace std;

extern int R,C,A,L,B,T,rs,cs;
extern bool** Lgrid;
extern vector<pair<int,int> > Lcells;
extern int**** winds;
extern vector<pair<int,int> > delVvec;
extern default_random_engine randomGenerator;

LoonState::LoonState(){
	Bmoves=new int*[B];
	Bstrength=new int[B];
	Bpos=new int*[B];
	for(int b=0;b<B;++b){
		Bmoves[b]=new int[T];
		Bpos[b]=new int[3];
		for(int t=0;t<T;++t)
			Bmoves[b][t]=0;
		Bstrength[b]=0;
		Bpos[b][0]=rs;
		Bpos[b][1]=cs;
		Bpos[b][2]=-2;  //-2 not lifted, -1 chrased, 0 lowest alt level
	}
	ticksLived=0;
	points=0;
}

void LoonState::set(LoonState copy){
	ticksLived=copy.ticksLived;
	for(int b=0;b<B;++b)
		for(int t=0;t<T;++t)
			Bmoves[b][t]=copy.Bmoves[b][t];
	for(int b=0;b<B;++b){
		Bstrength[b]=copy.Bstrength[b];
		Bpos[b][0]=copy.Bpos[b][0];
		Bpos[b][1]=copy.Bpos[b][1];
		Bpos[b][2]=copy.Bpos[b][2];
	}
	points=copy.points;
}

void LoonState::setOnlyBmoves(LoonState* copy){
	for(int b=0;b<B;++b)
		for(int t=0;t<T;++t)
			Bmoves[b][t]=copy->Bmoves[b][t];
}

void LoonState::dump(){
	ofstream writer("result.out");
	for(int t=0;t<T;++t){
		writer << Bmoves[0][t];
		for(int b=1;b<B;++b)
			writer << " " << Bmoves[b][t];
		writer << endl;
	}
	writer.close();
}

void LoonState::shortDump() const{
	cout <<  (points/(double)T/L) << " " << getWeakestB() << endl;
}

int LoonState::getWeakestB() const {
	int weakest=Bstrength[0];
	int weakestB=0;
	for(int b=1;b<B;++b){
		if(weakest>Bstrength[b]){
			weakest=Bstrength[b];
			weakestB=b;
		}
	}
	return weakestB;
}

void LoonState::tick(){
  //do ballon movement
	for(int b=0;b<B;++b){
		if(Bpos[b][0]==-1)
			continue;
		if(Bpos[b][2]==-2 && Bmoves[b][ticksLived]==1) //do liftof over crash state away
			Bpos[b][2]+=1;
		Bpos[b][2]+=Bmoves[b][ticksLived];
		if(Bpos[b][2]<-2 || Bpos[b][2]==-1 || Bpos[b][2]>=A){
			points=-1;
			//cerr << "reached not allowed altitude!" << endl;
			throw exception();
		}
		if(Bpos[b][2]==-2)
			continue;
		int row=Bpos[b][0];
		int col=Bpos[b][1];
		int alt=Bpos[b][2];
		int windR=winds[row][col][alt][0];
		int windC=winds[row][col][alt][1];
		Bpos[b][0]+=windR;
		Bpos[b][1]+=windC;
		if(Bpos[b][0]<0 || Bpos[b][0]>=R){
			Bpos[b][0]=-1;
			Bpos[b][1]=0;
		}
		Bpos[b][1]=(Bpos[b][1]+C)%C;
	}
	evalBpositions();
	++ticksLived;
}

void LoonState::evaluate(){
	points=0;
	ticksLived=0;
	for(int b=0;b<B;++b){
		Bpos[b][0]=rs;
		Bpos[b][1]=cs;
		Bpos[b][2]=-2;
		Bstrength[b]=0;
	}
	while(ticksLived<T){
		tick();
	}
	shortDump();
}

void LoonState::optimize(){
	int weakestB=getWeakestB();
	LoonState* test=new LoonState[27];
	for(int i=0;i<27;++i){
		test[i].setOnlyBmoves( this );
	}
	for(int i=0;i<27;++i){
		test[i].Bmoves[weakestB][0]=i%3-1;
		test[i].Bmoves[weakestB][1]=(i/3)%3-1;
		test[i].Bmoves[weakestB][2]=(i/9)%3-1;
		try{
			test[i].tick();
			test[i].tick();
			test[i].tick();
		}
		catch (...){
			//e.printStackTrace();
			continue;
		}
	}
	for(int t=3;t<T;++t){
		LoonState* bestTest=new LoonState[9];
		int weakestBestId=0;
		for(int i=0;i<27;++i){
			if(bestTest[weakestBestId].points<test[i].points){
				bestTest[weakestBestId].set( test[i] );
				weakestBestId=0;
				for(int k=1;k<9;++k){
					if(bestTest[k].points<bestTest[weakestBestId].points){
						weakestBestId=k;
					} else if(bestTest[k].points==bestTest[weakestBestId].points){
						if(uniform_int_distribution<int>(0,1)(randomGenerator))
							weakestBestId=k;
					}
				}
			} else if(bestTest[weakestBestId].points==test[i].points){
				if(uniform_int_distribution<int>(0,1)(randomGenerator))
					bestTest[weakestBestId].set( test[i] );
				weakestBestId=0;
				for(int k=1;k<9;++k){
					if(bestTest[k].points<bestTest[weakestBestId].points){
						weakestBestId=k;
					} else if(bestTest[k].points==bestTest[weakestBestId].points){
						if(uniform_int_distribution<int>(0,1)(randomGenerator))
							weakestBestId=k;
					}
				}
			}
		}
		bestTest[0].shortDump();
		for(int i=0;i<27;++i){
			test[i].set( bestTest[i/3] );
			test[i].Bmoves[weakestB][t]=(i%3)-1;
			try{
				test[i].tick();
			}
			catch (...){
				//e.printStackTrace();
				continue;
			}
		}
	}
	for(int i=0;i<27;++i)
		if(test[i].points>this->points)
			this->set( test[i] );
	dump();
}

void LoonState::evalBpositions(){
	// check for connected Lcells
	int** ballons= new int*[R];
	for(int row =0; row<R;++row)
		ballons[row]=new int[C];
	//HashMap<int[], Integer> LcellToBallon = new HashMap<int[], Integer>();
	for(int b=0;b<B;++b){
		int row=Bpos[b][0];
		int col=Bpos[b][1];
		//boolean connected=false;
		for(unsigned checkV=0;checkV<delVvec.size();++checkV){
			int delRow=delVvec[ checkV ].first;
			int delCol=delVvec[ checkV ].second;
			int curRow=row+delRow;
			int curCol=col+delCol;
			if(curRow<0 || curRow>=R)
				continue;
			curCol=(curCol+C)%C;
			if(Lgrid[curRow][curCol]==true){
				//Bstrength[-Bgrid[curRow][curCol]-1]+=1;
				if(ballons[curRow][curCol]!=0) {
				//int cur[] = {curRow, curCol};
				//if(LcellToBallon.get(cur)!=null) {
				//try {
					Bstrength[ballons[curRow][curCol]]-=1;
					//Bstrength[LcellToBallon.get(cur)]-=1;
					Lgrid[curRow][curCol]=false;
				}
				//catch (Exception e) {
				else {
					ballons[curRow][curCol] = b;
					//LcellToBallon.put(cur , b);
					Bstrength[b]+=1;
					++points;
				}
			}
		}
		/*
		if(connected)
			++points;*/
	}
	//reset Lgrid
	for(unsigned l=0;l<Lcells.size();++l) {
		Lgrid[Lcells[l].first][Lcells[l].second] = true;
	}
}
