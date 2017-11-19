#include<iostream>
#include<cstdio>
#include<thread>


//the simulator program
//this runs the simulation to test our SDN protocol

void RunSimulator(); // the simulator

void Tick(); //the tick function of the simulator (like a frame in a game)

void WaitFor(float time); //a function that waits for a certain amount of time

int main() //todo add command line params
{
    bool error = false;

    int c;

    //take input

    //error handling

    if (error) 
    {
        return -1;
    }

    //start sim in another thread
    //todo: start sim in another thread

    c = getchar();

    //after keyboard input end the simulator
    //todo: join the thread that the simulator is running in

    return 0;
}

void RunSimulator()
{
    while (true) 
    {
        //call sleep for to space ticks out

        Tick();
    }
}