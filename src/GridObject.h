#ifndef GRID_H
#define GRID_H

// #include <stdio.h>
#include <iostream>
#include <vector>
#include <functional>

class GridObject 
{
    private:
        std::vector<double> gridded_data;
        int nx, ny;

        void init_grid_obj(std::function<void(GridObject &, unsigned int, unsigned int)> init_fcn);

    public:
        /**********************************************************
        CONSTRUCTORS/DESTRUCTORS
        ***********************************************************/
        GridObject(); //TODO: see if this can be removed
        GridObject(uint nx, uint ny, std::function<void(GridObject &, unsigned int, unsigned int)> init_fcn);
        ~GridObject();
        //-----------------------------------------

        inline double get_grid_data(uint index_x, unsigned int index_y);
};
#endif
