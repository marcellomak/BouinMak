#ifndef BREAK_EVEN_VOLATILITY_HPP
#define BREAK_EVEN_VOLATILITY_HPP
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <numeric> //for std::iota
#include <ctime>
#include <iomanip> // for std::get_time
#include <algorithm>

class time_series
{
public:
    //constructor and destructor
    time_series(const std::string& filepath, const std::string& dataname);
    ~time_series();
    
    //methods
    const std::string& get_dataname() const; // get private data member m_dataname
    const std::vector<double>& get_data() const; // get the whole series of data
    const std::vector<time_t>& get_date() const;
    std::vector<ptrdiff_t> get_datapos(const std::string& maturity, const size_t& term_day) const; // determine the positions of the target data series
    std::vector<double> get_data(const ptrdiff_t& startpos, const ptrdiff_t& endpos) const; // get a part of the series with starting and ending positions
    std::vector<time_t> get_date(const ptrdiff_t& startpos, const ptrdiff_t& endpos) const;
    //std::vector<size_t> day_to_maturity(const size_t& day_number) const; // create a vector containing daily days to maturity
    /*
     // disable the operators (automatically generated by compilers) that could lead to mistakes
     time_series(const time_series& rhs) = delete;
     time_series& operator=(const time_series& rhs) = delete;
     time_series(time_series&& rhs) = delete;
     time_series& operator=(time_series&& rhs) = delete;
     */
private:
    std::string m_filepath;
    std::string m_dataname;
    std::vector<time_t> m_datadate;
    std::vector<double> m_data;
};

/*
class vol // maybe to delete
{
    public:
        explicit vol(const size_t mat, const double strike, const std::string underlying);
        virtual ~vol();

    private:
    protected:

};
*/

<<<<<<< HEAD
class option
{
public:
    // constructor and destructor
    option(const time_series& underlying, const double& strike, const double& vol, const time_series& rate, const std::string& maturity, const size_t& term_day, const int& type);
    option(const time_series& underlying, const double& strike, const double& vol, const double& rate, const std::string& maturity, const size_t& term_day, const int& type);

    ~option();

    // daily price and delta of the option
    std::vector<double> BS_price() const;
    std::vector<double> BS_delta() const;
    void modify_vol(const double& vol);

private:
    time_series m_underlying;
    int m_type;
    double m_strike;
    double m_vol;
    time_series m_rate;
    std::string m_maturity;
    size_t m_term_day;
    std::vector<ptrdiff_t> m_datapos;
    std::vector<double> m_fixedrate;

protected:
};

class portfolio
{
public:
private:
};

/*
class PnL : public vol //maybe to change
{
    public:

        //constructor and destructor
        PnL(const std::vector<double>& underlying, const double& strike, const double& vol, const std::vector<double>& rate ,const size_t& mat);
        ~PnL();

        //computation of the PnL, use of the pricers formulas
        const Hedged_PnL(const std::vector<double>& underlying, const double& strike, const double& vol, const std::vector<double>& rate, const size_t& mat) const;
        //maybe have a get_vol method here, since we will find the fair vol here

    
    private:
    protected:
}
*/

// function converting date string to time_t object
time_t c_str_timet(const std::string& targetdate);
double normalCDF(const double& x);

#endif
