#include "GridObject.h"


/**********************************************************
CONSTRUCTORS/DESTRUCTORS
***********************************************************/
GridObject::GridObject()
{
}

GridObject::GridObject(uint nx, uint ny, std::function<void(GridObject &, uint, uint)> init_fcn)
{
    this->nx = nx;
    this->ny = ny;

    init_grid_obj(init_fcn);
}

GridObject::GridObject(uint nx, uint ny, std::vector<double> data) // a 'copy' constructor
{
    this->nx = nx;
    this->ny = ny;

    this->gridded_data = data;
}

GridObject::~GridObject()
{
}
//-----------------------------------------

inline double GridObject::get_grid_data(uint index_x, uint index_y)
{
    return gridded_data.at(index_x * ny + index_y);
}

void GridObject::print_grid_data()
{
for (auto &f : this->gridded_data )
    {
        std::cout << f << '\t';
    }
    std::cout << std::endl;
}


/**********************************************************
PRIVATE FUNCTIONS
***********************************************************/
void GridObject::init_grid_obj(std::function<void(GridObject &, uint, uint)> init_fcn)
{
    init_fcn(*this, this->nx, this->ny);
}
