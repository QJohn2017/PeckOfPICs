#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <vector>
#include <functional>

typedef unsigned int uint;

class GridObject
{
    private:
        std::vector<double> gridded_data;
        int nx, ny;

        void init_grid_obj(std::function<void(GridObject &, uint, uint)> init_fcn);

    public:
        /**********************************************************
        CONSTRUCTORS/DESTRUCTORS
        ***********************************************************/
        GridObject(); //TODO: see if this can be removed
        GridObject(uint nx, uint ny);
        GridObject(uint nx, uint ny,
                   std::function<void(GridObject &, uint, uint)> init_fcn);
        GridObject(uint nx, uint ny,
                   std::vector<double> data); // a 'copy' constructor
        ~GridObject();
        //-----------------------------------------


        /**********************************************************
        ITERATOR FUNCTIONS
        ***********************************************************/
        inline GridObject::iterator begin() noexcept
        {
            return gridded_data.begin();
        }
        inline GridObject::const_iterator cbegin() const noexcept
        {
            return gridded_data.cbegin();
        }
        inline GridObject::iterator end() noexcept
        {
            return gridded_data.end();
        }
        inline GridObject::const_iterator cend() const noexcept
        {
            return gridded_data.cend();
        }
        //-----------------------------------------


        /**********************************************************
        OPERATOR FUNCTIONS
        ***********************************************************/
        /**
         * @brief Overload the += operator for element-wise addition between
         *        GridObject objects
         *
         */
        inline GridObject operator+=(GridObject grid)
        {
            for (uint i = 0; i < this->nx * this->ny; ++i)
            {
                this->gridded_data.at(i) += grid.gridded_data.at(i);
            }

            return *(this);
        }

        /**
         * @brief Overload the -= operator for element-wise subtraction between
         *        GridObject objects
         *
         */
        inline GridObject operator-=(GridObject grid)
        {
            for (uint i = 0; i < this->nx * this->ny; ++i)
            {
                this->gridded_data.at(i) -= grid.gridded_data.at(i);
            }

            return *(this);
        }
        //-----------------------------------------


        /**********************************************************
        CLASS METHODS
        ***********************************************************/
        /**
         * @brief Add the provided value to the current value stored in the
         *        grid at a given index.
         *
         * @param index_x x index of grid
         * @param index_y y index of grid
         * @param val number to sum with value at given indices
         */
        inline void add_to_grid_data(uint index_x, uint index_y, double val)
        {
            gridded_data.at(index_x * ny + index_y) += val;
        }

        /**
         * @brief Set the grid data object
         *
         * @param index_x x index of grid
         * @param index_y y index of grid
         * @param val number to set in grid at given indices
         */
        inline void set_grid_data(uint index_x, uint index_y, double val)
        {
            gridded_data.at(index_x * ny + index_y) = val;
        }

        /**
         * @brief Get the grid data object
         *
         * @param index_x x index of grid
         * @param index_y y index of grid
         * @return double get value stored in grid at given indices
         */
        inline double get_grid_data(uint index_x, uint index_y) const
        {
            return gridded_data.at(index_x * ny + index_y);
        }

        void print_grid_data();
        //-----------------------------------------
};

#endif
