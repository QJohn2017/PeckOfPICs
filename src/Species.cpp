#include "Species.h"

/**********************************************************
CONSTRUCTORS/DESTRUCTORS
***********************************************************/

/**
 * @brief Constructor for Species object
 *
 * @param npar Total number of particles in the species
 * @param nx Number of grid spaces in x direction
 * @param ny Number of grid spaces in y direction
 * @param Qpar Charge of species
 * @param density Base density of the species
 * @param init_fcn User provided function which initializes the density of the
 *                 species to the user's specification
 */
Species::Species()
{
}

Species::Species(uint npar, uint nx, uint ny, double Qpar)
{
    this->npar = npar;
    this->parts.reserve(npar);

    this->density_arr = GridObject(nx, ny);

    this->Qpar = Qpar;

    this->total_KE = 0.0;
}

Species::Species(uint npar, uint nx, uint ny, double Qpar, double density,
                 std::function<void(Species &, uint)> init_fcn)
{
    this->npar = npar;
    this->parts.reserve(npar);

    this->density = density;
    this->density_arr = GridObject(nx, ny);

    this->Qpar = Qpar;

    this->total_KE = 0.0;

    init_species(init_fcn);
}

/**
 * @brief Destructor for Species object
 *
 */
Species::~Species()
{
}
//-----------------------------------------


/**
 * @brief Adds a new particle to the species
 *
 * @param x_pos The physical x position of the particle
 * @param y_pos The physical y position of the particle
 * @param z_pos The physical z position of the particle
 * @param x_mom The physical x momentum of the particle
 * @param y_mom The physical y momentum of the particle
 * @param z_mom The physical z momentum of the particle
 * @param Wpar The weight of the particle
 */
void Species::add_particle(double x_pos, double y_pos, double z_pos,
                           double x_mom, double y_mom, double z_mom,
                           double Wpar)
{
    this->add_particle(Particle(x_pos, y_pos, z_pos,
                                x_mom, y_mom, z_mom,
                                Wpar));
}

/**
 * @brief Adds a new particle to the species
 *
 * @param pos A vector containing all position components of the particle
 * @param mom A vector containing all momentum components of the particle
 * @param Wpar The weight of the particle
 */
void Species::add_particle(ThreeVec pos, ThreeVec mom, double Wpar)
{
    this->add_particle(Particle(pos, mom, Wpar));
}

/**
 * @brief Adds a new particle to the species
 *
 * @param p A particle object to add to the species
 */
void Species::add_particle(Particle p)
{
    this->parts.push_back(p);
}
//-----------------------------------------


/**********************************************************
CLASS METHODS
***********************************************************/

/**
 * @brief Deposits and interpolates the species charge onto the grid
 *
 * @param dx Spatial grid step in x direction
 * @param dy Spatial grid step in y direction
 * @param L_x Physical length of system in x direction
 * @param L_y Physical length of system in y direction
 * @param Nx Number of grid spaces in x direction
 * @param Ny Number of grid spaces in y direction
 * @return int Returns an error code or 0 if successful
 */
int Species::deposit_charge(const double dx, const double dy,
                            const double L_x, const double L_y,
                            const uint Nx, const uint Ny)
{
    // Initialize
    for (auto &d : this->density_arr)
    {
        d = 0.0;
    }

    double x_pos, y_pos, par_weight;
    double fi, fj, hx, hy;
    double x_min = 0.0, y_min = 0.0;
    uint i, j;

    for (const auto &p : this->parts)
    {
        par_weight = p.get_weight() / dx / dy; // add normalization factor here
        x_pos = p.get_pos().get_x();
        y_pos = p.get_pos().get_y();

        // This is because I have chosen to start my boundary at -dx/2
        if (x_pos < 0.0)
        {
            x_pos += L_x;
        }
        if (y_pos < 0.0)
        {
            y_pos += L_y;
        }

        fi = (x_pos - x_min) / dx;
        i  = fi;
        hx = fi - i;

        fj = (y_pos - y_min) / dy;
        j  = fj;
        hy = fj - j;

        density_arr.add_to_grid_data(i,   j,   (1.-hx) * (1.-hy) * par_weight);
        density_arr.add_to_grid_data(i+1, j,   (1.-hx) * hy      * par_weight);
        density_arr.add_to_grid_data(i,   j+1, hx      * (1.-hy) * par_weight);
        density_arr.add_to_grid_data(i+1, j+1, hx      * hy      * par_weight);
    }
    return 0;
}

//TODO:
/**
 * @brief Interpolates the field values from the grid to the particle position
 *
 * @param f Field to interpolate to particle position
 * @param dx Spatial grid step in x direction
 * @param dy Spatial grid step in y direction
 * @param L_x Physical length of system in x direction
 * @param L_y Physical length of system in y direction
 * @param Nx Number of grid spaces in x direction
 * @param Ny Number of grid spaces in y direction
 * @return int Returns an error code or 0 if successful
 */
int Species::map_field_to_part(const Field &f,
                               const double dx, const double dy,
                               const double L_x, const double L_y,
                               const uint Nx, const uint Ny)
{
    double x_pos, y_pos, par_weight;
    double fi, fj, hx, hy;
    double x_min = 0.0, y_min = 0.0;
    uint i, j;

    double loc_f_x1 = 0.0;
    double loc_f_x2 = 0.0;
    double loc_f_x3 = 0.0;

    for (auto &p : this->parts)
    {
        par_weight = p.get_weight() / dx / dy; // add normalization factor here
        x_pos = p.get_pos().get_x();
        y_pos = p.get_pos().get_y();

        // This is because I have chosen to start my boundary at -dx/2
        if (x_pos < 0.0)
        {
            x_pos += L_x;
        }
        if (y_pos < 0.0)
        {
            y_pos += L_y;
        }

        fi = (x_pos - x_min) / dx;
        i = fi;
        hx = fi - i;

        fj = (y_pos - y_min) / dy;
        j = fj;
        hy = fj - j;

// added a print statement here
        std::cout << "hx " << hx << ", hy " << hy << std::endl;

        loc_f_x1 += (1.-hx) * (1.-hy) * par_weight * this->Qpar \
                    * f.f1.get_grid_data(i, j);
        loc_f_x1 += (1.-hx) * hy      * par_weight * this->Qpar \
                    * f.f1.get_grid_data(i+1, j);
        loc_f_x1 += hx      * (1.-hy) * par_weight * this->Qpar \
                    * f.f1.get_grid_data(i, j+1);
        loc_f_x1 += hx      * hy      * par_weight * this->Qpar \
                    * f.f1.get_grid_data(i+1, j+1);

        loc_f_x2 += (1.-hx) * (1.-hy) * par_weight * this->Qpar \
                    * f.f2.get_grid_data(i, j);
        loc_f_x2 += (1.-hx) * hy      * par_weight * this->Qpar \
                    * f.f2.get_grid_data(i+1, j);
        loc_f_x2 += hx      * (1.-hy) * par_weight * this->Qpar \
                    * f.f2.get_grid_data(i, j+1);
        loc_f_x2 += hx      * hy      * par_weight * this->Qpar \
                    * f.f2.get_grid_data(i+1, j+1);

        loc_f_x3 += (1.-hx) * (1.-hy) * par_weight * this->Qpar \
                    * f.f3.get_grid_data(i, j);
        loc_f_x3 += (1.-hx) * hy      * par_weight * this->Qpar \
                    * f.f3.get_grid_data(i+1, j);
        loc_f_x3 += hx      * (1.-hy) * par_weight * this->Qpar \
                    * f.f3.get_grid_data(i, j+1);
        loc_f_x3 += hx      * hy      * par_weight * this->Qpar \
                    * f.f3.get_grid_data(i+1, j+1);

        p.set_local_e_field(loc_f_x1, loc_f_x2, loc_f_x3);
    }
    return 0;
}

//TODO:
/**
 * @brief Performs a Boris push on all of the particles in the species
 *
 * @param L_x Physical length of system in x direction
 * @param L_y Physical length of system in y direction
 * @param dt Timestep
 * @param dx Spatial grid step in x direction
 * @param dy Spatial grid step in y direction
 * @return int Returns an error code or 0 if successful
 */
int Species::push_particles(const double L_x, const double L_y,
                            const double dt,
                            const double dx, const double dy)
{
    // For total kinetic energy diagnostic
    double KE = 0.0;
    this->total_KE = 0.0;

    // double B0 = sqrt(3.0);//1.0;
    double b2, mom2, gamma;
    ThreeVec t, s, pos, mom;
    // ThreeVec t = ThreeVec(0.0, 0.0, B0 * dt / 2);
    // ThreeVec s = t * (2 / (1 + t.square()));
    ThreeVec vstar, vperp;

    for (auto &p : this->parts)
    {
        pos = p.get_pos();
        mom = p.get_mom();

        // minus instead of plus b/c of convention used in this program
        mom -= p.get_local_e_field() * (dt * 0.5);

        mom2 = mom.square();
        gamma = 1. / sqrt(1. + mom2);

        b2 = p.get_local_b_field().square();
        if (b2) // test if non-zero
        {
            t = p.get_local_b_field() * dt * 0.5;
            s = t * (2. / (1. + t.square()));

            vperp = mom - ((mom.element_multiply(p.get_local_b_field()))
                           / sqrt(b2));
            vstar = vperp + (vperp^t);

            mom += vstar^s;
        }

        mom -= p.get_local_e_field() * (dt * 0.5);

        pos += mom * (dt / gamma);

        this->apply_bc(pos, L_x, L_y, dx, dy);
        p.set_mom(mom);
        p.set_pos(pos);

        // For total kinetic energy diagnostic
        // KE *= mom.mag();
        // this->total_KE += KE;
    }

    // For total kinetic energy diagnostic
    // this->total_KE *= (L_sys / (2.0 * double(this->npar)));

    return 0;
}

/**
 * @brief Returns all of the particles' x positions
 *
 * @return std::vector<double> Vector containing the x positions of all
 *                             particles in species
 */
std::vector<double> Species::get_x_phasespace()
{
    std::vector<double> to_ret = std::vector<double>(this->npar);

    for (int i = 0; i < this->npar; ++i)
    {
        to_ret[i]  = this->parts[i].get_pos().get_x();
    }

    return to_ret;
}

/**
 * @brief Returns all of the particles' y positions
 *
 * @return std::vector<double> Vector containing the y positions of all
 *                             particles in species
 */
std::vector<double> Species::get_y_phasespace()
{
    std::vector<double> to_ret = std::vector<double>(this->npar);

    for (int i = 0; i < this->npar; ++i)
    {
        to_ret[i] = this->parts[i].get_pos().get_y();
    }

    return to_ret;
}

/**
 * @brief Returns all of the particles' x momenta
 *
 * @return std::vector<double> Vector containing the x momenta of all particles
 *                             in species
 */
std::vector<double> Species::get_px_phasespace()
{
    std::vector<double> to_ret = std::vector<double>(this->npar);

    for (int i = 0; i < this->npar; ++i)
    {
        to_ret[i] = this->parts[i].get_mom().get_x();
    }

    return to_ret;
}

/**
 * @brief Returns all of the particles' y momenta
 *
 * @return std::vector<double> Vector containing the y momenta of all particles
 *                             in species
 */
std::vector<double> Species::get_py_phasespace()
{
    std::vector<double> to_ret = std::vector<double>(this->npar);

    for (int i = 0; i < this->npar; ++i)
    {
        to_ret[i] = this->parts[i].get_mom().get_y();
    }

    return to_ret;
}

void Species::print_part_v_coord(uint ii)
{
    for (auto &particle : parts)
    {
        std::cout << (particle.get_mom()).get(ii) << '\t';
    }
    std::cout << std::endl;   
}

void Species::print_part_coord(uint ii)
{
    for (auto &particle : parts)
    {
        std::cout << (particle.get_pos()).get(ii) << '\t';
    }
    std::cout << std::endl;      
}

void Species::print_E_x()
{
    for (auto &particle : parts)
    {
        std::cout << (particle.get_local_e_field()).get_x() << '\t';
    }
    std::cout << std::endl;
}

void Species::print_E_y()
{
    for (auto &particle : parts)
    {
        std::cout << (particle.get_local_e_field()).get_y() << '\t';
    }
    std::cout << std::endl;
}

void Species::print_E_z()
{
    for (auto &particle : parts)
    {
        std::cout << (particle.get_local_e_field()).get_z() << '\t';
    }
    std::cout << std::endl;
}

void Species::print_B_x()
{
    for (auto &particle : parts)
    {
        std::cout << (particle.get_local_b_field()).get_x() << '\t';
    }
    std::cout << std::endl;
}

void Species::print_B_y()
{
    for (auto &particle : parts)
    {
        std::cout << (particle.get_local_b_field()).get_y() << '\t';
    }
    std::cout << std::endl;
}

void Species::print_B_z()
{
    for (auto &particle : parts)
    {
        std::cout << (particle.get_local_b_field()).get_z() << '\t';
    }
    std::cout << std::endl;
}

void Species::print_weights()
{
    for (auto &particle : parts)
    {
        std::cout << particle.get_weight() << '\t';
    }
    std::cout << std::endl;
}

/**
 * @brief Prints the density distribution for this species
 *
 */
void Species::print_density()
{
    density_arr.print_grid_data();
}
//-----------------------------------------


/**********************************************************
PRIVATE CLASS METHODS
***********************************************************/

void Species::init_species(std::function<void(Species &, uint)> init_fcn)
{
    init_fcn(*this, this->npar);
}

/**
 * @brief Currently applies periodic boundary conditions in x and y directions
 *        for the species
 *
 * @param pos A vector containing all position components of the particle
 * @param L_x Physical length of system in x direction
 * @param L_y Physical length of system in y direction
 * @param dx Spatial grid step in x direction
 * @param dy Spatial grid step in y direction
 */
void Species::apply_bc(ThreeVec &pos,
                       const double L_x, const double L_y,
                       const double dx, const double dy)
{
    double x1 = pos.get_x();
    double y1 = pos.get_y();

    // Periodic x boundaries
    while (x1 < -dx / 2.0)
    {
        x1 += L_x;
        pos.set_x(x1);
    }
    while (x1 >= (L_x - (dx / 2.0)))
    {
        x1 -= L_x;
        pos.set_x(x1);
    }

    // Periodic y boundaries
    while (y1 < -dy / 2.0)
    {
        y1 += L_y;
        pos.set_y(y1);
    }
    while (y1 >= (L_y - (dy / 2.0)))
    {
        y1 -= L_y;
        pos.set_y(y1);
    }
}
//-----------------------------------------
