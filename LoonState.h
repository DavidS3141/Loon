/*
 * LoonState.h
 *
 *  Created on: 04.02.2016
 *      Author: David
 */

//==============================
// include guard
#ifndef LOONSTATE_H_
#define LOONSTATE_H_
//==============================
// included dependencies
//==============================
//==============================
// forward declared dependencies
//==============================
// the actual class


class LoonState
{
private:
    int** Bmoves;
    int* Bstrength;
    int** Bpos;
    int ticksLived;
    int points;

    void set(LoonState copy);

    void setOnlyBmoves(LoonState* copy);

    void dump();


    int getWeakestB() const ;

    void tick();


    void evalBpositions();

public:
    LoonState();
    void evaluate();

    void optimize();
    void shortDump() const;

    int getPoints() const { return points;}

};

#endif /* LOONSTATE_H_ */
